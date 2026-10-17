// IDirectDrawSurface7.h - DirectDraw 7's IDirectDrawSurface7 (ddraw.h), as far as the engine calls it.
#ifndef MBENGINE_IDIRECTDRAWSURFACE7_H
#define MBENGINE_IDIRECTDRAWSURFACE7_H

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
    virtual HRESULT __stdcall SetColorKey(DWORD, LPDDCOLORKEY) = 0;
    virtual HRESULT __stdcall SetOverlayPosition(LONG, LONG) = 0;
    virtual HRESULT __stdcall SetPalette(IDirectDrawPalette *) = 0;
    virtual HRESULT __stdcall Unlock(LPRECT) = 0;
};

#endif
