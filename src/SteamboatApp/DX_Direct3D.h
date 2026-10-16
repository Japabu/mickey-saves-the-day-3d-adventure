#pragma once
#include "DX_DirectDraw.h"

namespace DX {
class TextureMapManager;

// The Direct3D object on top of the DirectDraw one: driver enumeration,
// z-buffer formats and the texture manager (declaration from
// SteamboatApp.pdb).
class Direct3D : public DX::DirectDraw {
public:
    Direct3D(IDirectDraw7 *, GUID *);
    virtual ~Direct3D();
    virtual void Create(GUID *, unsigned long, unsigned long, unsigned long);
    virtual void Release();
    DX::TextureMapManager *GetTextureMapManager() { return m_pTextureMapManager; }
    DX::D3DDriverInfo DriverInfo() { return GetCurrentDevice().Direct3DDrivers.Curr(); }
    DX::D3DDriverInfo DriverInfo(int i)
    {
        int current = Driver();
        DX::D3DDriverInfo info = GetCurrentDevice().Direct3DDrivers[i];
        Driver(current);
        return info;
    }
    int Driver() { return GetCurrentDevice().Direct3DDrivers.Index(); }
    void Driver(int i) { GetCurrentDevice().Direct3DDrivers[i]; }
    int NumDrivers() { return GetCurrentDevice().Direct3DDrivers.Count(); }
    void SetEmulation(int emulation) { m_bEmulation = emulation; }
    int IsEmulation() { return m_bEmulation; }
    int GetNumZBufferFormats() { return m_aD3DZBufferFormat.Count(); }
    const DDPIXELFORMAT &GetZBufferFormat(int i) { return m_aD3DZBufferFormat[i]; }
    void EnumDrivers();
    void PickDriver(int *, unsigned long);
    void VerifyDriverAndMode(int *, int *);
    long CreateDevice(const GUID &guid, IDirectDrawSurface7 *surface, IDirect3DDevice7 **device)
    {
        return m_IDirect3D7->CreateDevice(guid, surface, device);
    }
protected:
    void EnumZBufferFormats();
    static long __stdcall EnumZBufferFormatsCallback(DDPIXELFORMAT *, void *);
    static long __stdcall EnumD3DDevicesCallback(char *, char *, D3DDEVICEDESC7 *, void *);
private:
    IDirect3D7 *m_IDirect3D7;
    ValList<DDPIXELFORMAT> m_aD3DZBufferFormat;
    DX::TextureMapManager *m_pTextureMapManager;
    int m_bEmulation;
};
}
