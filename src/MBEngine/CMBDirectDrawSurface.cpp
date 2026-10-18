// CMBDirectDrawSurface.cpp - MBEngine.dll CMBDirectDrawSurface.obj
#include "CMBDirectDrawSurface.h"

// 0x10057750 ?MakeCMBDirectDrawSurface@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBDirectDrawSurface(void)
{
    IMBUnknown *p = (IMBDirectDrawSurface *)new CMBDirectDrawSurface;
    if (p)
        p->AddRef();
    return p;
}

// 0x10057780 ??0CMBDirectDrawSurface@@QAE@XZ
CMBDirectDrawSurface::CMBDirectDrawSurface(void)
    : mBitmapData(0), mSurface(0), mSurfaceCopy(0), mLockedWriteable(0), mWidth(0), mHeight(0), mPitch(0),
      mLockCount(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    broadcaster->AddListener(-8, this);
}

// 0x10057890 ?QueryInterface@CMBDirectDrawSurface@@UAEJABUMBGUID@@PAPAX@Z
long CMBDirectDrawSurface::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBSurface)
        p = (IMBDirectDrawSurface *)this;
    if (iid == MBIID_IMBDirectDrawSurface)
        p = (IMBDirectDrawSurface *)this;
    if (iid == MBIID_IMBWinDC)
        p = (IMBWinDC *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBDirectDrawSurface *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10057a20 ?AddRef@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10057a30 ?Release@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// TODO 0x10057a60 ??1CMBDirectDrawSurface@@QAE@XZ: fixed 2026-10-16: RemoveListener(-8, this) went to the wrong vftable slot (IMBEventManager overload order in CMBDirectDrawSurface.h); left: the original calls _QueryInterface out of line, ours inlines it fully (guid ctor); inline budget (81% similar)
CMBDirectDrawSurface::~CMBDirectDrawSurface(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-8, this);
    if (mSurface)
        mSurface->Release();
    if (mSurfaceCopy)
        mSurfaceCopy->Release();
}

// 0x10057b10 ?GetWidth@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::GetWidth(void)
{
    return mWidth;
}

// 0x10057b20 ?GetHeight@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::GetHeight(void)
{
    return mHeight;
}

// 0x10057b30 ?GetPitch@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::GetPitch(void)
{
    return mPitch;
}

// 0x10057b40 ?GetSize@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::GetSize(void)
{
    return GetDepth() * GetHeight() * GetWidth() / 8;
}

// 0x10057b70 ?GetDepth@CMBDirectDrawSurface@@UAEKXZ
unsigned long CMBDirectDrawSurface::GetDepth(void)
{
    return mPixelFormat.mBitsPerPixel;
}

// 0x10057b80 ?GetFormat@CMBDirectDrawSurface@@UAE?AUMBPixelFormat@@XZ
MBPixelFormat CMBDirectDrawSurface::GetFormat(void)
{
    return mPixelFormat;
}

