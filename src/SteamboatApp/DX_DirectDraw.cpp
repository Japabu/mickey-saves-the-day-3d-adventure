#include "DX_DirectDraw.h"
#include "DX_Exception.h"
#include <stdio.h>

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXDirectDraw.cpp"

// 0x100ce5c0 ??0DirectDraw@DX@@QAE@XZ
DX::DirectDraw::DirectDraw() : m_IDirectDraw7(0), m_bOnlySystemMemory(0), m_pDeviceSelectionCallback(0)
{
    memset(m_Originalppe, 0, sizeof(m_Originalppe));
    memset(m_ppe, 0, sizeof(m_ppe));
}

// 0x100ce630 ??1DirectDraw@DX@@UAE@XZ
DX::DirectDraw::~DirectDraw()
{
    Release();
}

// 0x100ce780 ?Create@DirectDraw@DX@@UAEXPAU_GUID@@KKK@Z
void DX::DirectDraw::Create(GUID *guid, unsigned long width, unsigned long height, unsigned long depth)
{
    HRESULT hr;
    int i;
    if (!m_IDirectDraw7) {
        if (guid) {
            hr = DirectDrawCreateEx(guid, (void **)&m_IDirectDraw7, IID_IDirectDraw7, 0);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 52, "Error creating DirectDraw7 interface");
        } else {
            HMODULE ddraw = GetModuleHandle("DDRAW.DLL");
            if (!ddraw)
                return;
            if (GetProcAddress(ddraw, "DirectDrawEnumerateExA")) {
                hr = DirectDrawEnumerateExA(DirectDrawEnumCallbackEx, this,
                                            DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 71, "Error enumerating DirectDraw device (DirectDrawEnumerateEx)");
            } else {
                hr = DirectDrawEnumerateA(DirectDrawEnumCallback, this);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 76, "Error enumerating DirectDraw device (DirectDrawEnumerate)");
            }
            if (m_pDeviceSelectionCallback) {
                for (int device = 0; device < m_DirectDrawDevices.Count(); device++) {
                    GUID *deviceGuid = m_DirectDrawDevices[device].pGUID;
                    DDCAPS_DX7 caps = m_DirectDrawDevices[device].ddDriverCaps;
                    if (m_pDeviceSelectionCallback(this, deviceGuid, caps, m_pDeviceSelectionContext)
                        && SUCCEEDED(DirectDrawCreateEx(deviceGuid, (void **)&m_IDirectDraw7, IID_IDirectDraw7, 0)))
                        break;
                }
            }
            if (!m_IDirectDraw7) {
                IDirectDraw *directDraw;
                hr = DirectDrawCreate(0, &directDraw, 0);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 107, "Error creating DirectDraw interface");
                hr = directDraw->QueryInterface(IID_IDirectDraw7, (void **)&m_IDirectDraw7);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 110, "Error getting DirectDraw7 interface");
                for (i = 0; i < m_DirectDrawDevices.Count(); i++) {
                    if (!m_DirectDrawDevices[i].pGUID)
                        break;
                }
            }
        }
        HDC hdc = GetDC(0);
        GetSystemPaletteEntries(hdc, 0, 256, m_Originalppe);
        for (i = 0; i < 256; i++)
            m_ppe[i] = m_Originalppe[i];
        ReleaseDC(0, hdc);
        RememberWindowsMode();
    } else {
        CopyDriverInfo(m_IDirectDraw7, m_lpGuid);
    }
}

// 0x100cea80 ?Release@DirectDraw@DX@@UAEXXZ
void DX::DirectDraw::Release()
{
    if (m_IDirectDraw7) {
        m_IDirectDraw7->Release();
        m_IDirectDraw7 = 0;
    }
    m_IDirectDraw7 = 0;
    m_bOnlySystemMemory = 0;
}

