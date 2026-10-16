// Steamboat.exe .\GameShellRelease\GameApp.obj: the game shell. WinMain reads
// engine.ini, loads MBEngine.dll and the game DLL through MBCom, creates the window
// and runs the event loop; WndProc turns window messages into MBEngine events.
#include "GameApp.h"
#include "resource.h"

HINSTANCE gInst;
unsigned int gCurrentCodePage = CP_THREAD_ACP;

static bool sActivationState;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance, const char *szWindowClass);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void ProcessMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateCurrentCodePage(void);

// TODO 0x00401000 _WinMain@16: (88% similar) the rest is the same code: ours merges the `return FALSE` after the engine fails to load into the identical cleanup tail of the `return FALSE` after CreateWindow (the original repeats the tail), and calls gameClock's Release out of line where the original inlines it (inline budget: with the plain-if Release, or a few more statements in WinMain, it is inlined)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MBDebugStr("INFO: Process (0x%X) started.\n", GetCurrentProcessId());
    MBDebugStr("      Main thread (0x%X) started.\n", GetCurrentThreadId());
    MBDebugStr("INFO: Black box installed.\n");

    MBString gGameDllStr;
    __try {
        const int MAX_LOADSTRING = 100;
        long useDirect3DValue = 0;
        MBString applicationTitle;
        bool useDirect3D = false;
        MBString ddEnumFunctionStr;
        char szWindowClass[MAX_LOADSTRING];
        MSG msg;

        LoadString(hInstance, IDC_STEAMBOAT, szWindowClass, MAX_LOADSTRING);

        // only one instance: bring the running one to the front
        HANDLE hMutex = CreateMutex(NULL, FALSE, "OneMuffinAtATime");
        if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
            HWND hwnd = FindWindow(szWindowClass, NULL);
            if (hwnd)
                SetForegroundWindow(hwnd);
            ExitProcess(1);
        }

        MBComInitialize();
        long com_error = MBComLoadServer("mbengine.dll");
        if (com_error) {
            MessageBox(NULL, "Unable to load: \n Could not find the engine component.", "Game Error", MB_OK);
            MBComUninitialize();
            return FALSE;
        }

        MyRegisterClass(hInstance, szWindowClass);

        long windowedX = CW_USEDEFAULT;
        long windowedY = CW_USEDEFAULT;
        bool fullScreen = true;

        char pathbuf[256];
        GetModuleFileName(NULL, pathbuf, 256);
        *strrchr(pathbuf, '\\') = 0;
        SetCurrentDirectory(pathbuf);
        MBDebugStr("Setting current directory to:\n  %s\n", pathbuf);

        MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
        if (fileSpec) {
            MBComPtr<IMBStream> stream;
            fileSpec->SetPath("engine.ini");
            IMBStream *tempStream;
            MBOpenFile(fileSpec, 5, &tempStream);
            stream.Attach(tempStream);
            if (stream) {
                MBComPtr<IMBPref> pref(MBIID_CMBPref);
                MBString key;
                key = "EnableDirect3D";
                useDirect3DValue = pref->GetLong(stream, MBString("Screen"), key, 0);
                useDirect3D = useDirect3DValue != 0;
                pref->GetString(stream, MBString("Boot"), MBString("GameTitle"), applicationTitle, MBString());
                key = "Fullscreen";
                long fullScreenValue = pref->GetLong(stream, MBString("Screen"), key, 1);
                fullScreen = fullScreenValue != 0;
                key = "Game";
                pref->GetString(stream, MBString("Boot"), key, gGameDllStr, MBString("gamex86.dll"));
                key = "DDEnumCallback";
                pref->GetString(stream, MBString("Screen"), key, ddEnumFunctionStr, MBString(""));
            }
        }

        HINSTANCE hModule = LoadLibrary(gGameDllStr.c_str());
        if (hModule && !MBComLoadServer(gGameDllStr.c_str())) {
            IMBGameEngine *game;
            MBComCreateInstance(NULL, MBIID_IMBGameEngine, (IMBUnknown **)&game);

            MBCreateScreenFunc createScreenFunc = (MBCreateScreenFunc)GetProcAddress(hModule, "OnMBEngineCreateScreen");
            game->SetOnEngineCreateScreenFunction(createScreenFunc);
            MBEngineFunc startupFunc = (MBEngineFunc)GetProcAddress(hModule, "OnMBEngineStartup");
            game->SetOnEngineStartupFunction(startupFunc);
            MBEngineFunc shutdownFunc = (MBEngineFunc)GetProcAddress(hModule, "OnMBEngineShutdown");
            game->SetOnEngineShutdownFunction(shutdownFunc);
            FreeLibrary(hModule);

            MBComPtr<IMBDirectDrawScreenManager> screenManager(MBIID_CMBDirectDrawScreenManager);
            MBRegisterScreenManager(screenManager);
            screenManager->EnableDirect3D(useDirect3D);
            screenManager->SetDirectDrawEnumerator((MBDDEnumCallback)GetProcAddress(hModule, ddEnumFunctionStr.c_str()));
            if (fullScreen)
                screenManager->EnterFullScreenMode();
            else
                screenManager->EnterWindowedMode();

            unsigned short width = 640;
            unsigned short height = 480;
            unsigned short depth = 16;
            unsigned short windowWidth = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + width;
            unsigned short windowHeight = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + height;

            gInst = hInstance;
            MBString str(applicationTitle);
            HWND hWnd = CreateWindow(szWindowClass, str.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                     windowedX, windowedY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
            if (!hWnd)
                return FALSE;

            SetWindowLong(hWnd, GWL_USERDATA, (LONG)game);
            ShowWindow(hWnd, nCmdShow);
            UpdateWindow(hWnd);
            screenManager->SetHWND(hWnd);
            game->CreateScreen(screenManager);
            screenManager->Activate();

            MBComPtr<IMBWinSoundManager> soundmanager(MBIID_CMBMilesSoundManager);
            if (soundmanager)
                soundmanager->SetHWND(hWnd);

            game->Init();
            game->Start();

            bool isAppRunning = true;
            bool hasGameQuit = false;
            MBComPtr<IMBFPSMonitor> fpsMonitor(MBIID_CMBFPSMonitor);
            MBComPtr<IMBGameClock> gameClock(MBIID_IMBGameClock);
            unsigned long lastEventLoopTime = 0;
            UpdateCurrentCodePage();
            while (isAppRunning) {
                unsigned long currentTime = gameClock->GetRealTime();
                fpsMonitor->SetEventLoopTime(currentTime - lastEventLoopTime);
                lastEventLoopTime = currentTime;
                if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) || hasGameQuit) {
                    if (GetMessage(&msg, NULL, 0, 0)) {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    } else {
                        isAppRunning = false;
                    }
                } else if (sActivationState == true) {
                    game->Idle();
                }
                if (game->IsQuitting() && !hasGameQuit) {
                    hasGameQuit = true;
                    game->ShutDown();
                    DestroyWindow(hWnd);
                }
            }

            screenManager->Deactivate();
            screenManager->RemoveAllScreens();
            MBRegisterScreenManager(NULL);
            game->Release();
        } else {
            MessageBox(NULL, "Unable to load: \n Could not find the game component.", "Game Error", MB_OK);
        }

        MBComUninitialize();
        CloseHandle(hMutex);
        return msg.wParam;
    }
    __except (BlackBoxExceptionFilter(gGameDllStr.c_str(), GetExceptionInformation())) {
    }
    return -1;
}

