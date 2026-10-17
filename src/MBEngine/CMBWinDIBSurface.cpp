// CMBWinDIBSurface.cpp - MBEngine.dll CMBWinDIBSurface.obj
#include <windows.h>
#include <stdlib.h>
#include "CMBMemorySurface.h"

static const MBGUID MBIID_IMBWinDIBSurface =
    { 0x7fb10770, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

template <class T> inline const T &MBmin(const T &a, const T &b) { return a < b ? a : b; }

#include "IMBWinDIBSurface.h"

class CMBWinDIBSurface : public IMBSurface, public IMBWinDIBSurface {
public:
    CMBWinDIBSurface(void);
    ~CMBWinDIBSurface(void);
protected:
    unsigned long _mRefCnt; // +0x08
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long CreateLoad(IMBStream *source);
    virtual long CreateCopy(IMBSurface *source, MBRect *source_rect);
    virtual long CreateEmpty(unsigned long width, unsigned long height, MBPixelFormat pf);
    virtual unsigned long GetWidth(void);
    virtual unsigned long GetHeight(void);
    virtual unsigned long GetPitch(void);
    virtual unsigned long GetSize(void);
    virtual unsigned long GetDepth(void);
    virtual MBPixelFormat GetFormat(void);
    virtual long Lock(void **bits, unsigned long flags);
    virtual long Unlock(void **bits);
    virtual MBColor GetPixelColor(unsigned long x, unsigned long y);
    virtual long SetTransparentColor(MBColor pd);
    virtual MBColor GetTransparentColor(void);
    virtual long SetPalette(MBPalette *pal);
    virtual MBPalette *GetPalette(void);
    virtual long ColorFill(MBColor color, MBRect *destRect);
    virtual long Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect,
                      unsigned long blit_flags);
    virtual HBITMAP GetBitmapHandle(void);
    virtual BITMAPINFO *GetBitmapInfo(void);
protected:
    MBColor mTransPixel;        // +0x0c
    MBPixelFormat mPixelFormat; // +0x14
    HBITMAP mBitmapHandle;      // +0x2c
    BITMAPINFO *mBitmapInfo;    // +0x30
    void *mBitmapData;          // +0x34
    long mLockCount;            // +0x38
};

// 0x10059720 ?MakeCMBWinDIBSurface@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinDIBSurface(void)
{
    IMBUnknown *p = (IMBSurface *)new CMBWinDIBSurface;
    if (p)
        p->AddRef();
    return p;
}

// 0x10059750 ??0CMBWinDIBSurface@@QAE@XZ
CMBWinDIBSurface::CMBWinDIBSurface(void)
    : mBitmapHandle(0), mBitmapInfo(0), mBitmapData(0), mLockCount(0)
{
    _mRefCnt = 0;
}

