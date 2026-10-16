#include "DX_Direct3D.h"
#include "DX_TextureMapManager.h"
#include "DX_Exception.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXDirect3D.cpp"

// TODO 0x1012e230 ?EnumD3DDevicesCallback@Direct3D@DX@@KGJPAD0PAU_D3DDeviceDesc7@@PAX@Z: 58% - reconstruction; the flag computations and the desktop-mode search differ in register use and order
HRESULT __stdcall DX::Direct3D::EnumD3DDevicesCallback(char *description, char *name, D3DDEVICEDESC7 *desc, void *context)
{
    if (!description || !name || !context)
        return D3DENUMRET_OK;
    DX::Direct3D *direct3D = (DX::Direct3D *)context;
    DX::D3DDriverInfo info;
    memset(&info, 0, sizeof(info));
    info.pGUID = &desc->deviceGUID;
    info.bIsHardware = desc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
    info.guid = desc->deviceGUID;
    if (info.guid == IID_IDirect3DRefDevice)
        return D3DENUMRET_OK;
    strncpy(info.strName, description, 49);
    strncpy(info.strAbout, name, 49);
    info.ddDriverCaps = *desc;
    info.bTextureNonLocalVidMem = (info.ddDriverCaps.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) ? TRUE : FALSE;
    info.bDoesTextures = info.ddDriverCaps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE;
    if (!direct3D->GetTotalNonLocalVidMemory(info))
        info.bTextureNonLocalVidMem = FALSE;
    info.bDoesZBuffer = info.ddDriverCaps.dwDeviceZBufferBitDepth != 0;
    if (info.bDoesZBuffer && (info.ddDriverCaps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR))
        info.bDoesZBuffer = FALSE;
    if (info.ddDriverCaps.wMaxSimultaneousTextures > 1 && info.ddDriverCaps.wMaxTextureBlendStages > 1)
        info.bCanDoMultiTexture = TRUE;
    else
        info.bCanDoMultiTexture = FALSE;
    info.bCompatibleWithDesktop = FALSE;
    info.bCanDoWindow = (direct3D->m_DirectDrawDevices[direct3D->GetCurrentDeviceIndex()].ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) ? TRUE : FALSE;
    if (info.bCanDoWindow) {
        DX::DXDisplayModeInfo windows = direct3D->GetWindowsDisplayModeInfo();
        DX::DDDeviceInfo device = direct3D->m_DirectDrawDevices.Curr();
        for (int i = 0; i < device.DisplayModes.Count(); i++) {
            if (device.DisplayModes[i].dwWidth == windows.dwWidth && device.DisplayModes[i].dwHeight == windows.dwHeight
                && device.DisplayModes[i].dwRGBBitCount == windows.dwRGBBitCount) {
                info.bCompatibleWithDesktop = TRUE;
                break;
            }
        }
    }
    if (info.bIsHardware)
        info.bHasSufficientTextureMem = direct3D->GetTotalTextureMemory(info) >= 0xc00000;
    else
        info.bHasSufficientTextureMem = TRUE;
    if (info.bCanDoWindow && info.bDoesZBuffer && info.bDoesTextures && info.bHasSufficientTextureMem
        && (info.ddDriverCaps.dwTextureOpCaps & 0x1000))
        direct3D->GetCurrentDevice().Direct3DDrivers.Add(info);
    return D3DENUMRET_OK;
}

// 0x1012e6e0 ??0Direct3D@DX@@QAE@PAUIDirectDraw7@@PAU_GUID@@@Z
DX::Direct3D::Direct3D(IDirectDraw7 *directDraw, GUID *guid)
    : m_IDirect3D7(0), m_pTextureMapManager(0), m_bEmulation(0)
{
    m_pTextureMapManager = new DX::TextureMapManager(this);
    m_IDirectDraw7 = directDraw;
    m_lpGuid = guid;
}

// 0x1012e7a0 ??_GDirect3D@DX@@UAEPAXI@Z

// 0x1012e7c0 ??1Direct3D@DX@@UAE@XZ
DX::Direct3D::~Direct3D()
{
    Release();
    delete m_pTextureMapManager;
}

