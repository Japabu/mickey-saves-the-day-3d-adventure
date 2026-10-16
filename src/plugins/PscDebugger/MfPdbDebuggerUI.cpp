// PscDebugger.dll 0x10005970-0x1000699f: the debugger's user interface thread
// and its window (MfPdbDebuggerFrame), and the loader of the toolbar bitmap.
#include "PscDebugger.h"
#include "PdbOptions.h"

// 0x10016f88
int pdbUIRunning;

// 0x10005970 ?CreateObject@MfPdbDebuggerUI@@SGPAVCObject@@XZ
// 0x100059c0 ??_GMfPdbDebuggerUI@@UAEPAXI@Z
// 0x100059e0 ??1MfPdbDebuggerUI@@UAE@XZ
// 0x100059f0 ?_GetBaseClass@MfPdbDebuggerUI@@KGPAUCRuntimeClass@@XZ
// 0x10005a00 ?GetRuntimeClass@MfPdbDebuggerUI@@UBEPAUCRuntimeClass@@XZ
IMPLEMENT_DYNCREATE(MfPdbDebuggerUI, CWinThread)

// 0x10005a10 ?InitInstance@MfPdbDebuggerUI@@UAEHXZ
BOOL MfPdbDebuggerUI::InitInstance()
{
    m_pMainWnd = new MfPdbDebuggerFrame;
    pdbUIRunning = 1;
    pdbUIReady->Signal();
    return TRUE;
}

// 0x10005a90 ?ExitInstance@MfPdbDebuggerUI@@UAEHXZ
int MfPdbDebuggerUI::ExitInstance()
{
    pdbUIRunning = 0;
    if (m_pMainWnd && m_pMainWnd->GetSafeHwnd()) {
        ((MfPdbDebuggerFrame *)m_pMainWnd)->RemoveTrayIcon();
        m_pMainWnd->DestroyWindow();
        delete m_pMainWnd;
        m_pMainWnd = 0;
    }
    return 0;
}

// 0x10005ae0 ?_GetBaseMessageMap@MfPdbDebuggerFrame@@KGPBUAFX_MSGMAP@@XZ
// 0x10005af0 ?GetMessageMap@MfPdbDebuggerFrame@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbDebuggerFrame, CPiccoloFileEditor)
    ON_WM_CLOSE()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(0x3ff, OnExit)
    ON_COMMAND(0x3fe, OnShow)
    ON_MESSAGE(WM_USER + 1, OnClearOutput)
    ON_MESSAGE(WM_USER + 100, OnTrayNotify)
END_MESSAGE_MAP()

// 0x10005b00 ??0MfPdbDebuggerFrame@@QAE@XZ
MfPdbDebuggerFrame::MfPdbDebuggerFrame()
    : CPiccoloFileEditor(NULL_STRING, "Piccolo Editor"), mTrayIcon(0), mDebugWindow(-1), mDebugEdit(0)
{
    UtilityApp app;
    mOutputHints = app.GetProfileInt(pszDebuggerOptions, pszPaneSize, -1);
    CreateFrameWindow(0);
}

// 0x10005c30 ??_GMfPdbDebuggerFrame@@UAEPAXI@Z

// 0x10005c50 ??1MfPdbDebuggerFrame@@UAE@XZ
MfPdbDebuggerFrame::~MfPdbDebuggerFrame()
{
}

// 0x10005cb0 ?CreateFrameWindow@MfPdbDebuggerFrame@@UAEXPAVCWnd@@@Z
void MfPdbDebuggerFrame::CreateFrameWindow(CWnd *)
{
    PscDebuggerContext context;
    mEditorHints = 0x12;
    LoadFrame(0x68, WS_OVERLAPPEDWINDOW, NULL, NULL);
}

// 0x10005d40 ?Activate@MfPdbDebuggerFrame@@QAEXXZ
void MfPdbDebuggerFrame::Activate()
{
    StartUpdateTimer();
    if (IsIconic())
        ShowWindow(SW_RESTORE);
    else
        ShowWindow(SW_SHOW);
    SetForegroundWindow();
}

// 0x10005d90 ?LoadModule@MfPdbDebuggerFrame@@QAEHHAAVMfString@@PAVMfAiSourceType@@PAVMfScVmEnv@@H@Z
int MfPdbDebuggerFrame::LoadModule(int line, MfString &module, MfAiSourceType *source, MfScVmEnv *env,
                                   int showLine)
{
    mCurrentLine = line;
    CConsoleEdit *editor = EditorWnd();
    if (module != mFileName) {
        MfString name(module);
        MfString text;
        int result = source->LoadSource(module, &text, env);
        if (result != 0 && result != 5)
            return 0;
        if (result == 5) {
            // The source renamed the module: register its functions anew.
            MfScFunctionRegistry *registry = pdbDebugger->mRegistry;
            MfScHandleList functions;
            registry->Find(name, &functions);
            for (MfScDataHandle *h = functions.First(); h; h = functions.Next()) {
                MfScBytecodeRoutine *routine =
                    h ? dynamic_cast<MfScBytecodeRoutine *>(h->GetData()) : 0;
                if (routine) {
                    registry->Remove(h);
                    routine->ModuleName() = module;
                    registry->Add(h);
                }
            }
        }
        SetFileName(module);
        editor->SetEntireContents(text, 0);
    }
    editor->GotoLine(mCurrentLine, 0);
    if (showLine)
        editor->SetGlyphOnLine(mCurrentLine, 2);
    return 1;
}

