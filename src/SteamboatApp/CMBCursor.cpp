#include "d3d7.h"
#include "CMBCursor.h"
#include "MBInterfaces.h"
#include "MBRuntime.h"
#include "mss.h"
#include <process.h>

// Listens to the screen's flip events (channel -3): the cursor thread pauses while a flip is
// under way and draws the cursor again once it is done.
class FlipWatcher : public IMBEventListener {
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    FlipWatcher()
    {
        _mRefCnt = 0;
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster)
            broadcaster->AddListener(-3, this);
    }
    ~FlipWatcher();
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
};

static long sHideCount;
static void *sMutex;
static void *sThread;
static MBComPtr<IMBDirectXScreen> sDXScreen;
static MBComPtr<IMBAnim> sAnim;
static MBComPtr<IMBDirectDrawSurface> sScratchBuffer;
static MBRect sScreenRect;
static int sCursorWidth;
static int sCursorHeight;
static MBRect sCursorRect;
static MBPoint sScreenOrigin;
static MBComPtr<FlipWatcher> flipWatcher;
static bool sStayAlive = true;
static bool sPaused;

// 0x10013580 ?EraseLastCursorRect@@YAXXZ
void EraseLastCursorRect()
{
    RECT srcRect = { 0, 0, sCursorRect.Width(), sCursorRect.Height() };
    RECT destRect = { sCursorRect.left, sCursorRect.top, sCursorRect.right, sCursorRect.bottom };
    OffsetRect(&destRect, sScreenOrigin.x, sScreenOrigin.y);
    IDirectDrawSurface7 *screenSurface = sDXScreen->GetFrontBuffer();
    IDirectDrawSurface7 *scratchSurface;
    sScratchBuffer->GetSurface(&scratchSurface);
    screenSurface->Blt(&destRect, scratchSurface, &srcRect, 0, 0);
    screenSurface->Release();
    scratchSurface->Release();
}

// 0x10013660 ?SaveCurrentCursorRect@@YAXXZ
void SaveCurrentCursorRect()
{
    RECT destRect = { 0, 0, sCursorRect.Width(), sCursorRect.Height() };
    RECT srcRect = { sCursorRect.left, sCursorRect.top, sCursorRect.right, sCursorRect.bottom };
    OffsetRect(&srcRect, sScreenOrigin.x, sScreenOrigin.y);
    IDirectDrawSurface7 *screenSurface = sDXScreen->GetFrontBuffer();
    IDirectDrawSurface7 *scratchSurface;
    sScratchBuffer->GetSurface(&scratchSurface);
    scratchSurface->Blt(&destRect, screenSurface, &srcRect, 0, 0);
    screenSurface->Release();
    scratchSurface->Release();
}

// 0x10013740 ?DrawCursor@@YAXXZ
void DrawCursor()
{
    SaveCurrentCursorRect();
    RECT srcRect = { 0, 0, sCursorRect.Width(), sCursorRect.Height() };
    RECT destRect = { sCursorRect.left, sCursorRect.top, sCursorRect.right, sCursorRect.bottom };
    OffsetRect(&destRect, sScreenOrigin.x, sScreenOrigin.y);
    IDirectDrawSurface7 *screenSurface = sDXScreen->GetFrontBuffer();
    IMBSurface *cursorSurf = 0;
    sAnim->GetCastMemberSurface(sAnim->GetCastID(sAnim->GetCurrentSequenceIndex(), sAnim->GetCurrentFrameIndex()),
                                &cursorSurf);
    MBComPtr<IMBDirectDrawSurface> dxCursorSurface(cursorSurf);
    IDirectDrawSurface7 *cursorSurface;
    dxCursorSurface->GetSurface(&cursorSurface);
    screenSurface->Blt(&destRect, cursorSurface, &srcRect, DDBLT_KEYSRC, 0);
    screenSurface->Release();
    cursorSurface->Release();
    cursorSurf->Release();
}

