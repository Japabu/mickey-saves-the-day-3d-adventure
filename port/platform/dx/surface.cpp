// surface.cpp - DirectDraw surfaces, palettes and clippers in memory, the pixel format
// conversions, and the hand-over of the primary surface to the window layer.
#include "dx.h"

#include <stdlib.h>
#include <string.h>

namespace dx {

// ---- pixel formats

void PixFmt::set(const DDPIXELFORMAT &pf)
{
    zbuffer = (pf.dwFlags & DDPF_ZBUFFER) != 0;
    palette = (pf.dwFlags & DDPF_PALETTEINDEXED8) != 0;
    bytes = (int)(pf.dwRGBBitCount + 7) / 8;
    if (bytes < 1)
        bytes = 1;
    mask[0] = (pf.dwFlags & DDPF_ALPHAPIXELS) ? pf.dwRGBAlphaBitMask : 0;
    mask[1] = zbuffer || palette ? 0 : pf.dwRBitMask;
    mask[2] = zbuffer || palette ? 0 : pf.dwGBitMask;
    mask[3] = zbuffer || palette ? 0 : pf.dwBBitMask;
    for (int i = 0; i < 4; i++) {
        shift[i] = mask[i] ? __builtin_ctz(mask[i]) : 0;
        bits[i] = __builtin_popcount(mask[i]);
    }
}

uint32_t read_pixel(const uint8_t *p, int bytes)
{
    switch (bytes) {
    case 1: return p[0];
    case 2: return p[0] | p[1] << 8;
    case 3: return p[0] | p[1] << 8 | p[2] << 16;
    default: return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24;
    }
}

void write_pixel(uint8_t *p, int bytes, uint32_t v)
{
    p[0] = (uint8_t)v;
    if (bytes > 1)
        p[1] = (uint8_t)(v >> 8);
    if (bytes > 2)
        p[2] = (uint8_t)(v >> 16);
    if (bytes > 3)
        p[3] = (uint8_t)(v >> 24);
}

// A channel of n bits widened to 8 (and back), rounding to nearest.
static inline uint32_t widen(uint32_t v, int n)
{
    if (n >= 8)
        return v >> (n - 8);
    uint32_t max = (1u << n) - 1;
    return (v * 255 + max / 2) / max;
}

static inline uint32_t narrow(uint32_t c, int n)
{
    if (n >= 8)
        return c << (n - 8);
    uint32_t max = (1u << n) - 1;
    return (c * max + 127) / 255;
}

uint32_t to_argb(const PixFmt &f, uint32_t raw, const PALETTEENTRY *palette)
{
    if (f.palette) {
        if (!palette)
            return 0xff000000u | (raw & 0xff) * 0x010101u;
        const PALETTEENTRY &e = palette[raw & 0xff];
        return 0xff000000u | (uint32_t)e.peRed << 16 | (uint32_t)e.peGreen << 8 | e.peBlue;
    }
    uint32_t out = f.bits[0] ? 0 : 0xff000000u;
    for (int i = 0; i < 4; i++) {
        if (f.bits[i])
            out |= widen((raw & f.mask[i]) >> f.shift[i], f.bits[i]) << (24 - 8 * i);
    }
    return out;
}

uint32_t from_argb(const PixFmt &f, uint32_t argb, const PALETTEENTRY *palette)
{
    if (f.palette) {
        if (!palette)
            return ((argb >> 16 & 0xff) * 77 + (argb >> 8 & 0xff) * 150 + (argb & 0xff) * 29) >> 8;
        int best = 0, bestd = 1 << 30;
        for (int i = 0; i < 256 && bestd; i++) {
            int dr = (int)(argb >> 16 & 0xff) - palette[i].peRed;
            int dg = (int)(argb >> 8 & 0xff) - palette[i].peGreen;
            int db = (int)(argb & 0xff) - palette[i].peBlue;
            int d = dr * dr + dg * dg + db * db;
            if (d < bestd)
                bestd = d, best = i;
        }
        return best;
    }
    uint32_t out = 0;
    for (int i = 0; i < 4; i++) {
        if (f.bits[i])
            out |= narrow(argb >> (24 - 8 * i) & 0xff, f.bits[i]) << f.shift[i] & f.mask[i];
    }
    return out;
}

DDPIXELFORMAT rgb_format(int bpp, uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    DDPIXELFORMAT pf;
    memset(&pf, 0, sizeof(pf));
    pf.dwSize = sizeof(pf);
    pf.dwFlags = DDPF_RGB | (a ? DDPF_ALPHAPIXELS : 0);
    pf.dwRGBBitCount = bpp;
    pf.dwRBitMask = r;
    pf.dwGBitMask = g;
    pf.dwBBitMask = b;
    pf.dwRGBAlphaBitMask = a;
    return pf;
}

DDPIXELFORMAT display_format(int bpp)
{
    switch (bpp) {
    case 8: {
        DDPIXELFORMAT pf = rgb_format(8, 0, 0, 0, 0);
        pf.dwFlags |= DDPF_PALETTEINDEXED8;
        return pf;
    }
    case 16: return rgb_format(16, 0xf800, 0x07e0, 0x001f, 0);
    case 24: return rgb_format(24, 0xff0000, 0x00ff00, 0x0000ff, 0);
    default: return rgb_format(32, 0xff0000, 0x00ff00, 0x0000ff, 0);
    }
}

DDPIXELFORMAT z_format(int bits)
{
    DDPIXELFORMAT pf;
    memset(&pf, 0, sizeof(pf));
    pf.dwSize = sizeof(pf);
    pf.dwFlags = DDPF_ZBUFFER;
    pf.dwZBufferBitDepth = bits;
    pf.dwZBitMask = bits >= 32 ? 0xffffffffu : (1u << bits) - 1;
    return pf;
}

// Rows of any format as 0xAARRGGBB.
static void row_to_argb(const PixFmt &f, const uint8_t *src, uint32_t *dst, int n, const PALETTEENTRY *pal)
{
    if (f.bytes == 4 && f.mask[1] == 0xff0000 && f.mask[2] == 0xff00 && f.mask[3] == 0xff) {
        const uint32_t *s = (const uint32_t *)src;
        uint32_t a = f.mask[0] == 0xff000000u ? 0 : 0xff000000u;
        for (int i = 0; i < n; i++)
            dst[i] = s[i] | a;
    } else if (f.bytes == 2 && f.mask[1] == 0xf800 && f.mask[2] == 0x07e0 && f.mask[3] == 0x1f) {
        const uint16_t *s = (const uint16_t *)src;
        for (int i = 0; i < n; i++) {
            uint32_t v = s[i], r = v >> 11, g = v >> 5 & 63, b = v & 31;
            dst[i] = 0xff000000u | (r << 3 | r >> 2) << 16 | (g << 2 | g >> 4) << 8 | (b << 3 | b >> 2);
        }
    } else {
        for (int i = 0; i < n; i++)
            dst[i] = to_argb(f, read_pixel(src + i * f.bytes, f.bytes), pal);
    }
}

void present(Surface *s)
{
    RECT r = {0, 0, s->width(), s->height()};
    if (s->clipper && s->clipper->hwnd && !s->dd->fullscreen()) {
        // A windowed primary is the whole desktop: show the window's client area.
        RECT client;
        POINT origin = {0, 0};
        if (GetClientRect(s->clipper->hwnd, &client) && ClientToScreen(s->clipper->hwnd, &origin)) {
            RECT w = {origin.x, origin.y, origin.x + client.right, origin.y + client.bottom};
            r.left = w.left > 0 ? w.left : 0;
            r.top = w.top > 0 ? w.top : 0;
            r.right = w.right < r.right ? w.right : r.right;
            r.bottom = w.bottom < r.bottom ? w.bottom : r.bottom;
        }
    }
    int w = r.right - r.left, h = r.bottom - r.top;
    if (w <= 0 || h <= 0)
        return;
    static std::vector<uint32_t> frame;
    frame.resize((size_t)w * h);
    for (int y = 0; y < h; y++)
        row_to_argb(s->fmt, s->row(r.top + y) + r.left * s->fmt.bytes, &frame[(size_t)y * w], w, s->pal());
    port_present(frame.data(), w, h, w * 4);
}

// ---- palettes and clippers

HRESULT Palette::QueryInterface(REFIID, void **out)
{
    *out = 0;
    return E_NOINTERFACE;
}

HRESULT Palette::GetCaps(DWORD *c)
{
    *c = caps;
    return DD_OK;
}

HRESULT Palette::GetEntries(DWORD, DWORD base, DWORD n, PALETTEENTRY *e)
{
    if (!e || base > 256 || n > 256 - base)
        return DDERR_INVALIDPARAMS;
    memcpy(e, entries + base, n * sizeof(PALETTEENTRY));
    return DD_OK;
}

HRESULT Palette::Initialize(IDirectDraw *, DWORD, PALETTEENTRY *) { DX_UNSUPPORTED("IDirectDrawPalette::Initialize"); }

HRESULT Palette::SetEntries(DWORD, DWORD base, DWORD n, PALETTEENTRY *e)
{
    if (!e || base > 256 || n > 256 - base)
        return DDERR_INVALIDPARAMS;
    memcpy(entries + base, e, n * sizeof(PALETTEENTRY));
    version++;
    return DD_OK;
}

HRESULT Clipper::QueryInterface(REFIID, void **out)
{
    *out = 0;
    return E_NOINTERFACE;
}

HRESULT Clipper::GetClipList(RECT *, struct _RGNDATA *, DWORD *) { DX_UNSUPPORTED("IDirectDrawClipper::GetClipList"); }

HRESULT Clipper::GetHWnd(HWND *h)
{
    *h = hwnd;
    return DD_OK;
}

HRESULT Clipper::Initialize(IDirectDraw *, DWORD) { return DD_OK; }

HRESULT Clipper::IsClipListChanged(BOOL *changed)
{
    *changed = FALSE;
    return DD_OK;
}

HRESULT Clipper::SetClipList(struct _RGNDATA *, DWORD) { DX_UNSUPPORTED("IDirectDrawClipper::SetClipList"); }

HRESULT Clipper::SetHWnd(DWORD, HWND h)
{
    hwnd = h;
    return DD_OK;
}

// ---- surfaces

Surface::Surface(DirectDraw *dd, const DDSURFACEDESC2 &d) : dd(dd), desc(d)
{
    dd->AddRef();
    desc.lpSurface = 0;
    fmt.set(desc.ddpfPixelFormat);
    pitch = (width() * fmt.bytes + 15) & ~15;
    desc.lPitch = pitch;
    desc.dwFlags |= DDSD_PITCH;
    bits = (uint8_t *)calloc((size_t)pitch * (height() ? height() : 1), 1);
}

Surface::~Surface()
{
    if (dc)
        port_gdi_release_surface_dc(dc);
    free(dcShadow);
    for (Surface *s : attached)
        s->Release();
    if (palette)
        palette->Release();
    if (clipper)
        clipper->Release();
    if (dd->primary == this)
        dd->primary = 0;
    free(bits);
    dd->Release();
}

void Surface::changed()
{
    version++;
    if (primary())
        present(this);
}

HRESULT Surface::QueryInterface(REFIID iid, void **out)
{
    if (iid == IID_IUnknown || iid == IID_IDirectDrawSurface7) {
        AddRef();
        *out = static_cast<IDirectDrawSurface7 *>(this);
        return DD_OK;
    }
    *out = 0;
    return E_NOINTERFACE;
}

HRESULT Surface::AddAttachedSurface(IDirectDrawSurface7 *i)
{
    Surface *s = static_cast<Surface *>(i);
    if (!s || s == this)
        return DDERR_INVALIDPARAMS;
    s->AddRef();
    attached.push_back(s);
    return DD_OK;
}

HRESULT Surface::DeleteAttachedSurface(DWORD, IDirectDrawSurface7 *i)
{
    for (size_t k = 0; k < attached.size(); k++) {
        if (!i || attached[k] == i) {
            Surface *s = attached[k];
            attached.erase(attached.begin() + k);
            s->Release();
            return DD_OK;
        }
    }
    return DDERR_NOTFOUND;
}

HRESULT Surface::GetAttachedSurface(DDSCAPS2 *caps, IDirectDrawSurface7 **out)
{
    *out = 0;
    for (Surface *s : attached) {
        if ((s->desc.ddsCaps.dwCaps & caps->dwCaps) == caps->dwCaps) {
            s->AddRef();
            *out = s;
            return DD_OK;
        }
    }
    return DDERR_NOTFOUND;
}

HRESULT Surface::EnumAttachedSurfaces(void *ctx, LPDDENUMSURFACESCALLBACK7 cb)
{
    std::vector<Surface *> list = attached;
    for (Surface *s : list) {
        s->AddRef(); // the callback owns it
        DDSURFACEDESC2 d;
        s->GetSurfaceDesc(&d);
        if (cb(s, &d, ctx) == DDENUMRET_CANCEL)
            break;
    }
    return DD_OK;
}

HRESULT Surface::fill(const RECT *dst, uint32_t raw)
{
    RECT r = dst ? *dst : RECT{0, 0, width(), height()};
    if (r.left < 0) r.left = 0;
    if (r.top < 0) r.top = 0;
    if (r.right > width()) r.right = width();
    if (r.bottom > height()) r.bottom = height();
    for (int y = r.top; y < r.bottom; y++) {
        uint8_t *p = row(y) + r.left * fmt.bytes;
        if (fmt.bytes == 2) {
            uint16_t *q = (uint16_t *)p;
            for (int x = r.left; x < r.right; x++)
                *q++ = (uint16_t)raw;
        } else if (fmt.bytes == 4) {
            uint32_t *q = (uint32_t *)p;
            for (int x = r.left; x < r.right; x++)
                *q++ = raw;
        } else {
            for (int x = r.left; x < r.right; x++, p += fmt.bytes)
                write_pixel(p, fmt.bytes, raw);
        }
    }
    changed();
    return DD_OK;
}

static bool same_format(const PixFmt &a, const PixFmt &b)
{
    return a.bytes == b.bytes && a.palette == b.palette && !memcmp(a.mask, b.mask, sizeof(a.mask));
}

HRESULT Surface::blit(const RECT &d, Surface *src, const RECT &sr, bool key)
{
    int dw = d.right - d.left, dh = d.bottom - d.top;
    int sw = sr.right - sr.left, sh = sr.bottom - sr.top;
    if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0)
        return DD_OK;
    if (sr.left < 0 || sr.top < 0 || sr.right > src->width() || sr.bottom > src->height())
        return DDERR_INVALIDPARAMS;
    // the source pixels; a copy when source and destination are the same surface
    const uint8_t *sbits = src->bits;
    int spitch = src->pitch;
    std::vector<uint8_t> copy;
    if (src == this) {
        copy.assign(src->bits, src->bits + (size_t)src->pitch * src->height());
        sbits = copy.data();
    }
    const PixFmt &sf = src->fmt;
    bool convert = !same_format(sf, fmt);
    uint32_t klo = src->srcKey.dwColorSpaceLowValue, khi = src->srcKey.dwColorSpaceHighValue;
    key = key && src->hasSrcKey;
    int y0 = d.top < 0 ? 0 : d.top, y1 = d.bottom > height() ? height() : d.bottom;
    int x0 = d.left < 0 ? 0 : d.left, x1 = d.right > width() ? width() : d.right;
    for (int y = y0; y < y1; y++) {
        int sy = sr.top + (int)((int64_t)(y - d.top) * sh / dh);
        const uint8_t *srow = sbits + (size_t)sy * spitch;
        uint8_t *drow = row(y);
        if (!convert && !key && sw == dw) {
            memcpy(drow + x0 * fmt.bytes, srow + (sr.left + x0 - d.left) * sf.bytes, (size_t)(x1 - x0) * fmt.bytes);
            continue;
        }
        for (int x = x0; x < x1; x++) {
            int sx = sr.left + (int)((int64_t)(x - d.left) * sw / dw);
            uint32_t v = read_pixel(srow + sx * sf.bytes, sf.bytes);
            if (key && v >= klo && v <= khi)
                continue;
            if (convert)
                v = from_argb(fmt, to_argb(sf, v, src->pal()), pal());
            write_pixel(drow + x * fmt.bytes, fmt.bytes, v);
        }
    }
    changed();
    return DD_OK;
}

