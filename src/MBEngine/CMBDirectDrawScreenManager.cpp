// CMBDirectDrawScreenManager.cpp - MBEngine.dll CMBDirectDrawScreenManager.obj
#include "CMBDirectDrawScreenManager.h"

// 0x10056140 ?MakeCMBDirectDrawScreenManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBDirectDrawScreenManager(void)
{
    IMBUnknown *p = new CMBDirectDrawScreenManager;
    if (p)
        p->AddRef();
    return p;
}

// 0x10056170 ??0CMBDirectDrawScreenManager@@QAE@XZ
CMBDirectDrawScreenManager::CMBDirectDrawScreenManager(void)
    : mWindow(0), mIsFullScreen(false), mUseDirect3D(false), mDDrawEnumeratorFunc(0)
{
    _mRefCnt = 0;
}

// 0x100561a0 ?QueryInterface@CMBDirectDrawScreenManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBDirectDrawScreenManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBScreenManager)
        p = this;
    if (iid == MBIID_IMBDirectDrawScreenManager)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100562a0 ?AddRef@CMBDirectDrawScreenManager@@UAEKXZ
unsigned long CMBDirectDrawScreenManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100562b0 ?Release@CMBDirectDrawScreenManager@@UAEKXZ
unsigned long CMBDirectDrawScreenManager::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100562e0 ??1CMBDirectDrawScreenManager@@QAE@XZ
CMBDirectDrawScreenManager::~CMBDirectDrawScreenManager(void)
{
}

// 0x10056320 ?SetDirectDrawEnumerator@CMBDirectDrawScreenManager@@UAEXP6GHPAU_GUID@@PAD1PAXPAUHMONITOR__@@@Z@Z
void CMBDirectDrawScreenManager::SetDirectDrawEnumerator(MBDDEnumCallback func)
{
    mDDrawEnumeratorFunc = func;
}

// 0x10056330 ?EnableDirect3D@CMBDirectDrawScreenManager@@UAE_N_N@Z
bool CMBDirectDrawScreenManager::EnableDirect3D(bool on)
{
    mUseDirect3D = on;
    return true;
}

// 0x10056340 ?EnterWindowedMode@CMBDirectDrawScreenManager@@UAEXXZ
void CMBDirectDrawScreenManager::EnterWindowedMode(void)
{
    mIsFullScreen = false;
}

// 0x10056350 ?EnterFullScreenMode@CMBDirectDrawScreenManager@@UAEXXZ
void CMBDirectDrawScreenManager::EnterFullScreenMode(void)
{
    mIsFullScreen = true;
}

// 0x10056360 ?SetHWND@CMBDirectDrawScreenManager@@UAEXPAUHWND__@@@Z
void CMBDirectDrawScreenManager::SetHWND(HWND__ *hWnd)
{
    mWindow = hWnd;
}

// 0x10056370 ?RequestScreen@CMBDirectDrawScreenManager@@UAE_NJAAUMBScreenAttributes@@@Z
bool CMBDirectDrawScreenManager::RequestScreen(long screenID, MBScreenAttributes &attributes)
{
    MBGUID guid;
    if (mUseDirect3D)
        guid = MBIID_CMBDirect3DScreen;
    else
        guid = MBIID_CMBDirectDrawScreen;
    MBComPtr<IMBDirectXScreen> screen(guid);

    unsigned short width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + attributes.width;
    unsigned short height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + attributes.height;
    SetWindowPos(mWindow, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    screen->SetHWND(mWindow);
    screen->SetDirectDrawEnumerator(mDDrawEnumeratorFunc);
    if (mIsFullScreen)
        screen->EnterFullscreenMode(attributes.width, attributes.height, attributes.depth);
    else
        screen->EnterWindowedMode(attributes.width, attributes.height);
    AddScreen(screen, screenID);
    return true;
}

// 0x100564d0 ?Activate@CMBDirectDrawScreenManager@@UAEXXZ
void CMBDirectDrawScreenManager::Activate(void)
{
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it)
        it->screen->SetScreenState(kScreenStateActive);
}

// 0x10056500 ?Deactivate@CMBDirectDrawScreenManager@@UAEXXZ
void CMBDirectDrawScreenManager::Deactivate(void)
{
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it)
        it->screen->SetScreenState(kScreenStateInactive);
}

// 0x10056530 ?GetGlobalMouseLocation@CMBDirectDrawScreenManager@@UAE?AVMBPoint@@XZ
MBPoint CMBDirectDrawScreenManager::GetGlobalMouseLocation(void)
{
    POINT mouseLoc;
    GetCursorPos(&mouseLoc);
    return MBPoint(mouseLoc.x, mouseLoc.y);
}

// 0x10056560 ?ProcessSystemEvent@CMBDirectDrawScreenManager@@UAE_NPAX@Z
bool CMBDirectDrawScreenManager::ProcessSystemEvent(void *event)
{
    return false;
}

