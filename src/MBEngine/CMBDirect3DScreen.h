// CMBDirect3DScreen.h - declarations for MBEngine.dll CMBDirect3DScreen.obj
// (shared with CMBDirectDrawScreenManager): colors, screen interfaces, the DirectDraw 7
// pieces the installed (DirectX 5) SDK headers lack.
#ifndef CMBDIRECT3DSCREEN_H
#define CMBDIRECT3DSCREEN_H

#include <windows.h>
#include <ddraw.h>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

// HMONITOR is declared by windef.h only for WINVER >= 0x0500
struct HMONITOR__ { int unused; };
typedef int (__stdcall *MBDDEnumCallback)(_GUID *, char *, char *, void *, HMONITOR__ *);

// ---- DirectDraw 7 (not in the DirectX 5 headers) ----
typedef struct _DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    DWORD dwCaps4;
} DDSCAPS2;

typedef struct _DDSURFACEDESC2 {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    LONG lPitch;
    DWORD dwBackBufferCount;
    DWORD dwRefreshRate;
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    LPVOID lpSurface;
    DDCOLORKEY ddckCKDestOverlay;
    DDCOLORKEY ddckCKDestBlt;
    DDCOLORKEY ddckCKSrcOverlay;
    DDCOLORKEY ddckCKSrcBlt;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS2 ddsCaps;
    DWORD dwTextureStage;
} DDSURFACEDESC2;

struct IDirectDrawSurface7;

#include "IDirectDraw7.h"

#include "IDirectDrawSurface7.h"

// ---- MB core ----
#include "../MBCom/MBGUID.h"

#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventListener = {0x799755d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreenManager = {0x79df27a0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreen = {0x79d783b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSurface = {0x7ee752b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBDirectDrawSurface = {0x7ef9a8f0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectDrawSurface = {0x7f05e270, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBDirectDrawScreen = {0x7effc5b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBWinScreen = {0x7f9eb130, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectDrawScreenManager = {0x7faaeab0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBDirect3DScreen = {0x7fdedf10, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectXScreen = {0x7fe4fbd0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "MBPointStruct.h"
#include "../MBEngine/MBPoint.h"

// own copy: this unit uses Contains (in CMBDirectDrawScreenManager), whose inline body here differs from MBRect.h (store order / code shape the matches depend on)
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    bool Contains(const MBPoint &p) const {
        if (left <= p.x && top <= p.y && right >= p.x && bottom >= p.y)
            return true;
        return false;
    }
    long left;
    long top;
    long right;
    long bottom;
};

// ---- colors (header statics: every including TU has its own $E initializers) ----
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

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

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
            mColor = (mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor = (mRed >> -mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= (mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= (mGreen >> -mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= (mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= (mBlue >> -mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= (mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= (mAlpha >> -mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    unsigned char GetRed(void) const { return mRed; }
    unsigned char GetBlue(void) const { return mBlue; }
    unsigned char GetGreen(void) const { return mGreen; }
    unsigned char GetAlpha(void) const { return mAlpha; }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

const MBColor kWhite(0xff, 0xff, 0xff, 0);
const MBColor kBlack(0, 0, 0, 0);
const MBColor kRed(0xff, 0, 0, 0);
const MBColor kGreen(0, 0xff, 0, 0);
const MBColor kBlue(0, 0, 0xff, 0);
const MBColor kYellow(0xff, 0xff, 0, 0);
const MBColor kCyan(0, 0xff, 0xff, 0);
const MBColor kMagenta(0xff, 0, 0xff, 0);
const MBColor kGrey25(0x40, 0x40, 0x40, 0);
const MBColor kGrey50(0x80, 0x80, 0x80, 0);
const MBColor kGrey75(0xc0, 0xc0, 0xc0, 0);

// ---- screen interfaces ----
#include "MBScreenState.h"

#include "MBScreenAttributes.h"

struct MBPixelFormat;
class MBPalette;

#include "IMBSurface.h"

#include "IMBDirectDrawSurface.h"

#include "IMBEventListener.h"

#include "IMBScreen.h"

#include "IMBWinScreen.h"

#include "IMBDirectXScreen.h"

class CMBDirectDrawScreen : public IMBDirectXScreen, public IMBEventListener {
protected:
    ~CMBDirectDrawScreen(void);
public:
    CMBDirectDrawScreen(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool IsFullscreen(void);
    virtual void SetScreenState(MBScreenState screenState);
    virtual MBScreenState GetScreenState(void);
    virtual bool EnterWindowedMode(unsigned short bufferwidth, unsigned short bufferheight);
    virtual bool EnterFullscreenMode(unsigned short width, unsigned short height, unsigned short depth);
    virtual void ExitMode(void);
    virtual bool CreateSurface(unsigned short width, unsigned short height, unsigned long surfaceflags, IMBSurface **ptr);
    virtual IDirectDrawSurface7 *GetBackBuffer(void);
    virtual void GetBackBuffer(IMBSurface **surface);
    virtual void Flip(void);
    virtual unsigned short GetWidth(void);
    virtual unsigned short GetHeight(void);
    virtual MBRect GetGlobalBounds(void);
    virtual MBPoint GlobalToScreenPoint(const MBPoint &globalPoint);
    virtual void SetHWND(HWND__ *hWnd);
    virtual HWND__ *GetHWND(void);
    virtual long SetupBuffers(unsigned long width, unsigned long height, unsigned long depth);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual IDirectDraw7 *GetDirectDraw7(void);
    virtual _GUID *GetDeviceGUID(void);
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback func);
    virtual IDirectDrawSurface7 *GetFrontBuffer(void);
    virtual bool IsInGDIMode(void);
    virtual void GDIModeUpdateRect(tagRECT &r);
protected:
    bool mIsFullscreen;
    bool mGDIInUse;
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
    HWND__ *mWindow;
    IMBDirectDrawSurface *mBackBuffer;
    IMBDirectDrawSurface *mFrontBuffer;
    IDirectDraw7 *mDirectDraw;
    long mSavedWindowStyle;
    MBScreenState mScreenState;
    MBDDEnumCallback mDirectDrawEnumFunc;
    _GUID *mGUID;
};

class CMBDirect3DScreen : public CMBDirectDrawScreen {
protected:
    ~CMBDirect3DScreen(void);
public:
    CMBDirect3DScreen(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long SetupBuffers(unsigned long width, unsigned long height, unsigned long depth);
};

IMBUnknown *MakeCMBDirect3DScreen(void);

#endif
