// PORT override: 32-bit pixels of a locked surface: the original writes them through unsigned long (4 bytes on Win32, 8 here), so the row pointer is unsigned int *.
#include "DX_DirectDrawSurface.h"
#include "DX_DirectDraw.h"
#include "DX_Exception.h"
#include "ddutil.h"
#include "StdString.h"
#include "libpng/png.h"
#include <string.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXDirectDrawSurface.cpp"

// 0x100d0350 ??0DirectDrawSurface@DX@@QAE@PAVDirectDraw@1@@Z
DX::DirectDrawSurface::DirectDrawSurface(DX::DirectDraw *directDraw)
    : m_IDirectDrawSurface7(0), m_pDirectDraw(directDraw), m_szImageName(0)
{
}

// 0x100d0370 ??_GDirectDrawSurface@DX@@UAEPAXI@Z

// 0x100d0390 ?Create@DirectDrawSurface@DX@@QAEXPBDKK@Z
void DX::DirectDrawSurface::Create(const char *imageName, unsigned long width, unsigned long height)
{
    if (!imageName)
        return;
    m_szImageName = strdup(imageName);
    m_IDirectDrawSurface7 = DDLoadBitmap(m_pDirectDraw->GetCOMInterface(), imageName, width, height, DDSCAPS_OFFSCREENPLAIN);
    if (!m_IDirectDrawSurface7)
        throw DX::Exception(110, "Error loading bitmap into surface");
    memset(&m_ddsDesc2, 0, sizeof(m_ddsDesc2));
    m_ddsDesc2.dwSize = sizeof(m_ddsDesc2);
    HRESULT hr = m_IDirectDrawSurface7->GetSurfaceDesc(&m_ddsDesc2);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 45, "Error getting surface description");
}

// 0x100d0460 ?Create@DirectDrawSurface@DX@@QAEXAAU_DDSURFACEDESC2@@@Z
void DX::DirectDrawSurface::Create(DDSURFACEDESC2 &desc)
{
    HRESULT hr = m_pDirectDraw->CreateSurface(&desc, &m_IDirectDrawSurface7);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 53, "Error creating surface");
    memset(&m_ddsDesc2, 0, sizeof(m_ddsDesc2));
    m_ddsDesc2.dwSize = sizeof(m_ddsDesc2);
    m_ddsDesc2 = desc;
}

// 0x100d04d0 ?Attach@DirectDrawSurface@DX@@QAEXPAUIDirectDrawSurface7@@@Z
void DX::DirectDrawSurface::Attach(IDirectDrawSurface7 *surface)
{
    m_IDirectDrawSurface7 = surface;
    surface->AddRef();
    memset(&m_ddsDesc2, 0, sizeof(m_ddsDesc2));
    m_ddsDesc2.dwSize = sizeof(m_ddsDesc2);
    HRESULT hr = m_IDirectDrawSurface7->GetSurfaceDesc(&m_ddsDesc2);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 69, "Error getting surface description");
}

// 0x100d0540 ??1DirectDrawSurface@DX@@UAE@XZ
DX::DirectDrawSurface::~DirectDrawSurface()
{
    if (m_szImageName)
        free(m_szImageName);
    Release();
}

// 0x100d0580 ?Clear@DirectDrawSurface@DX@@QAEXXZ
void DX::DirectDrawSurface::Clear()
{
    DDSURFACEDESC2 ddsd;
    GetSurfDesc(&ddsd);
    DDBLTFX fx;
    memset(&fx, 0, sizeof(fx));
    fx.dwSize = sizeof(fx);
    RECT rect;
    SetRect(&rect, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
    HRESULT hr = m_IDirectDrawSurface7->Blt(&rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &fx);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 108, "Error clearing surface");
}