// 0x10005fb0 ?ClearCurrentLine@MfPdbDebuggerFrame@@QAEXXZ
void MfPdbDebuggerFrame::ClearCurrentLine()
{
    CConsoleEdit *editor = EditorWnd();
    editor->UnsetGlyphOnLine(mCurrentLine, 2);
}

// 0x10005fd0 ?AddTrayIcon@MfPdbDebuggerFrame@@QAEXXZ
void MfPdbDebuggerFrame::AddTrayIcon()
{
    if (mTrayIcon)
        return;
    mTrayIcon = 1;
    HICON icon = (HICON)LoadImage(hPscDebugger, MAKEINTRESOURCE(0x68), IMAGE_ICON, 16, 16, 0);
    NOTIFYICONDATA data;
    data.cbSize = sizeof(data);
    data.hWnd = m_hWnd;
    data.uID = (UINT)m_hWnd;
    data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    data.uCallbackMessage = WM_USER + 100;
    data.hIcon = icon;
    lstrcpy(data.szTip, "Piccolo Debugger");
    Shell_NotifyIcon(NIM_ADD, &data);
}

// 0x10006050 ?RemoveTrayIcon@MfPdbDebuggerFrame@@QAEXXZ
void MfPdbDebuggerFrame::RemoveTrayIcon()
{
    if (mTrayIcon == 1) {
        mTrayIcon = 0;
        NOTIFYICONDATA data;
        data.cbSize = sizeof(data);
        data.hWnd = m_hWnd;
        data.uID = (UINT)m_hWnd;
        data.uFlags = 0;
        data.hIcon = 0;
        Shell_NotifyIcon(NIM_DELETE, &data);
    }
}

// 0x100060a0 ?OnCmdMsg@MfPdbDebuggerFrame@@UAEHIHPAXPAUAFX_CMDHANDLERINFO@@@Z
BOOL MfPdbDebuggerFrame::OnCmdMsg(UINT id, int code, void *extra, AFX_CMDHANDLERINFO *handlerInfo)
{
    if (id >= ID_PDB_GO && id <= 0xb02c)
        return pdbDebugger->OnCmdMsg(id, code, extra, handlerInfo);
    return CFrameWnd::OnCmdMsg(id, code, extra, handlerInfo);
}

// 0x100060f0 ?PreCreateWindow@MfPdbDebuggerFrame@@UAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPdbDebuggerFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    UtilityApp app;
    cs.x = app.GetProfileInt(pszDebuggerOptions, pszX, CW_USEDEFAULT);
    cs.y = app.GetProfileInt(pszDebuggerOptions, pszY, CW_USEDEFAULT);
    cs.cx = app.GetProfileInt(pszDebuggerOptions, pszCX, CW_USEDEFAULT);
    cs.cy = app.GetProfileInt(pszDebuggerOptions, pszCY, CW_USEDEFAULT);
    return CFrameWnd::PreCreateWindow(cs);
}

// 0x100062a0 ?OnClose@MfPdbDebuggerFrame@@QAEXXZ
void MfPdbDebuggerFrame::OnClose()
{
    CConsoleEdit *editor = EditorWnd();
    if (editor->IsModified()) {
        int answer = MessageBox("Text has been modified. Save changes?", "Query", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES)
            SendMessage(WM_COMMAND, ID_PDB_SAVE);
        else if (answer == IDCANCEL)
            return;
    }
    SetFileName(NULL_STRING);
    editor->SetEntireContents(NULL_STRING, 0);
    WINDOWPLACEMENT placement;
    GetWindowPlacement(&placement);
    CRect rect(placement.rcNormalPosition);
    UtilityApp app;
    app.WriteProfileInt(pszDebuggerOptions, pszX, rect.left);
    app.WriteProfileInt(pszDebuggerOptions, pszY, rect.top);
    app.WriteProfileInt(pszDebuggerOptions, pszCX, rect.Width());
    app.WriteProfileInt(pszDebuggerOptions, pszCY, rect.Height());
    app.WriteProfileInt(pszDebuggerOptions, pszPaneSize, mPiccoloWnd->GetPaneSize());
    StopUpdateTimer();
    ShowWindow(SW_HIDE);
    pdbDebugger->Continue();
}

