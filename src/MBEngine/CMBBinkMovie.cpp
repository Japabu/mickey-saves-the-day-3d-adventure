// CMBBinkMovie.cpp - MBEngine.dll CMBBinkMovie.obj
#include "CMBBinkMovie.h"

// PDB ldata 0x10099818, $E49..$E52
static CMBCriticalSection sCriticalSection;

// 0x10016ca0 ?MakeCMBBinkMovie@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBBinkMovie(void)
{
    IMBUnknown *p = (IMBMovie *)new CMBBinkMovie;
    if (p)
        p->AddRef();
    return p;
}

// EQUIV 0x10016e90 ??0CMBBinkMovie@@QAE@XZ: reviewed: the original calls MBComPtr::CreateObject out of line, VC6 calls Release and inlines the rest; calls and order agree. Was: inline budget: original calls MBComPtr::CreateObject out of line, ours calls Release and inlines the rest (79% similar)
CMBBinkMovie::CMBBinkMovie(void)
    : mIsLoaded(false), mIsPlaying(false), mIsDone(false), mIsLooping(false), mPauseCount(0),
      mDroppedFrames(0), mUseTransparent(false), mID(0), mParent(0), mPos(0, 0, 100), mHideCount(0),
      mDirty(true), mBinkHandle(0), mFlags(0), mBinkFrameDelay(0), mSoundManager(MBIID_IMBSoundManager)
{
    static bool sSoundInit = false;
    sCriticalSection.Enter();
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    if (!sSoundInit) {
        _DIG_DRIVER *digitalDriver;
        _MDI_DRIVER *midiDrvr;
        _DLSDEVICE *dlsDrvr;
        AIL_quick_handles(&digitalDriver, &midiDrvr, &dlsDrvr);
        BinkSetSoundSystem(BinkOpenMiles, (unsigned long)digitalDriver);
        sSoundInit = true;
    }
    _periodicalManager.Release();
    sCriticalSection.Leave();
}

