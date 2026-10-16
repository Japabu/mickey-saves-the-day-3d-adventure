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

struct IDirectDraw7 : public IUnknown {
    virtual HRESULT __stdcall Compact(void) = 0;
    virtual HRESULT __stdcall CreateClipper(DWORD, IDirectDrawClipper **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreatePalette(DWORD, LPPALETTEENTRY, IDirectDrawPalette **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreateSurface(DDSURFACEDESC2 *, IDirectDrawSurface7 **, IUnknown *) = 0;
    // rest of the vtable added for CMBDirectDrawScreen (RestoreDisplayMode +0x4c,
    // SetCooperativeLevel +0x50, SetDisplayMode +0x54)
    virtual HRESULT __stdcall DuplicateSurface(IDirectDrawSurface7 *, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall EnumDisplayModes(DWORD, DDSURFACEDESC2 *, LPVOID, LPVOID) = 0;
    virtual HRESULT __stdcall EnumSurfaces(DWORD, DDSURFACEDESC2 *, LPVOID, LPVOID) = 0;
    virtual HRESULT __stdcall FlipToGDISurface(void) = 0;
    virtual HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS) = 0;
    virtual HRESULT __stdcall GetDisplayMode(DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall GetFourCCCodes(LPDWORD, LPDWORD) = 0;
    virtual HRESULT __stdcall GetGDISurface(IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall GetMonitorFrequency(LPDWORD) = 0;
    virtual HRESULT __stdcall GetScanLine(LPDWORD) = 0;
    virtual HRESULT __stdcall GetVerticalBlankStatus(LPBOOL) = 0;
    virtual HRESULT __stdcall Initialize(GUID *) = 0;
    virtual HRESULT __stdcall RestoreDisplayMode(void) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD) = 0;
};

struct IDirectDrawSurface7 : public IUnknown {
    virtual HRESULT __stdcall AddAttachedSurface(IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall AddOverlayDirtyRect(LPRECT) = 0;
    virtual HRESULT __stdcall Blt(LPRECT, IDirectDrawSurface7 *, LPRECT, DWORD, LPDDBLTFX) = 0;
    virtual HRESULT __stdcall BltBatch(LPDDBLTBATCH, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall BltFast(DWORD, DWORD, IDirectDrawSurface7 *, LPRECT, DWORD) = 0;
    virtual HRESULT __stdcall DeleteAttachedSurface(DWORD, IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall EnumAttachedSurfaces(LPVOID, LPVOID) = 0;
    virtual HRESULT __stdcall EnumOverlayZOrders(DWORD, LPVOID, LPVOID) = 0;
    virtual HRESULT __stdcall Flip(IDirectDrawSurface7 *, DWORD) = 0;
    virtual HRESULT __stdcall GetAttachedSurface(DDSCAPS2 *, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall GetBltStatus(DWORD) = 0;
    virtual HRESULT __stdcall GetCaps(DDSCAPS2 *) = 0;
    virtual HRESULT __stdcall GetClipper(IDirectDrawClipper **) = 0;
    virtual HRESULT __stdcall GetColorKey(DWORD, LPDDCOLORKEY) = 0;
    virtual HRESULT __stdcall GetDC(HDC *) = 0;
    virtual HRESULT __stdcall GetFlipStatus(DWORD) = 0;
    virtual HRESULT __stdcall GetOverlayPosition(LPLONG, LPLONG) = 0;
    virtual HRESULT __stdcall GetPalette(IDirectDrawPalette **) = 0;
    virtual HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT) = 0;
    virtual HRESULT __stdcall GetSurfaceDesc(DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall Initialize(IDirectDraw *, DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall IsLost(void) = 0;
    virtual HRESULT __stdcall Lock(LPRECT, DDSURFACEDESC2 *, DWORD, HANDLE) = 0;
    virtual HRESULT __stdcall ReleaseDC(HDC) = 0;
    virtual HRESULT __stdcall Restore(void) = 0;
    virtual HRESULT __stdcall SetClipper(IDirectDrawClipper *) = 0;
};

// ---- MB core ----
struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &g) const {
        return data1 == g.data1 && data2 == g.data2 && data3 == g.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1];
    }
};

static const MBGUID MBIID_IMBUnknown = {0x59d231f0, 0x9c6c, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
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

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

struct MBPointStruct { long x; long y; };
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

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
enum MBScreenState {
    kScreenStateActive = 0,
    kScreenStatePaused = 1,
    kScreenStateInactive = 2
};

struct MBScreenAttributes {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

struct MBPixelFormat;
class MBPalette;

class IMBSurface : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurface; }
    virtual unsigned long GetWidth(void) = 0;
    virtual unsigned long GetHeight(void) = 0;
    virtual unsigned long GetPitch(void) = 0;
    virtual unsigned long GetSize(void) = 0;
    virtual unsigned long GetDepth(void) = 0;
    virtual MBPixelFormat GetFormat(void) = 0;
    virtual long Lock(void **, unsigned long) = 0;
    virtual long Unlock(void **) = 0;
    virtual MBColor GetPixelColor(unsigned long, unsigned long) = 0;
    virtual long SetTransparentColor(MBColor) = 0;
    virtual MBColor GetTransparentColor(void) = 0;
    virtual long SetPalette(MBPalette *) = 0;
    virtual MBPalette *GetPalette(void) = 0;
    virtual long ColorFill(MBColor, MBRect *) = 0;
    virtual long Blit(IMBSurface *, MBRect *, MBRect *, unsigned long) = 0;
};

class IMBDirectDrawSurface : public IMBSurface {
public:
    virtual void SetSurface(IDirectDrawSurface7 *) = 0;
    virtual void GetSurface(IDirectDrawSurface7 **) = 0;
    virtual void SetSurfaceCopy(IDirectDrawSurface7 *) = 0;
    virtual void GetSurfaceCopy(IDirectDrawSurface7 **) = 0;
    virtual void QuerySurfaceInfo(void) = 0;
};

class IMBEventListener : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventListener; }
    virtual void ListenToEvent(long, long, void *) = 0;
};

class IMBScreen : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreen; }
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual bool CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
    virtual void Flip(void) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual MBRect GetGlobalBounds(void) = 0;
    virtual MBPoint GlobalToScreenPoint(const MBPoint &) = 0;
};

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND__ *) = 0;
    virtual HWND__ *GetHWND(void) = 0;
    virtual bool EnterWindowedMode(unsigned short, unsigned short) = 0;
    virtual bool EnterFullscreenMode(unsigned short, unsigned short, unsigned short) = 0;
};

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
    virtual bool IsFullscreen(void) = 0;
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback) = 0;
    virtual IDirectDraw7 *GetDirectDraw7(void) = 0;
    virtual _GUID *GetDeviceGUID(void) = 0;
    virtual IDirectDrawSurface7 *GetBackBuffer(void) = 0;
    virtual IDirectDrawSurface7 *GetFrontBuffer(void) = 0;
    virtual bool IsInGDIMode(void) = 0;
    virtual void GDIModeUpdateRect(tagRECT &) = 0;
};

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
