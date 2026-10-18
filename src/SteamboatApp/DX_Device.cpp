#include "DX_Device.h"
#include "DX_DirectDrawSurface.h"
#include "DX_Exception.h"
#include "DX_Direct3D.h"
#include "DX_TextureMapManager.h"
#include "DX_TextureMap.h"
#include <string.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXDevice.cpp"

// 0x100c9c40 ?Reset@D3DRenderState@Device@DX@@QAEXXZ
void DX::Device::D3DRenderState::Reset()
{
    m_bZEnable = TRUE;
    m_bZWriteEnable = TRUE;
    m_bEnableColorKeying = FALSE;
    m_bEnableAlphaBlending = FALSE;
    m_bEnableAlphaTesting = FALSE;
    m_kAlphaFunction = D3DCMP_ALWAYS;
    m_kSrcBlend = D3DBLEND_SRCALPHA;
    m_kDestBlend = D3DBLEND_INVSRCALPHA;
    m_bPerspCorrect = TRUE;
    m_kShadeMode = D3DSHADE_GOURAUD;
    m_kFillMode = D3DFILL_SOLID;
    m_kCullMode = D3DCULL_CCW;
    m_bDithering = TRUE;
    m_bSpecular = FALSE;
    m_bAntialiasing = FALSE;
    m_bFogEnabled = FALSE;
    m_FogColor = 0xe5e5ff;
    m_FogMode = D3DFOG_NONE;
    m_FogStart = 0.0f;
    m_FogEnd = 5000.0f;
    m_bTexturesOn = TRUE;
}

// 0x100c9cb0 ?Reset@D3DTextureStageState@Device@DX@@QAEXXZ
void DX::Device::D3DTextureStageState::Reset()
{
    m_kMinTextureFilter = D3DTFN_LINEAR;
    m_kMagTextureFilter = D3DTFG_LINEAR;
    m_kMipTextureFilter = D3DTFP_NONE;
    m_kColorOp = D3DTOP_DISABLE;
    m_kColorArg1 = D3DTA_TEXTURE;
    m_kColorArg2 = D3DTA_DIFFUSE;
    m_kAlphaOp = D3DTOP_DISABLE;
    m_kAlphaArg1 = D3DTA_TEXTURE;
    m_kAlphaArg2 = D3DTA_DIFFUSE;
    m_kAddressU = D3DTADDRESS_WRAP;
    m_kAddressV = D3DTADDRESS_WRAP;
}

// 0x100c9cf0 ??0Device@DX@@QAE@PAVDirect3D@1@@Z
DX::Device::Device(DX::Direct3D *direct3D)
    : m_IDirect3DDevice7(0), m_pDirect3D(direct3D), m_pBackBuffer(0), m_pZBuffer(0), m_bZBufferInVideo(FALSE),
      m_bZBufferAttached(FALSE), m_bEnableRendering(FALSE), m_nD3DTextureFormats(0), m_iD3DTextureFormat(0)
{
    m_D3DRenderState.Reset();
    m_D3DTextureStageState.Reset();
}

// 0x100c9d60 ??_GDevice@DX@@UAEPAXI@Z

// EQUIV 0x100c9d80 ??1Device@DX@@UAE@XZ: proven (renaming-equivalent); bytes differ: identical except the original keeps EH state 0 while the format list is destroyed (here it is set to -1 first)
DX::Device::~Device()
{
    Release();
    delete m_pZBuffer;
    while (m_aD3DTextureFormatInfo.Count() > 0)
        delete m_aD3DTextureFormatInfo.Remove();
}

// 0x100c9e80 ?Release@Device@DX@@QAEXXZ
void DX::Device::Release()
{
    if (m_pBackBuffer && m_pZBuffer) {
        if (m_bZBufferAttached) {
            m_pBackBuffer->GetCOMInterface()->DeleteAttachedSurface(0, m_pZBuffer->GetCOMInterface());
            m_bZBufferAttached = FALSE;
        }
        m_pZBuffer->Release();
    }
    if (m_IDirect3DDevice7) {
        m_IDirect3DDevice7->Release();
        m_IDirect3DDevice7 = 0;
    }
    m_D3DRenderState.Reset();
    m_D3DTextureStageState.Reset();
}