// 0x100d0620 ?Restore@DirectDrawSurface@DX@@QAEJXZ
long DX::DirectDrawSurface::Restore()
{
    HRESULT hr = m_IDirectDrawSurface7->Restore();
    if (hr == DD_OK) {
        if (m_szImageName)
            hr = DDReLoadBitmap(m_IDirectDrawSurface7, m_szImageName);
    } else if (hr == DDERR_WRONGMODE) {
        Release();
        if (m_szImageName) {
            if (m_IDirectDrawSurface7) {
                memset(&m_ddsDesc2, 0, sizeof(m_ddsDesc2));
                m_ddsDesc2.dwSize = sizeof(m_ddsDesc2);
                hr = m_IDirectDrawSurface7->GetSurfaceDesc(&m_ddsDesc2);
                if (hr != DD_OK)
                    throw DX::Exception(hr, THIS_FILE, 133, "Error getting surface description");
                return DD_OK;
            }
            return E_FAIL;
        }
        return m_pDirectDraw->CreateSurface(&m_ddsDesc2, &m_IDirectDrawSurface7);
    }
    return hr;
}

// TODO 0x100d0710 ?LoadSurface@DirectDrawSurface@DX@@QAEXPBDPAU_DDSURFACEDESC2@@KHHHPAK@Z: 58% - the string temporaries (Right, the lower-case copy) and their EH states differ
void DX::DirectDrawSurface::LoadSurface(const char *fileName, DDSURFACEDESC2 *desc, unsigned long flags, int mipMap,
                                        int alpha, int colorKeyed, unsigned long *colorKey)
{
    _SS<char> name(fileName);
    _SS<char> extension = name.Right(3);
    if (!extension.empty())
        _strupr(extension.Buffer());
    _SS<char> lower = extension;
    if (!lower.empty())
        _strlwr(lower.Buffer());
    if (extension == "PPM")
        LoadPPMSurface(fileName, desc, flags, colorKey);
    else if (extension == "PNG")
        LoadPNGSurface(fileName, desc, flags, mipMap, alpha, colorKeyed, colorKey);
    else
        throw DX::Exception(110, "Error loading bitmap");
}

// EQUIV 0x100d0980 ?LoadPNGSurface@DirectDrawSurface@DX@@QAEXPBDPAU_DDSURFACEDESC2@@KHHHPAK@Z: reviewed: order of independent instructions and register choice in one block (the caps argument is loaded at the top of the block and the descriptor field copies are interleaved differently); same values stored to the same fields
void DX::DirectDrawSurface::LoadPNGSurface(const char *fileName, DDSURFACEDESC2 *desc, unsigned long flags, int mipMap,
                                           int alpha, int colorKeyed, unsigned long *colorKey)
{
    HRESULT hr;
    DX::PNGFileFormat *png = LoadPNG(fileName);
    DDSURFACEDESC2 original = *desc;
    DDSURFACEDESC2 ddsd;
    int mipCount;
    if (mipMap && !colorKeyed) {
        mipCount = CountMipMapFiles(fileName);
        ddsd = original;
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
        ddsd.dwMipMapCount = mipCount;
    } else {
        ddsd = *desc;
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps = flags | DDSCAPS_TEXTURE;
    }
    ddsd.dwHeight = png->height;
    ddsd.dwWidth = png->width;
    if (m_pDirectDraw->GetCOMInterface()) {
        hr = m_pDirectDraw->GetCOMInterface()->CreateSurface(&ddsd, &m_IDirectDrawSurface7, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 235, "CreateSurface for texture failed");
    }
    if (png->width * 4 == png->rowBytes) {
        if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16 && ddsd.ddpfPixelFormat.dwRBitMask == 0xf00
            && ddsd.ddpfPixelFormat.dwGBitMask == 0xf0 && ddsd.ddpfPixelFormat.dwBBitMask == 0xf
            && ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xf000)
            Copy32_to_16(png, m_IDirectDrawSurface7);
    } else {
        if (png->lpDDPal) {
            if (ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
                Copy8_to_Palette8(png, colorKey, m_IDirectDrawSurface7);
            else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16 && !(ddsd.ddpfPixelFormat.dwFlags & (DDPF_ALPHAPIXELS | DDPF_ALPHA)))
                Copy8_to_16(png, colorKey, m_IDirectDrawSurface7);
        } else {
            throw DX::Exception(11, "this png file does not have 4 channels");
        }
    }
    if (png) {
        if (png->lpDDPal) {
            png->lpDDPal->Release();
            png->lpDDPal = 0;
        }
        if (png->pixels) {
            delete png->pixels;
            png->pixels = 0;
        }
        delete png;
        png = 0;
    }
    if (mipMap && !colorKeyed) {
        IDirectDrawSurface7 *surface = m_IDirectDrawSurface7;
        surface->AddRef();
        for (int level = 1; level < mipCount; level++) {
            _SS<char> mipName = GetMipMapFileName(fileName, level);
            if (!png)
                png = LoadPNG(mipName);
            if (!png)
                throw DX::Exception("Error loading png mipmap");
            DDSCAPS2 caps;
            IDirectDrawSurface7 *next;
            caps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
            hr = surface->GetAttachedSurface(&caps, &next);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 312, "Error getting attached surface for mipmap");
            if (png->width * 4 == png->rowBytes) {
                if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16 && ddsd.ddpfPixelFormat.dwRBitMask == 0xf00
                    && ddsd.ddpfPixelFormat.dwGBitMask == 0xf0 && ddsd.ddpfPixelFormat.dwBBitMask == 0xf
                    && ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xf000)
                    Copy32_to_16(png, next);
            } else if (png->lpDDPal) {
                if (ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
                    Copy8_to_Palette8(png, colorKey, next);
                else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16 && !(ddsd.ddpfPixelFormat.dwFlags & (DDPF_ALPHAPIXELS | DDPF_ALPHA)))
                    Copy8_to_16(png, colorKey, next);
            }
            if (png) {
                if (png->pixels)
                    delete png->pixels;
                delete png;
                png = 0;
            }
            surface->Release();
            surface = next;
        }
    }
}