// 0x10056570 ?RemoveScreen@CMBDirectDrawScreenManager@@UAEXJ@Z
void CMBDirectDrawScreenManager::RemoveScreen(long screenID)
{
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it) {
        if (it->screenID == screenID) {
            it->screen->Release();
            mScreens.erase(it);
            return;
        }
    }
}

// 0x100565d0 ?RemoveAllScreens@CMBDirectDrawScreenManager@@UAEXXZ
void CMBDirectDrawScreenManager::RemoveAllScreens(void)
{
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it)
        it->screen->Release();
    mScreens.clear();
}

// 0x10056630 ?GetNumScreens@CMBDirectDrawScreenManager@@UAEGXZ
unsigned short CMBDirectDrawScreenManager::GetNumScreens(void)
{
    return mScreens.size();
}

// 0x10056650 ?GetScreenByIndex@CMBDirectDrawScreenManager@@UAEXGPAPAVIMBScreen@@@Z
void CMBDirectDrawScreenManager::GetScreenByIndex(unsigned short index, IMBScreen **screen)
{
    *screen = 0;
    if (index < GetNumScreens()) {
        *screen = mScreens[index].screen;
        (*screen)->AddRef();
    }
}

// 0x10056690 ?GetScreenByID@CMBDirectDrawScreenManager@@UAEXJPAPAVIMBScreen@@@Z
void CMBDirectDrawScreenManager::GetScreenByID(long screenID, IMBScreen **screen)
{
    *screen = 0;
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it) {
        if (it->screenID == screenID) {
            *screen = it->screen;
            (*screen)->AddRef();
            return;
        }
    }
}

// 0x100566d0 ?FindScreenContainingPoint@CMBDirectDrawScreenManager@@UAEXABVMBPoint@@PAPAVIMBScreen@@@Z
void CMBDirectDrawScreenManager::FindScreenContainingPoint(const MBPoint &location, IMBScreen **screen)
{
    *screen = 0;
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it) {
        MBRect r = it->screen->GetGlobalBounds();
        if (r.Contains(location)) {
            *screen = it->screen;
            (*screen)->AddRef();
            return;
        }
    }
}

// 0x10056750 ?AddScreen@CMBDirectDrawScreenManager@@IAEXPAVIMBWinScreen@@J@Z
void CMBDirectDrawScreenManager::AddScreen(IMBWinScreen *screen, long screenID)
{
    for (MBVector<ScreenData>::iterator it = mScreens.begin(); it != mScreens.end(); ++it) {
        if (it->screenID == screenID) {
            it->screen->Release();
            it->screen = screen;
            screen->AddRef();
            return;
        }
    }
    ScreenData data;
    data.screen = screen;
    data.screenID = screenID;
    screen->AddRef();
    mScreens.push_back(data);
}


// Matched by this code:
// 0x10056960 _$E2
// 0x10056970 _$E1
// 0x10056a40 _$E5
// 0x10056a50 _$E4
// 0x10056b20 _$E8
// 0x10056b30 _$E7
// 0x10056c00 _$E11
// 0x10056c10 _$E10
// 0x10056cd0 _$E14
// 0x10056ce0 _$E13
// 0x10056da0 _$E17
// 0x10056db0 _$E16
// 0x10056e60 _$E20
// 0x10056e70 _$E19
// 0x10056f20 _$E23
// 0x10056f30 _$E22
// 0x10056fe0 _$E26
// 0x10056ff0 _$E25
// 0x100570b0 _$E29
// 0x100570c0 _$E28
// 0x10057180 _$E32
// 0x10057190 _$E31
// 0x10057240 _$E35
// 0x10057250 _$E34
// 0x10057300 _$E38
// 0x10057310 _$E37
// 0x100573c0 _$E41
// 0x100573d0 _$E40
// 0x10057480 _$E44
// 0x10057490 _$E43
// 0x10057540 _$E47
// 0x10057550 _$E46
// 0x10057600 ?_Destroy@?$vector@UScreenData@@V?$MBAllocator@UScreenData@@@@@std@@IAEXPAUScreenData@@0@Z
// 0x10057610 ?_Ucopy@?$vector@UScreenData@@V?$MBAllocator@UScreenData@@@@@std@@IAEPAUScreenData@@PBU3@0PAU3@@Z
// 0x10057650 ?_Ufill@?$vector@UScreenData@@V?$MBAllocator@UScreenData@@@@@std@@IAEXPAUScreenData@@IABU3@@Z
// 0x10057680 ?_QueryInterface@?$MBComPtr@VIMBDirectXScreen@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10057700 ?_Construct@std@@YAXPAUScreenData@@ABU2@@Z
// 0x10057720 _$E67
// 0x10057740 _$E66
