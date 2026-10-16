// CMBSmackerMovie.cpp - MBEngine.dll CMBSmackerMovie.obj
#include "CMBSmackerMovie.h"

// 0x100193a0 ?MakeCMBSmackerMovie@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBSmackerMovie(void)
{
    IMBUnknown *p = (IMBMovie *)new CMBSmackerMovie;
    if (p)
        p->AddRef();
    return p;
}

// 0x100194a0 ??0CMBSmackerMovie@@QAE@XZ
CMBSmackerMovie::CMBSmackerMovie(void)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    mID = 0;
    mParent = 0;
    mPos = MBPoint3(0, 0, 100);
    mHideCount = 0;
    mDirty = true;
    mIsLoaded = false;
    mIsPlaying = false;
    mIsDone = false;
    mIsLooping = false;
    mPauseCount = 0;
    mSmacker = 0;
    mPixDouble = false;
    mDroppedFrames = 0;
    mBlitter = 0;
    mBlitSource = 0;
    mUseTransparent = false;
    static bool sSoundInit = false;
    if (!sSoundInit) {
        _MDI_DRIVER *midiDrvr;
        _DLSDEVICE *dlsDrvr;
        _DIG_DRIVER *digitalDriver;
        AIL_quick_handles(&digitalDriver, &midiDrvr, &dlsDrvr);
        SmackSoundUseMSS(digitalDriver);
        sSoundInit = true;
    }
}

// 0x100195d0 ?QueryInterface@CMBSmackerMovie@@UAEJABUMBGUID@@PAPAX@Z
long CMBSmackerMovie::QueryInterface(const MBGUID &iid, void **obj)
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

// 0x10019770 ?AddRef@CMBSmackerMovie@@UAEKXZ
unsigned long CMBSmackerMovie::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10019780 ?Release@CMBSmackerMovie@@UAEKXZ
unsigned long CMBSmackerMovie::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100197b0 ?IsPlaying@CMBSmackerMovie@@UAE_NXZ
bool CMBSmackerMovie::IsPlaying(void)
{
    return mIsPlaying;
}

// 0x100197c0 ?Pause@CMBSmackerMovie@@UAEJXZ
long CMBSmackerMovie::Pause(void)
{
    return ++mPauseCount;
}

// 0x100197d0 ?Unpause@CMBSmackerMovie@@UAEJXZ
long CMBSmackerMovie::Unpause(void)
{
    if (mPauseCount > 0)
        return --mPauseCount;
    return 0;
}

// 0x100197e0 ?IsPaused@CMBSmackerMovie@@UAE_NXZ
bool CMBSmackerMovie::IsPaused(void)
{
    return mPauseCount > 0;
}

// 0x100197f0 ?SetLoop@CMBSmackerMovie@@UAEX_N@Z
void CMBSmackerMovie::SetLoop(bool loop)
{
    mIsLooping = loop;
}

// 0x10019800 ?IsLooping@CMBSmackerMovie@@UAE_NXZ
bool CMBSmackerMovie::IsLooping(void)
{
    return mIsLooping;
}

// 0x10019810 ?SetPalette@CMBSmackerMovie@@UAEXPAVMBPalette@@@Z
void CMBSmackerMovie::SetPalette(MBPalette *pal)
{
}

// 0x10019820 ?GetPalette@CMBSmackerMovie@@UAEPAVMBPalette@@XZ
MBPalette *CMBSmackerMovie::GetPalette(void)
{
    return 0;
}

// 0x10019830 ?GetOffset@CMBSmackerMovie@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBSmackerMovie::GetOffset(void) const
{
    return mPos;
}