// EQUIV 0x100d0df0 ?LoadPNG@DirectDrawSurface@DX@@QAEPAUPNGFileFormat@2@PBD@Z: reviewed: register choice around the two new[] sizes and the CreatePalette call, and the store of one PNGFileFormat field scheduled two instructions earlier (independent of the loads between); same calls, arguments and stores
DX::PNGFileFormat *DX::DirectDrawSurface::LoadPNG(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (ferror(file))
        throw DX::Exception(110, "Cannon load %s", fileName);
    if (!file)
        throw DX::Exception(110, "Cannot find %s", fileName);
    unsigned char header[8];
    size_t read = fread(header, 1, 8, file);
    if (ferror(file))
        throw DX::Exception(110, "Cannot read from %s because %s", fileName, strerror(ferror(file)));
    if (!read) {
        fclose(file);
        throw DX::Exception(11, "%s is not a PNG file", fileName);
    }
    if (png_sig_cmp(header, 0, 8)) {
        fclose(file);
        throw DX::Exception(11, "%s is not a PNG file", fileName);
    }
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png) {
        fclose(file);
        throw DX::Exception(11, "unable to create read struct");
    }
    png_infop info = png_create_info_struct(png);
    if (!info) {
        fclose(file);
        png_destroy_read_struct(&png, 0, 0);
        throw DX::Exception(11, "unable to create info struct");
    }
    png_infop endInfo = png_create_info_struct(png);
    if (!endInfo) {
        fclose(file);
        png_destroy_read_struct(&png, &info, 0);
        throw DX::Exception(11, "unable to create info struct");
    }
    png_init_io(png, file);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);
    if (info->bit_depth == 16)
        png_set_strip_16(png);
    if (info->bit_depth < 8)
        png_set_packing(png);
    if (info->color_type == PNG_COLOR_TYPE_GRAY || info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_read_update_info(png, info);
    unsigned char *pixels = new unsigned char[info->rowbytes * info->height];
    DX::PNGFileFormat *result = new DX::PNGFileFormat;
    result->pixels = 0;
    result->lpDDPal = 0;
    png_bytep *rows = new png_bytep[info->height];
    for (png_uint_32 row = 0; row < info->height; row++)
        rows[row] = pixels + row * info->rowbytes;
    png_read_image(png, rows);
    png_read_end(png, endInfo);
    result->width = info->width;
    result->height = info->height;
    result->channelDepth = info->bit_depth;
    result->numChannels = info->channels;
    result->pixelDepth = info->pixel_depth;
    result->pixels = pixels;
    result->rowBytes = info->rowbytes;
    result->numColors = info->num_palette;
    if (info->color_type == PNG_COLOR_TYPE_PALETTE) {
        IDirectDrawPalette *palette = 0;
        PALETTEENTRY entries[256];
        int i;
        for (i = 0; i < 256; i++) {
            entries[i].peRed = 0xff;
            entries[i].peGreen = 0xff;
            entries[i].peBlue = 0xff;
        }
        for (i = 0; i < info->num_palette; i++) {
            entries[i].peRed = info->palette[i].red;
            entries[i].peGreen = info->palette[i].green;
            entries[i].peBlue = info->palette[i].blue;
        }
        IDirectDraw7 *directDraw = m_pDirectDraw->GetCOMInterface();
        if (directDraw)
            directDraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, entries, &palette, 0);
        result->lpDDPal = palette;
    }
    delete[] rows;
    png_destroy_read_struct(&png, &info, &endInfo);
    fclose(file);
    return result;
}

