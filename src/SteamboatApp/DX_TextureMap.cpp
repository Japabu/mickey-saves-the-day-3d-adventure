#include "DX_TextureMap.h"
#include "DX_DirectDraw.h"
#include "DX_Device.h"
#include "DX_TextureMapManager.h"
#include "DX_Exception.h"

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXTextureMap.cpp"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d3040 ??0TextureMap@DX@@QAE@PAVDirectDraw@1@@Z
DX::TextureMap::TextureMap(DX::DirectDraw *directDraw)
    : DirectDrawSurface(directDraw), m_lpTexture(0), m_bOwnDC(false)
{
}

// 0x100d3070 ??_GTextureMap@DX@@UAEPAXI@Z

// 0x100d3090 ??1TextureMap@DX@@UAE@XZ
DX::TextureMap::~TextureMap()
{
    if (m_lpTexture) {
        m_lpTexture->Release();
        m_lpTexture = 0;
    }
}

// TODO 0x100d3100 ?LoadTexture@TextureMap@DX@@QAEXABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVDevice@2@AAUDirectDrawSurfaceAttrib@2@@Z: 81% - same calls now (CanMIPMap reads m_DirectDrawDevices.Curr() directly, as the original calls Curr); the original inlines one of the three DDDeviceInfo temporaries\' destructors further (its buffer delete) and gives each thrown exception its own stack slot inside the try block
void DX::TextureMap::LoadTexture(const std::string &fileName, DX::Device *device, DX::DirectDrawSurfaceAttrib &attrib)
{
    HRESULT hr;
    IDirectDrawPalette *palette = 0;
    unsigned long colorKey = 0;
    Release();
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    device->FindSurfaceDesc(&ddsd, GetDirectDraw()->CanMIPMap() && attrib.isMipMap, attrib.hasColorKey, attrib.isAlpha, m_bOwnDC);
    DX::DirectDrawSurface *source = new DX::DirectDrawSurface(GetDirectDraw());
    try {
        source->LoadSurface(fileName.c_str(), &ddsd, DDSCAPS_SYSTEMMEMORY, GetDirectDraw()->CanMIPMap() && attrib.isMipMap,
                            attrib.hasColorKey, attrib.isEmulation, &colorKey);
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        source->GetSurfDesc(&ddsd);
        unsigned long caps;
        if (GetDirectDraw()->CanMIPMap() && attrib.isMipMap && !attrib.isEmulation) {
            ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
            caps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX | DDSCAPS_ALLOCONLOAD;
        } else {
            ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
            caps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
        }
        if (attrib.isEmulation)
            caps |= DDSCAPS_SYSTEMMEMORY;
        else if (attrib.isAGP)
            caps |= DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
        else
            caps |= DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps = caps;
        if (m_bOwnDC)
            ddsd.ddsCaps.dwCaps |= DDSCAPS_OWNDC;
        hr = GetDirectDraw()->CreateSurface(&ddsd, &m_lpTexture);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 143, "Could not create the destination texture surface");
        unsigned long paletteCaps;
        if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
            paletteCaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
        else
            paletteCaps = (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) ? DDPCAPS_4BIT : 0;
        if (attrib.hasColorKey) {
            DDCOLORKEY key;
            key.dwColorSpaceLowValue = colorKey;
            key.dwColorSpaceHighValue = colorKey;
            hr = m_lpTexture->SetColorKey(DDCKEY_SRCBLT, &key);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 168, "Error setting color key");
        }
        if (paletteCaps) {
            PALETTEENTRY entries[256];
            memset(entries, 0, sizeof(entries));
            if (GetDirectDraw()->GetCOMInterface()) {
                hr = GetDirectDraw()->GetCOMInterface()->CreatePalette(paletteCaps, entries, &palette, 0);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 174, "Failed to create a palette for the destination texture");
            }
            hr = m_lpTexture->SetPalette(palette);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 175, "Failed to set the destination texture palette");
            if (palette)
                palette->Release();
        }
        POINT point;
        RECT rect;
        point.x = 0;
        point.y = 0;
        rect.left = 0;
        rect.top = 0;
        rect.right = ddsd.dwWidth;
        rect.bottom = ddsd.dwHeight;
        hr = device->GetCOMInterface()->Load(m_lpTexture, &point, source->GetCOMInterface(), &rect, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 197, "Could not load a source texture into a destination texture");
        delete source;
    } catch (...) {
        delete source;
        throw;
    }
}

// 0x100d35c0 ?Curr@?$ValList@UDDDeviceInfo@DX@@@@QAE?AUDDDeviceInfo@DX@@XZ

// 0x100d3850 _$E27
// 0x100d3880 _$E23
// 0x100d3890 _$E26