// 0x10019850 ?GetPosition@CMBSmackerMovie@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBSmackerMovie::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x100198c0 ?Place@CMBSmackerMovie@@UAEXABVMBPoint3@@@Z
void CMBSmackerMovie::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10019900 ?Move@CMBSmackerMovie@@UAEXABVMBPoint3@@@Z
void CMBSmackerMovie::Move(const MBPoint3 &point)
{
    mPos += point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10019950 ?IsVisible@CMBSmackerMovie@@UBE_NXZ
bool CMBSmackerMovie::IsVisible(void) const
{
    return mHideCount == 0 && mIsPlaying;
}

// 0x10019970 ?IsDirty@CMBSmackerMovie@@UBE_NXZ
bool CMBSmackerMovie::IsDirty(void) const
{
    return mDirty;
}

// 0x10019980 ?SetDirty@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::SetDirty(void)
{
    mDirty = true;
}

// 0x10019990 ?ClearDirty@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::ClearDirty(void)
{
    mDirty = false;
}

// 0x100199a0 ?GetID@CMBSmackerMovie@@UAEKXZ
unsigned long CMBSmackerMovie::GetID(void)
{
    return mID;
}

// 0x100199b0 ?GetType@CMBSmackerMovie@@UAEKXZ
unsigned long CMBSmackerMovie::GetType(void)
{
    return 'smk ';
}

// EQUIV 0x10019e40 ??1CMBSmackerMovie@@IAE@XZ: reviewed: the original calls MBComPtr::CreateObject out of line where VC6 inlines it, and stores the vftable pointers before the call setup instead of after; the calls and their order agree. Was: inline budget: original calls MBComPtr::CreateObject out of line and stores the vtables first; ours inlines it (74% similar)
CMBSmackerMovie::~CMBSmackerMovie(void)
{
    {
        MBComPtr<IMBEventManager> broadcaster;
        broadcaster.CreateObject(MBIID_IMBEventManager);
        MBMovieEventStruct data;
        data.movie = this;
        data.id = mID;
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 1, &data);
    }
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    FreeBuffers();
    if (mBlitter)
        SmackBlitClose(mBlitter);
    if (mSmacker)
        SmackClose(mSmacker);
}

// EQUIV 0x10019f80 ?InitializeBuffers@CMBSmackerMovie@@IAE_NXZ: reviewed: register choice only (this in ebp vs ebx)
bool CMBSmackerMovie::InitializeBuffers(void)
{
    MBComPtr<IMBScreen> screen;
    MBComPtr<IMBScreenManager> screenManager;
    IMBScreenManager *tempScreenManager;
    MBGetScreenManager(&tempScreenManager);
    screenManager.Attach(tempScreenManager);
    IMBScreen *tempScreen;
    screenManager->GetScreenByIndex(0, &tempScreen);
    screen.Attach(tempScreen);
    if (mPixDouble) {
        mBlitSourceWidth = mSmacker->Width;
        mBlitSourceHeight = mSmacker->Height;
        mBlitSourcePitch = ((mBlitSourceWidth + 7) / 8) * 8;
        mBlitSource = (unsigned char *)MBmalloc_release(mBlitSourceHeight * mBlitSourcePitch);
        if (!mBlitSource)
            return false;
    }
    IMBSurface *surface;
    screen->CreateSurface((unsigned short)Width(), (unsigned short)Height(), 0x10, &surface);
    mIntermediate.Attach(surface);
    return true;
}

// 0x1001a060 ?FreeBuffers@CMBSmackerMovie@@IAEXXZ
void CMBSmackerMovie::FreeBuffers(void)
{
    if (mBlitSource) {
        MBfree_release(mBlitSource);
        mBlitSource = 0;
    }
    mIntermediate.Release();
}

// 0x1001a090 ?IsTimeForNextFrame@CMBSmackerMovie@@IAE_NXZ
bool CMBSmackerMovie::IsTimeForNextFrame(void)
{
    return mSmacker && !SmackWait(mSmacker);
}

// EQUIV 0x1001a0b0 ?RenderCurrentFrame@CMBSmackerMovie@@IAE_NXZ: reviewed: the original materialises the frame comparison as a bool (sete/test) and returns the false result of PrepareNextFrame from al, VC6 branches on the compare and loads 0; the calls, the loop and the results agree (while/for/bool-local forms tried, 90%). Was: original materialises a bool (sete/test) for the frame comparison, ours branches directly (90% similar)
bool CMBSmackerMovie::RenderCurrentFrame(void)
{
    void *bits = 0;
    unsigned long blitFlags = 0;
    MBPixelFormat format = mIntermediate->GetFormat();
    if (mSmacker->NewPalette && mBlitSource)
        SmackBlitSetPalette(mBlitter, mSmacker->Palette, mSmacker->PalType);
    mIntermediate->Lock(&bits, 2);
    if (mBlitSource) {
        SmackToBuffer(mSmacker, 0, 0, mBlitSourcePitch, mBlitSourceHeight, mBlitSource, 0);
    } else {
        if (format.mBitsPerPixel == 16) {
            if (MBColorMask::GetBitCount(format.mColorMask.mGreenMask) == 6)
                blitFlags |= 0xc0000000;
            if (MBColorMask::GetBitCount(format.mColorMask.mGreenMask) == 5)
                blitFlags |= 0x80000000;
        }
        SmackToBuffer(mSmacker, 0, 0, mIntermediate->GetPitch(), mIntermediate->GetHeight(), bits, blitFlags);
    }
    while (SmackDoFrame(mSmacker) == 1) {
        mDroppedFrames++;
        if (!PrepareNextFrame())
            return false;
    }
    if (mBlitSource)
        SmackBlit(mBlitter, bits, mIntermediate->GetPitch(), 0, 0, mBlitSource, mBlitSourcePitch, 0, 0,
                  mBlitSourceWidth, mBlitSourceHeight);
    mIntermediate->Unlock(&bits);
    return true;
}