// TODO 0x100d1210 ?LoadPPMSurface@DirectDrawSurface@DX@@QAEXPBDPAU_DDSURFACEDESC2@@KPAK@Z: 75% - reconstruction of the SDK-style PPM loader; loop variables and the colour-key stores differ
void DX::DirectDrawSurface::LoadPPMSurface(const char *fileName, DDSURFACEDESC2 *desc, unsigned long flags, unsigned long *colorKey)
{
    HRESULT hr;
    BOOL paletted = FALSE;
    int maxColors;
    unsigned long paletteCaps;
    IDirectDrawPalette *palette;
    FILE *file = fopen(fileName, "rb");
    if (!file)
        throw DX::Exception(110, "Cannot find %s", fileName);
    char line[100];
    fgets(line, 100, file);
    if (lstrcmp(line, "P6\n")) {
        fclose(file);
        throw DX::Exception(11, "%s is not a PPM file", fileName);
    }
    do
        fgets(line, 100, file);
    while (line[0] == '#');
    int width, height;
    sscanf(line, "%d %d\n", &width, &height);
    fgets(line, 100, file);
    DDSURFACEDESC2 format = *desc;
    if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) {
        paletted = TRUE;
        maxColors = 256;
        paletteCaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
    } else if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) {
        paletted = TRUE;
        maxColors = 16;
        paletteCaps = DDPCAPS_4BIT;
    }
    DDSURFACEDESC2 ddsd = *desc;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.dwHeight = height;
    ddsd.dwWidth = width;
    ddsd.ddsCaps.dwCaps = flags | DDSCAPS_TEXTURE;
    if (m_pDirectDraw->GetCOMInterface()) {
        hr = m_pDirectDraw->GetCOMInterface()->CreateSurface(&ddsd, &m_IDirectDrawSurface7, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 614, "CreateSurface for texture failed");
    }
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    hr = m_IDirectDrawSurface7->Lock(0, &ddsd, 0, 0);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 622, "Lock failed while loading surface");
    int x, y;
    if (!paletted) {
        BOOL first = TRUE;
        unsigned long mask;
        int redShift, greenShift, blueShift;
        for (redShift = 0, mask = ddsd.ddpfPixelFormat.dwRBitMask; !(mask & 1); redShift++, mask >>= 1)
            ;
        int redScale = 255 / (ddsd.ddpfPixelFormat.dwRBitMask >> redShift);
        for (greenShift = 0, mask = ddsd.ddpfPixelFormat.dwGBitMask; !(mask & 1); greenShift++, mask >>= 1)
            ;
        int greenScale = 255 / (ddsd.ddpfPixelFormat.dwGBitMask >> greenShift);
        for (blueShift = 0, mask = ddsd.ddpfPixelFormat.dwBBitMask; !(mask & 1); blueShift++, mask >>= 1)
            ;
        int blueScale = 255 / (ddsd.ddpfPixelFormat.dwBBitMask >> blueShift);
        switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {
        case 32:
            for (y = 0; y < height; y++) {
                unsigned int *dst = (unsigned int *)((unsigned char *)ddsd.lpSurface + ddsd.lPitch * y);
                for (x = 0; x < width; x++) {
                    int red = getc(file) / redScale;
                    int green = getc(file) / greenScale;
                    int blue = getc(file) / blueScale;
                    *dst = (red << redShift) | (green << greenShift) | (blue << blueShift);
                    if (first) {
                        first = FALSE;
                        *colorKey = *dst;
                    }
                    dst++;
                }
            }
            break;
        case 16:
            for (y = 0; y < height; y++) {
                unsigned short *dst = (unsigned short *)((unsigned char *)ddsd.lpSurface + ddsd.lPitch * y);
                for (x = 0; x < width; x++) {
                    int red = getc(file) / redScale;
                    int green = getc(file) / greenScale;
                    int blue = getc(file) / blueScale;
                    *dst = (unsigned short)((red << redShift) | (green << greenShift) | (blue << blueShift));
                    if (first) {
                        first = FALSE;
                        *colorKey = (red << redShift) | (green << greenShift) | (blue << blueShift);
                    }
                    dst++;
                }
            }
            break;
        case 8:
            for (y = 0; y < height; y++) {
                unsigned char *dst = (unsigned char *)ddsd.lpSurface + ddsd.lPitch * y;
                for (x = 0; x < width; x++) {
                    int red = getc(file) / redScale;
                    int green = getc(file) / greenScale;
                    int blue = getc(file) / blueScale;
                    *dst = (unsigned char)((red << redShift) | (green << greenShift) | (blue << blueShift));
                    if (first) {
                        first = FALSE;
                        *colorKey = (red << redShift) | (green << greenShift) | (blue << blueShift);
                    }
                    dst++;
                }
            }
            break;
        default:
            *colorKey = 0;
            m_IDirectDrawSurface7->Unlock(0);
            fclose(file);
            m_IDirectDrawSurface7->Release();
            throw DX::Exception(11, "Unknown pixel format");
        }
        m_IDirectDrawSurface7->Unlock(0);
        fclose(file);
        return;
    }
    unsigned long colors[256];
    int numColors = 0;
    for (y = 0; y < height; y++) {
        unsigned char *dst = (unsigned char *)ddsd.lpSurface + ddsd.lPitch * y;
        for (x = 0; x < width; x++) {
            int red = getc(file);
            int green = getc(file);
            int blue = getc(file);
            unsigned long color = (((red << 8) | green) << 8) | blue;
            int i;
            for (i = 0; i < numColors; i++) {
                if (color == colors[i])
                    break;
            }
            if (i == numColors) {
                if (++numColors > maxColors)
                    numColors--;
                i = numColors - 1;
                colors[i] = color;
            }
            if (maxColors == 16) {
                if (x & 1)
                    *dst++ |= (unsigned char)(i << 4);
                else
                    *dst = (unsigned char)(i & 0xf);
            } else {
                *dst++ = (unsigned char)i;
            }
        }
    }
    fclose(file);
    m_IDirectDrawSurface7->Unlock(0);
    if (numColors > maxColors) {
        m_IDirectDrawSurface7->Unlock(0);
        m_IDirectDrawSurface7->Release();
        throw DX::Exception("Palette burst");
    }
    PALETTEENTRY entries[256];
    memset(entries, 0, sizeof(entries));
    int i;
    for (i = 0; i < numColors; i++) {
        entries[i].peRed = (unsigned char)(colors[i] >> 16);
        entries[i].peGreen = (unsigned char)(colors[i] >> 8);
        entries[i].peBlue = (unsigned char)colors[i];
    }
    for (; i < 256; i++)
        entries[i].peFlags = D3DPAL_RESERVED;
    if (m_pDirectDraw->GetCOMInterface()) {
        hr = m_pDirectDraw->GetCOMInterface()->CreatePalette(paletteCaps, entries, &palette, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 906, "Create palette failed while loading surface");
    }
    hr = m_IDirectDrawSurface7->SetPalette(palette);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 911, "SetPalette failed while loading surface");
    *colorKey = 0;
    palette->Release();
}