// 0x10017010 ?QueryInterface@CMBBinkMovie@@UAEJABUMBGUID@@PAPAX@Z
long CMBBinkMovie::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBMovie *)this;
    if (iid == MBIID_IMBResource)
        p = (IMBResource *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (iid == MBIID_IMBMovie)
        p = (IMBMovie *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100171b0 ?AddRef@CMBBinkMovie@@UAEKXZ
unsigned long CMBBinkMovie::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100171c0 ?Release@CMBBinkMovie@@UAEKXZ
unsigned long CMBBinkMovie::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100171f0 ?IsPlaying@CMBBinkMovie@@UAE_NXZ
bool CMBBinkMovie::IsPlaying(void)
{
    return mIsPlaying;
}

// 0x10017200 ?Pause@CMBBinkMovie@@UAEJXZ
long CMBBinkMovie::Pause(void)
{
    return ++mPauseCount;
}

// 0x10017210 ?Unpause@CMBBinkMovie@@UAEJXZ
long CMBBinkMovie::Unpause(void)
{
    if (mPauseCount > 0)
        return --mPauseCount;
    return 0;
}

// 0x10017220 ?IsPaused@CMBBinkMovie@@UAE_NXZ
bool CMBBinkMovie::IsPaused(void)
{
    return mPauseCount > 0;
}

// 0x10017230 ?SetLoop@CMBBinkMovie@@UAEX_N@Z
void CMBBinkMovie::SetLoop(bool loop)
{
    mIsLooping = loop;
}

// 0x10017240 ?IsLooping@CMBBinkMovie@@UAE_NXZ
bool CMBBinkMovie::IsLooping(void)
{
    return mIsLooping;
}

// 0x10017250 ?SetPalette@CMBBinkMovie@@UAEXPAVMBPalette@@@Z
void CMBBinkMovie::SetPalette(MBPalette *pal)
{
}

// 0x10017260 ?GetPalette@CMBBinkMovie@@UAEPAVMBPalette@@XZ
MBPalette *CMBBinkMovie::GetPalette(void)
{
    return 0;
}

// 0x10017270 ?GetOffset@CMBBinkMovie@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBBinkMovie::GetOffset(void) const
{
    return mPos;
}

// 0x10017290 ?GetPosition@CMBBinkMovie@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBBinkMovie::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x10017300 ?Place@CMBBinkMovie@@UAEXABVMBPoint3@@@Z
void CMBBinkMovie::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10017340 ?Move@CMBBinkMovie@@UAEXABVMBPoint3@@@Z
void CMBBinkMovie::Move(const MBPoint3 &point)
{
    mPos += point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10017390 ?IsVisible@CMBBinkMovie@@UBE_NXZ
bool CMBBinkMovie::IsVisible(void) const
{
    return mHideCount == 0 && mIsPlaying;
}

// 0x100173b0 ?IsDirty@CMBBinkMovie@@UBE_NXZ
bool CMBBinkMovie::IsDirty(void) const
{
    return mDirty;
}

// 0x100173c0 ?SetDirty@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::SetDirty(void)
{
    mDirty = true;
}

// 0x100173d0 ?ClearDirty@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::ClearDirty(void)
{
    mDirty = false;
}

// 0x100173e0 ?GetID@CMBBinkMovie@@UAEKXZ
unsigned long CMBBinkMovie::GetID(void)
{
    return mID;
}

// 0x100173f0 ?GetType@CMBBinkMovie@@UAEKXZ
unsigned long CMBBinkMovie::GetType(void)
{
    return 'bik ';
}

// EQUIV 0x10017680 ??1CMBBinkMovie@@IAE@XZ: reviewed: the original calls MBComPtr::CreateObject out of line, VC6 inlines it (frame 4 bytes bigger); calls and order agree. Was: inline budget: original calls MBComPtr::CreateObject out of line, ours inlines it (+4 frame) (66% similar)
CMBBinkMovie::~CMBBinkMovie(void)
{
    sCriticalSection.Enter();
    {
        MBComPtr<IMBEventManager> broadcaster;
        broadcaster.CreateObject(MBIID_IMBEventManager);
        MBMovieEventStruct event;
        event.movie = this;
        event.id = mID;
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 1, &event);
    }
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    FreeBuffers();
    if (mBinkHandle)
        BinkClose(mBinkHandle);
    _periodicalManager.Release();
    sCriticalSection.Leave();
}

// EQUIV 0x10017b90 ?InitializeBuffers@CMBBinkMovie@@IAE_NXZ: reviewed: register choices (the original keeps more values in ebx/ebp, which it saves) and the duplicated epilogues; the calls and stores agree. Was: register allocation: original saves ebx/ebp and keeps more values in registers (60% similar)
bool CMBBinkMovie::InitializeBuffers(void)
{
    IMBScreenManager *tempScreenManager;
    IMBScreen *tempScreen;
    IMBSurface *tempSurface;
    MBGetScreenManager(&tempScreenManager);
    tempScreenManager->GetScreenByIndex(0, &tempScreen);
    if (mFlags & 2)
        tempScreen->CreateSurface(mBinkHandle->Width * 2, mBinkHandle->Height * 2, 16, &tempSurface);
    else
        tempScreen->CreateSurface(mBinkHandle->Width, mBinkHandle->Height, 16, &tempSurface);
    mIntermediateSurface.Attach(tempSurface);
    if (mIntermediateSurface) {
        if (tempScreenManager)
            tempScreenManager->Release();
        if (tempScreen)
            tempScreen->Release();
        return true;
    }
    if (tempScreenManager)
        tempScreenManager->Release();
    if (tempScreen)
        tempScreen->Release();
    return false;
}

// 0x100180d0 ?FreeBuffers@CMBBinkMovie@@IAEXXZ
void CMBBinkMovie::FreeBuffers(void)
{
    mIntermediateSurface.Release();
}

// 0x100180f0 ?IsTimeForNextFrame@CMBBinkMovie@@IAE_NXZ
bool CMBBinkMovie::IsTimeForNextFrame(void)
{
    return mBinkHandle && !BinkWait(mBinkHandle);
}

// 0x10018110 ?PrepareNextFrame@CMBBinkMovie@@IAE_NXZ
bool CMBBinkMovie::PrepareNextFrame(void)
{
    if (mIsDone || !mIsPlaying)
        return false;
    if (CurrentFrameNum() == NumTotalFrames() && !mIsLooping) {
        Stop();
        return false;
    }
    BinkNextFrame(mBinkHandle);
    if (mIsLooping && CurrentFrameNum() == 1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        MBMovieEventStruct event;
        event.movie = this;
        event.id = GetID();
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 4, &event);
    }
    return true;
}

// EQUIV 0x100181e0 ?RenderCurrentFrame@CMBBinkMovie@@IAE_NXZ: reviewed: the original keeps the zero in ebp and materialises the frame comparison as a bool (sete/test), returning PrepareNextFrame's false from al; VC6 keeps the zero in a slot and branches on the compare; calls, loop and results agree (as CMBSmackerMovie::RenderCurrentFrame). Was: register allocation: original keeps the zero in ebp, ours in a stack slot (71% similar)
bool CMBBinkMovie::RenderCurrentFrame(void)
{
    void *bits = 0;
    MBPixelFormat pf = mIntermediateSurface->GetFormat();
    while (BinkDoFrame(mBinkHandle) == 1) {
        mDroppedFrames++;
        if (!PrepareNextFrame())
            return false;
    }
    mIntermediateSurface->Lock(&bits, 2);
    unsigned long copyFlags = 0;
    if (pf.mBitsPerPixel == 16) {
        if (MBColorMask::GetBitCount(pf.mColorMask.mGreenMask) == 6)
            copyFlags = 4;
        if (MBColorMask::GetBitCount(pf.mColorMask.mGreenMask) == 5)
            copyFlags |= 3;
    } else if (pf.mBitsPerPixel == 32) {
        copyFlags = 2;
    }
    if (mFlags & 2)
        copyFlags |= 0x40000000;
    BinkCopyToBuffer(mBinkHandle, bits, mIntermediateSurface->GetPitch(),
                     mIntermediateSurface->GetHeight(), 0, 0, copyFlags);
    mIntermediateSurface->Unlock(&bits);
    return true;
}

// EQUIV 0x100182f0 ?Load@CMBBinkMovie@@UAE_NKPAVIMBStream@@K@Z: reviewed: the original calls the MBComPtr Release copies out of line on the failure path and shares the screen/surface release tail; VC6 inlines and duplicates them, with one more stack slot; the aligned call and string sequences agree. Was: ours has one more stack slot (+4 frame); otherwise the same code (50% similar)
bool CMBBinkMovie::Load(unsigned long id, IMBStream *in, unsigned long flags)
{
    MBComPtr<IMBSurface> surface;
    MBComPtr<IMBScreen> screen;
    if (!in)
        return false;
    mFlags = flags;
    mStream = in;
    mID = id;
    IMBScreenManager *tempScreenManager;
    MBGetScreenManager(&tempScreenManager);
    MBComPtr<IMBScreenManager> screenManager;
    screenManager.Attach(tempScreenManager);
    IMBScreen *tempScreen;
    screenManager->GetScreenByIndex(0, &tempScreen);
    screen.Attach(tempScreen);
    IMBSurface *tempSurface;
    screen->GetBackBuffer(&tempSurface);
    surface.Attach(tempSurface);
    unsigned long binkFlags = 0x880000;
    if (mFlags & 1)
        binkFlags = 0x882000;
    MBComPtr<IMBWinFile> winfile(in);
    mBinkHandle = BinkOpen((const char *)winfile->GetFileHandle(), binkFlags);
    if (!mBinkHandle) {
        MBDebugStr("ERROR: Failed to open bink movie (id: %d) (err: %s)\n", id, BinkGetError());
        return false;
    }
    mIsLoaded = true;
    if (!InitializeBuffers()) {
        MBDebugStr("ERROR: Unable to allocate buffer memory for Bink movie.\n");
        mIsLoaded = false;
        return false;
    }
    mBinkFrameDelay = 1000 / (mBinkHandle->FrameRate / mBinkHandle->FrameRate);
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    MBMovieEventStruct event;
    event.movie = this;
    event.id = GetID();
    if (broadcaster)
        broadcaster->BroadcastEvent(-5, 0, &event);
    return true;
}

// 0x10018540 ?Rewind@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::Rewind(void)
{
    sCriticalSection.Enter();
    if (mBinkHandle) {
        BinkGoto(mBinkHandle, 1, 0);
        mIsDone = false;
    }
    sCriticalSection.Leave();
}

// 0x10018580 ?Play@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::Play(void)
{
    sCriticalSection.Enter();
    if (mIsLoaded) {
        mIsDone = false;
        mIsPlaying = true;
        mPauseCount = 0;
        {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            MBMovieEventStruct event;
            event.movie = this;
            event.id = GetID();
            if (broadcaster)
                broadcaster->BroadcastEvent(-5, 2, &event);
        }
        RenderCurrentFrame();
        PrepareNextFrame();
        SetDirty();
    }
    sCriticalSection.Leave();
}

// 0x10018650 ?Stop@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::Stop(void)
{
    sCriticalSection.Enter();
    if (mIsPlaying) {
        mIsPlaying = false;
        mIsDone = true;
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        MBMovieEventStruct event;
        event.movie = this;
        event.id = GetID();
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 3, &event);
    }
    sCriticalSection.Leave();
}