// 0x100598c0 ?QueryInterface@CMBWinDIBSurface@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinDIBSurface::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBSurface)
        p = (IMBSurface *)this;
    if (iid == MBIID_IMBWinDIBSurface)
        p = (IMBWinDIBSurface *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBSurface *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100599d0 ?AddRef@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100599e0 ?Release@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10059a10 ??1CMBWinDIBSurface@@QAE@XZ
CMBWinDIBSurface::~CMBWinDIBSurface(void)
{
    if (mBitmapHandle)
        DeleteObject(mBitmapHandle);
    if (mBitmapInfo)
        MBfree_release(mBitmapInfo);
}

// 0x10059a50 ?CreateLoad@CMBWinDIBSurface@@UAEJPAVIMBStream@@@Z
long CMBWinDIBSurface::CreateLoad(IMBStream *source)
{
    return -10003;
}

// 0x10059a60 ?CreateCopy@CMBWinDIBSurface@@UAEJPAVIMBSurface@@PAVMBRect@@@Z
long CMBWinDIBSurface::CreateCopy(IMBSurface *source, MBRect *source_rect)
{
    long result;
    if (!source)
        return -10001;
    if (!source_rect) {
        result = CreateEmpty(source->GetWidth(), source->GetHeight(), source->GetFormat());
        if (result)
            return result;
    } else {
        result = CreateEmpty(source_rect->Width(), source_rect->Height(), source->GetFormat());
        if (result)
            return result;
    }
    return source->Blit(this, source_rect, 0, 0);
}

// 0x10059b50 ?CreateEmpty@CMBWinDIBSurface@@UAEJKKUMBPixelFormat@@@Z
long CMBWinDIBSurface::CreateEmpty(unsigned long width, unsigned long height, MBPixelFormat pf)
{
    if (!width || !height)
        return -10002;
    if (pf.mBitsPerPixel != 16 && pf.mBitsPerPixel != 32)
        return -10002;
    mBitmapInfo = (BITMAPINFO *)MBmalloc_release(sizeof(BITMAPINFOHEADER));
    if (!mBitmapInfo)
        return -10006;
    mBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    mBitmapInfo->bmiHeader.biWidth = width;
    mBitmapInfo->bmiHeader.biHeight = -(long)height;
    mBitmapInfo->bmiHeader.biPlanes = 1;
    mBitmapInfo->bmiHeader.biBitCount = pf.mBitsPerPixel;
    mBitmapInfo->bmiHeader.biCompression = BI_RGB;
    mBitmapInfo->bmiHeader.biSizeImage = 0;
    mBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
    mBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    HDC dc = CreateCompatibleDC(0);
    if (!dc)
        return -10005;
    mBitmapHandle = CreateDIBSection(dc, mBitmapInfo, DIB_RGB_COLORS, &mBitmapData, 0, 0);
    if (!mBitmapHandle) {
        GetLastError();
        return -10005;
    }
    DeleteDC(dc);
    mPixelFormat = pf;
    return 0;
}

// 0x10059c60 ?GetWidth@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::GetWidth(void)
{
    if (mBitmapInfo)
        return mBitmapInfo->bmiHeader.biWidth;
    return 0;
}

// 0x10059c70 ?GetHeight@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::GetHeight(void)
{
    if (mBitmapInfo)
        return abs(mBitmapInfo->bmiHeader.biHeight);
    return 0;
}

// 0x10059c90 ?GetPitch@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::GetPitch(void)
{
    return (GetDepth() * GetWidth()) >> 3;
}

// 0x10059cb0 ?GetSize@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::GetSize(void)
{
    return (GetDepth() * GetHeight() * GetWidth()) >> 3;
}

// 0x10059ce0 ?GetDepth@CMBWinDIBSurface@@UAEKXZ
unsigned long CMBWinDIBSurface::GetDepth(void)
{
    return mBitmapInfo ? mBitmapInfo->bmiHeader.biBitCount : 1;
}

// 0x10059d00 ?GetFormat@CMBWinDIBSurface@@UAE?AUMBPixelFormat@@XZ
MBPixelFormat CMBWinDIBSurface::GetFormat(void)
{
    MBPixelFormat format;
    format.mBitsPerPixel = (unsigned char)GetDepth();
    return format;
}

// 0x10059da0 ?Lock@CMBWinDIBSurface@@UAEJPAPAXK@Z
long CMBWinDIBSurface::Lock(void **bits, unsigned long flags)
{
    *bits = mBitmapData;
    return ++mLockCount;
}

// 0x10059dc0 ?Unlock@CMBWinDIBSurface@@UAEJPAPAX@Z
long CMBWinDIBSurface::Unlock(void **bits)
{
    *bits = 0;
    return --mLockCount;
}

// 0x10059de0 ?GetPixelColor@CMBWinDIBSurface@@UAE?AVMBColor@@KK@Z
MBColor CMBWinDIBSurface::GetPixelColor(unsigned long x, unsigned long y)
{
    MBColor color;
    MBPixelFormat format = GetFormat();
    unsigned char *bits;
    Lock((void **)&bits, 1);
    if (bits) {
        unsigned long value;
        switch (format.mBitsPerPixel) {
        case 32:
            value = GetPitch() * y + x * 4;
            value = *(unsigned long *)(bits + value);
            break;
        case 16:
            value = GetPitch() * y + x * 2;
            value = *(unsigned short *)(bits + value);
            break;
        case 8:
            value = 0;
            break;
        }
        color.Set(value, format.mColorMask);
        Unlock((void **)&bits);
    }
    return color;
}

// 0x10059fb0 ?SetTransparentColor@CMBWinDIBSurface@@UAEJVMBColor@@@Z
long CMBWinDIBSurface::SetTransparentColor(MBColor pd)
{
    mTransPixel = pd;
    return 0;
}

// 0x10059fd0 ?GetTransparentColor@CMBWinDIBSurface@@UAE?AVMBColor@@XZ
MBColor CMBWinDIBSurface::GetTransparentColor(void)
{
    return mTransPixel;
}

// 0x10059ff0 ?SetPalette@CMBWinDIBSurface@@UAEJPAVMBPalette@@@Z
long CMBWinDIBSurface::SetPalette(MBPalette *pal)
{
    return -10003;
}

// 0x1005a000 ?GetPalette@CMBWinDIBSurface@@UAEPAVMBPalette@@XZ
MBPalette *CMBWinDIBSurface::GetPalette(void)
{
    return 0;
}

// 0x1005a010 ?ColorFill@CMBWinDIBSurface@@UAEJVMBColor@@PAVMBRect@@@Z
long CMBWinDIBSurface::ColorFill(MBColor color, MBRect *destRect)
{
    void *source_lock = 0;
    Lock(&source_lock, 2);
    if (!source_lock)
        return -10002;
    long result = MBFill(source_lock, GetWidth(), GetHeight(), GetPitch(), GetDepth(),
                         color.Convert(mPixelFormat.mColorMask));
    Unlock(&source_lock);
    return result;
}

// TODO 0x1005a170 ?Blit@CMBWinDIBSurface@@UAEJPAVIMBSurface@@PAVMBRect@@1K@Z: register allocation (dest in ebp vs ebx) and one extra stack slot; GDI BitBlt min()s computed in a different order (59% similar)
long CMBWinDIBSurface::Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect,
                            unsigned long blit_flags)
{
    IMBWinDIBSurface *win_dest = 0;
    MBRect def_source_rect;
    MBRect def_dest_rect;
    if (!dest)
        return -10001;
    if (mLockCount)
        return -10010;
    if (!source_rect) {
        source_rect = &def_source_rect;
        def_source_rect.left = def_source_rect.top = 0;
        def_source_rect.right = GetWidth();
        def_source_rect.bottom = GetHeight();
    } else if (source_rect->top < 0 || source_rect->top > GetHeight() ||
               source_rect->bottom <= source_rect->top || source_rect->bottom > GetHeight() ||
               source_rect->left < 0 || source_rect->left > GetWidth() ||
               source_rect->right <= source_rect->left || source_rect->right > GetWidth())
        return -10009;
    if (!dest_rect) {
        dest_rect = &def_dest_rect;
        def_dest_rect.left = def_dest_rect.top = 0;
        def_dest_rect.right = dest->GetWidth();
        def_dest_rect.bottom = dest->GetHeight();
    } else if (dest_rect->top < 0 || dest_rect->top > dest->GetHeight() ||
               dest_rect->bottom <= dest_rect->top || dest_rect->bottom > dest->GetHeight() ||
               dest_rect->left < 0 || dest_rect->left > dest->GetWidth() ||
               dest_rect->right <= dest_rect->left || dest_rect->right > dest->GetWidth())
        return -10009;
    dest->QueryInterface(MBIID_IMBWinDIBSurface, (void **)&win_dest);
    if (win_dest && !(blit_flags & 1)) {
        if (!win_dest->GetBitmapHandle() || !win_dest->GetBitmapInfo())
            return -10001;
        HDC dest_dc = CreateCompatibleDC(0);
        SelectObject(dest_dc, win_dest->GetBitmapHandle());
        HDC source_dc = CreateCompatibleDC(0);
        SelectObject(source_dc, GetBitmapHandle());
        if (blit_flags & 2) {
            StretchBlt(dest_dc, dest_rect->left, dest_rect->top, dest_rect->Width(),
                       dest_rect->Height(), source_dc, source_rect->left, source_rect->top,
                       source_rect->Width(), source_rect->Height(), SRCCOPY);
            DeleteDC(dest_dc);
            DeleteDC(source_dc);
        } else {
            BitBlt(dest_dc, dest_rect->left, dest_rect->top,
                   MBmin(source_rect->Width(), dest_rect->Width()),
                   MBmin(source_rect->Height(), dest_rect->Height()), source_dc,
                   source_rect->left, source_rect->top, SRCCOPY);
            DeleteDC(dest_dc);
            DeleteDC(source_dc);
        }
    } else {
        if (GetDepth() != dest->GetDepth()) {
            if (win_dest)
                win_dest->Release();
            return -10002;
        }
        void *source_lock;
        Lock(&source_lock, 1);
        if (!source_lock) {
            if (win_dest)
                win_dest->Release();
            return -10002;
        }
        void *dest_lock;
        dest->Lock(&dest_lock, 2);
        if (!dest_lock) {
            if (win_dest)
                win_dest->Release();
            Unlock(&source_lock);
            return -10002;
        }
        unsigned char *source_data = (unsigned char *)source_lock +
                                     ((GetDepth() * source_rect->left) >> 3) +
                                     GetPitch() * source_rect->top;
        unsigned char *dest_data = (unsigned char *)dest_lock +
                                   ((GetDepth() * dest_rect->left) >> 3) +
                                   dest->GetPitch() * dest_rect->top;
        if (blit_flags & 1) {
            if (blit_flags & 2)
                MBStretchBlitTrans(source_data, source_rect, GetPitch(), dest_data, dest_rect,
                                   dest->GetPitch(), GetDepth(), mTransPixel.mColor);
            else
                MBBlitTrans(source_data, source_rect, GetPitch(), dest_data, dest_rect,
                            dest->GetPitch(), GetDepth(), mTransPixel.mColor);
        } else {
            if (blit_flags & 2)
                MBStretchBlit(source_data, source_rect, GetPitch(), dest_data, dest_rect,
                              dest->GetPitch(), GetDepth());
            else
                MBBlit(source_data, source_rect, GetPitch(), dest_data, dest_rect,
                       dest->GetPitch(), GetDepth());
        }
        Unlock(&source_lock);
        dest->Unlock(&dest_lock);
    }
    if (win_dest)
        win_dest->Release();
    return 0;
}

