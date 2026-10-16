// ddraw.h - the DirectDraw API the game uses, written for the port.
//
// The game was built against the DirectX 5 header of Visual C++ 6; the DirectX 7
// interfaces (IDirectDraw7, IDirectDrawSurface7, DDSURFACEDESC2, ...) are declared by
// the game's own headers (SteamboatApp/d3d7.h, MBEngine/CMBDirect3DScreen.h). This file
// provides the older part those headers build on: the structures, flags, error codes
// and the DirectDraw 1 interfaces, with the original vtable order (the implementation
// in port/platform depends on it).
#ifndef PORT_DDRAW_H
#define PORT_DDRAW_H

#include <windows.h>

struct IDirectDraw;
struct IDirectDrawSurface;
struct IDirectDrawPalette;
struct IDirectDrawClipper;
struct _RGNDATA;
typedef struct IDirectDraw *LPDIRECTDRAW;
typedef struct IDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct IDirectDrawPalette *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawClipper *LPDIRECTDRAWCLIPPER;

// ---- results
#define DD_OK 0
#define DD_FALSE 1
#define MAKE_DDHRESULT(code) ((HRESULT)(0x88760000u + (code)))
#define DDERR_SURFACELOST MAKE_DDHRESULT(450)
#define DDERR_WASSTILLDRAWING MAKE_DDHRESULT(540)
#define DDERR_WRONGMODE MAKE_DDHRESULT(587)
#define DDERR_GENERIC E_FAIL
#define DDERR_UNSUPPORTED E_NOTIMPL
#define DDERR_INVALIDPARAMS E_INVALIDARG
#define DDERR_OUTOFMEMORY E_OUTOFMEMORY
#define DDERR_NOTFOUND MAKE_DDHRESULT(255)
#define DDERR_SURFACEBUSY MAKE_DDHRESULT(430)
#define DDERR_INVALIDPIXELFORMAT MAKE_DDHRESULT(145)
#define DDERR_NOTFLIPPABLE MAKE_DDHRESULT(582)
#define DDERR_NOTLOCKED MAKE_DDHRESULT(584)
#define DDENUMRET_CANCEL 0
#define DDENUMRET_OK 1

// ---- structures
typedef struct _DDCOLORKEY {
    DWORD dwColorSpaceLowValue;
    DWORD dwColorSpaceHighValue;
} DDCOLORKEY, *LPDDCOLORKEY;

typedef struct _DDSCAPS {
    DWORD dwCaps;
} DDSCAPS, *LPDDSCAPS;

typedef struct _DDPIXELFORMAT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    union {
        DWORD dwRGBBitCount;
        DWORD dwYUVBitCount;
        DWORD dwZBufferBitDepth;
        DWORD dwAlphaBitDepth;
        DWORD dwLuminanceBitCount;
        DWORD dwBumpBitCount;
    };
    union {
        DWORD dwRBitMask;
        DWORD dwYBitMask;
        DWORD dwStencilBitDepth;
        DWORD dwLuminanceBitMask;
        DWORD dwBumpDuBitMask;
    };
    union {
        DWORD dwGBitMask;
        DWORD dwUBitMask;
        DWORD dwZBitMask;
        DWORD dwBumpDvBitMask;
    };
    union {
        DWORD dwBBitMask;
        DWORD dwVBitMask;
        DWORD dwStencilBitMask;
        DWORD dwBumpLuminanceBitMask;
    };
    union {
        DWORD dwRGBAlphaBitMask;
        DWORD dwYUVAlphaBitMask;
        DWORD dwLuminanceAlphaBitMask;
        DWORD dwRGBZBitMask;
        DWORD dwYUVZBitMask;
    };
} DDPIXELFORMAT, *LPDDPIXELFORMAT;

