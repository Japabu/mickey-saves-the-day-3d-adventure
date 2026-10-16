#pragma once
#include "d3d7.h"
#include "ValList.h"

namespace DX {
class Direct3D;
class DirectDrawSurface;
struct DirectDrawSurfaceAttrib;

// The Direct3D device: the IDirect3DDevice7 with the render and texture
// stage states the library last set (declaration from SteamboatApp.pdb).
class Device {
public:
    struct D3DRenderState {
        void Reset();
        BOOL m_bZEnable;
        BOOL m_bZWriteEnable;
        BOOL m_bEnableColorKeying;
        BOOL m_bEnableAlphaBlending;
        BOOL m_bEnableAlphaTesting;
        D3DCMPFUNC m_kAlphaFunction;
        D3DBLEND m_kSrcBlend;
        D3DBLEND m_kDestBlend;
        BOOL m_bPerspCorrect;
        D3DSHADEMODE m_kShadeMode;
        D3DFILLMODE m_kFillMode;
        D3DCULL m_kCullMode;
        BOOL m_bDithering;
        BOOL m_bSpecular;
        BOOL m_bAntialiasing;
        BOOL m_bFogEnabled;
        DWORD m_FogColor;
        D3DFOGMODE m_FogMode;
        float m_FogStart;
        float m_FogEnd;
        BOOL m_bTexturesOn;
    };
    struct D3DTextureStageState {
        void Reset();
        D3DTEXTUREMIPFILTER m_kMipTextureFilter;
        D3DTEXTUREMAGFILTER m_kMagTextureFilter;
        D3DTEXTUREMINFILTER m_kMinTextureFilter;
        D3DTEXTUREOP m_kColorOp;
        int m_kColorArg1;
        int m_kColorArg2;
        D3DTEXTUREOP m_kAlphaOp;
        int m_kAlphaArg1;
        int m_kAlphaArg2;
        D3DTEXTUREADDRESS m_kAddressU;
        D3DTEXTUREADDRESS m_kAddressV;
    };
    struct D3DTextureFormatInfo {
        DDSURFACEDESC2 m_ddsd2;
        BOOL m_bPalettized;
        BOOL m_bAlpha;
        int m_RedBPP;
        int m_BlueBPP;
        int m_GreenBPP;
        int m_AlphaBPP;
        int m_IndexBPP;
    };

    Device(DX::Direct3D *);
    virtual ~Device();
    void Create(int, DX::DirectDrawSurface *, int, int);
    void Release();
    long SetLight(int i, D3DLIGHT7 *light) { return m_IDirect3DDevice7->SetLight(i, light); }
    long GetLight(int i, D3DLIGHT7 *light) { return m_IDirect3DDevice7->GetLight(i, light); }
    long LightEnable(int i, int enable) { return m_IDirect3DDevice7->LightEnable(i, enable); }
    long GetLightEnable(int i, int *enable) { return m_IDirect3DDevice7->GetLightEnable(i, enable); }
    long Clear(unsigned long count, D3DRECT *rects, unsigned long flags, unsigned long color, float z, unsigned long stencil)
    {
        return m_IDirect3DDevice7->Clear(count, rects, flags, color, z, stencil);
    }
    long SetViewport(D3DVIEWPORT7 *viewport) { return m_IDirect3DDevice7->SetViewport(viewport); }
    long GetViewport(D3DVIEWPORT7 *viewport) { return m_IDirect3DDevice7->GetViewport(viewport); }
    long BeginScene() { return m_IDirect3DDevice7->BeginScene(); }
    long EndScene() { return m_IDirect3DDevice7->EndScene(); }
    IDirect3DDevice7 *GetCOMInterface() { return m_IDirect3DDevice7; }
    DX::Direct3D *GetDirect3D() { return m_pDirect3D; }
    long SetRenderState(D3DRENDERSTATETYPE, unsigned long);
    long SetRenderState();
    long ResetRenderState();
    long GetRenderState(D3DRENDERSTATETYPE, unsigned long *);
    long SetTextureStageState(int, D3DTEXTURESTAGESTATETYPE, unsigned long);
    long SetTextureStageState();
    long ResetTextureStageState();
    void SetTexture(DX::DirectDrawSurfaceAttrib &, int);
    int RenderingIsEnabled() { return m_bEnableRendering; }
    void EnableRendering(int enable) { m_bEnableRendering = enable; }
    void CreateZBuffer(int, int, int);
    void CheckForLostSurfaces();
    void FindSurfaceDesc(struct _DDSURFACEDESC2 *, int, int, int, int);
protected:
    void EnumTextureFormats();
    static long __stdcall EnumTextureFormatsCallback(DDPIXELFORMAT *, void *);
private:
    int GetDDErrorString(long, char *, unsigned long);
public:
    IDirect3DDevice7 *m_IDirect3DDevice7;
    DX::Direct3D *m_pDirect3D;
    DX::DirectDrawSurface *m_pBackBuffer;
    DX::DirectDrawSurface *m_pZBuffer;
    BOOL m_bZBufferInVideo;
    BOOL m_bZBufferAttached;
    BOOL m_bEnableRendering;
    D3DRenderState m_D3DRenderState;
    D3DTextureStageState m_D3DTextureStageState;
    int m_nD3DTextureFormats;
    int m_iD3DTextureFormat;
    ValList<D3DTextureFormatInfo *> m_aD3DTextureFormatInfo;
private:
    static int m_StartFormat;
};
}