// 0x10057ba0 ?ColorFill@CMBDirectDrawSurface@@UAEJVMBColor@@PAVMBRect@@@Z
long CMBDirectDrawSurface::ColorFill(MBColor color, MBRect *dest_rect)
{
    DDBLTFX ddbltfx;
    memset(&ddbltfx, 0, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = color.Convert(mPixelFormat.mColorMask);
    if (dest_rect) {
        RECT rcDest;
        rcDest.left = dest_rect->left;
        rcDest.top = dest_rect->top;
        rcDest.right = dest_rect->right;
        rcDest.bottom = dest_rect->bottom;
        mSurface->Blt(&rcDest, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        if (mSurfaceCopy)
            mSurfaceCopy->Blt(&rcDest, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    } else {
        mSurface->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        if (mSurfaceCopy)
            mSurfaceCopy->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    }
    return 0;
}

// 0x10057d70 ?Blit@CMBDirectDrawSurface@@UAEJPAVIMBSurface@@PAVMBRect@@1K@Z
long CMBDirectDrawSurface::Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect, unsigned long blit_flags)
{
    if (!dest)
        return -10001;
    RECT rcDest;
    rcDest.left = dest_rect->left;
    rcDest.top = dest_rect->top;
    rcDest.right = dest_rect->right;
    rcDest.bottom = dest_rect->bottom;
    RECT rcSrc;
    rcSrc.left = source_rect->left;
    rcSrc.top = source_rect->top;
    rcSrc.right = source_rect->right;
    rcSrc.bottom = source_rect->bottom;

    IMBDirectDrawSurface *destSurfDDraw;
    dest->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&destSurfDDraw);
    if (destSurfDDraw) {
        unsigned long flags = DDBLT_WAIT;
        if (blit_flags & 1)
            flags = DDBLT_WAIT | DDBLT_KEYSRC;
        IDirectDrawSurface7 *lpDest;
        IDirectDrawSurface7 *lpDestCopy;
        destSurfDDraw->GetSurface(&lpDest);
        destSurfDDraw->GetSurfaceCopy(&lpDestCopy);
        HRESULT hr = lpDest->Blt(&rcDest, mSurface, &rcSrc, flags, 0);
        if (lpDestCopy)
            hr = lpDestCopy->Blt(&rcDest, mSurface, &rcSrc, flags, 0);
        lpDest->Release();
        if (lpDestCopy)
            lpDestCopy->Release();
        destSurfDDraw->Release();
        return hr >= 0 ? 0 : -10002;
    }

    IMBWinDIBSurface *destDIB;
    dest->QueryInterface(MBIID_IMBWinDIBSurface, (void **)&destDIB);
    if (destDIB) {
        IDirectDrawSurface7 *ddSurface;
        if (mSurfaceCopy)
            ddSurface = mSurfaceCopy;
        else
            ddSurface = mSurface;
        HDC ddDC;
        if (ddSurface->GetDC(&ddDC) >= 0) {
            HDC dc = CreateCompatibleDC(0);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, destDIB->GetBitmapHandle());
            BitBlt(dc, dest_rect->left, dest_rect->top, dest_rect->right - dest_rect->left,
                   dest_rect->bottom - dest_rect->top, ddDC, source_rect->left, source_rect->top, SRCCOPY);
            SelectObject(dc, oldBitmap);
            DeleteDC(dc);
            ddSurface->ReleaseDC(ddDC);
        }
        destDIB->Release();
    }

    IMBMemorySurface *destMem;
    dest->QueryInterface(MBIID_IMBMemorySurface, (void **)&destMem);
    if (destMem) {
        void *source_lock = 0;
        Lock(&source_lock, 1);
        if (!source_lock)
            return -10002;
        void *dest_lock = 0;
        dest->Lock(&dest_lock, 2);
        if (!dest_lock) {
            Unlock(&source_lock);
            return -10002;
        }
        void *source_data = (char *)source_lock + GetDepth() * source_rect->left / 8 + GetPitch() * source_rect->top;
        void *dest_data = (char *)dest_lock + GetDepth() * dest_rect->left / 8 + dest->GetPitch() * dest_rect->top;
        if (blit_flags & 1) {
            mTransparentColor.Convert(GetFormat().mColorMask);
            if (blit_flags & 2)
                MBStretchBlitTrans(source_data, source_rect, GetPitch(), dest_data, dest_rect, dest->GetPitch(),
                                   GetDepth(), mTransparentColor.mColor);
            else
                MBBlitTrans(source_data, source_rect, GetPitch(), dest_data, dest_rect, dest->GetPitch(),
                            GetDepth(), mTransparentColor.mColor);
        } else {
            if (blit_flags & 2)
                MBStretchBlit(source_data, source_rect, GetPitch(), dest_data, dest_rect, dest->GetPitch(),
                              GetDepth());
            else
                MBBlit(source_data, source_rect, GetPitch(), dest_data, dest_rect, dest->GetPitch(), GetDepth());
        }
        Unlock(&source_lock);
        dest->Unlock(&dest_lock);
    }
    return 0;
}