// 0x100d1a80 ?GetSurfDesc@DirectDrawSurface@DX@@QAEXPAU_DDSURFACEDESC2@@@Z
void DX::DirectDrawSurface::GetSurfDesc(DDSURFACEDESC2 *desc)
{
    memset(desc, 0, sizeof(*desc));
    desc->dwSize = sizeof(*desc);
    HRESULT hr = m_IDirectDrawSurface7->GetSurfaceDesc(desc);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 931, "Error getting surface description");
}

// TODO 0x100d1ae0 ?CountMipMapFiles@DirectDrawSurface@DX@@IAEHPBD@Z: 61% - the string temporaries and their EH states differ
int DX::DirectDrawSurface::CountMipMapFiles(const char *fileName)
{
    int count = 1;
    _SS<char> name(fileName);
    _SS<char> extension = name.Right(4);
    char number[16];
    sprintf(number, "%d", 1);
    _SS<char> mipName = name.Left(name.GetLength() - 4) + number + extension;
    FILE *file;
    while ((file = fopen(mipName, "r")) != 0) {
        fclose(file);
        count++;
        sprintf(number, "%d", count);
        mipName = name.Left(name.GetLength() - 4) + number + extension;
    }
    return count;
}

// TODO 0x100d1eb0 ?GetMipMapFileName@DirectDrawSurface@DX@@IAE?AV?$_SS@D@@PBDH@Z: 71% - the string temporaries and their EH states differ
_SS<char> DX::DirectDrawSurface::GetMipMapFileName(const char *fileName, int level)
{
    _SS<char> name(fileName);
    _SS<char> extension = name.Right(4);
    char number[16];
    sprintf(number, "%d", level);
    _SS<char> mipName = name.Left(name.GetLength() - 4) + number + extension;
    return mipName;
}

