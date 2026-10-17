#pragma once
// d3d7.h: the DirectX 7 parts of the SDK's ddraw.h, d3d.h and d3dtypes.h the graphics library uses. The
// compiler's own include directory has the DirectX 5 headers, so the DirectX 6/7 types are
// declared here, from SteamboatApp.pdb.
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

struct IDirect3D7;

#ifndef DDCAPS2_CANRENDERWINDOWED
#define DDCAPS2_CANRENDERWINDOWED 0x00080000l
#endif
#ifndef DDSCL_MULTITHREADED
#define DDSCL_MULTITHREADED 0x00000400l
#endif
// Multiple-monitor handle (windows.h of VC6 predates it).
struct HMONITOR__;
typedef HMONITOR__ *HMONITOR;

// DirectX 7 entry points the VC6 ddraw.h lacks.
extern "C" const GUID IID_IDirectDraw7;
extern "C" HRESULT __stdcall DirectDrawCreateEx(GUID *, void **, REFIID, IUnknown *);
extern "C" HRESULT __stdcall DirectDrawEnumerateExA(BOOL(__stdcall *)(GUID *, LPSTR, LPSTR, LPVOID, HMONITOR), LPVOID, DWORD);
struct IDirect3DVertexBuffer7;
struct _D3DDeviceDesc7;
// Flexible vertex format flags (DirectX 6+).
#ifndef D3DFVF_XYZ
#define D3DFVF_XYZ 0x002
#endif
#ifndef D3DFVF_NORMAL
#define D3DFVF_NORMAL 0x010
#endif
#ifndef D3DDP_DONOTLIGHT
#define D3DDP_DONOTLIGHT 0x00000010l
#endif
#ifndef D3DFVF_RESERVED1
#define D3DFVF_RESERVED1 0x020
#endif
#ifndef D3DFVF_DIFFUSE
#define D3DFVF_DIFFUSE 0x040
#endif
#ifndef D3DFVF_SPECULAR
#define D3DFVF_SPECULAR 0x080
#endif
#ifndef D3DFVF_TEX0
#define D3DFVF_TEX0 0x000
#endif
#ifndef D3DFVF_TEX1
#define D3DFVF_TEX1 0x100
#endif
#ifndef D3DFVF_TEX2
#define D3DFVF_TEX2 0x200
#endif
#ifndef D3DFVF_TEX3
#define D3DFVF_TEX3 0x300
#endif
#ifndef D3DFVF_TEX4
#define D3DFVF_TEX4 0x400
#endif
#ifndef D3DFVF_TEX5
#define D3DFVF_TEX5 0x500
#endif
#ifndef D3DFVF_TEX6
#define D3DFVF_TEX6 0x600
#endif
#ifndef D3DFVF_TEX7
#define D3DFVF_TEX7 0x700
#endif
#ifndef D3DFVF_TEX8
#define D3DFVF_TEX8 0x800
#endif

// Vertex data for the strided draw calls: one pointer and stride per component.
typedef struct _D3DDP_PTRSTRIDE {
    LPVOID lpvData;
    DWORD dwStride;
} D3DDP_PTRSTRIDE;

typedef struct _D3DDRAWPRIMITIVESTRIDEDDATA {
    D3DDP_PTRSTRIDE position;
    D3DDP_PTRSTRIDE normal;
    D3DDP_PTRSTRIDE diffuse;
    D3DDP_PTRSTRIDE specular;
    D3DDP_PTRSTRIDE textureCoords[8];
} D3DDRAWPRIMITIVESTRIDEDDATA;

typedef struct _DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    union {
        DWORD dwCaps4;
        DWORD dwVolumeDepth;
    };
} DDSCAPS2, *LPDDSCAPS2;