// 0x100ceaa0 ?EnumDisplayModes@DirectDraw@DX@@QAEXPAUIDirectDraw7@@KKK@Z
void DX::DirectDraw::EnumDisplayModes(IDirectDraw7 *directDraw, unsigned long width, unsigned long height, unsigned long depth)
{
    if (!directDraw)
        return;
    HRESULT hr = directDraw->EnumDisplayModes(0, 0, this, EnumDisplayModesCallback);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 202, "Error enumerating display modes");
    int device = GetCurrentDeviceIndex();
    for (int i = m_DirectDrawDevices[device].DisplayModes.Count() - 1; i > 0; i--) {
        if (m_DirectDrawDevices[device].DisplayModes[i].dwWidth == width
            && m_DirectDrawDevices[device].DisplayModes[i].dwHeight == height
            && m_DirectDrawDevices[device].DisplayModes[i].dwRGBBitCount == depth)
            return;
    }
}

// 0x100ced10 ?RememberWindowsMode@DirectDraw@DX@@QAEXXZ
void DX::DirectDraw::RememberWindowsMode()
{
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    HRESULT hr = m_IDirectDraw7->GetDisplayMode(&ddsd);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 233, "Error getting display mode");
    m_d3ddmWindows.ddsd = ddsd;
    m_d3ddmWindows.dwWidth = ddsd.dwWidth;
    m_d3ddmWindows.dwHeight = ddsd.dwHeight;
    m_d3ddmWindows.dwRGBBitCount = ddsd.ddpfPixelFormat.dwRGBBitCount;
    _snprintf(m_d3ddmWindows.strDesc, sizeof(m_d3ddmWindows.strDesc) - 1, "%d x %d x %d",
        ddsd.dwWidth, ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount);
}

// 0x100cedc0 ?RestoreDispMode@DirectDraw@DX@@QAEXXZ
void DX::DirectDraw::RestoreDispMode()
{
    HRESULT hr = m_IDirectDraw7->RestoreDisplayMode();
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 252, "Error restoring display mode");
}

// 0x100cee10 ?PickDisplayMode@DirectDraw@DX@@QAEXPAHK@Z
void DX::DirectDraw::PickDisplayMode(int *mode, unsigned long depthMask)
{
    int device = GetCurrentDeviceIndex();
    int i;
    for (i = 0; i < m_DirectDrawDevices[device].DisplayModes.Count(); i++) {
        if (BPPToDDBD(m_DirectDrawDevices[device].DisplayModes[i].dwRGBBitCount) & depthMask)
            break;
    }
    int found = i;
    for (; i < m_DirectDrawDevices[device].DisplayModes.Count(); i++) {
        if ((BPPToDDBD(m_DirectDrawDevices[device].DisplayModes[i].dwRGBBitCount) & depthMask)
            && m_DirectDrawDevices[device].DisplayModes[i].dwWidth == 640
            && m_DirectDrawDevices[device].DisplayModes[i].dwHeight == 480
            && m_DirectDrawDevices[device].DisplayModes[i].dwRGBBitCount == 16) {
            found = i;
            break;
        }
    }
    if (found >= m_DirectDrawDevices[device].DisplayModes.Count())
        *mode = -100;
    else
        *mode = found;
}

// 0x100cf100 ?BPPToDDBD@DirectDraw@DX@@QAEKH@Z
unsigned long DX::DirectDraw::BPPToDDBD(int bpp)
{
    switch (bpp) {
    case 1:
        return DDBD_1;
    case 2:
        return DDBD_2;
    case 4:
        return DDBD_4;
    case 8:
        return DDBD_8;
    case 16:
        return DDBD_16;
    case 24:
        return DDBD_24;
    case 32:
        return DDBD_32;
    default:
        return (unsigned long)-100;
    }
}

// 0x100cf1a0 ?CreateSurface@DirectDraw@DX@@QAEJPAU_DDSURFACEDESC2@@PAPAUIDirectDrawSurface7@@@Z
long DX::DirectDraw::CreateSurface(DDSURFACEDESC2 *desc, IDirectDrawSurface7 **surface)
{
    if (m_bOnlySystemMemory)
        desc->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    return CreateSurface(desc, surface, 0);
}

