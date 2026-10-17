// CMBMemorySurface.cpp - MBEngine.dll CMBMemorySurface.obj
#include "CMBMemorySurface.h"

static const MBGUID MBIID_IMBMemorySurface =
    { 0x7f30abb0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

#include "IMBMemorySurface.h"

class CMBMemorySurface : public IMBMemorySurface {
public:
    CMBMemorySurface(void);
    ~CMBMemorySurface(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
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
    virtual long ColorFill(MBColor color, MBRect *dest_rect);
    virtual long Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect,
                      unsigned long blit_flags);
protected:
    void *mRawImage;            // +0x08
    unsigned long mWidth;       // +0x0c
    unsigned long mHeight;      // +0x10
    MBPixelFormat mPixelFormat; // +0x14
    MBColor mTransPixel;        // +0x2c
    MBPalette *mPalette;        // +0x34
    long mLockCount;            // +0x38
};

// 0x1006b4a0 ?MakeCMBMemorySurface@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBMemorySurface(void)
{
    IMBUnknown *p = new CMBMemorySurface;
    if (p)
        p->AddRef();
    return p;
}

// TODO 0x1006b4d0 ??0CMBMemorySurface@@QAE@XZ: alpha mask Set: original calls GetShiftCount and GetBitCount out of line (inline budget), ours inlines GetBitCount(0); rest shifted (51% similar)
CMBMemorySurface::CMBMemorySurface(void)
    : mRawImage(0), mWidth(0), mHeight(0), mPalette(0), mLockCount(0)
{
    _mRefCnt = 0;
}

// 0x1006b630 ?QueryInterface@CMBMemorySurface@@UAEJABUMBGUID@@PAPAX@Z
long CMBMemorySurface::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBSurface)
        p = this;
    if (iid == MBIID_IMBMemorySurface)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1006b730 ?AddRef@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1006b740 ?Release@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1006b770 ??1CMBMemorySurface@@QAE@XZ
CMBMemorySurface::~CMBMemorySurface(void)
{
    if (mRawImage)
        MBfree_release(mRawImage);
}

// 0x1006b790 ?CreateEmpty@CMBMemorySurface@@UAEJKKUMBPixelFormat@@@Z
long CMBMemorySurface::CreateEmpty(unsigned long width, unsigned long height, MBPixelFormat pf)
{
    if (mRawImage)
        return -10004;
    unsigned long size = (pf.mBitsPerPixel * width * height) >> 3;
    if (!size)
        return -10002;
    mRawImage = MBmalloc_release(size);
    mWidth = width;
    mHeight = height;
    mPixelFormat = pf;
    return 0;
}

// 0x1006b800 ?GetWidth@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::GetWidth(void)
{
    return mWidth;
}

// 0x1006b810 ?GetHeight@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::GetHeight(void)
{
    return mHeight;
}

// 0x1006b820 ?GetPitch@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::GetPitch(void)
{
    return (GetDepth() * GetWidth()) >> 3;
}

// 0x1006b840 ?GetSize@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::GetSize(void)
{
    return GetPitch() * GetHeight();
}

// 0x1006b860 ?GetDepth@CMBMemorySurface@@UAEKXZ
unsigned long CMBMemorySurface::GetDepth(void)
{
    return mPixelFormat.mBitsPerPixel;
}

// 0x1006b870 ?GetFormat@CMBMemorySurface@@UAE?AUMBPixelFormat@@XZ
MBPixelFormat CMBMemorySurface::GetFormat(void)
{
    return mPixelFormat;
}

// 0x1006b890 ?Lock@CMBMemorySurface@@UAEJPAPAXK@Z
long CMBMemorySurface::Lock(void **bits, unsigned long flags)
{
    *bits = mRawImage;
    return ++mLockCount;
}

// 0x1006b8b0 ?Unlock@CMBMemorySurface@@UAEJPAPAX@Z
long CMBMemorySurface::Unlock(void **bits)
{
    *bits = 0;
    return --mLockCount;
}

// 0x1006b8d0 ?GetPixelColor@CMBMemorySurface@@UAE?AVMBColor@@KK@Z
MBColor CMBMemorySurface::GetPixelColor(unsigned long x, unsigned long y)
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