// Blit parameters. The surface pointers of the original's unions are kept as
// pointers, so the structure is larger on 64-bit; the game only sets dwSize from
// sizeof and fills dwFillColor and the colour keys.
typedef struct _DDBLTFX {
    DWORD dwSize;
    DWORD dwDDFX;
    DWORD dwROP;
    DWORD dwDDROP;
    DWORD dwRotationAngle;
    DWORD dwZBufferOpCode;
    DWORD dwZBufferLow;
    DWORD dwZBufferHigh;
    DWORD dwZBufferBaseDest;
    DWORD dwZDestConstBitDepth;
    union {
        DWORD dwZDestConst;
        struct IDirectDrawSurface *lpDDSZBufferDest;
    };
    DWORD dwZSrcConstBitDepth;
    union {
        DWORD dwZSrcConst;
        struct IDirectDrawSurface *lpDDSZBufferSrc;
    };
    DWORD dwAlphaEdgeBlendBitDepth;
    DWORD dwAlphaEdgeBlend;
    DWORD dwReserved;
    DWORD dwAlphaDestConstBitDepth;
    union {
        DWORD dwAlphaDestConst;
        struct IDirectDrawSurface *lpDDSAlphaDest;
    };
    DWORD dwAlphaSrcConstBitDepth;
    union {
        DWORD dwAlphaSrcConst;
        struct IDirectDrawSurface *lpDDSAlphaSrc;
    };
    union {
        DWORD dwFillColor;
        DWORD dwFillDepth;
        DWORD dwFillPixel;
        struct IDirectDrawSurface *lpDDSPattern;
    };
    DDCOLORKEY ddckDestColorkey;
    DDCOLORKEY ddckSrcColorkey;
} DDBLTFX, *LPDDBLTFX;

typedef struct _DDBLTBATCH {
    RECT *lprDest;
    struct IDirectDrawSurface *lpDDSSrc;
    RECT *lprSrc;
    DWORD dwFlags;
    LPDDBLTFX lpDDBltFx;
} DDBLTBATCH, *LPDDBLTBATCH;

typedef struct _DDOVERLAYFX {
    DWORD dwSize;
    DWORD dwAlphaEdgeBlendBitDepth;
    DWORD dwAlphaEdgeBlend;
    DWORD dwReserved;
    DWORD dwAlphaDestConstBitDepth;
    union {
        DWORD dwAlphaDestConst;
        struct IDirectDrawSurface *lpDDSAlphaDest;
    };
    DWORD dwAlphaSrcConstBitDepth;
    union {
        DWORD dwAlphaSrcConst;
        struct IDirectDrawSurface *lpDDSAlphaSrc;
    };
    DDCOLORKEY dckDestColorkey;
    DDCOLORKEY dckSrcColorkey;
    DWORD dwDDFX;
    DWORD dwFlags;
} DDOVERLAYFX, *LPDDOVERLAYFX;

// The capability block is declared by the game's DirectX 7 header (_DDCAPS_DX7);
// only pointers to the older name are used.
struct _DDCAPS;
typedef struct _DDCAPS DDCAPS, *LPDDCAPS;

// ---- surface description flags (DDSD_)
#define DDSD_CAPS 0x00000001l
#define DDSD_HEIGHT 0x00000002l
#define DDSD_WIDTH 0x00000004l
#define DDSD_PITCH 0x00000008l
#define DDSD_BACKBUFFERCOUNT 0x00000020l
#define DDSD_ZBUFFERBITDEPTH 0x00000040l
#define DDSD_ALPHABITDEPTH 0x00000080l
#define DDSD_LPSURFACE 0x00000800l
#define DDSD_PIXELFORMAT 0x00001000l
#define DDSD_CKDESTOVERLAY 0x00002000l
#define DDSD_CKDESTBLT 0x00004000l
#define DDSD_CKSRCOVERLAY 0x00008000l
#define DDSD_CKSRCBLT 0x00010000l
#define DDSD_MIPMAPCOUNT 0x00020000l
#define DDSD_REFRESHRATE 0x00040000l
#define DDSD_LINEARSIZE 0x00080000l