typedef struct _DDSURFACEDESC2 {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    union {
        LONG lPitch;
        DWORD dwLinearSize;
    };
    union {
        DWORD dwBackBufferCount;
        DWORD dwDepth;
    };
    union {
        DWORD dwMipMapCount;
        DWORD dwRefreshRate;
        DWORD dwSrcVBHandle;
    };
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    LPVOID lpSurface;
    union {
        DDCOLORKEY ddckCKDestOverlay;
        DWORD dwEmptyFaceColor;
    };
    DDCOLORKEY ddckCKDestBlt;
    DDCOLORKEY ddckCKSrcOverlay;
    DDCOLORKEY ddckCKSrcBlt;
    union {
        DDPIXELFORMAT ddpfPixelFormat;
        DWORD dwFVF;
    };
    DDSCAPS2 ddsCaps;
    DWORD dwTextureStage;
} DDSURFACEDESC2, *LPDDSURFACEDESC2;

typedef HRESULT(__stdcall *LPDDENUMSURFACESCALLBACK7)(struct IDirectDrawSurface7 *, DDSURFACEDESC2 *, void *);

// own copy: the whole DirectX 7 vtable; MBEngine's IDirectDrawSurface7.h declares the slots up to
// Unlock only, and MBEngine's DirectDraw headers do not declare DDSURFACEDESC2 and the other
// DirectX 7 types this header adds.
struct IDirectDrawSurface7 : public IUnknown {
    virtual HRESULT __stdcall AddAttachedSurface(IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall AddOverlayDirtyRect(RECT *) = 0;
    virtual HRESULT __stdcall Blt(RECT *, IDirectDrawSurface7 *, RECT *, DWORD, DDBLTFX *) = 0;
    virtual HRESULT __stdcall BltBatch(DDBLTBATCH *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall BltFast(DWORD, DWORD, IDirectDrawSurface7 *, RECT *, DWORD) = 0;
    virtual HRESULT __stdcall DeleteAttachedSurface(DWORD, IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall EnumAttachedSurfaces(void *, LPDDENUMSURFACESCALLBACK7) = 0;
    virtual HRESULT __stdcall EnumOverlayZOrders(DWORD, void *, LPDDENUMSURFACESCALLBACK7) = 0;
    virtual HRESULT __stdcall Flip(IDirectDrawSurface7 *, DWORD) = 0;
    virtual HRESULT __stdcall GetAttachedSurface(DDSCAPS2 *, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall GetBltStatus(DWORD) = 0;
    virtual HRESULT __stdcall GetCaps(DDSCAPS2 *) = 0;
    virtual HRESULT __stdcall GetClipper(IDirectDrawClipper **) = 0;
    virtual HRESULT __stdcall GetColorKey(DWORD, DDCOLORKEY *) = 0;
    virtual HRESULT __stdcall GetDC(HDC *) = 0;
    virtual HRESULT __stdcall GetFlipStatus(DWORD) = 0;
    virtual HRESULT __stdcall GetOverlayPosition(LONG *, LONG *) = 0;
    virtual HRESULT __stdcall GetPalette(IDirectDrawPalette **) = 0;
    virtual HRESULT __stdcall GetPixelFormat(DDPIXELFORMAT *) = 0;
    virtual HRESULT __stdcall GetSurfaceDesc(DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall Initialize(IDirectDraw *, DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall IsLost() = 0;
    virtual HRESULT __stdcall Lock(RECT *, DDSURFACEDESC2 *, DWORD, HANDLE) = 0;
    virtual HRESULT __stdcall ReleaseDC(HDC) = 0;
    virtual HRESULT __stdcall Restore() = 0;
    virtual HRESULT __stdcall SetClipper(IDirectDrawClipper *) = 0;
    virtual HRESULT __stdcall SetColorKey(DWORD, DDCOLORKEY *) = 0;
    virtual HRESULT __stdcall SetOverlayPosition(LONG, LONG) = 0;
    virtual HRESULT __stdcall SetPalette(IDirectDrawPalette *) = 0;
    virtual HRESULT __stdcall Unlock(RECT *) = 0;
    virtual HRESULT __stdcall UpdateOverlay(RECT *, IDirectDrawSurface7 *, RECT *, DWORD, DDOVERLAYFX *) = 0;
    virtual HRESULT __stdcall UpdateOverlayDisplay(DWORD) = 0;
    virtual HRESULT __stdcall UpdateOverlayZOrder(DWORD, IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall GetDDInterface(void **) = 0;
    virtual HRESULT __stdcall PageLock(DWORD) = 0;
    virtual HRESULT __stdcall PageUnlock(DWORD) = 0;
    virtual HRESULT __stdcall SetSurfaceDesc(DDSURFACEDESC2 *, DWORD) = 0;
    virtual HRESULT __stdcall SetPrivateData(REFGUID, void *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall GetPrivateData(REFGUID, void *, DWORD *) = 0;
    virtual HRESULT __stdcall FreePrivateData(REFGUID) = 0;
    virtual HRESULT __stdcall GetUniquenessValue(DWORD *) = 0;
    virtual HRESULT __stdcall ChangeUniquenessValue() = 0;
    virtual HRESULT __stdcall SetPriority(DWORD) = 0;
    virtual HRESULT __stdcall GetPriority(DWORD *) = 0;
    virtual HRESULT __stdcall SetLOD(DWORD) = 0;
    virtual HRESULT __stdcall GetLOD(DWORD *) = 0;
};

// D3DRENDERSTATETYPE values added after DirectX 5
#define D3DRENDERSTATE_AMBIENT ((D3DRENDERSTATETYPE)139)
#define D3DRENDERSTATE_LIGHTING ((D3DRENDERSTATETYPE)137)
#define D3DRENDERSTATE_EXTENTS ((D3DRENDERSTATETYPE)138)
#define D3DRENDERSTATE_TEXTUREFACTOR ((D3DRENDERSTATETYPE)60)

typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP = 1,
    D3DTSS_COLORARG1 = 2,
    D3DTSS_COLORARG2 = 3,
    D3DTSS_ALPHAOP = 4,
    D3DTSS_ALPHAARG1 = 5,
    D3DTSS_ALPHAARG2 = 6,
    D3DTSS_BUMPENVMAT00 = 7,
    D3DTSS_BUMPENVMAT01 = 8,
    D3DTSS_BUMPENVMAT10 = 9,
    D3DTSS_BUMPENVMAT11 = 10,
    D3DTSS_TEXCOORDINDEX = 11,
    D3DTSS_ADDRESS = 12,
    D3DTSS_ADDRESSU = 13,
    D3DTSS_ADDRESSV = 14,
    D3DTSS_BORDERCOLOR = 15,
    D3DTSS_MAGFILTER = 16,
    D3DTSS_MINFILTER = 17,
    D3DTSS_MIPFILTER = 18,
    D3DTSS_MIPMAPLODBIAS = 19,
    D3DTSS_MAXMIPLEVEL = 20,
    D3DTSS_MAXANISOTROPY = 21,
    D3DTSS_BUMPENVLSCALE = 22,
    D3DTSS_BUMPENVLOFFSET = 23,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24,
    D3DTSS_FORCE_DWORD = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

// texture argument flags (D3DTSS_COLORARG1 and the like)
#define D3DTA_SELECTMASK 0x0000000f
#define D3DTA_DIFFUSE 0x00000000
#define D3DTA_CURRENT 0x00000001
#define D3DTA_TEXTURE 0x00000002
#define D3DTA_TFACTOR 0x00000003
#define D3DTA_SPECULAR 0x00000004
#define D3DTA_COMPLEMENT 0x00000010
#define D3DTA_ALPHAREPLICATE 0x00000020

typedef enum _D3DTEXTUREOP {
    D3DTOP_DISABLE = 1,
    D3DTOP_SELECTARG1 = 2,
    D3DTOP_SELECTARG2 = 3,
    D3DTOP_MODULATE = 4,
    D3DTOP_MODULATE2X = 5,
    D3DTOP_MODULATE4X = 6,
    D3DTOP_ADD = 7,
    D3DTOP_ADDSIGNED = 8,
    D3DTOP_ADDSIGNED2X = 9,
    D3DTOP_SUBTRACT = 10,
    D3DTOP_ADDSMOOTH = 11,
    D3DTOP_BLENDDIFFUSEALPHA = 12,
    D3DTOP_BLENDTEXTUREALPHA = 13,
    D3DTOP_BLENDFACTORALPHA = 14,
    D3DTOP_BLENDTEXTUREALPHAPM = 15,
    D3DTOP_BLENDCURRENTALPHA = 16,
    D3DTOP_PREMODULATE = 17,
    D3DTOP_MODULATEALPHA_ADDCOLOR = 18,
    D3DTOP_MODULATECOLOR_ADDALPHA = 19,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
    D3DTOP_BUMPENVMAP = 22,
    D3DTOP_BUMPENVMAPLUMINANCE = 23,
    D3DTOP_DOTPRODUCT3 = 24,
    D3DTOP_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREOP;

typedef enum _D3DTEXTUREMAGFILTER {
    D3DTFG_POINT = 1,
    D3DTFG_LINEAR = 2,
    D3DTFG_FLATCUBIC = 3,
    D3DTFG_GAUSSIANCUBIC = 4,
    D3DTFG_ANISOTROPIC = 5,
    D3DTFG_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREMAGFILTER;

typedef enum _D3DTEXTUREMINFILTER {
    D3DTFN_POINT = 1,
    D3DTFN_LINEAR = 2,
    D3DTFN_ANISOTROPIC = 3,
    D3DTFN_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREMINFILTER;

typedef enum _D3DTEXTUREMIPFILTER {
    D3DTFP_NONE = 1,
    D3DTFP_POINT = 2,
    D3DTFP_LINEAR = 3,
    D3DTFP_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREMIPFILTER;

typedef enum _D3DSTATEBLOCKTYPE {
    D3DSBT_ALL = 1,
    D3DSBT_PIXELSTATE = 2,
    D3DSBT_VERTEXSTATE = 3,
    D3DSBT_FORCE_DWORD = 0xffffffff
} D3DSTATEBLOCKTYPE;

typedef struct _D3DLIGHT7 {
    D3DLIGHTTYPE dltType;
    D3DCOLORVALUE dcvDiffuse;
    D3DCOLORVALUE dcvSpecular;
    D3DCOLORVALUE dcvAmbient;
    D3DVECTOR dvPosition;
    D3DVECTOR dvDirection;
    D3DVALUE dvRange;
    D3DVALUE dvFalloff;
    D3DVALUE dvAttenuation0;
    D3DVALUE dvAttenuation1;
    D3DVALUE dvAttenuation2;
    D3DVALUE dvTheta;
    D3DVALUE dvPhi;
} D3DLIGHT7, *LPD3DLIGHT7;

typedef struct _D3DMATERIAL7 {
    union {
        D3DCOLORVALUE diffuse;
        D3DCOLORVALUE dcvDiffuse;
    };
    union {
        D3DCOLORVALUE ambient;
        D3DCOLORVALUE dcvAmbient;
    };
    union {
        D3DCOLORVALUE specular;
        D3DCOLORVALUE dcvSpecular;
    };
    union {
        D3DCOLORVALUE emissive;
        D3DCOLORVALUE dcvEmissive;
    };
    union {
        D3DVALUE power;
        D3DVALUE dvPower;
    };
} D3DMATERIAL7, *LPD3DMATERIAL7;

typedef struct _D3DVIEWPORT7 {
    DWORD dwX;
    DWORD dwY;
    DWORD dwWidth;
    DWORD dwHeight;
    D3DVALUE dvMinZ;
    D3DVALUE dvMaxZ;
} D3DVIEWPORT7, *LPD3DVIEWPORT7;

struct IDirect3DDevice7 : public IUnknown {
    virtual HRESULT __stdcall GetCaps(_D3DDeviceDesc7 *) = 0;
    virtual HRESULT __stdcall EnumTextureFormats(HRESULT(__stdcall *)(DDPIXELFORMAT *, void *), void *) = 0;
    virtual HRESULT __stdcall BeginScene() = 0;
    virtual HRESULT __stdcall EndScene() = 0;
    virtual HRESULT __stdcall GetDirect3D(IDirect3D7 **) = 0;
    virtual HRESULT __stdcall SetRenderTarget(IDirectDrawSurface7 *, DWORD) = 0;
    virtual HRESULT __stdcall GetRenderTarget(IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall Clear(DWORD, D3DRECT *, DWORD, D3DCOLOR, D3DVALUE, DWORD) = 0;
    virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) = 0;
    virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) = 0;
    virtual HRESULT __stdcall SetViewport(D3DVIEWPORT7 *) = 0;
    virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) = 0;
    virtual HRESULT __stdcall GetViewport(D3DVIEWPORT7 *) = 0;
    virtual HRESULT __stdcall SetMaterial(D3DMATERIAL7 *) = 0;
    virtual HRESULT __stdcall GetMaterial(D3DMATERIAL7 *) = 0;
    virtual HRESULT __stdcall SetLight(DWORD, D3DLIGHT7 *) = 0;
    virtual HRESULT __stdcall GetLight(DWORD, D3DLIGHT7 *) = 0;
    virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE, DWORD) = 0;
    virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE, DWORD *) = 0;
    virtual HRESULT __stdcall BeginStateBlock() = 0;
    virtual HRESULT __stdcall EndStateBlock(DWORD *) = 0;
    virtual HRESULT __stdcall PreLoad(IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE, DWORD, void *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, DWORD, void *, DWORD, WORD *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall SetClipStatus(D3DCLIPSTATUS *) = 0;
    virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS *) = 0;
    virtual HRESULT __stdcall DrawPrimitiveStrided(D3DPRIMITIVETYPE, DWORD, _D3DDRAWPRIMITIVESTRIDEDDATA *, DWORD,
                                                   DWORD) = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE, DWORD, _D3DDRAWPRIMITIVESTRIDEDDATA *,
                                                          DWORD, WORD *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall DrawPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, WORD *,
                                                     DWORD, DWORD) = 0;
    virtual HRESULT __stdcall ComputeSphereVisibility(D3DVECTOR *, D3DVALUE *, DWORD, DWORD, DWORD *) = 0;
    virtual HRESULT __stdcall GetTexture(DWORD, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall SetTexture(DWORD, IDirectDrawSurface7 *) = 0;
    virtual HRESULT __stdcall GetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD *) = 0;
    virtual HRESULT __stdcall SetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) = 0;
    virtual HRESULT __stdcall ValidateDevice(DWORD *) = 0;
    virtual HRESULT __stdcall ApplyStateBlock(DWORD) = 0;
    virtual HRESULT __stdcall CaptureStateBlock(DWORD) = 0;
    virtual HRESULT __stdcall DeleteStateBlock(DWORD) = 0;
    virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE, DWORD *) = 0;
    virtual HRESULT __stdcall Load(IDirectDrawSurface7 *, POINT *, IDirectDrawSurface7 *, RECT *, DWORD) = 0;
    virtual HRESULT __stdcall LightEnable(DWORD, BOOL) = 0;
    virtual HRESULT __stdcall GetLightEnable(DWORD, BOOL *) = 0;
    virtual HRESULT __stdcall SetClipPlane(DWORD, D3DVALUE *) = 0;
    virtual HRESULT __stdcall GetClipPlane(DWORD, D3DVALUE *) = 0;
    virtual HRESULT __stdcall GetInfo(DWORD, void *, DWORD) = 0;
};

struct _DDCAPS_DX7;
typedef struct tagDDDEVICEIDENTIFIER2 {
    char szDriver[512];
    char szDescription[512];
    LARGE_INTEGER liDriverVersion;
    DWORD dwVendorId;
    DWORD dwDeviceId;
    DWORD dwSubSysId;
    DWORD dwRevision;
    GUID guidDeviceIdentifier;
    DWORD dwWHQLLevel;
} DDDEVICEIDENTIFIER2;

// own copy: the whole DirectX 7 vtable; MBEngine's IDirectDraw7.h ends at SetDisplayMode and
// declares GetCaps with the DirectX 5 DDCAPS.
struct IDirectDraw7 : public IUnknown {
    virtual HRESULT __stdcall Compact() = 0;
    virtual HRESULT __stdcall CreateClipper(DWORD, IDirectDrawClipper **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreatePalette(DWORD, PALETTEENTRY *, IDirectDrawPalette **, IUnknown *) = 0;
    virtual HRESULT __stdcall CreateSurface(DDSURFACEDESC2 *, IDirectDrawSurface7 **, IUnknown *) = 0;
    virtual HRESULT __stdcall DuplicateSurface(IDirectDrawSurface7 *, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall EnumDisplayModes(DWORD, DDSURFACEDESC2 *, void *,
                                               HRESULT(__stdcall *)(DDSURFACEDESC2 *, void *)) = 0;
    virtual HRESULT __stdcall EnumSurfaces(DWORD, DDSURFACEDESC2 *, void *, LPDDENUMSURFACESCALLBACK7) = 0;
    virtual HRESULT __stdcall FlipToGDISurface() = 0;
    virtual HRESULT __stdcall GetCaps(_DDCAPS_DX7 *, _DDCAPS_DX7 *) = 0;
    virtual HRESULT __stdcall GetDisplayMode(DDSURFACEDESC2 *) = 0;
    virtual HRESULT __stdcall GetFourCCCodes(DWORD *, DWORD *) = 0;
    virtual HRESULT __stdcall GetGDISurface(IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall GetMonitorFrequency(DWORD *) = 0;
    virtual HRESULT __stdcall GetScanLine(DWORD *) = 0;
    virtual HRESULT __stdcall GetVerticalBlankStatus(BOOL *) = 0;
    virtual HRESULT __stdcall Initialize(GUID *) = 0;
    virtual HRESULT __stdcall RestoreDisplayMode() = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE) = 0;
    virtual HRESULT __stdcall GetAvailableVidMem(DDSCAPS2 *, DWORD *, DWORD *) = 0;
    virtual HRESULT __stdcall GetSurfaceFromDC(HDC, IDirectDrawSurface7 **) = 0;
    virtual HRESULT __stdcall RestoreAllSurfaces() = 0;
    virtual HRESULT __stdcall TestCooperativeLevel() = 0;
    virtual HRESULT __stdcall GetDeviceIdentifier(tagDDDEVICEIDENTIFIER2 *, DWORD) = 0;
    virtual HRESULT __stdcall StartModeTest(SIZE *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall EvaluateMode(DWORD, DWORD *) = 0;
};

typedef struct _DDCAPS_DX7 {
    unsigned long dwSize;
    unsigned long dwCaps;
    unsigned long dwCaps2;
    unsigned long dwCKeyCaps;
    unsigned long dwFXCaps;
    unsigned long dwFXAlphaCaps;
    unsigned long dwPalCaps;
    unsigned long dwSVCaps;
    unsigned long dwAlphaBltConstBitDepths;
    unsigned long dwAlphaBltPixelBitDepths;
    unsigned long dwAlphaBltSurfaceBitDepths;
    unsigned long dwAlphaOverlayConstBitDepths;
    unsigned long dwAlphaOverlayPixelBitDepths;
    unsigned long dwAlphaOverlaySurfaceBitDepths;
    unsigned long dwZBufferBitDepths;
    unsigned long dwVidMemTotal;
    unsigned long dwVidMemFree;
    unsigned long dwMaxVisibleOverlays;
    unsigned long dwCurrVisibleOverlays;
    unsigned long dwNumFourCCCodes;
    unsigned long dwAlignBoundarySrc;
    unsigned long dwAlignSizeSrc;
    unsigned long dwAlignBoundaryDest;
    unsigned long dwAlignSizeDest;
    unsigned long dwAlignStrideAlign;
    unsigned long dwRops[8];
    DDSCAPS ddsOldCaps;
    unsigned long dwMinOverlayStretch;
    unsigned long dwMaxOverlayStretch;
    unsigned long dwMinLiveVideoStretch;
    unsigned long dwMaxLiveVideoStretch;
    unsigned long dwMinHwCodecStretch;
    unsigned long dwMaxHwCodecStretch;
    unsigned long dwReserved1;
    unsigned long dwReserved2;
    unsigned long dwReserved3;
    unsigned long dwSVBCaps;
    unsigned long dwSVBCKeyCaps;
    unsigned long dwSVBFXCaps;
    unsigned long dwSVBRops[8];
    unsigned long dwVSBCaps;
    unsigned long dwVSBCKeyCaps;
    unsigned long dwVSBFXCaps;
    unsigned long dwVSBRops[8];
    unsigned long dwSSBCaps;
    unsigned long dwSSBCKeyCaps;
    unsigned long dwSSBFXCaps;
    unsigned long dwSSBRops[8];
    unsigned long dwMaxVideoPorts;
    unsigned long dwCurrVideoPorts;
    unsigned long dwSVBCaps2;
    unsigned long dwNLVBCaps;
    unsigned long dwNLVBCaps2;
    unsigned long dwNLVBCKeyCaps;
    unsigned long dwNLVBFXCaps;
    unsigned long dwNLVBRops[8];
    DDSCAPS2 ddsCaps;
} DDCAPS_DX7;

typedef struct _D3DDeviceDesc7 {
    unsigned long dwDevCaps;
    D3DPRIMCAPS dpcLineCaps;
    D3DPRIMCAPS dpcTriCaps;
    unsigned long dwDeviceRenderBitDepth;
    unsigned long dwDeviceZBufferBitDepth;
    unsigned long dwMinTextureWidth;
    unsigned long dwMinTextureHeight;
    unsigned long dwMaxTextureWidth;
    unsigned long dwMaxTextureHeight;
    unsigned long dwMaxTextureRepeat;
    unsigned long dwMaxTextureAspectRatio;
    unsigned long dwMaxAnisotropy;
    float dvGuardBandLeft;
    float dvGuardBandTop;
    float dvGuardBandRight;
    float dvGuardBandBottom;
    float dvExtentsAdjust;
    unsigned long dwStencilCaps;
    unsigned long dwFVFCaps;
    unsigned long dwTextureOpCaps;
    unsigned short wMaxTextureBlendStages;
    unsigned short wMaxSimultaneousTextures;
    unsigned long dwMaxActiveLights;
    float dvMaxVertexW;
    GUID deviceGUID;
    unsigned short wMaxUserClipPlanes;
    unsigned short wMaxVertexBlendMatrices;
    unsigned long dwVertexProcessingCaps;
    unsigned long dwReserved1;
    unsigned long dwReserved2;
    unsigned long dwReserved3;
    unsigned long dwReserved4;
} D3DDEVICEDESC7;

typedef HRESULT(__stdcall *LPD3DENUMDEVICESCALLBACK7)(char *, char *, D3DDEVICEDESC7 *, void *);
typedef HRESULT(__stdcall *LPD3DENUMPIXELFORMATSCALLBACK)(DDPIXELFORMAT *, void *);

extern "C" const GUID IID_IDirect3D7;
extern "C" const GUID IID_IDirect3DRefDevice;
#ifndef D3DDEVCAPS_HWRASTERIZATION
#define D3DDEVCAPS_HWRASTERIZATION 0x00080000L
#endif

struct IDirect3D7 : public IUnknown {
    virtual HRESULT __stdcall EnumDevices(LPD3DENUMDEVICESCALLBACK7, void *) = 0;
    virtual HRESULT __stdcall CreateDevice(const GUID &, IDirectDrawSurface7 *, IDirect3DDevice7 **) = 0;
    virtual HRESULT __stdcall CreateVertexBuffer(struct _D3DVERTEXBUFFERDESC *, IDirect3DVertexBuffer7 **, unsigned long) = 0;
    virtual HRESULT __stdcall EnumZBufferFormats(const GUID &, LPD3DENUMPIXELFORMATSCALLBACK, void *) = 0;
    virtual HRESULT __stdcall EvictManagedTextures() = 0;
};