HRESULT Surface::Blt(RECT *dst, IDirectDrawSurface7 *isrc, RECT *srcRect, DWORD flags, DDBLTFX *fx)
{
    if (locks || dc)
        return DDERR_SURFACEBUSY;
    if (flags & DDBLT_COLORFILL)
        return fx ? fill(dst, fx->dwFillColor) : DDERR_INVALIDPARAMS;
    if (flags & DDBLT_DEPTHFILL)
        return fx ? fill(dst, fx->dwFillDepth) : DDERR_INVALIDPARAMS;
    Surface *src = static_cast<Surface *>(isrc);
    if (!src)
        return DDERR_INVALIDPARAMS;
    RECT d = dst ? *dst : RECT{0, 0, width(), height()};
    RECT s = srcRect ? *srcRect : RECT{0, 0, src->width(), src->height()};
    if (flags & DDBLT_KEYSRCOVERRIDE) {
        if (!fx)
            return DDERR_INVALIDPARAMS;
        DDCOLORKEY saved = src->srcKey;
        bool had = src->hasSrcKey;
        src->srcKey = fx->ddckSrcColorkey;
        src->hasSrcKey = true;
        HRESULT hr = blit(d, src, s, true);
        src->srcKey = saved;
        src->hasSrcKey = had;
        return hr;
    }
    if (flags & DDBLT_KEYDEST)
        DX_LOG("Blt: DDBLT_KEYDEST is not implemented (copied without the key)\n");
    return blit(d, src, s, (flags & DDBLT_KEYSRC) != 0);
}