// 0x100186f0 ?CurrentFrameNum@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::CurrentFrameNum(void) const
{
    if (mIsLoaded && mBinkHandle)
        return mBinkHandle->FrameNum;
    return 0;
}

// 0x10018710 ?NumTotalFrames@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::NumTotalFrames(void) const
{
    if (mIsLoaded && mBinkHandle)
        return mBinkHandle->Frames;
    return 0;
}

// 0x10018730 ?FrameDelay@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::FrameDelay(void) const
{
    if (mIsLoaded && mBinkHandle)
        return mBinkFrameDelay;
    return 0;
}

// 0x10018750 ?Width@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::Width(void) const
{
    if (mIsLoaded && mBinkHandle) {
        if (mFlags & 2)
            return mBinkHandle->Width * 2;
        return mBinkHandle->Width;
    }
    return 0;
}

// 0x10018770 ?Height@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::Height(void) const
{
    if (mIsLoaded && mBinkHandle) {
        if (mFlags & 2)
            return mBinkHandle->Height * 2;
        return mBinkHandle->Height;
    }
    return 0;
}

// 0x10018790 ?Depth@CMBBinkMovie@@UBEJXZ
long CMBBinkMovie::Depth(void) const
{
    return 8;
}

// 0x100187a0 ?SetTransparentColor@CMBBinkMovie@@UAEXVMBColor@@@Z
void CMBBinkMovie::SetTransparentColor(MBColor color)
{
    if (mIntermediateSurface)
        mIntermediateSurface->SetTransparentColor(color);
}

