// MBAnimCast.cpp - MBEngine.dll MBAnimCast.obj
#include "MBAnimCast_unit.h"

// 0x1002cc30 ??0MBAnimCast@@QAE@XZ
MBAnimCast::MBAnimCast(void)
    : mSourceOffset(0), mFrame(0), mDepth(0), mSquishPixelFmt(0), mLastAccess(0),
      mCanBePurged(true), mHasAttachmentPoint(false)
{
    _mRefCnt = 0;
}

// 0x1002cc70 ??1MBAnimCast@@AAE@XZ
MBAnimCast::~MBAnimCast(void)
{
    mSurface.Release();
    mSource.Release();
    delete mFrame;
}

// 0x1002cce0 ?GetSize@MBAnimCast@@QBE?AVMBSize@@XZ
MBSize MBAnimCast::GetSize(void) const
{
    return mSize;
}

// 0x1002cd00 ?GetApproxSize@MBAnimCast@@QBEKXZ
unsigned long MBAnimCast::GetApproxSize(void) const
{
    return mDepth / 8 * mSize.cy * mSize.cx;
}

// 0x1002cd10 ?Lock@MBAnimCast@@QAEXXZ
void MBAnimCast::Lock(void)
{
    IMBSurface *surface;
    GetSurface(&surface);
    if (surface) {
        mCanBePurged = false;
        mSource.Release();
        surface->Release();
    }
}

// 0x1002cd50 ?GetSurface@MBAnimCast@@QAEXPAPAVIMBSurface@@@Z
void MBAnimCast::GetSurface(IMBSurface **surface)
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    GetSurface(clock->GetGameTime(), surface);
}

// 0x1002cdf0 ?GetSurface@MBAnimCast@@QAEXKPAPAVIMBSurface@@@Z
void MBAnimCast::GetSurface(unsigned long currentTime, IMBSurface **surface)
{
    *surface = 0;
    mLastAccess = currentTime;
    if (!mSurface) {
        if (!mFrame)
            ReloadSquishFrame(true);
        SquishFrameToSurface();
    }
    if (mSurface) {
        *surface = mSurface;
        mSurface->AddRef();
    }
}

// 0x1002ce40 ?SetSurface@MBAnimCast@@QAEXPAVIMBSurface@@@Z
void MBAnimCast::SetSurface(IMBSurface *surface)
{
    mSurface = surface;
    mCanBePurged = false;
    mSize.cx = surface->GetWidth();
    mSize.cy = surface->GetHeight();
    mDepth = surface->GetDepth();
    mSourceOffset = 0;
    delete mFrame;
    mFrame = 0;
}

// 0x1002cec0 ?GetRegistrationPoint@MBAnimCast@@QBE?AVMBPoint@@XZ
MBPoint MBAnimCast::GetRegistrationPoint(void) const
{
    return mRegistrationPoint;
}

// 0x1002cee0 ?SetRegistrationPoint@MBAnimCast@@QAEXABVMBPoint@@@Z
void MBAnimCast::SetRegistrationPoint(const MBPoint &registrationPoint)
{
    mRegistrationPoint = registrationPoint;
}

// 0x1002cf00 ?GetAttachmentPoint@MBAnimCast@@QBE?AVMBPoint@@XZ
MBPoint MBAnimCast::GetAttachmentPoint(void) const
{
    return mAttachmentPoint;
}

// 0x1002cf20 ?SetAttachmentPoint@MBAnimCast@@QAEXABVMBPoint@@@Z
void MBAnimCast::SetAttachmentPoint(const MBPoint &attachmentPoint)
{
    mAttachmentPoint = attachmentPoint;
    mHasAttachmentPoint = true;
}

// 0x1002cf40 ?ClearAttachmentPoint@MBAnimCast@@QAEXXZ
void MBAnimCast::ClearAttachmentPoint(void)
{
    mHasAttachmentPoint = false;
}

// 0x1002cf50 ?HasAttachmentPoint@MBAnimCast@@QBE_NXZ
bool MBAnimCast::HasAttachmentPoint(void) const
{
    return mHasAttachmentPoint;
}