HRESULT Surface::BltFast(DWORD x, DWORD y, IDirectDrawSurface7 *isrc, RECT *srcRect, DWORD flags)
{
    if (locks || dc)
        return DDERR_SURFACEBUSY;
    Surface *src = static_cast<Surface *>(isrc);
    if (!src)
        return DDERR_INVALIDPARAMS;
    RECT s = srcRect ? *srcRect : RECT{0, 0, src->width(), src->height()};
    RECT d = {(LONG)x, (LONG)y, (LONG)x + (s.right - s.left), (LONG)y + (s.bottom - s.top)};
    if (flags & DDBLTFAST_DESTCOLORKEY)
        DX_LOG("BltFast: DDBLTFAST_DESTCOLORKEY is not implemented (copied without the key)\n");
    return blit(d, src, s, (flags & DDBLTFAST_SRCCOLORKEY) != 0);
}

HRESULT Surface::BltBatch(DDBLTBATCH *, DWORD, DWORD) { DX_UNSUPPORTED("IDirectDrawSurface7::BltBatch"); }
HRESULT Surface::AddOverlayDirtyRect(RECT *) { DX_UNSUPPORTED("IDirectDrawSurface7::AddOverlayDirtyRect"); }
HRESULT Surface::EnumOverlayZOrders(DWORD, void *, LPDDENUMSURFACESCALLBACK7) { DX_UNSUPPORTED("IDirectDrawSurface7::EnumOverlayZOrders"); }