// 0x1001a220 ?PrepareNextFrame@CMBSmackerMovie@@IAE_NXZ
bool CMBSmackerMovie::PrepareNextFrame(void)
{
    if (mIsDone || !mIsPlaying)
        return false;
    if (CurrentFrameNum() == NumTotalFrames() - 1 && !mIsLooping) {
        Stop();
        return false;
    }
    SmackNextFrame(mSmacker);
    if (mIsLooping && CurrentFrameNum() == 1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        MBMovieEventStruct data;
    data.movie = this;
    data.id = GetID();
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 4, &data);
    }
    return true;
}

// EQUIV 0x1001a300 ?Load@CMBSmackerMovie@@UAE_NKPAVIMBStream@@K@Z: reviewed: the original calls the MBComPtr Release copies out of line on the failure path and shares the release tail; VC6 inlines and duplicates them, with one more stack slot; the aligned call and string sequences agree. Was: ours has one more stack slot (+4 frame); otherwise the same code (79% similar)
bool CMBSmackerMovie::Load(unsigned long id, IMBStream *in, unsigned long flags)
{
    MBComPtr<IMBSurface> backBuffer;
    MBComPtr<IMBScreen> screen;
    if (!in)
        return false;
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
    backBuffer.Attach(tempSurface);
    unsigned long smackFlags = 0xff400;
    if (flags & 1)
        smackFlags = 0xff600;
    MBComPtr<IMBWinFile> winFile(in);
    mSmacker = SmackOpen((const char *)winFile->GetFileHandle(), smackFlags, (unsigned long)-1);
    if (!mSmacker) {
        MBDebugStr("ERROR: Failed to open smacker movie (id: %d)\n", id);
        return false;
    }
    mIsLoaded = true;
    mPixDouble = (flags & 2) == 2;
    if (!InitializeBuffers()) {
        MBDebugStr("ERROR: Unable to allocate buffer memory for Smacker movie.\n");
        mIsLoaded = false;
        return false;
    }
    if (mBlitSource) {
        unsigned long blitFlags;
        SmackUseMMX(1);
        if (mPixDouble)
            blitFlags = SmackUseMMX(2) == 1 ? 4 : 2;
        else
            blitFlags = 1;
        MBPixelFormat format = mIntermediate->GetFormat();
        if (format.mBitsPerPixel == 16) {
            if (MBColorMask::GetBitCount(format.mColorMask.mGreenMask) == 6)
                blitFlags |= 0xc0000000;
            if (MBColorMask::GetBitCount(format.mColorMask.mGreenMask) == 5)
                blitFlags |= 0x80000000;
        }
        mBlitter = SmackBlitOpen(blitFlags);
    }
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    MBMovieEventStruct data;
    data.movie = this;
    data.id = GetID();
    if (broadcaster)
        broadcaster->BroadcastEvent(-5, 0, &data);
    return true;
}

// 0x1001a5f0 ?Play@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::Play(void)
{
    if (mIsLoaded) {
        mIsDone = false;
        mIsPlaying = true;
        mPauseCount = 0;
        {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            MBMovieEventStruct data;
    data.movie = this;
    data.id = GetID();
            if (broadcaster)
                broadcaster->BroadcastEvent(-5, 2, &data);
        }
        RenderCurrentFrame();
        PrepareNextFrame();
    }
}