// TODO 0x100d2080 ?Copy8_to_5551@DirectDrawSurface@DX@@IAEXPAUHBITMAP__@@@Z: 47% - the colour-table reads (the original loads the RGBQUAD as a dword and shifts) and the register allocation differ
void DX::DirectDrawSurface::Copy8_to_5551(HBITMAP bitmap)
{
    BITMAP bm;
    GetObject(bitmap, sizeof(bm), &bm);
    HDC hdc = CreateCompatibleDC(0);
    SelectObject(hdc, bitmap);
    RGBQUAD colors[256];
    GetDIBColorTable(hdc, 0, 256, colors);
    unsigned char *bits = (unsigned char *)bm.bmBits;
    unsigned char colorKey = bits[0];
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if (m_IDirectDrawSurface7->Lock(0, &ddsd, DDLOCK_WAIT, 0) != DD_OK)
        return;
    int redScale, greenScale, blueScale, alphaScale;
    int redShift, greenShift, blueShift, alphaShift;
    GetShiftAndScale(ddsd.ddpfPixelFormat, redScale, greenScale, blueScale, alphaScale,
                     redShift, greenShift, blueShift, alphaShift);
    int row = bm.bmHeight - 1;
    for (int y = 0; y < bm.bmHeight; y++, row--) {
        unsigned short *dst = (unsigned short *)((unsigned char *)ddsd.lpSurface + ddsd.lPitch * row);
        unsigned char *src = bits + bm.bmWidthBytes * y;
        for (int x = 0; x < bm.bmWidth; x++) {
            unsigned char index = *src++;
            RGBQUAD *color = &colors[index];
            int alpha = index != colorKey ? 0xff : 0;
            *dst++ = (unsigned short)(((color->rgbRed / redScale) << redShift) | ((color->rgbGreen / greenScale) << greenShift)
                                      | ((color->rgbBlue / blueScale) << blueShift) | ((alpha / alphaScale) << alphaShift));
        }
    }
    m_IDirectDrawSurface7->Unlock(0);
}