// 0x10058200 ?Lock@CMBDirectDrawSurface@@UAEJPAPAXK@Z
long CMBDirectDrawSurface::Lock(void **bits, unsigned long flags)
{
    DDSURFACEDESC2 surfaceDesc;
    memset(&surfaceDesc, 0, sizeof(surfaceDesc));
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    IDirectDrawSurface7 *s = mSurfaceCopy;
    if (!s)
        s = mSurface;
    if (s->Lock(0, &surfaceDesc, DDLOCK_WAIT, 0) >= 0) {
        *bits = surfaceDesc.lpSurface;
        if (!mLockedWriteable)
            mLockedWriteable = (flags & 2) != 0;
        return ++mLockCount;
    }
    *bits = 0;
    return mLockCount;
}

// 0x10058280 ?Unlock@CMBDirectDrawSurface@@UAEJPAPAX@Z
long CMBDirectDrawSurface::Unlock(void **bits)
{
    mLockCount--;
    if (mSurfaceCopy) {
        mSurfaceCopy->Unlock(0);
        if (!mLockCount && mLockedWriteable) {
            mSurface->Blt(0, mSurfaceCopy, 0, DDBLT_WAIT, 0);
            mLockedWriteable = 0;
        }
    } else {
        mSurface->Unlock(0);
    }
    return mLockCount;
}

// 0x100582f0 ?GetDC@CMBDirectDrawSurface@@UAEJPAPAUHDC__@@K@Z
long CMBDirectDrawSurface::GetDC(HDC__ **hdcPtr, unsigned long lockFlag)
{
    *hdcPtr = 0;
    IDirectDrawSurface7 *s = mSurfaceCopy;
    if (!s)
        s = mSurface;
    if (s->GetDC(hdcPtr) < 0) {
        *hdcPtr = 0;
        return -1;
    }
    if (!mLockedWriteable)
        mLockedWriteable = (lockFlag & 2) != 0;
    return ++mLockCount;
}

// 0x10058340 ?ReleaseDC@CMBDirectDrawSurface@@UAEJPAUHDC__@@@Z
long CMBDirectDrawSurface::ReleaseDC(HDC__ *hdc)
{
    if (hdc) {
        mLockCount--;
        IDirectDrawSurface7 *s = mSurfaceCopy;
        if (!s)
            s = mSurface;
        s->ReleaseDC(hdc);
        if (!mLockCount && mLockedWriteable && mSurfaceCopy) {
            mSurface->Blt(0, mSurfaceCopy, 0, DDBLT_WAIT, 0);
            mLockedWriteable = 0;
        }
    }
    return 0;
}

// EQUIV 0x100583a0 ?GetPixelColor@CMBDirectDrawSurface@@UAE?AVMBColor@@KK@Z: reviewed: the pixel address is summed in another order (same address); otherwise identical
MBColor CMBDirectDrawSurface::GetPixelColor(unsigned long x, unsigned long y)
{
    MBPixelFormat format = GetFormat();
    unsigned long pixel = 0;
    void *bits = 0;
    Lock(&bits, 1);
    if (bits) {
        switch (format.mBitsPerPixel) {
        case 32:
            pixel = *(unsigned long *)((char *)bits + GetPitch() * y + x * 4);
            break;
        case 16:
            pixel = *(unsigned short *)((char *)bits + GetPitch() * y + x * 2);
            break;
        }
    }
    Unlock(&bits);
    MBColor ret_val;
    if (format.mColorMask.mRedMask)
        ret_val.mRed = (unsigned char)(((pixel & format.mColorMask.mRedMask) * 255) / format.mColorMask.mRedMask);
    if (format.mColorMask.mGreenMask)
        ret_val.mGreen = (unsigned char)(((pixel & format.mColorMask.mGreenMask) * 255) / format.mColorMask.mGreenMask);
    if (format.mColorMask.mBlueMask)
        ret_val.mBlue = (unsigned char)(((pixel & format.mColorMask.mBlueMask) * 255) / format.mColorMask.mBlueMask);
    if (format.mColorMask.mAlphaMask)
        ret_val.mAlpha = (unsigned char)(((pixel & format.mColorMask.mAlphaMask) * 255) / format.mColorMask.mAlphaMask);
    ret_val.mColor = pixel;
    return ret_val;
}

