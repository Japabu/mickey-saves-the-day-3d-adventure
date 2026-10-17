// MBScreenUtils.h - declarations for MBEngine.dll MBScreenUtils.obj
// (also used by MBMouseUtils and MBMsgBoxWin: same header-defined colour statics)
#ifndef MBSCREENUTILS_H
#define MBSCREENUTILS_H

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// header-defined interface IDs (local data of every object that includes them)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBEventManager = { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBEventManager = { 0x79a51680, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBView = { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager = { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBDirectXScreen = { 0x7fe4fbd0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
class MBColorMask {
public:
    MBColorMask(unsigned long r, unsigned long g, unsigned long b, unsigned long a) { Set(r, g, b, a); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask, unsigned long alphamask) {
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

static MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor = ((unsigned long)mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor = ((unsigned long)mRed >> -mask.mRedShift) & mask.mRedMask;
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

static MBColor kWhite(255, 255, 255, 0);
static MBColor kBlack(0, 0, 0, 0);
static MBColor kRed(255, 0, 0, 0);
static MBColor kGreen(0, 255, 0, 0);
static MBColor kBlue(0, 0, 255, 0);
static MBColor kYellow(255, 255, 0, 0);
static MBColor kCyan(0, 255, 255, 0);
static MBColor kMagenta(255, 0, 255, 0);
static MBColor kGrey25(64, 64, 64, 0);
static MBColor kGrey50(128, 128, 128, 0);
static MBColor kGrey75(192, 192, 192, 0);

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

#include "MBPointStruct.h"

// own copy: this unit uses the default constructor (x = y = 0), whose inline body here differs from MBPoint.h (store order / code shape the matches depend on)
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

// own copy: a plain struct without constructors
struct MBRect {
    long left, top, right, bottom;
};

struct MBScreenAttributes;
#include "MBScreenState.h"
class IMBSurface;
class IMBWorld;

#include "IMBScreen.h"

#include "IMBScreenManager.h"

extern "C" {
void MBRegisterScreenManager(IMBScreenManager *screenManager);
void MBGetScreenManager(IMBScreenManager **screenManager);
bool MBTransformGlobalToScreenPoint(const MBPoint &globalPoint, MBPoint &screenPoint, IMBScreen **screen);
}

#endif