// ---- surface capabilities (DDSCAPS_)
#define DDSCAPS_ALPHA 0x00000002l
#define DDSCAPS_BACKBUFFER 0x00000004l
#define DDSCAPS_COMPLEX 0x00000008l
#define DDSCAPS_FLIP 0x00000010l
#define DDSCAPS_FRONTBUFFER 0x00000020l
#define DDSCAPS_OFFSCREENPLAIN 0x00000040l
#define DDSCAPS_OVERLAY 0x00000080l
#define DDSCAPS_PALETTE 0x00000100l
#define DDSCAPS_PRIMARYSURFACE 0x00000200l
#define DDSCAPS_SYSTEMMEMORY 0x00000800l
#define DDSCAPS_TEXTURE 0x00001000l
#define DDSCAPS_3DDEVICE 0x00002000l
#define DDSCAPS_VIDEOMEMORY 0x00004000l
#define DDSCAPS_VISIBLE 0x00008000l
#define DDSCAPS_WRITEONLY 0x00010000l
#define DDSCAPS_ZBUFFER 0x00020000l
#define DDSCAPS_OWNDC 0x00040000l
#define DDSCAPS_MIPMAP 0x00400000l
#define DDSCAPS_ALLOCONLOAD 0x04000000l
#define DDSCAPS_LOCALVIDMEM 0x10000000l
#define DDSCAPS_NONLOCALVIDMEM 0x20000000l

// ---- pixel format flags (DDPF_)
#define DDPF_ALPHAPIXELS 0x00000001l
#define DDPF_ALPHA 0x00000002l
#define DDPF_FOURCC 0x00000004l
#define DDPF_PALETTEINDEXED4 0x00000008l
#define DDPF_PALETTEINDEXEDTO8 0x00000010l
#define DDPF_PALETTEINDEXED8 0x00000020l
#define DDPF_RGB 0x00000040l
#define DDPF_ZBUFFER 0x00000400l

// ---- driver capabilities (DDCAPS_, DDCAPS2_)
#define DDCAPS_3D 0x00000001l
#define DDCAPS_COLORKEY 0x00400000l
#define DDCAPS2_NONLOCALVIDMEM 0x00000200l

// ---- bit depths (DDBD_)
#define DDBD_1 0x00004000l
#define DDBD_2 0x00002000l
#define DDBD_4 0x00001000l
#define DDBD_8 0x00000800l
#define DDBD_16 0x00000400l
#define DDBD_24 0x00000200l
#define DDBD_32 0x00000100l

// ---- cooperative levels (DDSCL_)
#define DDSCL_FULLSCREEN 0x00000001l
#define DDSCL_ALLOWREBOOT 0x00000002l
#define DDSCL_NOWINDOWCHANGES 0x00000004l
#define DDSCL_NORMAL 0x00000008l
#define DDSCL_EXCLUSIVE 0x00000010l
#define DDSCL_ALLOWMODEX 0x00000040l

// ---- blits, flips, locks, colour keys, palettes
#define DDBLT_COLORFILL 0x00000400l
#define DDBLT_KEYDEST 0x00002000l
#define DDBLT_KEYSRC 0x00008000l
#define DDBLT_KEYSRCOVERRIDE 0x00010000l
#define DDBLT_DEPTHFILL 0x02000000l
#define DDBLT_WAIT 0x01000000l
#define DDBLTFAST_NOCOLORKEY 0x00000000
#define DDBLTFAST_SRCCOLORKEY 0x00000001
#define DDBLTFAST_DESTCOLORKEY 0x00000002
#define DDBLTFAST_WAIT 0x00000010
#define DDFLIP_WAIT 0x00000001l
#define DDLOCK_SURFACEMEMORYPTR 0x00000000l
#define DDLOCK_WAIT 0x00000001l
#define DDWAITVB_BLOCKBEGIN 0x00000001l
#define DDCKEY_COLORSPACE 0x00000001l
#define DDCKEY_DESTBLT 0x00000002l
#define DDCKEY_DESTOVERLAY 0x00000004l
#define DDCKEY_SRCBLT 0x00000008l
#define DDCKEY_SRCOVERLAY 0x00000010l
#define DDPCAPS_4BIT 0x00000001l
#define DDPCAPS_8BITENTRIES 0x00000002l
#define DDPCAPS_8BIT 0x00000004l
#define DDPCAPS_ALLOW256 0x00000040l
#define DDENUM_ATTACHEDSECONDARYDEVICES 0x00000001l
#define DDENUM_DETACHEDSECONDARYDEVICES 0x00000002l
#define DDENUM_NONDISPLAYDEVICES 0x00000004l