// 0x1001a6a0 ?Stop@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::Stop(void)
{
    if (mIsPlaying) {
        mIsPlaying = false;
        mIsDone = true;
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        MBMovieEventStruct data;
    data.movie = this;
    data.id = GetID();
        if (broadcaster)
            broadcaster->BroadcastEvent(-5, 3, &data);
    }
}

// 0x1001a730 ?CurrentFrameNum@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::CurrentFrameNum(void) const
{
    if (mIsLoaded && mSmacker)
        return mSmacker->FrameNum;
    return 0;
}

// 0x1001a750 ?NumTotalFrames@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::NumTotalFrames(void) const
{
    if (mIsLoaded && mSmacker)
        return mSmacker->Frames;
    return 0;
}

// 0x1001a770 ?FrameDelay@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::FrameDelay(void) const
{
    if (mIsLoaded && mSmacker)
        return mSmacker->MSPerFrame;
    return 0;
}

// 0x1001a790 ?Width@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::Width(void) const
{
    if (mIsLoaded && mSmacker)
        return mPixDouble ? mSmacker->Width * 2 : mSmacker->Width;
    return 0;
}

// 0x1001a7b0 ?Height@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::Height(void) const
{
    if (mIsLoaded && mSmacker)
        return mPixDouble ? mSmacker->Height * 2 : mSmacker->Height;
    return 0;
}

// 0x1001a7d0 ?Depth@CMBSmackerMovie@@UBEJXZ
long CMBSmackerMovie::Depth(void) const
{
    return 8;
}

// 0x1001a7e0 ?SetTransparentColor@CMBSmackerMovie@@UAEXVMBColor@@@Z
void CMBSmackerMovie::SetTransparentColor(MBColor color)
{
    if (mIntermediate)
        mIntermediate->SetTransparentColor(color);
}

// 0x1001a800 ?GetTransparentColor@CMBSmackerMovie@@UAE?AVMBColor@@XZ
MBColor CMBSmackerMovie::GetTransparentColor(void)
{
    if (mIntermediate)
        return mIntermediate->GetTransparentColor();
    return MBColor(255, 0, 255, 0);
}

// 0x1001a8d0 ?GetBounds@CMBSmackerMovie@@UBE?AVMBRect@@XZ
MBRect CMBSmackerMovie::GetBounds(void) const
{
    return MBRect(GetPosition(), MBSize(Width(), Height()));
}

// 0x1001a930 ?Show@CMBSmackerMovie@@UAEJXZ
long CMBSmackerMovie::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x1001a960 ?Hide@CMBSmackerMovie@@UAEJXZ
long CMBSmackerMovie::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x1001a980 ?Draw@CMBSmackerMovie@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBSmackerMovie::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    if (mIsPlaying && mIntermediate)
        return mIntermediate->Blit(dest, &srcRect, &destRect, 0);
    return 0;
}

// 0x1001a9b0 ?GetParent@CMBSmackerMovie@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBSmackerMovie::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x1001a9d0 ?SetParent@CMBSmackerMovie@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBSmackerMovie::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x1001aa10 ?Idle@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::Idle(void)
{
    if (mIsPlaying && !IsPaused() && IsTimeForNextFrame()) {
        RenderCurrentFrame();
        PrepareNextFrame();
    }
}

// 0x1001aa50 ?Rewind@CMBSmackerMovie@@UAEXXZ
void CMBSmackerMovie::Rewind(void)
{
    if (mSmacker)
        SmackGoto(mSmacker, 0);
}


// Matched by this code:
// 0x1001aa90 ?QueryInterface@CMBSmackerMovie@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1001aaa0 ?AddRef@CMBSmackerMovie@@W3AEKXZ
// 0x1001aab0 ?Release@CMBSmackerMovie@@W3AEKXZ
// 0x1001aac0 ?QueryInterface@CMBSmackerMovie@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1001aad0 ?AddRef@CMBSmackerMovie@@W7AEKXZ
// 0x1001aae0 ?Release@CMBSmackerMovie@@W7AEKXZ
// 0x1001aaf0 ?QueryInterface@CMBSmackerMovie@@WM@AEJABUMBGUID@@PAPAX@Z
// 0x1001ab00 ?AddRef@CMBSmackerMovie@@WM@AEKXZ
// 0x1001ab10 ?Release@CMBSmackerMovie@@WM@AEKXZ