// 0x1002cf60 ?Init@MBAnimCast@@QAE_NPAVIMBStream@@UMBPixelFormat@@@Z
bool MBAnimCast::Init(IMBStream *stream, MBPixelFormat pf)
{
    if (!stream)
        return false;
    mDepth = pf.mBitsPerPixel;
    switch (mDepth) {
    case 32:
        if (pf.mColorMask.mBlueMask == 0xff)
            mSquishPixelFmt = kSquishPixFmtARGB8888;
        else
            mSquishPixelFmt = kSquishPixFmtBGRA8888;
        break;
    case 16:
        if (MBColorMask::GetBitCount(pf.mColorMask.mGreenMask) == 6)
            mSquishPixelFmt = kSquishPixFmtRGB565;
        else
            mSquishPixelFmt = kSquishPixFmtARGB1555;
        break;
    default:
        mSquishPixelFmt = 0;
    }
    mSource = stream;
    mSourceOffset = stream->GetPosition();
    return ReloadSquishFrame(false) ? true : false;
}

// 0x1002d010 ?GetLastAccess@MBAnimCast@@QBEKXZ
unsigned long MBAnimCast::GetLastAccess(void) const
{
    return mLastAccess;
}

// EQUIV 0x1002d020 ?ReloadSquishFrame@MBAnimCast@@IAE_N_N@Z: reviewed: a member is kept in a register instead of being reloaded, and independent stores come in another order
bool MBAnimCast::ReloadSquishFrame(bool needToSeek)
{
    if (mFrame)
        return true;
    if (!mSource)
        return false;
    mCanBePurged = !mCanBePurged;
    if (needToSeek)
        mSource->Seek(mSourceOffset, kIMBStreamSeekStart);
    SquishStream squishFile((IMBStream *)mSource);
    mFrame = new SquishImageFrame;
    if (!mFrame) {
        mCanBePurged = !mCanBePurged;
        return false;
    }
    if (!mFrame->StreamIn(&squishFile)) {
        delete mFrame;
        mFrame = 0;
        mCanBePurged = !mCanBePurged;
        return false;
    }
    mSize = MBSize(mFrame->GetWidth(), mFrame->GetHeight());
    mCanBePurged = !mCanBePurged;
    return true;
}

// TODO 0x1002d120 ?SquishFrameToSurface@MBAnimCast@@IAE_NXZ: SquishFrameToSurface: original calls MBColor::Convert out of line (inline budget) and uses ebp for this; ours inlines Convert (49% similar)
bool MBAnimCast::SquishFrameToSurface(void)
{
    if (mSurface)
        return true;
    if (!mFrame) {
        if (!ReloadSquishFrame(true))
            return false;
    }
    mCanBePurged = !mCanBePurged;
    if (!mFrame->Remap(mDepth, mSquishPixelFmt, 0)) {
        mCanBePurged = !mCanBePurged;
        return false;
    }
    IMBScreenManager *screenManager;
    IMBScreen *screen;
    MBGetScreenManager(&screenManager);
    screenManager->GetScreenByIndex(0, &screen);
    screenManager->Release();
    if (!screen) {
        mCanBePurged = !mCanBePurged;
        return false;
    }
    IMBSurface *surface;
    screen->CreateSurface(mFrame->GetWidth(), mFrame->GetHeight(), 0x10, &surface);
    screen->Release();
    if (!surface) {
        mCanBePurged = !mCanBePurged;
        return false;
    }
    mSurface.Attach(surface);
    SquishImageDescriptor desc;
    desc.width = mFrame->GetWidth();
    desc.height = mFrame->GetHeight();
    desc.depth = surface->GetDepth();
    desc.rowbytes = surface->GetPitch();
    desc.datasize = desc.rowbytes * desc.depth * desc.height / 8;
    desc.SetPixelFmt(mSquishPixelFmt);
    desc.SetCompFmt(0x200);
    MBColor magenta = kMagenta;
    surface->ColorFill(magenta, 0);
    unsigned char *bits;
    surface->Lock((void **)&bits, 2);
    mFrame->Blit(bits, &desc);
    surface->Unlock((void **)&bits);
    if (mFrame->GetBlock(0)->desc.TransEnabled()) {
        SquishImageBlock *block = mFrame->GetBlock(0);
        SquishPixel trans = block->desc.TransEnabled() ? block->desc.GetTransColor() : SquishPixel();
        surface->SetTransparentColor(MBColor(trans.Red(), trans.Green(), trans.Blue(), 0));
    }
    delete mFrame;
    mFrame = 0;
    mCanBePurged = !mCanBePurged;
    return true;
}