// EQUIV 0x100584d0 ?QuerySurfaceInfo@CMBDirectDrawSurface@@UAEXXZ: reviewed: the original stores the pixel format members directly, VC6 builds the same values in a temporary and copies it with rep movsd; two independent member stores swap; the GetShiftCount/GetBitCount calls are out of line in the original and inline here. Was: QuerySurfaceInfo: original inlines Set fully on success, else-branch Set(0..) calls GetShiftCount+GetBitCount out of line for alpha; Set(0,0,0,0) gives only GetShiftCount out (267 diff), MBColorMask(0,0,0,0) temp reproduces both calls but adds a copy (85% similar)
void CMBDirectDrawSurface::QuerySurfaceInfo(void)
{
    DDSURFACEDESC2 surfaceDesc;
    memset(&surfaceDesc, 0, sizeof(surfaceDesc));
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    IDirectDrawSurface7 *s = mSurfaceCopy;
    if (!s)
        s = mSurface;
    if (s && s->GetSurfaceDesc(&surfaceDesc) >= 0) {
        mWidth = surfaceDesc.dwWidth;
        mHeight = surfaceDesc.dwHeight;
        mPitch = surfaceDesc.lPitch;
        mPixelFormat.mBitsPerPixel = (short)surfaceDesc.ddpfPixelFormat.dwRGBBitCount;
        mPixelFormat.mColorMask.Set(surfaceDesc.ddpfPixelFormat.dwRBitMask, surfaceDesc.ddpfPixelFormat.dwGBitMask,
                                    surfaceDesc.ddpfPixelFormat.dwBBitMask,
                                    surfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask);
    } else {
        mWidth = 0;
        mHeight = 0;
        mPitch = 0;
        mPixelFormat.mBitsPerPixel = 0;
        mPixelFormat.mColorMask = MBColorMask(0, 0, 0, 0);
    }
}

// 0x100586a0 ?SetSurface@CMBDirectDrawSurface@@UAEXPAUIDirectDrawSurface7@@@Z
void CMBDirectDrawSurface::SetSurface(IDirectDrawSurface7 *surface)
{
    if (mSurface)
        mSurface->Release();
    mSurface = surface;
    surface->AddRef();
    QuerySurfaceInfo();
}

// 0x100586d0 ?GetSurface@CMBDirectDrawSurface@@UAEXPAPAUIDirectDrawSurface7@@@Z
void CMBDirectDrawSurface::GetSurface(IDirectDrawSurface7 **surface)
{
    mSurface->AddRef();
    *surface = mSurface;
}

// 0x100586f0 ?SetSurfaceCopy@CMBDirectDrawSurface@@UAEXPAUIDirectDrawSurface7@@@Z
void CMBDirectDrawSurface::SetSurfaceCopy(IDirectDrawSurface7 *surface)
{
    if (mSurfaceCopy)
        mSurfaceCopy->Release();
    mSurfaceCopy = surface;
    surface->AddRef();
    QuerySurfaceInfo();
}

// 0x10058720 ?GetSurfaceCopy@CMBDirectDrawSurface@@UAEXPAPAUIDirectDrawSurface7@@@Z
void CMBDirectDrawSurface::GetSurfaceCopy(IDirectDrawSurface7 **surface)
{
    if (mSurfaceCopy) {
        mSurfaceCopy->AddRef();
        *surface = mSurfaceCopy;
    } else {
        *surface = 0;
    }
}