// EQUIV 0x10006580 ?OnCreate@MfPdbDebuggerFrame@@IAEHPAUtagCREATESTRUCTA@@@Z: reviewed: register choice only (edi/ebp swapped: the original keeps output in edi, the new edit in ebp)
int MfPdbDebuggerFrame::OnCreate(LPCREATESTRUCT cs)
{
    if (CPiccoloFileEditor::OnCreate(cs) == -1)
        return -1;
    StopUpdateTimer();
    PscDebuggerContext context;
    if (!mToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP, AFX_IDW_TOOLBAR)
        || !mToolBar.LoadToolBar(0x68))
        return -1;
    mToolBar.SetBarStyle(mToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    CConsoleOutput *output = mPiccoloWnd->mOutput;
    mDebugEdit = new CConsoleEdit;
    mDebugEdit->CreateEx(0, NULL, "Exception Window", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
                         output->GetSafeHwnd(), 0, NULL);
    mDebugEdit->mEditable = 1;
    mDebugEdit->DisableSyntaxHighlight();
    mDebugWindow = output->InstallUserWnd("Exceptions", mDebugEdit);
    SetWindowText("Piccolo Debugger");
    AddTrayIcon();
    return 0;
}

// 0x10006740 ??_GCConsoleEdit@@UAEPAXI@Z

// EQUIV 0x10006760 ?OnSize@MfPdbDebuggerFrame@@IAEXIHH@Z: reviewed: operand order of one integer sum only (the original computes (top - bottom) + cy - 5 with one lea, this one (top + cy) - bottom - 5; tried cy - (Height() + 5), top - bottom + cy - 5, -Height() + cy - 5, a local, cy - 5 - Height()); same value and calls
void MfPdbDebuggerFrame::OnSize(UINT type, int cx, int cy)
{
    CPiccoloFileEditor::OnSize(type, cx, cy);
    CRect rect;
    ::GetClientRect(mToolBar.m_hWnd, &rect);
    mPiccoloWnd->MoveWindow(0, rect.bottom + 5, cx, cy - rect.Height() - 5, TRUE);
}

// 0x100067c0 ?OnClearOutput@MfPdbDebuggerFrame@@IAEJIJ@Z
LRESULT MfPdbDebuggerFrame::OnClearOutput(WPARAM, LPARAM)
{
    OutputClear(0);
    return 0;
}

// 0x100067d0 ?OnTrayNotify@MfPdbDebuggerFrame@@IAEJIJ@Z
LRESULT MfPdbDebuggerFrame::OnTrayNotify(WPARAM, LPARAM message)
{
    switch (message) {
    case WM_LBUTTONDBLCLK:
        Activate();
        break;
    case WM_RBUTTONUP: {
        HMENU menu = LoadMenu(hPscDebugger, MAKEINTRESOURCE(0x69));
        HMENU popup = GetSubMenu(menu, 0);
        POINT point;
        GetCursorPos(&point);
        SetForegroundWindow();
        SetMenuDefaultItem(popup, 0x3fe, FALSE);
        TrackPopupMenu(popup, 0, point.x, point.y, 0, m_hWnd, NULL);
        DestroyMenu(menu);
        break;
    }
    }
    return 0;
}

// 0x10006870 ?OnExit@MfPdbDebuggerFrame@@IAEXXZ
void MfPdbDebuggerFrame::OnExit()
{
    PostMessage(WM_CLOSE);
}

// 0x10006890 ?OnShow@MfPdbDebuggerFrame@@IAEXXZ
void MfPdbDebuggerFrame::OnShow()
{
    Activate();
}

// 0x100068a0 ?GetInterfaceName@MfPdbToolbarBitmapLoader@@UAEXPADHAAH1@Z
void MfPdbToolbarBitmapLoader::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "DebuggerToolbarLoader", size);
    major = 1;
    minor = 0;
}

// 0x100068d0 ?Release@MfPdbToolbarBitmapLoader@@UAEHXZ
int MfPdbToolbarBitmapLoader::Release()
{
    if (--mRefs == 0) {
        delete this;
        return 0;
    }
    return mRefs;
}

// 0x100068f0 ?LoadBitmapA@MfPdbToolbarBitmapLoader@@UAEHPAVCToolBar@@@Z
BOOL MfPdbToolbarBitmapLoader::LoadBitmap(CToolBar *toolbar)
{
    PscDebuggerContext context;
    return toolbar->LoadBitmap((LPCTSTR)(mBitmap & 0xffff));
}

// 0x10006980 ?BeginModalState@CWnd@@UAEXXZ
// 0x10006990 ?EndModalState@CWnd@@UAEXXZ