// EQUIV 0x100d2270 ?Copy8_to_Palette8@DirectDrawSurface@DX@@IAEXPAUPNGFileFormat@2@PAKPAUIDirectDrawSurface7@@@Z: reviewed: register choice (the first-pixel flag and the row counter swap ebx/edi, so the flag register doubles as the DDLOCK_WAIT argument either way) and ebx saved in the prologue in the original, after the Lock call here; same calls, loop and stores
void DX::DirectDrawSurface::Copy8_to_Palette8(DX::PNGFileFormat *png, unsigned long *colorKey, IDirectDrawSurface7 *surface)
{
    if (surface->SetPalette(png->lpDDPal) != DD_OK) {
        surface->Release();
        png->lpDDPal->Release();
        throw DX::Exception("SetPalette failed while loading surface");
    }
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    BOOL first = TRUE;
    if (surface->Lock(0, &ddsd, DDLOCK_WAIT, 0) == DD_OK) {
        int row = png->height - 1;
        for (int y = 0; y < png->height; y++, row--) {
            unsigned char *dst = (unsigned char *)ddsd.lpSurface + ddsd.lPitch * row;
            unsigned char *src = png->pixels + png->rowBytes * y;
            for (int x = 0; x < png->rowBytes; x++) {
                *dst++ = src[x];
                if (first) {
                    *colorKey = src[x];
                    first = FALSE;
                }
            }
        }
        surface->Unlock(0);
    }
}

// TODO 0x100d2380 ?Copy8_to_16@DirectDrawSurface@DX@@IAEXPAUPNGFileFormat@2@PAKPAUIDirectDrawSurface7@@@Z: 86% - register allocation of the shift/scale locals differs
void DX::DirectDrawSurface::Copy8_to_16(DX::PNGFileFormat *png, unsigned long *colorKey, IDirectDrawSurface7 *surface)
{
    PALETTEENTRY *palette = new PALETTEENTRY[256];
    if (png->lpDDPal->GetEntries(0, 0, 256, palette) != DD_OK)
        throw DX::Exception("could not get the palette for the png");
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    BOOL first = TRUE;
    if (surface->Lock(0, &ddsd, DDLOCK_WAIT, 0) == DD_OK) {
        int redScale, greenScale, blueScale, alphaScale;
        int redShift, greenShift, blueShift, alphaShift;
        GetShiftAndScale(ddsd.ddpfPixelFormat, redScale, greenScale, blueScale, alphaScale,
                         redShift, greenShift, blueShift, alphaShift);
        int row = png->height - 1;
        for (int y = 0; y < png->height; y++, row--) {
            unsigned short *dst = (unsigned short *)((unsigned char *)ddsd.lpSurface + ddsd.lPitch * row);
            unsigned char *src = png->pixels + png->rowBytes * y;
            for (int x = 0; x < png->width; x++) {
                PALETTEENTRY *entry = &palette[src[x]];
                int red = entry->peRed / redScale;
                int green = entry->peGreen / greenScale;
                int blue = entry->peBlue / blueScale;
                *dst++ = (unsigned short)((red << redShift) | (green << greenShift) | (blue << blueShift));
                if (first) {
                    first = FALSE;
                    *colorKey = (red << redShift) | (green << greenShift) | (blue << blueShift);
                }
            }
        }
        surface->Unlock(0);
    }
    delete[] palette;
}

