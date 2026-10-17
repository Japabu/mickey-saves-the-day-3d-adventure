// IDirectDraw7.h - DirectDraw 7's IDirectDraw7 (ddraw.h), as far as the engine calls it.
#ifndef MBENGINE_IDIRECTDRAW7_H
#define MBENGINE_IDIRECTDRAW7_H

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

#endif