// 0x100cf1d0 ?GetTotalVideoMemory@DirectDraw@DX@@QAEKXZ
unsigned long DX::DirectDraw::GetTotalVideoMemory()
{
    DDCAPS_DX7 hal, hel;
    memset(&hal, 0, sizeof(hal));
    memset(&hel, 0, sizeof(hel));
    hel.dwSize = hal.dwSize = sizeof(DDCAPS_DX7);
    HRESULT hr = GetCaps(&hal, &hel);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 371, "Error getting DirectDraw capabilities");
    if (hal.dwVidMemTotal)
        return hal.dwVidMemTotal;
    return hel.dwVidMemTotal;
}

// 0x100cf270 ?GetTotalTextureMemory@DirectDraw@DX@@QAEKABUD3DDriverInfo@2@@Z
unsigned long DX::DirectDraw::GetTotalTextureMemory(const DX::D3DDriverInfo &driver)
{
    DWORD total;
    DWORD free;
    DDSCAPS2 caps;
    memset(&caps, 0, sizeof(caps));
    caps.dwCaps = DDSCAPS_TEXTURE;
    HRESULT hr = m_IDirectDraw7->GetAvailableVidMem(&caps, &total, &free);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 393, "Error getting total texture memory");
    if (total > GetTotalVideoMemory() && !(driver.ddDriverCaps.dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
        return GetTotalVideoMemory();
    return total;
}

// 0x100cf320 ?GetTotalNonLocalVidMemory@DirectDraw@DX@@QAEKABUD3DDriverInfo@2@@Z
unsigned long DX::DirectDraw::GetTotalNonLocalVidMemory(const DX::D3DDriverInfo &driver)
{
    return GetTotalTextureMemory(driver) - GetTotalVideoMemory();
}

// 0x100cf340 ?GetFreeTextureMemory@DirectDraw@DX@@QAEKXZ
unsigned long DX::DirectDraw::GetFreeTextureMemory()
{
    DWORD total;
    DWORD free;
    DDSCAPS2 caps;
    memset(&caps, 0, sizeof(caps));
    caps.dwCaps = DDSCAPS_TEXTURE;
    HRESULT hr = m_IDirectDraw7->GetAvailableVidMem(&caps, &total, &free);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 436, "Error getting free texture memory");
    return free;
}

// 0x100cf3b0 ?DirectDrawEnumCallback@DirectDraw@DX@@KGHPAU_GUID@@PAD1PAX@Z
int __stdcall DX::DirectDraw::DirectDrawEnumCallback(GUID *guid, char *description, char *name, void *context)
{
    return DirectDrawEnumCallbackEx(guid, description, name, context, 0);
}

// EQUIV 0x100cf3d0 ?DirectDrawEnumCallbackEx@DirectDraw@DX@@KGHPAU_GUID@@PAD1PAXPAUHMONITOR__@@@Z: reviewed: the only difference is the test of the DirectDrawCreateEx result, cmp eax, ebx with ebx holding 0 in the original, test eax, eax here (same flags for the jl)
int __stdcall DX::DirectDraw::DirectDrawEnumCallbackEx(GUID *guid, char *description, char *name, void *context, HMONITOR monitor)
{
    IDirectDraw7 *directDraw;
    if (description && name && context && SUCCEEDED(DirectDrawCreateEx(guid, (void **)&directDraw, IID_IDirectDraw7, 0))) {
        DX::DDDeviceInfo info;
        memset(&info, 0, sizeof(info));
        info.ddDriverCaps.dwSize = sizeof(DDCAPS_DX7);
        info.ddHELCaps.dwSize = sizeof(DDCAPS_DX7);
        if (FAILED(directDraw->GetCaps(&info.ddDriverCaps, &info.ddHELCaps)))
            return DDENUMRET_OK;
        if (guid)
            info.guid = *guid;
        info.pGUID = guid;
        if (!guid && (info.ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)) {
            DEVMODE mode;
            mode.dmSize = sizeof(mode);
            EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode);
        }
        strncpy(info.strDriverDesc, description, 39);
        strncpy(info.strDriverName, name, 39);
        info.hMonitor = monitor;
        DX::DirectDraw *self = (DX::DirectDraw *)context;
        self->m_DirectDrawDevices.Add(info);
        if (directDraw)
            self->EnumDisplayModes(directDraw, 640, 480, 16);
    }
    return DDENUMRET_OK;
}