// ---- interfaces (DirectDraw 1 level; vtable order as in DirectX)
typedef BOOL(__stdcall *LPDDENUMCALLBACKA)(GUID *, LPSTR, LPSTR, LPVOID);
struct _DDSURFACEDESC;
typedef HRESULT(__stdcall *LPDDENUMMODESCALLBACK)(struct _DDSURFACEDESC *, LPVOID);
typedef HRESULT(__stdcall *LPDDENUMSURFACESCALLBACK)(struct IDirectDrawSurface *, struct _DDSURFACEDESC *, LPVOID);

struct IDirectDrawPalette : public IUnknown {
    virtual HRESULT __stdcall GetCaps(DWORD *) = 0;
    virtual HRESULT __stdcall GetEntries(DWORD, DWORD, DWORD, PALETTEENTRY *) = 0;
    virtual HRESULT __stdcall Initialize(IDirectDraw *, DWORD, PALETTEENTRY *) = 0;
    virtual HRESULT __stdcall SetEntries(DWORD, DWORD, DWORD, PALETTEENTRY *) = 0;
};

struct IDirectDrawClipper : public IUnknown {
    virtual HRESULT __stdcall GetClipList(RECT *, struct _RGNDATA *, DWORD *) = 0;
    virtual HRESULT __stdcall GetHWnd(HWND *) = 0;
    virtual HRESULT __stdcall Initialize(IDirectDraw *, DWORD) = 0;
    virtual HRESULT __stdcall IsClipListChanged(BOOL *) = 0;
    virtual HRESULT __stdcall SetClipList(struct _RGNDATA *, DWORD) = 0;
    virtual HRESULT __stdcall SetHWnd(DWORD, HWND) = 0;
};

struct IDirectDraw : public IUnknown {
    virtual HRESULT __stdcall Compact() = 0;
    virtual HRESULT __stdcall CreateClipper(DWORD, IDirectDrawClipper **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreatePalette(DWORD, PALETTEENTRY *, IDirectDrawPalette **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreateSurface(struct _DDSURFACEDESC *, IDirectDrawSurface **, IUnknown *) = 0;
    virtual HRESULT __stdcall DuplicateSurface(IDirectDrawSurface *, IDirectDrawSurface **) = 0;
    virtual HRESULT __stdcall EnumDisplayModes(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMMODESCALLBACK) = 0;
    virtual HRESULT __stdcall EnumSurfaces(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMSURFACESCALLBACK) = 0;
    virtual HRESULT __stdcall FlipToGDISurface() = 0;
    virtual HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS) = 0;
    virtual HRESULT __stdcall GetDisplayMode(struct _DDSURFACEDESC *) = 0;
    virtual HRESULT __stdcall GetFourCCCodes(DWORD *, DWORD *) = 0;
    virtual HRESULT __stdcall GetGDISurface(IDirectDrawSurface **) = 0;
    virtual HRESULT __stdcall GetMonitorFrequency(DWORD *) = 0;
    virtual HRESULT __stdcall GetScanLine(DWORD *) = 0;
    virtual HRESULT __stdcall GetVerticalBlankStatus(BOOL *) = 0;
    virtual HRESULT __stdcall Initialize(GUID *) = 0;
    virtual HRESULT __stdcall RestoreDisplayMode() = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE) = 0;
};

extern "C" HRESULT __stdcall DirectDrawCreate(GUID *, LPDIRECTDRAW *, IUnknown *);
extern "C" HRESULT __stdcall DirectDrawEnumerateA(LPDDENUMCALLBACKA, LPVOID);

#endif
