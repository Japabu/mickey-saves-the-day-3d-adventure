// CMBMemorySurface.h - declarations for MBEngine.dll CMBMemorySurface.obj
// (also used by CMBWinDIBSurface.cpp). Colour header copied from CMBSurfaceObject.h,
// plus the default arguments (PDB `__dflt_ctor_closure`) and MBColor::Set(value, mask).
#ifndef CMBMEMORYSURFACE_H
#define CMBMEMORYSURFACE_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

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

// PDB ldata (CMBMemorySurface.obj 0x1008ff18..): IMBUnknown, IMBClassFactory, IMBSurface,
// IMBMemorySurface.
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// ---- MBColor header: masks and colour constants (internal linkage, one $E each) ----
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
class MBColorMask {
public:
    MBColorMask(unsigned long redmask = 0, unsigned long greenmask = 0, unsigned long bluemask = 0,
                unsigned long alphamask = 0) { Set(redmask, greenmask, bluemask, alphamask); }
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

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0)
        : mColor(0) { Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    void Set(unsigned long color, MBColorMask mask) {
        mColor = color;
        if (mask.mRedMask)
            mRed = (unsigned char)(((color & mask.mRedMask) * 255) / mask.mRedMask);
        if (mask.mGreenMask)
            mGreen = (unsigned char)(((color & mask.mGreenMask) * 255) / mask.mGreenMask);
        if (mask.mBlueMask)
            mBlue = (unsigned char)(((color & mask.mBlueMask) * 255) / mask.mBlueMask);
        if (mask.mAlphaMask)
            mAlpha = (unsigned char)(((color & mask.mAlphaMask) * 255) / mask.mAlphaMask);
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

// ---- geometry ----
// own copy: CMBWinDIBSurface.cpp, which includes this header, defines its own MBmin (a < b ? a : b), which MBRect.h's MBMinMax.h would collide with
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    long Width(void) const { return right - left; }
    long Height(void) const { return bottom - top; }
    long left;
    long top;
    long right;
    long bottom;
};

// ---- interfaces ----
class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBStream;

// own copy: built on this unit's own MBColorMask
struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};
class MBPalette {
};

#include "IMBSurface.h"

// MBMemoryBlit.obj
bool MBBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
            MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth);
bool MBBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                 MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                 unsigned long trans_value);
bool MBFill(void *dest, unsigned long width, unsigned long height, unsigned long pitch,
            unsigned long depth, unsigned long fill_value);
bool MBStretchBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                   MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth);
bool MBStretchBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                        MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                        unsigned long trans_value);

#endif