// 0x1002d3a0 ?Purge@MBAnimCast@@QAEKXZ
unsigned long MBAnimCast::Purge(void)
{
    unsigned long size = 0;
    if (mCanBePurged) {
        if (mSurface) {
            size = GetApproxSize();
            mSurface.Release();
        }
        if (mFrame) {
            size += GetApproxSize();
            delete mFrame;
            mFrame = 0;
        }
    }
    return size;
}

// 0x1002d410 ?IsSurfaceResident@MBAnimCast@@QBE_NXZ
bool MBAnimCast::IsSurfaceResident(void) const
{
    return mSurface != 0;
}

// 0x1002e0f0 ?QueryInterface@MBAnimCast@@UAEJABUMBGUID@@PAPAX@Z
long MBAnimCast::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    return -3;
}

// 0x1002e110 ?AddRef@MBAnimCast@@UAEKXZ
unsigned long MBAnimCast::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002e120 ?Release@MBAnimCast@@UAEKXZ
unsigned long MBAnimCast::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}


// Matched by this code:
// 0x1002d420 _$E2
// 0x1002d430 _$E1
// 0x1002d500 _$E5
// 0x1002d510 _$E4
// 0x1002d5e0 _$E8
// 0x1002d5f0 _$E7
// 0x1002d6c0 _$E11
// 0x1002d6d0 _$E10
// 0x1002d790 _$E14
// 0x1002d7a0 _$E13
// 0x1002d860 _$E17
// 0x1002d870 _$E16
// 0x1002d920 _$E20
// 0x1002d930 _$E19
// 0x1002d9e0 _$E23
// 0x1002d9f0 _$E22
// 0x1002daa0 _$E26
// 0x1002dab0 _$E25
// 0x1002db70 _$E29
// 0x1002db80 _$E28
// 0x1002dc40 _$E32
// 0x1002dc50 _$E31
// 0x1002dd00 _$E35
// 0x1002dd10 _$E34
// 0x1002ddc0 _$E38
// 0x1002ddd0 _$E37
// 0x1002de80 _$E41
// 0x1002de90 _$E40
// 0x1002df40 _$E44
// 0x1002df50 _$E43
// 0x1002e000 _$E47
// 0x1002e010 _$E46
// 0x1002e0c0 _$E67
// 0x1002e0e0 _$E66
// 0x1002e150 ?Open@SquishStream@@UAE_NPBDW4SquishFileOpenFlags@@@Z
// 0x1002e160 ?Close@SquishStream@@UAE_NXZ
// 0x1002e170 ?WriteBytes@SquishStream@@UAE_NPBXK@Z
// 0x1002e190 ?ReadBytes@SquishStream@@UAE_NPAXK@Z
// 0x1002e1d0 ?Seek@SquishStream@@UAE_NJW4SquishFileSeekValues@@@Z
// 0x1002e240 ??_GSquishStream@@UAEPAXI@Z
// 0x1002e270 ?Open@SquishFile@@UAE_NPBDW4SquishFileOpenFlags@@@Z
// 0x1002e280 ?Close@SquishFile@@UAE_NXZ
// 0x1002e290 ?Seek@SquishFile@@UAE_NJW4SquishFileSeekValues@@@Z
// 0x1002e2a0 ??_GSquishFile@@UAEPAXI@Z