// 0x100187c0 ?GetTransparentColor@CMBBinkMovie@@UAE?AVMBColor@@XZ
MBColor CMBBinkMovie::GetTransparentColor(void)
{
    if (mIntermediateSurface)
        return mIntermediateSurface->GetTransparentColor();
    return MBColor(255, 0, 255, 0);
}

// 0x10018890 ?GetBounds@CMBBinkMovie@@UBE?AVMBRect@@XZ
MBRect CMBBinkMovie::GetBounds(void) const
{
    return MBRect(GetPosition(), MBSize(Width(), Height()));
}

// 0x100188f0 ?Show@CMBBinkMovie@@UAEJXZ
long CMBBinkMovie::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x10018920 ?Hide@CMBBinkMovie@@UAEJXZ
long CMBBinkMovie::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x10018940 ?Draw@CMBBinkMovie@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBBinkMovie::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    if (mIsPlaying && mIntermediateSurface && IsVisible())
        return mIntermediateSurface->Blit(dest, &srcRect, &destRect, 0);
    return 0;
}

// 0x10018980 ?GetParent@CMBBinkMovie@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBBinkMovie::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100189a0 ?SetParent@CMBBinkMovie@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBBinkMovie::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x100189e0 ?Idle@CMBBinkMovie@@UAEXXZ
void CMBBinkMovie::Idle(void)
{
    sCriticalSection.Enter();
    if (mIsPlaying && !IsPaused() && IsTimeForNextFrame()) {
        RenderCurrentFrame();
        PrepareNextFrame();
    }
    sCriticalSection.Leave();
}


// Matched by this code:
// 0x10008ac0 ?_QueryInterface@?$MBComPtr@VIMBPeriodicalManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10016c60 _$E52
// 0x10016c70 _$E49
// 0x10016c80 _$E51
// 0x10016c90 _$E50
// 0x10018a30 ?Release@?$MBComPtr@VIMBSurface@@@@QAEXXZ
// 0x10018a50 ?AddRef@?$MBComPtr@VIMBStream@@@@QAEXXZ
// 0x10018a60 ?Release@?$MBComPtr@VIMBSoundManager@@@@QAEXXZ
// 0x10018aa0 ?Release@?$MBComPtr@VIMBScreenManager@@@@QAEXXZ
// 0x10018ac0 ?_QueryInterface@?$MBComPtr@VIMBSoundManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10018b40 ?_QueryInterface@?$MBComPtr@VIMBWinFile@@@@AAEJABQAVIMBStream@@@Z
// 0x10018bc0 ?QueryInterface@CMBBinkMovie@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10018bd0 ?AddRef@CMBBinkMovie@@W3AEKXZ
// 0x10018be0 ?Release@CMBBinkMovie@@W3AEKXZ
// 0x10018bf0 ?QueryInterface@CMBBinkMovie@@W7AEJABUMBGUID@@PAPAX@Z
// 0x10018c00 ?AddRef@CMBBinkMovie@@W7AEKXZ
// 0x10018c10 ?Release@CMBBinkMovie@@W7AEKXZ
// 0x10018c20 ?QueryInterface@CMBBinkMovie@@WM@AEJABUMBGUID@@PAPAX@Z
// 0x10018c30 ?AddRef@CMBBinkMovie@@WM@AEKXZ
// 0x10018c40 ?Release@CMBBinkMovie@@WM@AEKXZ