HRESULT Surface::Flip(IDirectDrawSurface7 *, DWORD)
{
    if (chain.empty())
        return DDERR_NOTFLIPPABLE;
    if (locks || dc)
        return DDERR_SURFACEBUSY;
    // front <- first back buffer <- second ... <- old front
    uint8_t *front = bits;
    for (size_t i = 0; i < chain.size(); i++) {
        Surface *b = chain[i];
        if (i == 0)
            bits = b->bits;
        else
            chain[i - 1]->bits = b->bits;
        b->version++;
    }
    chain.back()->bits = front;
    changed();
    return DD_OK;
}

HRESULT Surface::GetBltStatus(DWORD) { return DD_OK; }
HRESULT Surface::GetFlipStatus(DWORD) { return DD_OK; }

HRESULT Surface::GetCaps(DDSCAPS2 *caps)
{
    *caps = desc.ddsCaps;
    return DD_OK;
}

HRESULT Surface::GetClipper(IDirectDrawClipper **out)
{
    *out = clipper;
    if (!clipper)
        return DDERR_NOTFOUND;
    clipper->AddRef();
    return DD_OK;
}

HRESULT Surface::GetColorKey(DWORD which, DDCOLORKEY *k)
{
    if (which & DDCKEY_SRCBLT) {
        if (!hasSrcKey)
            return DDERR_NOTFOUND;
        *k = srcKey;
        return DD_OK;
    }
    if (which & DDCKEY_DESTBLT) {
        if (!hasDestKey)
            return DDERR_NOTFOUND;
        *k = destKey;
        return DD_OK;
    }
    return DDERR_INVALIDPARAMS;
}