// TODO 0x100c9ef0 ?Create@Device@DX@@QAEXHPAVDirectDrawSurface@2@HH@Z: (77% similar) right statements, but VC6 here inlines a different subset of the ValList lookups (Driver/DriverInfo/GetCurrentDevice) and closes the try block differently
void DX::Device::Create(int driver, DX::DirectDrawSurface *backBuffer, int width, int height)
{
    m_pBackBuffer = backBuffer;
    if (m_D3DRenderState.m_bZEnable) {
        try {
            CreateZBuffer(width, height, driver);
        } catch (...) {
            m_D3DRenderState.m_bZEnable = FALSE;
        }
    }
    if (m_IDirect3DDevice7) {
        m_IDirect3DDevice7->Release();
        m_IDirect3DDevice7 = 0;
    }
    m_pDirect3D->Driver(driver);
    IDirectDrawSurface7 *surface = m_pBackBuffer->GetCOMInterface();
    DX::D3DDriverInfo info = m_pDirect3D->DriverInfo(driver);
    HRESULT hr = m_pDirect3D->CreateDevice(info.guid, surface, &m_IDirect3DDevice7);
    if (hr != DD_OK) {
        char error[120];
        error[0] = 0;
        GetDDErrorString(hr, error, sizeof(error));
    }
    m_pDirect3D->Driver(driver);
    info = m_pDirect3D->DriverInfo(driver);
    if (info.bDoesTextures)
        EnumTextureFormats();
}

// the first texture format to try (the original's .data)
int DX::Device::m_StartFormat = -1;

// TODO 0x100ca1c0 ?CreateZBuffer@Device@DX@@QAEXHHH@Z: (77% similar) as Device::Create: the original calls ValList<D3DDriverInfo>::operator[] out of line inside DriverInfo(int), VC6 here inlines it
void DX::Device::CreateZBuffer(int width, int height, int driver)
{
    delete m_pZBuffer;
    m_pZBuffer = 0;
    if (!m_pDirect3D->DriverInfo(driver).bDoesZBuffer)
        return;
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;
    ddsd.ddpfPixelFormat = m_pDirect3D->GetZBufferFormat(0);
    if (m_pDirect3D->DriverInfo(driver).bIsHardware)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    m_pZBuffer = new DX::DirectDrawSurface(m_pDirect3D);
    m_pZBuffer->Create(ddsd);
    m_pZBuffer->GetSurfDesc(&ddsd);
    m_bZBufferInVideo = (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;
    if (m_pDirect3D->DriverInfo(driver).bIsHardware && !m_bZBufferInVideo)
        throw DX::Exception("Could not fit the Z-buffer in video memory for this hardware device");
    HRESULT hr = m_pBackBuffer->GetCOMInterface()->AddAttachedSurface(m_pZBuffer->GetCOMInterface());
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 240, "AddAttachedBuffer failed for Z-Buffer");
    m_bZBufferAttached = TRUE;
}

// 0x100ca510 ?CheckForLostSurfaces@Device@DX@@QAEXXZ
void DX::Device::CheckForLostSurfaces()
{
    HRESULT hr;
    if (m_pBackBuffer->GetCOMInterface() && m_pBackBuffer->GetCOMInterface()->IsLost() == DDERR_SURFACELOST) {
        hr = m_pBackBuffer->GetCOMInterface()->Restore();
        if (hr)
            throw DX::Exception(hr, THIS_FILE, 269, "Error restoring a lost surface");
    }
    if (m_pZBuffer->GetCOMInterface() && m_pZBuffer->GetCOMInterface()->IsLost() == DDERR_SURFACELOST) {
        hr = m_pZBuffer->GetCOMInterface()->Restore();
        if (hr)
            throw DX::Exception(hr, THIS_FILE, 270, "Error restoring a lost surface");
    }
}

// 0x100ca5c0 ?SetTextureStageState@Device@DX@@QAEJXZ
long DX::Device::SetTextureStageState()
{
    HRESULT hr;
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_MAGFILTER, m_D3DTextureStageState.m_kMagTextureFilter);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 276, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_MINFILTER, m_D3DTextureStageState.m_kMinTextureFilter);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 277, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_MIPFILTER, m_D3DTextureStageState.m_kMipTextureFilter);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 278, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_COLOROP, m_D3DTextureStageState.m_kColorOp);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 280, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_COLORARG1, m_D3DTextureStageState.m_kColorArg1);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 281, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_COLORARG2, m_D3DTextureStageState.m_kColorArg2);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 282, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_ALPHAOP, m_D3DTextureStageState.m_kAlphaOp);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 283, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_ALPHAARG1, m_D3DTextureStageState.m_kAlphaArg1);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 284, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_ALPHAARG2, m_D3DTextureStageState.m_kAlphaArg2);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 285, "Error setting texture stage state");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_ADDRESSU, m_D3DTextureStageState.m_kAddressU);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 287, "Error setting texture stage wrapu");
    hr = m_IDirect3DDevice7->SetTextureStageState(0, D3DTSS_ADDRESSV, m_D3DTextureStageState.m_kAddressV);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 288, "Error setting texture stage wrapv");
    return DD_OK;
}