// 0x10013940 ?InBackground@@YA_NXZ
bool InBackground()
{
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (soundManager == 0)
        return false;
    _DIG_DRIVER *digitalDriver;
    _MDI_DRIVER *midiDrvr;
    _DLSDEVICE *dlsDrvr;
    AIL_quick_handles(&digitalDriver, &midiDrvr, &dlsDrvr);
    if (soundManager->GetMasterVolume() == AIL_digital_master_volume(digitalDriver))
        return true;
    return false;
}

// 0x10013a60 ?CursorThread@@YAXPAX@Z
void CursorThread(void *param)
{
    MBDebugStr("INFO: CMBCursor background thread (0x%X) started.\n", GetCurrentThreadId());
    while (sStayAlive) {
        WaitForSingleObject(sMutex, INFINITE);
        if (!sStayAlive)
            break;
        if (sHideCount == 0 && sAnim != 0) {
            sDXScreen->GetDirectDraw7()->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
            IMBScreen *screen;
            MBPoint localPos;
            if (MBGetScreenMouseLocation(localPos, &screen)) {
                sScreenOrigin = screen->GetGlobalBounds().TopLeft();
                EraseLastCursorRect();
                if (localPos.x + sCursorWidth > sScreenRect.Right())
                    localPos.x = sScreenRect.Right() - sCursorWidth;
                if (localPos.y + sCursorHeight > sScreenRect.Bottom())
                    localPos.y = sScreenRect.Bottom() - sCursorHeight;
                sCursorRect.SetRect(localPos.x, localPos.y, localPos.x + sCursorWidth, localPos.y + sCursorHeight);
                DrawCursor();
            }
            if (screen)
                screen->Release();
        }
        ReleaseMutex(sMutex);
        Sleep(0);
    }
    _endthread();
}

// 0x10013c10 ?Initialize@CMBCursor@@SA_NXZ
bool CMBCursor::Initialize()
{
    if (!sThread) {
        MBComPtr<IMBScreen> screen;
        IMBScreenManager *screenManager;
        MBGetScreenManager(&screenManager);
        IMBScreen *tempScreen;
        screenManager->GetScreenByIndex(0, &tempScreen);
        screen.Attach(tempScreen);
        screenManager->Release();
        sDXScreen = screen;
        if (sDXScreen == 0) {
            MBDebugStr("ERROR: unable to get IMBDirectXScreen interface from main screen.\n");
            MBDebugStr("       Cursor initialization failed.\n");
            return false;
        }
        flipWatcher = new FlipWatcher;
        sCursorRect.left = 0;
        sCursorRect.top = 0;
        sCursorRect.right = 1;
        sCursorRect.bottom = 1;
        sScreenRect.left = 0;
        sScreenRect.top = 0;
        sScreenRect.right = screen->GetWidth() - 1;
        sScreenRect.bottom = screen->GetHeight() - 1;
        sMutex = CreateMutexA(0, FALSE, 0);
        if (!sMutex) {
            MBDebugStr("CMBCursor::Initialize: failed to create mutex.\n");
            return false;
        }
        if (InBackground())
            Pause();
        sThread = (void *)_beginthread(CursorThread, 0, 0);
        if (!sThread) {
            MBDebugStr("CMBCursor::Initialize: failed to create thread.\n");
            return false;
        }
        sStayAlive = true;
    }
    return true;
}

// 0x10013f30 ?QueryInterface@FlipWatcher@@UAEJABUMBGUID@@PAPAX@Z
long FlipWatcher::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10014040 ?AddRef@FlipWatcher@@UAEKXZ
unsigned long FlipWatcher::AddRef()
{
    return ++_mRefCnt;
}

// 0x10014060 ?Release@FlipWatcher@@UAEKXZ
unsigned long FlipWatcher::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100140a0 ??1FlipWatcher@@QAE@XZ
FlipWatcher::~FlipWatcher()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-3, this);
}

// 0x10014170 ?ListenToEvent@FlipWatcher@@UAEXJJPAX@Z
void FlipWatcher::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -3) {
        switch (event) {
        case 1:
            CMBCursor::Pause();
            break;
        case 2:
            if (sHideCount == 0 && sAnim != 0)
                DrawCursor();
            CMBCursor::Resume();
            break;
        }
    }
}