HRESULT Surface::SetColorKey(DWORD which, DDCOLORKEY *k)
{
    DDCOLORKEY key = {};
    if (k) {
        key = *k;
        if (!(which & DDCKEY_COLORSPACE))
            key.dwColorSpaceHighValue = key.dwColorSpaceLowValue;
    }
    if (which & DDCKEY_SRCBLT) {
        srcKey = key;
        hasSrcKey = k != 0;
    } else if (which & DDCKEY_DESTBLT) {
        destKey = key;
        hasDestKey = k != 0;
    } else {
        return DDERR_INVALIDPARAMS;
    }
    version++;
    return DD_OK;
}

// GDI draws straight into 8-bit, RGB 565, 24-bit and X888/A888 pixels; other formats
// (555, 1555, 4444) get a 32-bit copy that ReleaseDC converts back.
HRESULT Surface::GetDC(HDC *out)
{
    *out = 0;
    if (dc || locks)
        return DDERR_SURFACEBUSY;
    int bpp = fmt.bytes * 8;
    bool direct = fmt.palette || (fmt.bytes == 2 && fmt.mask[1] == 0xf800 && fmt.mask[2] == 0x07e0 && fmt.mask[3] == 0x1f)
        || (fmt.bytes >= 3 && fmt.mask[1] == 0xff0000 && fmt.mask[2] == 0xff00 && fmt.mask[3] == 0xff);
    if (fmt.zbuffer)
        return DDERR_UNSUPPORTED;
    if (direct) {
        dc = port_gdi_surface_dc(bits, width(), height(), pitch, bpp, pal());
    } else {
        dcShadow = (uint8_t *)malloc((size_t)width() * height() * 4);
        for (int y = 0; y < height(); y++)
            row_to_argb(fmt, row(y), (uint32_t *)dcShadow + (size_t)y * width(), width(), pal());
        dc = port_gdi_surface_dc(dcShadow, width(), height(), width() * 4, 32, 0);
    }
    if (!dc) {
        free(dcShadow);
        dcShadow = 0;
        return DDERR_GENERIC;
    }
    *out = dc;
    return DD_OK;
}