// 0x100cf7d0 ?EnumDisplayModesCallback@DirectDraw@DX@@KGJPAU_DDSURFACEDESC2@@PAX@Z
HRESULT __stdcall DX::DirectDraw::EnumDisplayModesCallback(DDSURFACEDESC2 *desc, void *context)
{
    if (!desc || !context)
        return DDENUMRET_OK;
    DX::DirectDraw *directDraw = (DX::DirectDraw *)context;
    DX::DXDisplayModeInfo mode;
    mode.ddsd = *desc;
    mode.dwWidth = desc->dwWidth;
    mode.dwHeight = desc->dwHeight;
    mode.dwRGBBitCount = desc->ddpfPixelFormat.dwRGBBitCount;
    _snprintf(mode.strDesc, 19, "%d x %d x %d", mode.dwWidth, mode.dwHeight, mode.dwRGBBitCount);
    directDraw->GetCurrentDevice().DisplayModes.Add(mode);
    return DDENUMRET_OK;
}

// EQUIV 0x100cf940 ?CopyDriverInfo@DirectDraw@DX@@IAEXPAUIDirectDraw7@@PAU_GUID@@@Z: reviewed: cmp eax, ebx (ebx = 0) against test eax, eax for the GetDeviceIdentifier result; VC6 here reuses the known-zero eax for the rep stosd instead of xor eax, eax, and stores the EH state before that memset instead of after it; same calls and stores
void DX::DirectDraw::CopyDriverInfo(IDirectDraw7 *directDraw, GUID *guid)
{
    DDDEVICEIDENTIFIER2 identifier;
    memset(&identifier, 0, sizeof(identifier));
    if (directDraw->GetDeviceIdentifier(&identifier, 0) != DD_OK)
        return;
    DX::DDDeviceInfo info;
    memset(&info, 0, sizeof(info));
    info.ddDriverCaps.dwSize = sizeof(DDCAPS_DX7);
    info.ddHELCaps.dwSize = sizeof(DDCAPS_DX7);
    directDraw->GetCaps(&info.ddDriverCaps, &info.ddHELCaps);
    if (guid)
        info.guid = *guid;
    info.pGUID = guid;
    if (!guid && (info.ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)) {
        DEVMODE mode;
        mode.dmSize = sizeof(mode);
        EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode);
    }
    strncpy(info.strDriverDesc, identifier.szDescription, 39);
    strncpy(info.strDriverName, identifier.szDriver, 39);
    info.hMonitor = 0;
    m_DirectDrawDevices.Add(info);
    if (directDraw)
        EnumDisplayModes(directDraw, 640, 480, 16);
}

// 0x100ce610 ??_GDirectDraw@DX@@UAEPAXI@Z
// 0x100cfd40 ??1?$ValList@UDDDeviceInfo@DX@@@@QAE@XZ
// 0x100d0260 ??1L@?$ValList@UDDDeviceInfo@DX@@@@QAE@XZ
// 0x100cfd00 ??A?$ValList@UDXDisplayModeInfo@DX@@@@QAEAAUDXDisplayModeInfo@DX@@H@Z
// TODO 0x100cfd90 ?Add@?$ValList@UDDDeviceInfo@DX@@@@QAEHUDDDeviceInfo@DX@@@Z: 99% - same code; the original also keeps this in a stack slot for the EH unwind, so every stack offset moves
// TODO 0x100d0060 ?RemoveAll@?$ValList@UDXDisplayModeInfo@DX@@@@QAEXXZ: (100% similar) inline in ValList.h; this unit inlines every call, so no out-of-line copy is emitted here
// 0x100d00a0 ?AddList@?$ValList@UDXDisplayModeInfo@DX@@@@QAEHABV1@@Z
// TODO 0x100d0160 ?RemoveAll@?$ValList@UD3DDriverInfo@DX@@@@QAEXXZ: (100% similar) inline in ValList.h; this unit inlines every call, so no out-of-line copy is emitted here
// 0x100d01a0 ?AddList@?$ValList@UD3DDriverInfo@DX@@@@QAEHABV1@@Z