// 0x00401e30 ??2@YAPAXI@Z
void *__cdecl operator new(unsigned int size)
{
    return MBmalloc_release(size);
}

// 0x00401e40 ??3@YAXPAX@Z
void __cdecl operator delete(void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}

// 0x00401e60 ?MyRegisterClass@@YAGPAUHINSTANCE__@@PBD@Z
ATOM MyRegisterClass(HINSTANCE hInstance, const char *szWindowClass)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_STEAMBOAT);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx(&wcex);
}

// TODO 0x00401ef0 ?WndProc@@YGJPAUHWND__@@IIJ@Z: (66% similar) cases, jump tables and bodies right; inline budget differs: the original calls MBComPtr::Release out of line in CreateObject at every broadcaster but WM_CHAR, calls CreateObject out of line in WM_KEYUP and _QueryInterface out of line everywhere; ours inlines Release in WM_KEYDOWN, WM_IME_ENDCOMPOSITION and WM_IME_CHAR, CreateObject in WM_KEYUP and _QueryInterface in WM_IME_STARTCOMPOSITION, WM_CHAR and WM_IME_CHAR (the budget grows with the caller's own size; Release/CreateObject/destructor/operator bool forms and declaration order tried), and register use follows from that
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char sAnsiChar[3];
    IMBGameEngine *engine = (IMBGameEngine *)GetWindowLong(hWnd, GWL_USERDATA);

    switch (message) {
    case WM_ACTIVATE: {
        bool willActivate;
        if (LOWORD(wParam) != WA_INACTIVE && HIWORD(wParam) == 0)
            willActivate = true;
        else
            willActivate = false;
        MBShowSystemCursor(!willActivate);
        MBDebugStr(" willActivate: %s\n", willActivate ? "true" : "false");
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster) {
            if (willActivate != sActivationState) {
                if (willActivate)
                    broadcaster->BroadcastEvent(-9, kApplicationActivate, 0);
                else
                    broadcaster->BroadcastEvent(-9, kApplicationDeactivate, 0);
                sActivationState = willActivate;
            }
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    case WM_KEYDOWN: {
        MBKeyboardEvent event;
        event.mCharCode = 0;
        event.mSpecialCode = 0;
        switch (wParam) {
        case VK_F1: event.mSpecialCode = kSpecialKeyF1; break;
        case VK_F2: event.mSpecialCode = kSpecialKeyF2; break;
        case VK_F3: event.mSpecialCode = kSpecialKeyF3; break;
        case VK_F4: event.mSpecialCode = kSpecialKeyF4; break;
        case VK_F5: event.mSpecialCode = kSpecialKeyF5; break;
        case VK_F6: event.mSpecialCode = kSpecialKeyF6; break;
        case VK_F7: event.mSpecialCode = kSpecialKeyF7; break;
        case VK_F8: event.mSpecialCode = kSpecialKeyF8; break;
        case VK_F9: event.mSpecialCode = kSpecialKeyF9; break;
        case VK_F10: event.mSpecialCode = kSpecialKeyF10; break;
        case VK_F11: event.mSpecialCode = kSpecialKeyF11; break;
        case VK_F12: event.mSpecialCode = kSpecialKeyF12; break;
        case VK_HOME: event.mSpecialCode = kSpecialKeyHome; break;
        case VK_END: event.mSpecialCode = kSpecialKeyEnd; break;
        case VK_DELETE: event.mSpecialCode = kSpecialKeyDelete; break;
        case VK_INSERT: event.mSpecialCode = kSpecialKeyInsert; break;
        case VK_PRIOR: event.mSpecialCode = kSpecialKeyPageUp; break;
        case VK_NEXT: event.mSpecialCode = kSpecialKeyPageDown; break;
        case VK_LEFT: event.mSpecialCode = kSpecialKeyLeftCursor; break;
        case VK_RIGHT: event.mSpecialCode = kSpecialKeyRightCursor; break;
        case VK_UP: event.mSpecialCode = kSpecialKeyUpCursor; break;
        case VK_DOWN: event.mSpecialCode = kSpecialKeyDownCursor; break;
        case VK_CONTROL: event.mSpecialCode = kSpecialKeyControl; break;
        case VK_MENU: event.mSpecialCode = kSpecialKeyAlt; break;
        case VK_SHIFT: event.mSpecialCode = kSpecialKeyShift; break;
        case VK_LWIN:
        case VK_RWIN: event.mSpecialCode = kSpecialKeyWindows; break;
        default: event.mCharCode = (unsigned short)wParam; break;
        }
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(-1, kKeyDownEvent, &event);
        break;
    }
    case WM_INPUTLANGCHANGE:
        UpdateCurrentCodePage();
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        RECT r = ps.rcPaint;
        IMBScreenManager *screenManager;
        MBGetScreenManager(&screenManager);
        if (screenManager) {
            IMBScreen *screen;
            screenManager->GetScreenByIndex(0, &screen);
            screenManager->Release();
            if (screen) {
                IMBDirectXScreen *dxscreen;
                screen->QueryInterface(MBIID_IMBDirectXScreen, (void **)&dxscreen);
                screen->Release();
                if (dxscreen) {
                    if (dxscreen->IsInGDIMode())
                        dxscreen->GDIModeUpdateRect(r);
                    dxscreen->Release();
                }
            }
        }
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYUP: {
        MBKeyboardEvent event;
        event.mCharCode = 0;
        event.mSpecialCode = 0;
        switch (wParam) {
        case VK_F1: event.mSpecialCode = kSpecialKeyF1; break;
        case VK_F2: event.mSpecialCode = kSpecialKeyF2; break;
        case VK_F3: event.mSpecialCode = kSpecialKeyF3; break;
        case VK_F4: event.mSpecialCode = kSpecialKeyF4; break;
        case VK_F5: event.mSpecialCode = kSpecialKeyF5; break;
        case VK_F6: event.mSpecialCode = kSpecialKeyF6; break;
        case VK_F7: event.mSpecialCode = kSpecialKeyF7; break;
        case VK_F8: event.mSpecialCode = kSpecialKeyF8; break;
        case VK_F9: event.mSpecialCode = kSpecialKeyF9; break;
        case VK_F10: event.mSpecialCode = kSpecialKeyF10; break;
        case VK_F11: event.mSpecialCode = kSpecialKeyF11; break;
        case VK_F12: event.mSpecialCode = kSpecialKeyF12; break;
        case VK_HOME: event.mSpecialCode = kSpecialKeyHome; break;
        case VK_END: event.mSpecialCode = kSpecialKeyEnd; break;
        case VK_DELETE: event.mSpecialCode = kSpecialKeyDelete; break;
        case VK_INSERT: event.mSpecialCode = kSpecialKeyInsert; break;
        case VK_PRIOR: event.mSpecialCode = kSpecialKeyPageUp; break;
        case VK_NEXT: event.mSpecialCode = kSpecialKeyPageDown; break;
        case VK_LEFT: event.mSpecialCode = kSpecialKeyLeftCursor; break;
        case VK_RIGHT: event.mSpecialCode = kSpecialKeyRightCursor; break;
        case VK_UP: event.mSpecialCode = kSpecialKeyUpCursor; break;
        case VK_DOWN: event.mSpecialCode = kSpecialKeyDownCursor; break;
        case VK_CONTROL: event.mSpecialCode = kSpecialKeyControl; break;
        case VK_MENU: event.mSpecialCode = kSpecialKeyAlt; break;
        case VK_SHIFT: event.mSpecialCode = kSpecialKeyShift; break;
        case VK_LWIN:
        case VK_RWIN: event.mSpecialCode = kSpecialKeyWindows; break;
        default: event.mCharCode = (unsigned short)wParam; break;
        }
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(-1, kKeyUpEvent, &event);
        break;
    }
    case WM_IME_ENDCOMPOSITION: {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster)
            broadcaster->BroadcastEvent(-3, 4, 0);
        break;
    }
    case WM_IME_STARTCOMPOSITION: {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster)
            broadcaster->BroadcastEvent(-3, 3, 0);
        break;
    }
    case WM_CHAR: {
        MBKeyboardEvent event;
        MBmemclr(&event, sizeof(event));
        sAnsiChar[0] = (char)wParam;
        sAnsiChar[1] = 0;
        sAnsiChar[2] = 0;
        MultiByteToWideChar(gCurrentCodePage, 0, sAnsiChar, -1, &event.mCharCode, 1);
        event.mSpecialCode = 0;
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (event.mCharCode)
            broadcaster->BroadcastEvent(-1, kInputCharacter, &event);
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(gInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
            break;
        case IDM_EXIT:
            engine->Quit();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_CLOSE:
        engine->Quit();
        break;
    case WM_IME_CHAR: {
        MBKeyboardEvent event;
        unsigned short temp[10];
        sAnsiChar[0] = HIBYTE(wParam);
        sAnsiChar[1] = LOBYTE(wParam);
        sAnsiChar[2] = 0;
        event.mSpecialCode = 0;
        int count = MultiByteToWideChar(gCurrentCodePage, MB_ERR_INVALID_CHARS, sAnsiChar, 3, temp, 10);
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        for (int i = 0; i < count; i++) {
            event.mCharCode = temp[i];
            if (event.mCharCode)
                broadcaster->BroadcastEvent(-1, kInputCharacter, &event);
        }
        break;
    }
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
        ProcessMouseEvent(hWnd, message, wParam, lParam);
        break;
    case WM_INPUTLANGCHANGEREQUEST:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 0x00402a70 ?ProcessMouseEvent@@YAXPAUHWND__@@IIJ@Z
void ProcessMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static UINT sLastMessageTypeSent;
    static DWORD sLastClickSentTimeMS;

    POINT globalPoint;
    globalPoint.x = LOWORD(lParam);
    globalPoint.y = HIWORD(lParam);
    ClientToScreen(hWnd, &globalPoint);

    MBPoint screenPoint;
    IMBScreen *screen;
    if (MBTransformGlobalToScreenPoint(MBPoint(globalPoint.x, globalPoint.y), screenPoint, &screen)) {
        MBMouseEvent mouseEvent;
        mouseEvent.mMousePos = screenPoint;
        mouseEvent.mScreen = screen;

        bool sendEvent = false;
        long eventType;
        switch (message) {
        case WM_MOUSEMOVE:
            eventType = kMouseMoveEvent;
            mouseEvent.mButton = 0;
            sendEvent = true;
            break;
        case WM_LBUTTONUP:
            if (sLastMessageTypeSent == WM_LBUTTONDOWN) {
                sLastMessageTypeSent = WM_LBUTTONUP;
                eventType = kMouseUpEvent;
                mouseEvent.mButton = 1;
                sLastClickSentTimeMS = timeGetTime();
                sendEvent = true;
            }
            break;
        case WM_LBUTTONDOWN: {
            DWORD now = timeGetTime();
            // timeGetTime wrapped around
            if ((sLastClickSentTimeMS & 0x80000000) && !(now & 0x80000000))
                sLastClickSentTimeMS = 0;
            if (now > sLastClickSentTimeMS + 200) {
                sLastClickSentTimeMS = now;
                sLastMessageTypeSent = WM_LBUTTONDOWN;
                eventType = kMouseDownEvent;
                mouseEvent.mButton = 1;
                sendEvent = true;
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            sendEvent = false;
            break;
        }
        if (sendEvent) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(-1, eventType, &mouseEvent);
        }
        screen->Release();
    }
}

// 0x00402c40 ?About@@YGJPAUHWND__@@IIJ@Z
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// 0x00402c80 ?UpdateCurrentCodePage@@YAXXZ
void UpdateCurrentCodePage(void)
{
    char acpBuffer[12];
    LCID locale = LOWORD(GetKeyboardLayout(0));
    GetLocaleInfo(locale, LOCALE_IDEFAULTANSICODEPAGE, acpBuffer, sizeof(acpBuffer));
    gCurrentCodePage = atoi(acpBuffer);
}

// WinMain and WndProc inline these members where the original calls them out of
// line (see their TODOs). VC6 ignores the explicit instantiation of an inline
// member function; the instantiation of the whole class emits the out-of-line copies
// (unreferenced COMDATs that the linker drops).
// 0x00403d20 ?CreateObject@?$MBComPtr@VIMBDirectDrawScreenManager@@@@QAEJABUMBGUID@@@Z
template class MBComPtr<IMBDirectDrawScreenManager>;
// 0x00403dc0 ?Release@?$MBComPtr@VIMBWinSoundManager@@@@QAEXXZ
template class MBComPtr<IMBWinSoundManager>;
// 0x00403e00 ?CreateObject@?$MBComPtr@VIMBEventManager@@@@QAEJABUMBGUID@@@Z
template class MBComPtr<IMBEventManager>;

// Matched by this code:
// 0x00402cc0 _$E2
// 0x00402cd0 _$E1
// 0x00402da0 _$E5
// 0x00402db0 _$E4
// 0x00402e80 _$E8
// 0x00402e90 _$E7
// 0x00402f60 _$E11
// 0x00402f70 _$E10
// 0x00403030 _$E14
// 0x00403040 _$E13
// 0x00403100 _$E17
// 0x00403110 _$E16
// 0x004031c0 _$E20
// 0x004031d0 _$E19
// 0x00403280 _$E23
// 0x00403290 _$E22
// 0x00403340 _$E26
// 0x00403350 _$E25
// 0x00403410 _$E29
// 0x00403420 _$E28
// 0x004034e0 _$E32
// 0x004034f0 _$E31
// 0x004035a0 _$E35
// 0x004035b0 _$E34
// 0x00403660 _$E38
// 0x00403670 _$E37
// 0x00403720 _$E41
// 0x00403730 _$E40
// 0x004037e0 _$E44
// 0x004037f0 _$E43
// 0x004038a0 _$E47
// 0x004038b0 _$E46
// 0x00403de0 ?Release@?$MBComPtr@VIMBEventManager@@@@QAEXXZ
// 0x004044d0 ?id@?$ctype@G@std@@$E
// 0x00403960 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@ABV12@II@Z
// 0x00403b70 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEX_N@Z
// 0x00403bc0 ?CreateObject@?$MBComPtr@VIMBFileSpec@@@@QAEJABUMBGUID@@@Z
// 0x00403c60 ?Release@?$MBComPtr@VIMBStream@@@@QAEXXZ
// 0x00403c80 ?CreateObject@?$MBComPtr@VIMBPref@@@@QAEJABUMBGUID@@@Z
// 0x00403ea0 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x00403f40 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x00403f60 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAE_NI_N@Z
// 0x00404110 ?max_size@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIXZ
// 0x00404120 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x004041d0 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXXZ
// 0x004042a0 ?_QueryInterface@?$MBComPtr@VIMBWinSoundManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x00404320 ?_QueryInterface@?$MBComPtr@VIMBFPSMonitor@@@@AAEJABQAVIMBUnknown@@@Z
// 0x004043a0 ?_QueryInterface@?$MBComPtr@VIMBGameClock@@@@AAEJABQAVIMBUnknown@@@Z
// 0x00404420 ?_QueryInterface@?$MBComPtr@VIMBEventManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x004044a0 _$E69
// 0x004044c0 _$E68