HRESULT Surface::ReleaseDC(HDC h)
{
    if (!dc || h != dc)
        return DDERR_INVALIDPARAMS;
    port_gdi_release_surface_dc(dc);
    dc = 0;
    if (dcShadow) {
        // GDI leaves alpha alone in principle; keep the surface's own alpha anyway
        for (int y = 0; y < height(); y++) {
            uint8_t *p = row(y);
            const uint32_t *s = (const uint32_t *)dcShadow + (size_t)y * width();
            for (int x = 0; x < width(); x++, p += fmt.bytes) {
                uint32_t old = to_argb(fmt, read_pixel(p, fmt.bytes), 0);
                write_pixel(p, fmt.bytes, from_argb(fmt, (old & 0xff000000u) | (s[x] & 0xffffff), 0));
            }
        }
        free(dcShadow);
        dcShadow = 0;
    }
    changed();
    return DD_OK;
}

HRESULT Surface::GetOverlayPosition(LONG *, LONG *) { DX_UNSUPPORTED("IDirectDrawSurface7::GetOverlayPosition"); }

HRESULT Surface::GetPalette(IDirectDrawPalette **out)
{
    *out = palette;
    if (!palette)
        return DDERR_NOTFOUND;
    palette->AddRef();
    return DD_OK;
}