// 0x10058750 ?SetTransparentColor@CMBDirectDrawSurface@@UAEJVMBColor@@@Z
long CMBDirectDrawSurface::SetTransparentColor(MBColor pd)
{
    mTransparentColor = pd;
    DDCOLORKEY ddcolorKey;
    ddcolorKey.dwColorSpaceLowValue = mTransparentColor.Convert(mPixelFormat.mColorMask);
    ddcolorKey.dwColorSpaceHighValue = ddcolorKey.dwColorSpaceLowValue;
    mSurface->SetColorKey(DDCKEY_SRCBLT, &ddcolorKey);
    return 0;
}

// 0x10058860 ?GetTransparentColor@CMBDirectDrawSurface@@UAE?AVMBColor@@XZ
MBColor CMBDirectDrawSurface::GetTransparentColor(void)
{
    return mTransparentColor;
}

// 0x10058880 ?SetPalette@CMBDirectDrawSurface@@UAEJPAVMBPalette@@@Z
long CMBDirectDrawSurface::SetPalette(MBPalette *pal)
{
    return -10003;
}

// 0x10058890 ?GetPalette@CMBDirectDrawSurface@@UAEPAVMBPalette@@XZ
MBPalette *CMBDirectDrawSurface::GetPalette(void)
{
    return 0;
}

// 0x100588a0 ?ListenToEvent@CMBDirectDrawSurface@@UAEXJJPAX@Z
void CMBDirectDrawSurface::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -8 && event == 0) {
        if (mSurface->IsLost() == DDERR_SURFACELOST) {
            mSurface->Restore();
            if (mSurfaceCopy)
                mSurface->Blt(0, mSurfaceCopy, 0, DDBLT_WAIT, 0);
        }
    }
}


// Matched by this code:
// 0x1002a300 ?Convert@MBColor@@QAEKVMBColorMask@@@Z
// 0x100588f0 ?QueryInterface@CMBDirectDrawSurface@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10058900 ?AddRef@CMBDirectDrawSurface@@W3AEKXZ
// 0x10058910 ?Release@CMBDirectDrawSurface@@W3AEKXZ
// 0x10058920 ?QueryInterface@CMBDirectDrawSurface@@W7AEJABUMBGUID@@PAPAX@Z
// 0x10058930 ?AddRef@CMBDirectDrawSurface@@W7AEKXZ
// 0x10058940 ?Release@CMBDirectDrawSurface@@W7AEKXZ
// 0x10058950 _$E2
// 0x10058960 _$E1
// 0x10058a30 _$E5
// 0x10058a40 _$E4
// 0x10058b10 _$E8
// 0x10058b20 _$E7
// 0x10058bf0 _$E11
// 0x10058c00 _$E10
// 0x10058cc0 _$E14
// 0x10058cd0 _$E13
// 0x10058d90 ?Set@MBColorMask@@QAEXKKKK@Z
// 0x10058e80 ?GetShiftCount@MBColorMask@@SAEK@Z
// 0x10058ea0 ?GetBitCount@MBColorMask@@SAEK@Z
// 0x10058ec0 _$E17
// 0x10058ed0 _$E16
// 0x10058f80 _$E20
// 0x10058f90 _$E19
// 0x10059040 _$E23
// 0x10059050 _$E22
// 0x10059100 _$E26
// 0x10059110 _$E25
// 0x100591d0 _$E29
// 0x100591e0 _$E28
// 0x100592a0 _$E32
// 0x100592b0 _$E31
// 0x10059360 _$E35
// 0x10059370 _$E34
// 0x10059420 _$E38
// 0x10059430 _$E37
// 0x100594e0 _$E41
// 0x100594f0 _$E40
// 0x100595a0 _$E44
// 0x100595b0 _$E43
// 0x10059660 _$E47
// 0x10059670 _$E46
