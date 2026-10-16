#pragma once
#include "d3d7.h"
#include "ValList.h"

namespace DXGfx { class DisplayD3D; }

namespace DX {
class DirectDrawSurface;
class TextureMap;
class TextureMapManager;
// A Direct3D driver of a DirectDraw device (from SteamboatApp.pdb).
struct D3DDriverInfo {
    GUID guid;
    GUID *pGUID;
    char strName[50];
    char strAbout[50];
    D3DDEVICEDESC7 ddDriverCaps;
    int bIsHardware;
    int bCompatibleWithDesktop;
    int bCanDoWindow;
    int bDoesTextures;
    int bDoesZBuffer;
    int bTextureNonLocalVidMem;
    int bCanDoMultiTexture;
    int bHasSufficientTextureMem;
};

// A display mode of a device.
struct DXDisplayModeInfo {
    DDSURFACEDESC2 ddsd;
    unsigned long dwWidth;
    unsigned long dwHeight;
    unsigned long dwRGBBitCount;
    char strDesc[20];
};

// A DirectDraw device with its display modes and Direct3D drivers.
struct DDDeviceInfo {
    GUID guid;
    GUID *pGUID;
    char strDriverDesc[50];
    char strDriverName[50];
    DDCAPS_DX7 ddDriverCaps;
    DDCAPS_DX7 ddHELCaps;
    void *hMonitor;
    ValList<DX::DXDisplayModeInfo> DisplayModes;
    ValList<DX::D3DDriverInfo> Direct3DDrivers;
};

// The DirectDraw object: device enumeration, display modes and surfaces
// (declaration from SteamboatApp.pdb).
class DirectDraw {
    friend class DXGfx::DisplayD3D;
    friend class DX::DirectDrawSurface;
    friend class DX::TextureMap;
    friend class DX::TextureMapManager;
    typedef int __cdecl DeviceSelectionCallback(DX::DirectDraw *, GUID *, const DDCAPS_DX7 &, void *);
public:
    DirectDraw(const DX::DirectDraw &);
    DirectDraw();
    virtual ~DirectDraw();
    virtual void Create(GUID *, unsigned long, unsigned long, unsigned long);
    virtual void Release();
    int GetNumDevices() { return m_DirectDrawDevices.Count(); }
    int GetCurrentDeviceIndex() { return m_DirectDrawDevices.Index(); }
    const DX::DDDeviceInfo GetCurrentDeviceInfo() { return m_DirectDrawDevices.Curr(); }
    const DX::DDDeviceInfo GetDeviceInfo(int);
    DX::DDDeviceInfo *GetDeviceInfoPtr(int i) { return &m_DirectDrawDevices[i]; }
    int GetNumDisplayModes();
    DX::DDDeviceInfo &GetCurrentDevice() { return m_DirectDrawDevices[GetCurrentDeviceIndex()]; }
    long SetDisplayMode(int);
    int GetDisplayMode() { return GetCurrentDevice().DisplayModes.Index(); }
    DX::DXDisplayModeInfo GetDisplayModeInfo();
    DX::DXDisplayModeInfo GetDisplayModeInfo(int);
    DX::DXDisplayModeInfo &GetWindowsDisplayModeInfo() { return m_d3ddmWindows; }
    PALETTEENTRY *GetPalette() { return m_ppe; }
    PALETTEENTRY *GetOriginalPalette() { return m_Originalppe; }
    unsigned long GetTotalVideoMemory();
    unsigned long GetTotalNonLocalVidMemory(const DX::D3DDriverInfo &);
    unsigned long GetTotalTextureMemory(const DX::D3DDriverInfo &);
    unsigned long GetFreeTextureMemory();
    long GetCaps(DDCAPS_DX7 *hal, DDCAPS_DX7 *hel) { return m_IDirectDraw7->GetCaps(hal, hel); }
    int IsPrimaryDevice(int);
    int CanMIPMap()
    {
        if (!GetNumDevices())
            return FALSE;
        return (m_DirectDrawDevices.Curr().ddDriverCaps.dwCaps & DDCAPS_COLORKEY) != 0;
    }
    void SetDeviceSelectionCallback(DeviceSelectionCallback *callback, void *context)
    {
        m_pDeviceSelectionCallback = callback;
        m_pDeviceSelectionContext = context;
    }
    void SetOnlySystemMemory(bool only) { m_bOnlySystemMemory = only; }
    void EnumDisplayModes(IDirectDraw7 *, unsigned long, unsigned long, unsigned long);
    void RememberWindowsMode();
    void RestoreDispMode();
    void PickDisplayMode(int *, unsigned long);
    unsigned long BPPToDDBD(int);
    long SetCooperativeLevel(HWND window, unsigned long flags) { return m_IDirectDraw7->SetCooperativeLevel(window, flags); }
    long CreateSurface(DDSURFACEDESC2 *desc, IDirectDrawSurface7 **surface, IUnknown *outer)
    {
        return m_IDirectDraw7->CreateSurface(desc, surface, outer);
    }
    long CreateSurface(DDSURFACEDESC2 *, IDirectDrawSurface7 **);
    long CreateClipper(unsigned long flags, IDirectDrawClipper **clipper, IUnknown *outer)
    {
        return m_IDirectDraw7->CreateClipper(flags, clipper, outer);
    }
    long CreatePalette(unsigned long flags, PALETTEENTRY *entries, IDirectDrawPalette **palette, IUnknown *outer)
    {
        return m_IDirectDraw7->CreatePalette(flags, entries, palette, outer);
    }
    long FlipToGDISurface() { return m_IDirectDraw7->FlipToGDISurface(); }
protected:
    IDirectDraw7 *GetCOMInterface() { return m_IDirectDraw7; }
    static long __stdcall EnumDisplayModesCallback(DDSURFACEDESC2 *, void *);
    static int __stdcall DirectDrawEnumCallback(GUID *, char *, char *, void *);
    static int __stdcall DirectDrawEnumCallbackEx(GUID *, char *, char *, void *, HMONITOR);
    void CopyDriverInfo(IDirectDraw7 *, GUID *);
    ValList<DX::DDDeviceInfo> m_DirectDrawDevices;
    IDirectDraw7 *m_IDirectDraw7;
    GUID *m_lpGuid;
private:
    int m_bOnlySystemMemory;
    DX::DXDisplayModeInfo m_d3ddmWindows;
    PALETTEENTRY m_ppe[256];
    PALETTEENTRY m_Originalppe[256];
    DeviceSelectionCallback *m_pDeviceSelectionCallback;
    void *m_pDeviceSelectionContext;
};
}