// 0x1012e880 ?Create@Direct3D@DX@@UAEXPAU_GUID@@KKK@Z
void DX::Direct3D::Create(GUID *guid, unsigned long width, unsigned long height, unsigned long depth)
{
    int driver = -25;
    int mode = -25;
    DirectDraw::Create(guid, width, height, depth);
    HRESULT hr = m_IDirectDraw7->QueryInterface(IID_IDirect3D7, (void **)&m_IDirect3D7);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 184, "Error getting Direct3D interface");
    EnumDrivers();
    VerifyDriverAndMode(&driver, &mode);
    EnumZBufferFormats();
}

// 0x1012e930 ?Release@Direct3D@DX@@UAEXXZ
void DX::Direct3D::Release()
{
    if (m_IDirect3D7) {
        m_IDirect3D7->Release();
        m_IDirect3D7 = 0;
    }
    m_bEmulation = 0;
    DirectDraw::Release();
}

// 0x1012e960 ?EnumDrivers@Direct3D@DX@@QAEXXZ
void DX::Direct3D::EnumDrivers()
{
    HRESULT hr = m_IDirect3D7->EnumDevices(EnumD3DDevicesCallback, this);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 213, "Error enumerating Direct3D devices");
}

// TODO 0x1012e9b0 ?PickDriver@Direct3D@DX@@QAEXPAHK@Z: 24% - reconstruction; the original mixes inlined and out-of-line ValList lookups and the loop structure is not recovered
void DX::Direct3D::PickDriver(int *driver, unsigned long depthMask)
{
    int device = GetCurrentDeviceIndex();
    int count = m_DirectDrawDevices[device].Direct3DDrivers.Count();
    int i;
    for (i = 0; i < count; i++) {
        if (m_DirectDrawDevices[device].Direct3DDrivers[i].ddDriverCaps.dwDeviceRenderBitDepth & depthMask)
            break;
    }
    if (i >= m_DirectDrawDevices[device].Direct3DDrivers.Count())
        *driver = -100;
    int found = i;
    for (i = 0; i < count; i++) {
        if (!(m_DirectDrawDevices[device].Direct3DDrivers[i].ddDriverCaps.dwDeviceRenderBitDepth & depthMask))
            continue;
        if (!m_bEmulation) {
            if (!m_DirectDrawDevices[device].Direct3DDrivers[i].bIsHardware)
                continue;
            if (!m_DirectDrawDevices[device].Direct3DDrivers[found].bIsHardware) {
                found = i;
                continue;
            }
        } else if (m_DirectDrawDevices[device].Direct3DDrivers[i].bIsHardware) {
            continue;
        }
        if (GetCurrentDevice().Direct3DDrivers[i].bIsHardware != GetCurrentDevice().Direct3DDrivers[found].bIsHardware)
            continue;
        if ((GetCurrentDevice().Direct3DDrivers[i].ddDriverCaps.dpcTriCaps.dwShadeCaps & 0x200)
            && !(GetCurrentDevice().Direct3DDrivers[found].ddDriverCaps.dpcTriCaps.dwShadeCaps & 0x200))
            found = i;
    }
    if (found < m_DirectDrawDevices[device].Direct3DDrivers.Count())
        *driver = found;
    else
        *driver = -100;
}