// 0x100ca8b0 ?SetRenderState@Device@DX@@QAEJXZ
long DX::Device::SetRenderState()
{
    HRESULT hr;
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_SHADEMODE, m_D3DRenderState.m_kShadeMode);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 301, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZENABLE, m_D3DRenderState.m_bZEnable);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 302, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, m_D3DRenderState.m_bZWriteEnable);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 303, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, m_D3DRenderState.m_bEnableColorKeying);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 304, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, m_D3DRenderState.m_bEnableAlphaBlending);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 305, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, m_D3DRenderState.m_bEnableAlphaTesting);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 306, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, m_D3DRenderState.m_kAlphaFunction);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 307, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_SRCBLEND, m_D3DRenderState.m_kSrcBlend);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 308, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_DESTBLEND, m_D3DRenderState.m_kDestBlend);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 309, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 310, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, m_D3DRenderState.m_bPerspCorrect);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 311, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_FILLMODE, m_D3DRenderState.m_kFillMode);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 314, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_CULLMODE, m_D3DRenderState.m_kCullMode);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 315, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_DITHERENABLE, m_D3DRenderState.m_bDithering);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 316, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, m_D3DRenderState.m_bSpecular);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 317, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ANTIALIAS, m_D3DRenderState.m_bAntialiasing);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 318, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_FOGENABLE, m_D3DRenderState.m_bFogEnabled);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 319, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_FOGCOLOR, m_D3DRenderState.m_FogColor);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 320, "Error setting render state");
    hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 325, "Error setting lighting render state");
    return DD_OK;
}

// 0x100cad30 ?ResetRenderState@Device@DX@@QAEJXZ
long DX::Device::ResetRenderState()
{
    m_D3DRenderState.Reset();
    return SetRenderState();
}

// 0x100cad50 ?ResetTextureStageState@Device@DX@@QAEJXZ
long DX::Device::ResetTextureStageState()
{
    m_D3DTextureStageState.Reset();
    return SetTextureStageState();
}

// TODO 0x100cad70 ?SetTextureStageState@Device@DX@@QAEJHW4_D3DTEXTURESTAGESTATETYPE@@K@Z: (32% similar) the original lowers the switch to one 22-entry jump table, VC6 splits it here (case layout not found)
long DX::Device::SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, unsigned long value)
{
    HRESULT hr = D3D_OK;
    switch (type) {
    case D3DTSS_COLOROP:
    case D3DTSS_COLORARG1:
    case D3DTSS_COLORARG2:
    case D3DTSS_ALPHAOP:
    case D3DTSS_ALPHAARG1:
    case D3DTSS_ALPHAARG2:
        return m_IDirect3DDevice7->SetTextureStageState(stage, type, value);
    case D3DTSS_BUMPENVMAT00:
    case D3DTSS_BUMPENVMAT01:
    case D3DTSS_BUMPENVMAT10:
    case D3DTSS_BUMPENVMAT11:
        break;
    case D3DTSS_TEXCOORDINDEX:
        return m_IDirect3DDevice7->SetTextureStageState(stage, type, value);
    case D3DTSS_ADDRESS:
        break;
    case D3DTSS_ADDRESSU:
    case D3DTSS_ADDRESSV:
        return m_IDirect3DDevice7->SetTextureStageState(stage, type, value);
    case D3DTSS_BORDERCOLOR:
        break;
    case D3DTSS_MAGFILTER:
    case D3DTSS_MINFILTER:
    case D3DTSS_MIPFILTER:
        return m_IDirect3DDevice7->SetTextureStageState(stage, type, value);
    case D3DTSS_MIPMAPLODBIAS:
    case D3DTSS_MAXMIPLEVEL:
    case D3DTSS_MAXANISOTROPY:
    case D3DTSS_BUMPENVLSCALE:
        break;
    case D3DTSS_FORCE_DWORD:
        return m_IDirect3DDevice7->SetTextureStageState(stage, type, value);
    default:
        return E_INVALIDARG;
    }
    return hr;
}

// 0x100cae30 ?GetRenderState@Device@DX@@QAEJW4_D3DRENDERSTATETYPE@@PAK@Z
long DX::Device::GetRenderState(D3DRENDERSTATETYPE state, DWORD *value)
{
    return m_IDirect3DDevice7->GetRenderState(state, value);
}

// 0x100cae50 ?SetRenderState@Device@DX@@QAEJW4_D3DRENDERSTATETYPE@@K@Z
long DX::Device::SetRenderState(D3DRENDERSTATETYPE type, unsigned long value)
{
    HRESULT hr = DD_OK;
    switch (type) {
    case D3DRENDERSTATE_ZENABLE:
        if (m_pDirect3D->DriverInfo().bDoesZBuffer)
            hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZENABLE, value);
        break;
    case D3DRENDERSTATE_ZWRITEENABLE:
        if (m_pDirect3D->DriverInfo().bDoesZBuffer)
            hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, value);
        break;
    case D3DRENDERSTATE_ZFUNC:
        if (m_pDirect3D->DriverInfo().bDoesZBuffer)
            hr = m_IDirect3DDevice7->SetRenderState(D3DRENDERSTATE_ZFUNC, value);
        break;
    default:
        hr = m_IDirect3DDevice7->SetRenderState(type, value);
        break;
    }
    return hr;
}