// 0x1005a660 ?GetBitmapHandle@CMBWinDIBSurface@@UAEPAUHBITMAP__@@XZ
HBITMAP CMBWinDIBSurface::GetBitmapHandle(void)
{
    return mBitmapHandle;
}

// 0x1005a670 ?GetBitmapInfo@CMBWinDIBSurface@@UAEPAUtagBITMAPINFO@@XZ
BITMAPINFO *CMBWinDIBSurface::GetBitmapInfo(void)
{
    return mBitmapInfo;
}


// Matched by this code:
// 0x1005a680 ?QueryInterface@CMBWinDIBSurface@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1005a690 ?AddRef@CMBWinDIBSurface@@W3AEKXZ
// 0x1005a6a0 ?Release@CMBWinDIBSurface@@W3AEKXZ
// 0x1005a6b0 _$E2
// 0x1005a6c0 _$E1
// 0x1005a790 _$E5
// 0x1005a7a0 _$E4
// 0x1005a870 _$E8
// 0x1005a880 _$E7
// 0x1005a950 _$E11
// 0x1005a960 _$E10
// 0x1005aa20 _$E14
// 0x1005aa30 _$E13
// 0x1005aaf0 _$E17
// 0x1005ab00 _$E16
// 0x1005abb0 _$E20
// 0x1005abc0 _$E19
// 0x1005ac70 _$E23
// 0x1005ac80 _$E22
// 0x1005ad30 _$E26
// 0x1005ad40 _$E25
// 0x1005ae00 _$E29
// 0x1005ae10 _$E28
// 0x1005aed0 _$E32
// 0x1005aee0 _$E31
// 0x1005af90 _$E35
// 0x1005afa0 _$E34
// 0x1005b050 _$E38
// 0x1005b060 _$E37
// 0x1005b110 _$E41
// 0x1005b120 _$E40
// 0x1005b1d0 _$E44
// 0x1005b1e0 _$E43
// 0x1005b290 _$E47
// 0x1005b2a0 _$E46
