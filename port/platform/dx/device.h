// device.h - the Direct3D 7 device of the software renderer: the state the game sets,
// kept as Direct3D defines it.
#ifndef PORT_DX_DEVICE_H
#define PORT_DX_DEVICE_H

#include "dx.h"

namespace dx {

// Render states the game's headers do not name.
enum {
    RS_CLIPPING = 136,
    RS_COLORVERTEX = 141,
    RS_LOCALVIEWER = 142,
    RS_NORMALIZENORMALS = 143,
    RS_DIFFUSEMATERIALSOURCE = 145,
    RS_SPECULARMATERIALSOURCE = 146,
    RS_AMBIENTMATERIALSOURCE = 147,
    RS_EMISSIVEMATERIALSOURCE = 148,
    RS_FOGVERTEXMODE = 140,
    RS_COUNT = 256,
};

enum { MAX_STAGES = 8, TSS_COUNT = 32 };

struct Device : Object<IDirect3DDevice7> {
    DirectDraw *dd;           // holds a reference
    Surface *target;          // holds a reference
    Surface *textures[MAX_STAGES] = {};
    DWORD rs[RS_COUNT];
    DWORD tss[MAX_STAGES][TSS_COUNT];
    D3DMATRIX world, view, proj;
    D3DVIEWPORT7 viewport;
    D3DMATERIAL7 material;
    struct Light {
        D3DLIGHT7 light;
        bool enabled;
    };
    std::vector<Light> lights;
    D3DCLIPSTATUS clipStatus;

    Device(DirectDraw *dd, Surface *target);
    ~Device();
    Surface *zbuffer() const; // the z-buffer attached to the render target, if any

    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    HRESULT __stdcall GetCaps(_D3DDeviceDesc7 *) override;
    HRESULT __stdcall EnumTextureFormats(HRESULT(__stdcall *)(DDPIXELFORMAT *, void *), void *) override;
    HRESULT __stdcall BeginScene() override;
    HRESULT __stdcall EndScene() override;
    HRESULT __stdcall GetDirect3D(IDirect3D7 **) override;
    HRESULT __stdcall SetRenderTarget(IDirectDrawSurface7 *, DWORD) override;
    HRESULT __stdcall GetRenderTarget(IDirectDrawSurface7 **) override;
    HRESULT __stdcall Clear(DWORD, D3DRECT *, DWORD, D3DCOLOR, D3DVALUE, DWORD) override;
    HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) override;
    HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) override;
    HRESULT __stdcall SetViewport(D3DVIEWPORT7 *) override;
    HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) override;
    HRESULT __stdcall GetViewport(D3DVIEWPORT7 *) override;
    HRESULT __stdcall SetMaterial(D3DMATERIAL7 *) override;
    HRESULT __stdcall GetMaterial(D3DMATERIAL7 *) override;
    HRESULT __stdcall SetLight(DWORD, D3DLIGHT7 *) override;
    HRESULT __stdcall GetLight(DWORD, D3DLIGHT7 *) override;
    HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE, DWORD) override;
    HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE, DWORD *) override;
    HRESULT __stdcall BeginStateBlock() override;
    HRESULT __stdcall EndStateBlock(DWORD *) override;
    HRESULT __stdcall PreLoad(IDirectDrawSurface7 *) override;
    HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE, DWORD, void *, DWORD, DWORD) override;
    HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, DWORD, void *, DWORD, WORD *, DWORD, DWORD) override;
    HRESULT __stdcall SetClipStatus(D3DCLIPSTATUS *) override;
    HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS *) override;
    HRESULT __stdcall DrawPrimitiveStrided(D3DPRIMITIVETYPE, DWORD, _D3DDRAWPRIMITIVESTRIDEDDATA *, DWORD, DWORD) override;
    HRESULT __stdcall DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE, DWORD, _D3DDRAWPRIMITIVESTRIDEDDATA *, DWORD, WORD *,
                                                  DWORD, DWORD) override;
    HRESULT __stdcall DrawPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, DWORD) override;
    HRESULT __stdcall DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, WORD *, DWORD,
                                             DWORD) override;
    HRESULT __stdcall ComputeSphereVisibility(D3DVECTOR *, D3DVALUE *, DWORD, DWORD, DWORD *) override;
    HRESULT __stdcall GetTexture(DWORD, IDirectDrawSurface7 **) override;
    HRESULT __stdcall SetTexture(DWORD, IDirectDrawSurface7 *) override;
    HRESULT __stdcall GetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD *) override;
    HRESULT __stdcall SetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) override;
    HRESULT __stdcall ValidateDevice(DWORD *) override;
    HRESULT __stdcall ApplyStateBlock(DWORD) override;
    HRESULT __stdcall CaptureStateBlock(DWORD) override;
    HRESULT __stdcall DeleteStateBlock(DWORD) override;
    HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE, DWORD *) override;
    HRESULT __stdcall Load(IDirectDrawSurface7 *, POINT *, IDirectDrawSurface7 *, RECT *, DWORD) override;
    HRESULT __stdcall LightEnable(DWORD, BOOL) override;
    HRESULT __stdcall GetLightEnable(DWORD, BOOL *) override;
    HRESULT __stdcall SetClipPlane(DWORD, D3DVALUE *) override;
    HRESULT __stdcall GetClipPlane(DWORD, D3DVALUE *) override;
    HRESULT __stdcall GetInfo(DWORD, void *, DWORD) override;
};

} // namespace dx

#endif