// 0x100caff0 ?SetTexture@Device@DX@@QAEXAAUDirectDrawSurfaceAttrib@2@H@Z
void DX::Device::SetTexture(DX::DirectDrawSurfaceAttrib &attrib, int stage)
{
    HRESULT hr;
    if (attrib.render) {
        DX::TextureMap *textureMap = m_pDirect3D->GetTextureMapManager()->GetTextureMap(attrib, this, false);
        hr = m_IDirect3DDevice7->SetTexture(stage, textureMap->GetTexture());
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 497, "Error setting texture pointer");
    } else {
        hr = m_IDirect3DDevice7->SetTextureStageState(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 502, "Error setting texture stage state");
    }
}

// 0x100cb0b0 ?EnumTextureFormats@Device@DX@@IAEXXZ
void DX::Device::EnumTextureFormats()
{
    m_StartFormat = -1;
    m_nD3DTextureFormats = 0;
    HRESULT hr = m_IDirect3DDevice7->EnumTextureFormats(EnumTextureFormatsCallback, this);
    if (hr)
        throw DX::Exception(hr, THIS_FILE, 524, "Enumeration of texture formats failed");
    m_iD3DTextureFormat = m_StartFormat;
}

// TODO 0x100cb120 ?EnumTextureFormatsCallback@Device@DX@@KGJPAU_DDPIXELFORMAT@@PAX@Z: 87% - reconstruction; the original mixes inlined and out-of-line ValList lookups and its bit-count loops differ
HRESULT __stdcall DX::Device::EnumTextureFormatsCallback(DDPIXELFORMAT *format, void *context)
{
    DX::Device *device = (DX::Device *)context;
    device->m_aD3DTextureFormatInfo.Add(new D3DTextureFormatInfo);
    D3DTextureFormatInfo *info = device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats];
    memset(info, 0, sizeof(*info));
    info->m_ddsd2.ddpfPixelFormat = *format;
    if (format->dwFlags & DDPF_PALETTEINDEXED8) {
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bPalettized = TRUE;
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_IndexBPP = 8;
    } else if (format->dwFlags & DDPF_PALETTEINDEXED4) {
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bPalettized = TRUE;
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_IndexBPP = 4;
    } else if (format->dwFlags & DDPF_RGB) {
        if (format->dwFlags & DDPF_ALPHAPIXELS)
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bAlpha = TRUE;
        else
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bAlpha = FALSE;
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bPalettized = FALSE;
        device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_IndexBPP = 0;
        unsigned long mask;
        int red, green, blue, alpha;
        for (mask = format->dwRBitMask; !(mask & 1); mask >>= 1)
            ;
        for (red = 0; mask & 1; mask >>= 1)
            red++;
        for (mask = format->dwGBitMask; !(mask & 1); mask >>= 1)
            ;
        for (green = 0; mask & 1; mask >>= 1)
            green++;
        for (mask = format->dwBBitMask; !(mask & 1); mask >>= 1)
            ;
        for (blue = 0; mask & 1; mask >>= 1)
            blue++;
        if (format->dwFlags & DDPF_ALPHAPIXELS) {
            for (mask = format->dwRGBAlphaBitMask; !(mask & 1); mask >>= 1)
                ;
            for (alpha = 0; mask & 1; mask >>= 1)
                alpha++;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_RedBPP = red;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_GreenBPP = green;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_BlueBPP = blue;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_AlphaBPP = alpha;
        } else {
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_RedBPP = red;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_GreenBPP = green;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_BlueBPP = blue;
            device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_AlphaBPP = 0;
            m_StartFormat = -1;
            m_StartFormat = device->m_nD3DTextureFormats;
        }
    }
    if (device->m_aD3DTextureFormatInfo[device->m_nD3DTextureFormats]->m_bPalettized)
        m_StartFormat = device->m_nD3DTextureFormats;
    device->m_nD3DTextureFormats++;
    return DDENUMRET_OK;
}