// 0x1006baa0 ?SetTransparentColor@CMBMemorySurface@@UAEJVMBColor@@@Z
long CMBMemorySurface::SetTransparentColor(MBColor pd)
{
    switch (mPixelFormat.mBitsPerPixel) {
    case 16:
        pd.Convert(mPixelFormat.mColorMask);
        break;
    case 8:
    case 24:
    case 32:
        break;
    default:
        return -10002;
    }
    mTransPixel = pd;
    return 0;
}

// 0x1006bc20 ?GetTransparentColor@CMBMemorySurface@@UAE?AVMBColor@@XZ
MBColor CMBMemorySurface::GetTransparentColor(void)
{
    return mTransPixel;
}

// 0x1006bc40 ?SetPalette@CMBMemorySurface@@UAEJPAVMBPalette@@@Z
long CMBMemorySurface::SetPalette(MBPalette *pal)
{
    if (mPixelFormat.mBitsPerPixel != 8)
        return -10002;
    if (!pal)
        return -10001;
    mPalette = pal;
    return 0;
}

// 0x1006bc70 ?GetPalette@CMBMemorySurface@@UAEPAVMBPalette@@XZ
MBPalette *CMBMemorySurface::GetPalette(void)
{
    return mPalette;
}

// 0x1006bc80 ?ColorFill@CMBMemorySurface@@UAEJVMBColor@@PAVMBRect@@@Z
long CMBMemorySurface::ColorFill(MBColor color, MBRect *dest_rect)
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

// 0x1006bde0 ?Blit@CMBMemorySurface@@UAEJPAVIMBSurface@@PAVMBRect@@1K@Z
long CMBMemorySurface::Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect,
                            unsigned long blit_flags)
{
    MBRect def_source_rect;
    MBRect def_dest_rect;
    if (!dest)
        return -10001;
    if (mLockCount)
        return -10010;
    if (GetDepth() != dest->GetDepth())
        return -10002;
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
    unsigned char *source = (unsigned char *)source_lock +
                            ((GetDepth() * source_rect->left) >> 3) + GetPitch() * source_rect->top;
    unsigned char *destination = (unsigned char *)dest_lock +
                                 ((GetDepth() * dest_rect->left) >> 3) + dest->GetPitch() * dest_rect->top;
    if (blit_flags & 1) {
        if (blit_flags & 2)
            MBStretchBlitTrans(source, source_rect, GetPitch(), destination, dest_rect,
                               dest->GetPitch(), GetDepth(), mTransPixel.mColor);
        else
            MBBlitTrans(source, source_rect, GetPitch(), destination, dest_rect,
                        dest->GetPitch(), GetDepth(), mTransPixel.mColor);
    } else {
        if (blit_flags & 2)
            MBStretchBlit(source, source_rect, GetPitch(), destination, dest_rect,
                          dest->GetPitch(), GetDepth());
        else
            MBBlit(source, source_rect, GetPitch(), destination, dest_rect, dest->GetPitch(),
                   GetDepth());
    }
    Unlock(&source_lock);
    dest->Unlock(&dest_lock);
    return 0;
}


// Matched by this code:
// 0x1006c170 _$E2
// 0x1006c180 _$E1
// 0x1006c250 _$E5
// 0x1006c260 _$E4
// 0x1006c330 _$E8
// 0x1006c340 _$E7
// 0x1006c410 _$E11
// 0x1006c420 _$E10
// 0x1006c4e0 _$E14
// 0x1006c4f0 _$E13
// 0x1006c5b0 _$E17
// 0x1006c5c0 _$E16
// 0x1006c670 _$E20
// 0x1006c680 _$E19
// 0x1006c730 _$E23
// 0x1006c740 _$E22
// 0x1006c7f0 _$E26
// 0x1006c800 _$E25
// 0x1006c8c0 _$E29
// 0x1006c8d0 _$E28
// 0x1006c990 _$E32
// 0x1006c9a0 _$E31
// 0x1006ca50 _$E35
// 0x1006ca60 _$E34
// 0x1006cb10 _$E38
// 0x1006cb20 _$E37
// 0x1006cbd0 _$E41
// 0x1006cbe0 _$E40
// 0x1006cc90 _$E44
// 0x1006cca0 _$E43
// 0x1006cd50 _$E47
// 0x1006cd60 _$E46
