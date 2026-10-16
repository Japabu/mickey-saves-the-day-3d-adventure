// CMBWinSystemFont.h - declarations for MBEngine.dll CMBWinSystemFont.obj
#ifndef CMBWINSYSTEMFONT_H
#define CMBWINSYSTEMFONT_H

// ---- MBColor header (copied from CMBSurfaceObject.h): $E1..$E47 come first ----
class MBColorMask {
public:
    MBColorMask(unsigned long redmask, unsigned long greenmask, unsigned long bluemask,
                unsigned long alphamask) { Set(redmask, greenmask, bluemask, alphamask); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask,
             unsigned long alphamask) {
        mRedMask = redmask;
        mBlueMask = bluemask;
        mGreenMask = greenmask;
        mAlphaMask = alphamask;
        mRedShift = GetShiftCount(redmask) + GetBitCount(redmask) - 8;
        mBlueShift = GetShiftCount(bluemask) + GetBitCount(bluemask) - 8;
        mGreenShift = GetShiftCount(greenmask) + GetBitCount(greenmask) - 8;
        mAlphaShift = GetShiftCount(alphamask) + GetBitCount(alphamask) - 8;
    }
    unsigned long mRedMask;
    unsigned long mBlueMask;
    unsigned long mGreenMask;
    unsigned long mAlphaMask;
    char mRedShift;
    char mBlueShift;
    char mGreenShift;
    char mAlphaShift;
    static unsigned char GetShiftCount(unsigned long mask) {
        unsigned char count = 0;
        while (!(mask & 1) && count < 32) {
            mask >>= 1;
            count++;
        }
        return count;
    }
    static unsigned char GetBitCount(unsigned long mask) {
        unsigned char count = 0;
        while (mask) {
            mask &= mask - 1;
            count++;
        }
        return count;
    }
};

static const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor |= ((unsigned long)mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor |= ((unsigned long)mRed >> -mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= ((unsigned long)mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= ((unsigned long)mGreen >> -mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= ((unsigned long)mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= ((unsigned long)mBlue >> -mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= ((unsigned long)mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= ((unsigned long)mAlpha >> -mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

static const MBColor kWhite(255, 255, 255, 0);
static const MBColor kBlack(0, 0, 0, 0);
static const MBColor kRed(255, 0, 0, 0);
static const MBColor kGreen(0, 255, 0, 0);
static const MBColor kBlue(0, 0, 255, 0);
static const MBColor kYellow(255, 255, 0, 0);
static const MBColor kCyan(0, 255, 255, 0);
static const MBColor kMagenta(255, 0, 255, 0);
static const MBColor kGrey25(64, 64, 64, 0);
static const MBColor kGrey50(128, 128, 128, 0);
static const MBColor kGrey75(192, 192, 192, 0);

// <windows.h> comes before the font interface: DrawText is DrawTextA throughout this TU
#include <windows.h>
#include <stdlib.h>
#include <string> // evidence: ctype<unsigned short>::id $E68/$E69 after the colour statics

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    unsigned char operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

// PDB ldata order in CMBWinSystemFont.obj (0x1008fcf0..0x1008fd80)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont =
    { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinDC =
    { 0x7fb72430, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSystemFont =
    { 0x7ff75210, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

class MBRect {
public:
    long left;
    long top;
    long right;
    long bottom;
};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBSurface : public IMBUnknown {
};

class IMBWinDC : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWinDC; }
    virtual long GetDC(HDC__ **, unsigned long) = 0;
    virtual long ReleaseDC(HDC__ *) = 0;
};

class IMBFont : public IMBUnknown {
public:
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual long DrawText(IMBSurface *, long, long, const unsigned short *const, unsigned long) = 0; // +0x18
    virtual long DrawText(IMBSurface *, MBRect &, const unsigned short *const, unsigned long) = 0;    // +0x14
    virtual long DrawText(IMBSurface *, long, long, const char *const, unsigned long) = 0;           // +0x10
    virtual long DrawText(IMBSurface *, MBRect &, const char *const, unsigned long) = 0;              // +0x0c
    virtual long GetTextSize(IMBSurface *, const unsigned short *, unsigned long, MBSize &) = 0;
    virtual long GetTextSize(IMBSurface *, const char *, unsigned long, MBSize &) = 0;
};

class IMBSystemFont : public IMBFont {
public:
    virtual long LoadFont(IMBSurface *, const char *, short, const MBColor &, unsigned long) = 0;
};

template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
private:
    template <class U> long _QueryInterface(U *const &p) {
        long hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

class CMBWinSystemFont : public IMBSystemFont {
public:
    CMBWinSystemFont(void);
    virtual ~CMBWinSystemFont(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long LoadFont(IMBSurface *dest, const char *name, short pointsize, const MBColor &color, unsigned long flags);
    virtual long DrawText(IMBSurface *dest, MBRect &destRect, const unsigned short *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, long x, long y, const unsigned short *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, MBRect &destRect, const char *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, long x, long y, const char *const text, unsigned long count);
    virtual long GetTextSize(IMBSurface *dest, const unsigned short *text, unsigned long count, MBSize &size);
    virtual long GetTextSize(IMBSurface *dest, const char *text, unsigned long count, MBSize &size);
protected:
    void UpdateCodePage(void);
    unsigned int GetCharSetForCodePage(void);
    char *ConvertUnicodeToSystemCodePage(const unsigned short *data, unsigned long count);
    unsigned short mPointSize;   // +0x08
    unsigned long mColor;        // +0x0c
    HFONT mSystemFont;           // +0x10
    HDC mCachedOuputDC;          // +0x14
    static unsigned int sCurrentCodePage;
};

IMBUnknown *MakeCMBWinSystemFont(void);

#endif