// TODO 0x100cb690 ?FindSurfaceDesc@Device@DX@@QAEXPAU_DDSURFACEDESC2@@HHHH@Z: 95% - the original keeps found in a stack slot compared with a zeroed esi; VC6 here tests it in a register, so the register use differs
void DX::Device::FindSurfaceDesc(DDSURFACEDESC2 *desc, int mipMap, int colorKey, int alpha, int rgb16)
{
    int count = m_aD3DTextureFormatInfo.Count();
    BOOL found = FALSE;
    int i;
    if (alpha) {
        for (i = 0; i < count; i++) {
            if (found)
                continue;
            DDSURFACEDESC2 *ddsd = &m_aD3DTextureFormatInfo[i]->m_ddsd2;
            DDPIXELFORMAT *format = &m_aD3DTextureFormatInfo[i]->m_ddsd2.ddpfPixelFormat;
            if (format->dwRGBBitCount == 16 && format->dwRBitMask == 0xf00 && format->dwGBitMask == 0xf0
                && format->dwBBitMask == 0xf && format->dwRGBAlphaBitMask == 0xf000) {
                *desc = *ddsd;
                found = TRUE;
            }
        }
    } else if (rgb16) {
        for (i = 0; i < count; i++) {
            if (found)
                continue;
            DDSURFACEDESC2 *ddsd = &m_aD3DTextureFormatInfo[i]->m_ddsd2;
            DDPIXELFORMAT *format = &m_aD3DTextureFormatInfo[i]->m_ddsd2.ddpfPixelFormat;
            if (format->dwRGBBitCount == 16 && (format->dwFlags & DDPF_RGB)
                && !(format->dwFlags & (DDPF_ALPHAPIXELS | DDPF_ALPHA))) {
                *desc = *ddsd;
                found = TRUE;
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (found)
                continue;
            DDSURFACEDESC2 *ddsd = &m_aD3DTextureFormatInfo[i]->m_ddsd2;
            DDPIXELFORMAT *format = &m_aD3DTextureFormatInfo[i]->m_ddsd2.ddpfPixelFormat;
            if (format->dwRGBBitCount == 8 && (format->dwFlags & DDPF_PALETTEINDEXED8)) {
                *desc = *ddsd;
                found = TRUE;
            }
        }
    }
    for (i = 0; i < count; i++) {
        if (found)
            continue;
        DDSURFACEDESC2 *ddsd = &m_aD3DTextureFormatInfo[i]->m_ddsd2;
        DDPIXELFORMAT *format = &m_aD3DTextureFormatInfo[i]->m_ddsd2.ddpfPixelFormat;
        if (format->dwRGBBitCount == 16 && (format->dwFlags & DDPF_RGB)
            && !(format->dwFlags & (DDPF_ALPHAPIXELS | DDPF_ALPHA))) {
            *desc = *ddsd;
            found = TRUE;
        }
    }
}

// 0x100cbac0 ?GetDDErrorString@Device@DX@@AAEHJPADK@Z
int DX::Device::GetDDErrorString(long hr, char *buffer, unsigned long size)
{
    char unknown[120];
    char *error;
    if (!buffer || !size)
        return 0;
    switch (hr) {
    case 0x80004005:
        error = "DDERR_GENERIC";
        break;
    case 0x80004001:
        error = "DDERR_UNSUPPORTED";
        break;
    case 0x800401f0:
        error = "DDERR_NOTINITIALIZED";
        break;
    case 0x80070057:
        error = "DDERR_INVALIDPARAMS";
        break;
    case 0x8007000e:
        error = "DDERR_OUTOFMEMORY";
        break;
    case 0x88760005:
        error = "DDERR_ALREADYINITIALIZED";
        break;
    case 0x8876000a:
        error = "DDERR_CANNOTATTACHSURFACE";
        break;
    case 0x88760014:
        error = "DDERR_CANNOTDETACHSURFACE";
        break;
    case 0x88760028:
        error = "DDERR_CURRENTLYNOTAVAIL";
        break;
    case 0x88760037:
        error = "DDERR_EXCEPTION";
        break;
    case 0x8876005a:
        error = "DDERR_HEIGHTALIGN";
        break;
    case 0x8876005f:
        error = "DDERR_INCOMPATIBLEPRIMARY";
        break;
    case 0x88760064:
        error = "DDERR_INVALIDCAPS";
        break;
    case 0x8876006e:
        error = "DDERR_INVALIDCLIPLIST";
        break;
    case 0x88760078:
        error = "DDERR_INVALIDMODE";
        break;
    case 0x88760082:
        error = "DDERR_INVALIDOBJECT";
        break;
    case 0x88760091:
        error = "DDERR_INVALIDPIXELFORMAT";
        break;
    case 0x88760096:
        error = "DDERR_INVALIDRECT";
        break;
    case 0x887600a0:
        error = "DDERR_LOCKEDSURFACES";
        break;
    case 0x887600aa:
        error = "DDERR_NO3D";
        break;
    case 0x887600b4:
        error = "DDERR_NOALPHAHW";
        break;
    case 0x887600cd:
        error = "DDERR_NOCLIPLIST";
        break;
    case 0x887600d2:
        error = "DDERR_NOCOLORCONVHW";
        break;
    case 0x887600d7:
        error = "DDERR_NOCOLORKEY";
        break;
    case 0x887600dc:
        error = "DDERR_NOCOLORKEYHW";
        break;
    case 0x887600d4:
        error = "DDERR_NOCOOPERATIVELEVELSET";
        break;
    case 0x887600de:
        error = "DDERR_NODIRECTDRAWSUPPORT";
        break;
    case 0x887600e1:
        error = "DDERR_NOEXCLUSIVEMODE";
        break;
    case 0x887600e6:
        error = "DDERR_NOFLIPHW";
        break;
    case 0x887600f0:
        error = "DDERR_NOGDI";
        break;
    case 0x887600fa:
        error = "DDERR_NOMIRRORHW";
        break;
    case 0x88760104:
        error = "DDERR_NOOVERLAYHW";
        break;
    case 0x88760118:
        error = "DDERR_NORASTEROPHW";
        break;
    case 0x88760122:
        error = "DDERR_NOROTATIONHW";
        break;
    case 0x88760136:
        error = "DDERR_NOSTRETCHHW";
        break;
    case 0x8876013c:
        error = "DDERR_NOT4BITCOLOR";
        break;
    case 0x8876013d:
        error = "DDERR_NOT4BITCOLORINDEX";
        break;
    case 0x88760140:
        error = "DDERR_NOT8BITCOLOR";
        break;
    case 0x8876014a:
        error = "DDERR_NOTEXTUREHW";
        break;
    case 0x887600ff:
        error = "DDERR_NOTFOUND";
        break;
    case 0x8876014f:
        error = "DDERR_NOVSYNCHW";
        break;
    case 0x88760154:
        error = "DDERR_NOZBUFFERHW";
        break;
    case 0x8876015e:
        error = "DDERR_NOZOVERLAYHW";
        break;
    case 0x8876023e:
        error = "DDERR_BLTFASTCANTCLIP";
        break;
    case 0x887601b3:
        error = "DDERR_CANTLOCKSURFACE";
        break;
    case 0x88760237:
        error = "DDERR_CLIPPERISUSINGHWND";
        break;
    case 0x88760190:
        error = "DDERR_COLORKEYNOTSET";
        break;
    case 0x88760232:
        error = "DDERR_DIRECTDRAWALREADYCREATED";
        break;
    case 0x8876023b:
        error = "DDERR_HWNDALREADYSET";
        break;
    case 0x8876023a:
        error = "DDERR_HWNDSUBCLASSED";
        break;
    case 0x88760231:
        error = "DDERR_INVALIDDIRECTDRAWGUID";
        break;
    case 0x88760243:
        error = "DDERR_INVALIDPOSITION";
        break;
    case 0x8876023f:
        error = "DDERR_NOBLTHW";
        break;
    case 0x88760238:
        error = "DDERR_NOCLIPPERATTACHED";
        break;
    case 0x88760240:
        error = "DDERR_NODDROPSHW";
        break;
    case 0x88760233:
        error = "DDERR_NODIRECTDRAWHW";
        break;
    case 0x88760235:
        error = "DDERR_NOEMULATION";
        break;
    case 0x88760239:
        error = "DDERR_NOHWND";
        break;
    case 0x88760242:
        error = "DDERR_NOOVERLAYDEST";
        break;
    case 0x8876023c:
        error = "DDERR_NOPALETTEATTACHED";
        break;
    case 0x8876023d:
        error = "DDERR_NOPALETTEHW";
        break;
    case 0x88760244:
        error = "DDERR_NOTAOVERLAYSURFACE";
        break;
    case 0x88760168:
        error = "DDERR_OUTOFCAPS";
        break;
    case 0x8876017c:
        error = "DDERR_OUTOFVIDEOMEMORY";
        break;
    case 0x8876017e:
        error = "DDERR_OVERLAYCANTCLIP";
        break;
    case 0x88760180:
        error = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
        break;
    case 0x88760241:
        error = "DDERR_OVERLAYNOTVISIBLE";
        break;
    case 0x88760183:
        error = "DDERR_PALETTEBUSY";
        break;
    case 0x88760234:
        error = "DDERR_PRIMARYSURFACEALREADYEXISTS";
        break;
    case 0x88760236:
        error = "DDERR_REGIONTOOSMALL";
        break;
    case 0x8876019a:
        error = "DDERR_SURFACEALREADYATTACHED";
        break;
    case 0x887601a4:
        error = "DDERR_SURFACEALREADYDEPENDENT";
        break;
    case 0x887601ae:
        error = "DDERR_SURFACEBUSY";
        break;
    case 0x887601b8:
        error = "DDERR_SURFACEISOBSCURED";
        break;
    case 0x887601c2:
        error = "DDERR_SURFACELOST";
        break;
    case 0x887601cc:
        error = "DDERR_SURFACENOTATTACHED";
        break;
    case 0x887601d6:
        error = "DDERR_TOOBIGHEIGHT";
        break;
    case 0x887601e0:
        error = "DDERR_TOOBIGSIZE";
        break;
    case 0x887601ea:
        error = "DDERR_TOOBIGWIDTH";
        break;
    case 0x887601fe:
        error = "DDERR_UNSUPPORTEDFORMAT";
        break;
    case 0x88760208:
        error = "DDERR_UNSUPPORTEDMASK";
        break;
    case 0x88760219:
        error = "DDERR_VERTICALBLANKINPROGRESS";
        break;
    case 0x8876021c:
        error = "DDERR_WASSTILLDRAWING";
        break;
    case 0x88760230:
        error = "DDERR_XALIGN";
        break;
    case 0x88760245:
        error = "DDERR_EXCLUSIVEMODEALREADYSET";
        break;
    case 0x88760249:
        error = "DDERR_CANTCREATEDC";
        break;
    case 0x88760247:
        error = "DDERR_CANTDUPLICATE";
        break;
    case 0x8876024c:
        error = "DDERR_IMPLICITLYCREATED";
        break;
    case 0x88760250:
        error = "DDERR_INVALIDSURFACETYPE";
        break;
    case 0x8876024a:
        error = "DDERR_NODC";
        break;
    case 0x8876024f:
        error = "DDERR_NOMIPMAPHW";
        break;
    case 0x88760246:
        error = "DDERR_NOTFLIPPABLE";
        break;
    case 0x88760248:
        error = "DDERR_NOTLOCKED";
        break;
    case 0x8876024d:
        error = "DDERR_NOTPALETTIZED";
        break;
    case 0x8876024e:
        error = "DDERR_UNSUPPORTEDMODE";
        break;
    case 0x8876024b:
        error = "DDERR_WRONGMODE";
        break;
    case 0x8876026c:
        error = "DDERR_DCALREADYCREATED";
        break;
    case 0x88760280:
        error = "DDERR_CANTPAGELOCK";
        break;
    case 0x88760294:
        error = "DDERR_CANTPAGEUNLOCK";
        break;
    case 0x887602b2:
        error = "DDERR_MOREDATA";
        break;
    case 0x88760276:
        error = "DDERR_NONONLOCALVIDMEM";
        break;
    case 0x887602a8:
        error = "DDERR_NOTPAGELOCKED";
        break;
    case 0x887602b7:
        error = "DDERR_VIDEONOTACTIVE";
        break;
    case 0x887602bc:
        error = "D3DERR_BADMAJORVERSION";
        break;
    case 0x887602bd:
        error = "D3DERR_BADMINORVERSION";
        break;
    case 0x887602c1:
        error = "D3DERR_INVALID_DEVICE";
        break;
    case 0x887602c2:
        error = "D3DERR_INITFAILED";
        break;
    case 0x887602c6:
        error = "D3DERR_EXECUTE_CREATE_FAILED";
        break;
    case 0x887602c7:
        error = "D3DERR_EXECUTE_DESTROY_FAILED";
        break;
    case 0x887602c8:
        error = "D3DERR_EXECUTE_LOCK_FAILED";
        break;
    case 0x887602c9:
        error = "D3DERR_EXECUTE_UNLOCK_FAILED";
        break;
    case 0x887602ca:
        error = "D3DERR_EXECUTE_LOCKED";
        break;
    case 0x887602cb:
        error = "D3DERR_EXECUTE_NOT_LOCKED";
        break;
    case 0x887602cc:
        error = "D3DERR_EXECUTE_FAILED";
        break;
    case 0x887602cd:
        error = "D3DERR_CLIPPED_FAILED";
        break;
    case 0x887602d0:
        error = "D3DERR_TEXTURE_NO_SUPPORT";
        break;
    case 0x887602d1:
        error = "D3DERR_TEXTURE_CREATE_FAILED";
        break;
    case 0x887602d2:
        error = "D3DERR_TEXTURE_DESTROY_FAILED";
        break;
    case 0x887602d3:
        error = "D3DERR_TEXTURE_LOCK_FAILED";
        break;
    case 0x887602d4:
        error = "D3DERR_TEXTURE_UNLOCK_FAILED";
        break;
    case 0x887602d5:
        error = "D3DERR_LOAD_FAILED";
        break;
    case 0x887602d6:
        error = "D3DERR_SWAP_FAILED";
        break;
    case 0x887602d7:
        error = "D3DERR_TEXTURE_LOCKED";
        break;
    case 0x887602d8:
        error = "D3DERR_TEXTURE_NOT_LOCKED";
        break;
    case 0x887602d9:
        error = "D3DERR_TEXTURE_GETSURF_FAILED";
        break;
    case 0x887602da:
        error = "D3DERR_MATRIX_CREATE_FAILED";
        break;
    case 0x887602db:
        error = "D3DERR_MATRIX_DESTROY_FAILED";
        break;
    case 0x887602dc:
        error = "D3DERR_MATRIX_SETDATA_FAILED";
        break;
    case 0x887602dd:
        error = "D3DERR_MAXTRIX_GETDATA_FAILED";
        break;
    case 0x887602de:
        error = "D3DERR_SETVIEWPORTDATA_FAILED";
        break;
    case 0x887602df:
        error = "D3DERR_INVALIDCURRENTVIEWPORT";
        break;
    case 0x887602e0:
        error = "D3DERR_INVALIDPRIMITIVETYPE";
        break;
    case 0x887602e1:
        error = "D3DERR_INVALIDVERTEXTYPE";
        break;
    case 0x887602e2:
        error = "D3DERR_TEXTURE_BADSIZE";
        break;
    case 0x887602e4:
        error = "D3DERR_MATERIAL_CREATE_FAILED";
        break;
    case 0x887602e5:
        error = "D3DERR_MATERIAL_DESTROY_FAILED";
        break;
    case 0x887602e6:
        error = "D3DERR_MATERIAL_SETDATA_FAILED";
        break;
    case 0x887602e7:
        error = "D3DERR_MATERIAL_GETDATA_FAILED";
        break;
    case 0x887602e8:
        error = "D3DERR_INVALIDPALETTE";
        break;
    case 0x887602e9:
        error = "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY";
        break;
    case 0x887602ea:
        error = "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY";
        break;
    case 0x887602eb:
        error = "D3DERR_SURFACENOTINVIDMEM";
        break;
    case 0x887602ee:
        error = "D3DERR_LIGHT_SET_FAILED";
        break;
    case 0x887602ef:
        error = "D3DERR_LIGHTHASVIEWPORT";
        break;
    case 0x887602f0:
        error = "D3DERR_LIGHTNOTINTHISVIEWPORT";
        break;
    case 0x887602f8:
        error = "D3DERR_SCENE_IN_SCENE";
        break;
    case 0x887602f9:
        error = "D3DERR_SCENE_NOT_IN_SCENE";
        break;
    case 0x887602fa:
        error = "D3DERR_SCENE_BEGIN_FAILED";
        break;
    case 0x887602fb:
        error = "D3DERR_SCENE_END_FAILED";
        break;
    case 0x88760302:
        error = "D3DERR_INBEGIN";
        break;
    case 0x88760303:
        error = "D3DERR_NOTINBEGIN";
        break;
    case 0x88760304:
        error = "D3DERR_NOVIEWPORTS";
        break;
    case 0x88760305:
        error = "D3DERR_VIEWPORTDATANOTSET";
        break;
    case 0x88760306:
        error = "D3DERR_VIEWPORTHASNODEVICE";
        break;
    case 0x8876030d:
        error = "D3DRMERR_BADOBJECT";
        break;
    case 0x8876030e:
        error = "D3DRMERR_BADTYPE";
        break;
    case 0x8876030f:
        error = "D3DRMERR_BADALLOC";
        break;
    case 0x88760310:
        error = "D3DRMERR_FACEUSED";
        break;
    case 0x88760311:
        error = "D3DRMERR_NOTFOUND";
        break;
    case 0x88760312:
        error = "D3DRMERR_NOTDONEYET";
        break;
    case 0x88760313:
        error = "D3DRMERR_FILENOTFOUND";
        break;
    case 0x88760314:
        error = "D3DRMERR_BADFILE";
        break;
    case 0x88760315:
        error = "D3DRMERR_BADDEVICE";
        break;
    case 0x88760316:
        error = "D3DRMERR_BADVALUE";
        break;
    case 0x88760317:
        error = "D3DRMERR_BADMAJORVERSION";
        break;
    case 0x88760318:
        error = "D3DRMERR_BADMINORVERSION";
        break;
    case 0x88760319: // D3DRMERR_UNSUPPORTED; its own case with the copied string (one more case group sets VC6's split point)
        error = "D3DRMERR_BADMINORVERSION";
        break;
    case 0x00000000:
        error = "DD_OK";
        break;
    default:
        wsprintf(unknown, "Unknown Error #%ld", hr & 0xffff);
        error = unknown;
        break;
    }
    unsigned long length = strlen(error);
    if (length >= size)
        length = size - 1;
    if (length) {
        strncpy(buffer, error, length);
        buffer[length] = 0;
    }
    return 1;
}

// 0x100cc830 ??1?$ValList@PAUD3DTextureFormatInfo@Device@DX@@@@QAE@XZ
// TODO 0x100ca180 ?GetCurrentDevice@DirectDraw@DX@@QAEAAUDDDeviceInfo@2@XZ: inline in DX_DirectDraw.h (matches in isolation); emitted here only once Device::Create calls it out of line
// 0x100cc870 ??A?$ValList@PAUD3DTextureFormatInfo@Device@DX@@@@QAEAAPAUD3DTextureFormatInfo@Device@DX@@H@Z
// 0x100cc8b0 ??A?$ValList@UD3DDriverInfo@DX@@@@QAEAAUD3DDriverInfo@DX@@H@Z

// 0x100cc8f0 _$E27
// 0x100cc920 _$E23
// 0x100cc930 _$E26