// TODO 0x100d25a0 ?Copy32_to_16@DirectDrawSurface@DX@@IAEXPAUPNGFileFormat@2@PAUIDirectDrawSurface7@@@Z: 65% - the channel order and the shift/scale arguments are guessed
void DX::DirectDrawSurface::Copy32_to_16(DX::PNGFileFormat *png, IDirectDrawSurface7 *surface)
{
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if (surface->Lock(0, &ddsd, DDLOCK_WAIT, 0) != DD_OK)
        return;
    int redScale, greenScale, blueScale, alphaScale;
    int redShift, greenShift, blueShift, alphaShift;
    GetShiftAndScale(ddsd.ddpfPixelFormat, redScale, greenScale, blueScale, alphaScale,
                     redShift, greenShift, blueShift, alphaShift);
    int row = png->height - 1;
    for (int y = 0; y < png->height; y++, row--) {
        unsigned short *dst = (unsigned short *)((unsigned char *)ddsd.lpSurface + ddsd.lPitch * row);
        unsigned char *src = png->pixels + png->rowBytes * y;
        for (int x = 0; x < png->width; x++, src += 4) {
            *dst++ = (unsigned short)(((src[0] / redScale) << redShift) | ((src[1] / greenScale) << greenShift)
                                      | ((src[3] / alphaScale) << alphaShift) | ((src[2] / blueScale) << blueShift));
        }
    }
    surface->Unlock(0);
}

// 0x100d2710 ?GetShiftAndScale@DirectDrawSurface@DX@@IAEXAAU_DDPIXELFORMAT@@AAH1111111@Z
void DX::DirectDrawSurface::GetShiftAndScale(DDPIXELFORMAT &format, int &redScale, int &greenScale, int &blueScale, int &alphaScale,
                                             int &redShift, int &greenShift, int &blueShift, int &alphaShift)
{
    unsigned long mask;
    int shift;
    for (shift = 0, mask = format.dwRBitMask; !(mask & 1); shift++, mask >>= 1)
        ;
    redShift = shift;
    redScale = 255 / (format.dwRBitMask >> shift);
    for (shift = 0, mask = format.dwGBitMask; !(mask & 1); shift++, mask >>= 1)
        ;
    greenShift = shift;
    greenScale = 255 / (format.dwGBitMask >> shift);
    for (shift = 0, mask = format.dwBBitMask; !(mask & 1); shift++, mask >>= 1)
        ;
    blueShift = shift;
    blueScale = 255 / (format.dwBBitMask >> shift);
    if (format.dwFlags & DDPF_ALPHAPIXELS) {
        for (shift = 0, mask = format.dwRGBAlphaBitMask; !(mask & 1); shift++, mask >>= 1)
            ;
        alphaShift = shift;
        alphaScale = 255 / (format.dwRGBAlphaBitMask >> shift);
    }
}

// 0x100d2e30 _$E24
// 0x100d2e60 _$E20
// 0x100d2e70 _$E23

// Template and inline instances this unit also emits:

// 0x100d1d60 ?ssasn@@YAXAAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ABV12@@Z
// TODO 0x100d27d0 ??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@ABV?$allocator@D@1@@Z: not emitted by this unit yet
// TODO 0x100d27f0 ?Right@?$_SS@D@@QBE?AV1@H@Z: 54% - the original passes a constant -1 length to substr and builds the result through its own EH frame
// TODO 0x100d2980 ??H@YA?AV?$_SS@D@@ABV0@PBD@Z: 43% - the original builds _SS(pA) first and inlines operator+(s1, s2) differently
// TODO 0x100d2af0 ??H@YA?AV?$_SS@D@@ABV0@0@Z: 81% - append(s2) in the original uses a constant npos (VC6 here loads basic_string::npos)
// 0x100d2e80 ?BufferRel@?$_SS@D@@QAEXH@Z