HRESULT Surface::GetPixelFormat(DDPIXELFORMAT *pf)
{
    *pf = desc.ddpfPixelFormat;
    return DD_OK;
}

HRESULT Surface::GetSurfaceDesc(DDSURFACEDESC2 *out)
{
    DWORD size = out->dwSize;
    *out = desc;
    out->dwSize = size ? size : sizeof(DDSURFACEDESC2);
    return DD_OK;
}

HRESULT Surface::Initialize(IDirectDraw *, DDSURFACEDESC2 *) { return DD_OK; }
HRESULT Surface::IsLost() { return DD_OK; }

HRESULT Surface::Lock(RECT *r, DDSURFACEDESC2 *out, DWORD, HANDLE)
{
    if (!out)
        return DDERR_INVALIDPARAMS;
    GetSurfaceDesc(out);
    out->dwFlags |= DDSD_LPSURFACE | DDSD_PITCH;
    out->lpSurface = r ? row(r->top) + r->left * fmt.bytes : bits;
    locks++;
    return DD_OK;
}

HRESULT Surface::Unlock(RECT *)
{
    if (!locks)
        return DDERR_NOTLOCKED;
    locks--;
    changed();
    return DD_OK;
}

HRESULT Surface::Restore() { return DD_OK; }

HRESULT Surface::SetClipper(IDirectDrawClipper *c)
{
    if (c)
        c->AddRef();
    if (clipper)
        clipper->Release();
    clipper = static_cast<Clipper *>(c);
    return DD_OK;
}

HRESULT Surface::SetOverlayPosition(LONG, LONG) { DX_UNSUPPORTED("IDirectDrawSurface7::SetOverlayPosition"); }

HRESULT Surface::SetPalette(IDirectDrawPalette *p)
{
    if (p)
        p->AddRef();
    if (palette)
        palette->Release();
    palette = static_cast<Palette *>(p);
    changed();
    return DD_OK;
}

HRESULT Surface::UpdateOverlay(RECT *, IDirectDrawSurface7 *, RECT *, DWORD, DDOVERLAYFX *) { DX_UNSUPPORTED("IDirectDrawSurface7::UpdateOverlay"); }
HRESULT Surface::UpdateOverlayDisplay(DWORD) { DX_UNSUPPORTED("IDirectDrawSurface7::UpdateOverlayDisplay"); }
HRESULT Surface::UpdateOverlayZOrder(DWORD, IDirectDrawSurface7 *) { DX_UNSUPPORTED("IDirectDrawSurface7::UpdateOverlayZOrder"); }

HRESULT Surface::GetDDInterface(void **out)
{
    dd->AddRef();
    *out = static_cast<IDirectDraw7 *>(dd);
    return DD_OK;
}

HRESULT Surface::PageLock(DWORD) { return DD_OK; }
HRESULT Surface::PageUnlock(DWORD) { return DD_OK; }
HRESULT Surface::SetSurfaceDesc(DDSURFACEDESC2 *, DWORD) { DX_UNSUPPORTED("IDirectDrawSurface7::SetSurfaceDesc"); }
HRESULT Surface::SetPrivateData(REFGUID, void *, DWORD, DWORD) { DX_UNSUPPORTED("IDirectDrawSurface7::SetPrivateData"); }
HRESULT Surface::GetPrivateData(REFGUID, void *, DWORD *) { DX_UNSUPPORTED("IDirectDrawSurface7::GetPrivateData"); }
HRESULT Surface::FreePrivateData(REFGUID) { DX_UNSUPPORTED("IDirectDrawSurface7::FreePrivateData"); }

HRESULT Surface::GetUniquenessValue(DWORD *v)
{
    *v = version;
    return DD_OK;
}

HRESULT Surface::ChangeUniquenessValue()
{
    version++;
    return DD_OK;
}

HRESULT Surface::SetPriority(DWORD) { return DD_OK; }

HRESULT Surface::GetPriority(DWORD *p)
{
    *p = 0;
    return DD_OK;
}

HRESULT Surface::SetLOD(DWORD) { return DD_OK; }

HRESULT Surface::GetLOD(DWORD *l)
{
    *l = 0;
    return DD_OK;
}

} // namespace dx