// 0x100141c0 ?Terminate@CMBCursor@@SAXXZ
void CMBCursor::Terminate()
{
    if (sThread) {
        flipWatcher = 0;
        Pause();
        SetCursor(0);
        sStayAlive = false;
        Resume();
        WaitForSingleObject(sThread, INFINITE);
        sThread = 0;
        CloseHandle(sMutex);
        sMutex = 0;
    }
}

// 0x10014250 ?SetCursor@CMBCursor@@SAXPAVIMBAnim@@@Z
void CMBCursor::SetCursor(IMBAnim *anim)
{
    if (Initialize()) {
        Pause();
        sAnim = anim;
        sScratchBuffer = 0;
        if (sAnim) {
            const MBRect &bounds = sAnim->GetBounds();
            sCursorWidth = bounds.Width();
            sCursorHeight = bounds.Height();
            MBComPtr<IMBScreen> tempScreen(sDXScreen);
            IMBSurface *tempSurf;
            tempScreen->CreateSurface(sCursorWidth, sCursorHeight, 0x44, &tempSurf);
            sScratchBuffer = tempSurf;
            tempSurf->Release();
            if (sScratchBuffer == 0) {
                MBDebugStr("ERROR: couldn't get scratch buffer for cursor!\n");
                sAnim = 0;
            }
        }
        Resume();
    }
}

// 0x10014480 ?GetHideCount@CMBCursor@@SAJXZ
long __cdecl CMBCursor::GetHideCount()
{
    return sHideCount;
}

// 0x100144a0 ?SetHideCount@CMBCursor@@SAXJ@Z
void CMBCursor::SetHideCount(long newCount)
{
    Pause();
    sHideCount = newCount;
    Resume();
}

// 0x100144d0 ?Pause@CMBCursor@@SAXXZ
void CMBCursor::Pause()
{
    if (sMutex && !sPaused) {
        WaitForSingleObject(sMutex, INFINITE);
        sPaused = true;
    }
}

// 0x10014510 ?Resume@CMBCursor@@SAXXZ
void CMBCursor::Resume()
{
    if (sMutex && sPaused) {
        sPaused = false;
        ReleaseMutex(sMutex);
    }
}

// 0x10014550 ?IsPaused@CMBCursor@@SA_NXZ
bool CMBCursor::IsPaused()
{
    return sPaused;
}

// Matched by this code:
// 0x100145a0 ??1?$MBComPtr@VIMBDirectDrawSurface@@@@QAE@XZ
// 0x10014600 ??4?$MBComPtr@VIMBDirectDrawSurface@@@@QAEAAV0@PAVIMBDirectDrawSurface@@@Z
// 0x10014640 ?Release@?$MBComPtr@VIMBDirectDrawSurface@@@@QAEXXZ
// 0x100147b0 ?_QueryInterface@?$MBComPtr@VIMBDirectXScreen@@@@AAEJABV?$MBComPtr@VIMBScreen@@@@@Z
// 0x100148d0 ??4?$MBComPtr@VIMBScreen@@@@QAEAAV0@PAVIMBScreen@@@Z

// Not claimed (static initializers and header instances of this object):
// 0x10013260 _$E17
// 0x10013280 _$E14
// 0x100132a0 _$E16
// 0x100132c0 _$E15
// 0x100132f0 _$E22
// 0x10013310 _$E19
// 0x10013330 _$E21
// 0x10013350 _$E20
// 0x10013380 _$E27
// 0x100133a0 _$E24
// 0x100133c0 _$E26
// 0x100133e0 _$E25
// 0x10013410 _$E30
// 0x10013430 _$E29
// 0x10013460 _$E33
// 0x10013480 _$E32
// 0x100134b0 _$E36
// 0x100134d0 _$E35
// 0x100134f0 _$E41
// 0x10013510 _$E38
// 0x10013530 _$E40
// 0x10013550 _$E39
// TODO 0x10014850 $E326: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E319); its $E number depends on the original header set
// 0x10014890 _$E48
// TODO 0x100148b0 $E322: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