// TODO 0x1012eca0 ?VerifyDriverAndMode@Direct3D@DX@@QAEXPAH0@Z: 60% - the branch structure is recovered, the inlined lookups and the order of the mode cases are not
void DX::Direct3D::VerifyDriverAndMode(int *driverOut, int *modeOut)
{
    int driver = *driverOut;
    int mode = *modeOut;
    bool canWindow = (GetCurrentDeviceInfo().ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) != 0;
    if (mode == -24 && !canWindow)
        throw DX::Exception("Cannot render to a window - the current DirectDraw device is not capable of windowed rendering");
    if (driver == -25) {
        if (mode == -24) {
            PickDriver(&driver, BPPToDDBD(GetWindowsDisplayModeInfo().dwRGBBitCount));
            if (driver == -100)
                throw DX::Exception("Cannot find a D3D device driver which is compatible with the current display depth");
        } else if (mode == -25) {
            unsigned long depths = canWindow ? BPPToDDBD(GetWindowsDisplayModeInfo().dwRGBBitCount) : 0;
            for (int i = 0; i < m_DirectDrawDevices.Curr().DisplayModes.Count(); i++)
                depths |= BPPToDDBD(GetDisplayModeInfo(i).dwRGBBitCount);
            PickDriver(&driver, depths);
            if (driver == -100)
                throw DX::Exception("Cannot find a D3D device driver which is compatible with the current display depth or any supported fullscreen mode");
        } else {
            PickDriver(&driver, BPPToDDBD(GetDisplayModeInfo(mode).dwRGBBitCount));
            if (driver == -100)
                throw DX::Exception("Cannot find a D3D device driver which is compatible with the specified fullscreen mode");
        }
        *driverOut = driver;
        *modeOut = mode;
        return;
    }
    if (mode == -25) {
        if (canWindow && (m_DirectDrawDevices[GetCurrentDeviceIndex()].Direct3DDrivers[driver].ddDriverCaps.dwDeviceRenderBitDepth
                          & BPPToDDBD(GetWindowsDisplayModeInfo().dwRGBBitCount))) {
            mode = -24;
        } else {
            PickDisplayMode(&mode, m_DirectDrawDevices[GetCurrentDeviceIndex()].Direct3DDrivers[driver].ddDriverCaps.dwDeviceRenderBitDepth);
            if (mode == -100)
                throw DX::Exception("The selected D3D device driver is not compatible with the current display depth or any supported fullscreen modes");
        }
    } else if (mode == -24) {
        if (!(m_DirectDrawDevices[GetCurrentDeviceIndex()].Direct3DDrivers[driver].ddDriverCaps.dwDeviceRenderBitDepth
              & BPPToDDBD(GetWindowsDisplayModeInfo().dwRGBBitCount))) {
            driver = -25;
            VerifyDriverAndMode(&driver, &mode);
            if (driver == -100)
                throw DX::Exception("Unable to select driver");
        }
    } else {
        if (!(m_DirectDrawDevices[GetCurrentDeviceIndex()].Direct3DDrivers[driver].ddDriverCaps.dwDeviceRenderBitDepth
              & BPPToDDBD(GetDisplayModeInfo(mode).dwRGBBitCount))) {
            driver = -25;
            VerifyDriverAndMode(&driver, &mode);
            if (driver == -100)
                throw DX::Exception("Unable to select driver");
        }
    }
    *driverOut = driver;
    *modeOut = mode;
}

// 0x1012f290 ?GetCurrentDeviceInfo@DirectDraw@DX@@QAE?BUDDDeviceInfo@2@XZ

// 0x1012f2b0 ?GetDisplayModeInfo@DirectDraw@DX@@QAE?AUDXDisplayModeInfo@2@H@Z
DX::DXDisplayModeInfo DX::DirectDraw::GetDisplayModeInfo(int i)
{
    return m_DirectDrawDevices.Curr().DisplayModes[i];
}

// 0x1012f3d0 ?EnumZBufferFormats@Direct3D@DX@@IAEXXZ
void DX::Direct3D::EnumZBufferFormats()
{
    HRESULT hr = m_IDirect3D7->EnumZBufferFormats(DriverInfo().guid, EnumZBufferFormatsCallback, this);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 409, "Enumeration of texture formats failed");
}

// 0x1012f470 ?EnumZBufferFormatsCallback@Direct3D@DX@@KGJPAU_DDPIXELFORMAT@@PAX@Z
HRESULT __stdcall DX::Direct3D::EnumZBufferFormatsCallback(DDPIXELFORMAT *format, void *context)
{
    DX::Direct3D *direct3D = (DX::Direct3D *)context;
    direct3D->m_aD3DZBufferFormat.Add(*format);
    return D3DENUMRET_OK;
}

// 0x1012f520 ??1?$ValList@U_DDPIXELFORMAT@@@@QAE@XZ
// 0x1012f560 _$E26
// 0x1012f590 _$E22
// 0x1012f5a0 _$E25
