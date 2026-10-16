// gdi.cpp - GDI in software, as far as the game uses it: device contexts (memory DCs,
// window DCs, DCs on DirectDraw surfaces' pixels), bitmaps and DIB sections in 8, 16, 24
// and 32 bits, palettes, BitBlt/StretchBlt/StretchDIBits, fills, lines and ellipses,
// and text through CoreText (fonts by their Windows names; macOS has Arial and the other
// faces the game asks for).
#include "user.h"
#include "text.h"
#include "../port.h"

#include <math.h>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace win32 {

// ---------------------------------------------------------------------------------------
// Objects.

struct GdiObject {
    enum Kind { Bitmap = OBJ_BITMAP, Brush = OBJ_BRUSH, Pen = OBJ_PEN, Font = OBJ_FONT, Palette = OBJ_PAL, Region = 8 };
    explicit GdiObject(Kind k) : kind(k) {}
    virtual ~GdiObject() {}
    Kind kind;
    bool stock = false;
    int selected = 0;  // DCs it is selected into
};

// Pixels in memory: rows of pitch bytes, top-down (a bottom-up DIB is addressed upside
// down through row()).
struct Pixels {
    uint8_t *bits = 0;
    int w = 0, h = 0, pitch = 0, bpp = 32;
    bool bottom_up = false;
    bool rgb565 = false;     // 16 bits: 5-6-5, else 5-5-5
    RGBQUAD pal[256];        // 8 bits and below
    uint8_t *row(int y) const { return bits + (size_t)(bottom_up ? h - 1 - y : y) * (size_t)pitch; }
};

struct BitmapObject : GdiObject {
    BitmapObject() : GdiObject(Bitmap) {}
    ~BitmapObject()
    {
        if (owned)
            free(px.bits);
    }
    Pixels px;
    bool owned = true;
    bool dib = false;
};

struct BrushObject : GdiObject {
    BrushObject(UINT style, COLORREF c) : GdiObject(Brush), style(style), color(c) {}
    UINT style;
    COLORREF color;
};

struct PenObject : GdiObject {
    PenObject(int style, int width, COLORREF c) : GdiObject(Pen), style(style), width(width), color(c) {}
    int style, width;
    COLORREF color;
};

struct FontObject : GdiObject {
    explicit FontObject(const LOGFONTA &lf) : GdiObject(Font), lf(lf) {}
    ~FontObject() { text::font_release(ct); }
    LOGFONTA lf;
    void *ct = 0;
};

struct PaletteObject : GdiObject {
    PaletteObject() : GdiObject(Palette) {}
    std::vector<PALETTEENTRY> entries;
};

struct RegionObject : GdiObject {
    RegionObject() : GdiObject(Region) {}
    RECT rect;
};

static std::mutex s_mu;

static BrushObject s_white_brush(BS_SOLID, RGB(255, 255, 255)), s_ltgray_brush(BS_SOLID, RGB(192, 192, 192)),
    s_gray_brush(BS_SOLID, RGB(128, 128, 128)), s_dkgray_brush(BS_SOLID, RGB(64, 64, 64)),
    s_black_brush(BS_SOLID, RGB(0, 0, 0)), s_null_brush(BS_NULL, 0);
static PenObject s_white_pen(PS_SOLID, 1, RGB(255, 255, 255)), s_black_pen(PS_SOLID, 1, 0), s_null_pen(PS_NULL, 1, 0);
static FontObject *s_system_font;
static PaletteObject *s_default_palette;
static BitmapObject *s_default_bitmap;

static FontObject *system_font()
{
    if (!s_system_font) {
        LOGFONTA lf;
        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = 16;
        lf.lfWeight = FW_BOLD;
        strcpy(lf.lfFaceName, "System");
        s_system_font = new FontObject(lf);
        s_system_font->stock = true;
    }
    return s_system_font;
}

static PaletteObject *default_palette()
{
    if (!s_default_palette) {
        s_default_palette = new PaletteObject;
        s_default_palette->stock = true;
        // The 20 static colours of the Windows system palette.
        static const BYTE c[20][3] = {{0, 0, 0},       {128, 0, 0},     {0, 128, 0},     {128, 128, 0},
                                      {0, 0, 128},     {128, 0, 128},   {0, 128, 128},   {192, 192, 192},
                                      {192, 220, 192}, {166, 202, 240}, {255, 251, 240}, {160, 160, 164},
                                      {128, 128, 128}, {255, 0, 0},     {0, 255, 0},     {255, 255, 0},
                                      {0, 0, 255},     {255, 0, 255},   {0, 255, 255},   {255, 255, 255}};
        for (int i = 0; i < 20; i++)
            s_default_palette->entries.push_back({c[i][0], c[i][1], c[i][2], 0});
    }
    return s_default_palette;
}

static BitmapObject *default_bitmap()
{
    if (!s_default_bitmap) {
        s_default_bitmap = new BitmapObject;
        s_default_bitmap->stock = true;
        s_default_bitmap->px.w = s_default_bitmap->px.h = 1;
        s_default_bitmap->px.bpp = 1;
        s_default_bitmap->px.pitch = 4;
        s_default_bitmap->px.bits = (uint8_t *)calloc(1, 4);
    }
    return s_default_bitmap;
}

// ---------------------------------------------------------------------------------------
// Device contexts.

struct DCState {
    BitmapObject *bitmap = 0;
    BrushObject *brush = &s_white_brush;
    PenObject *pen = &s_black_pen;
    FontObject *font = 0;
    PaletteObject *palette = 0;
    COLORREF text = 0, bk = RGB(255, 255, 255);
    int bk_mode = OPAQUE, rop2 = R2_COPYPEN, stretch_mode = BLACKONWHITE;
    UINT text_align = 0;
    POINT pos = {0, 0};
    bool clip = false;
    RECT clip_rect;
};

struct DC : DCState {
    enum Kind { Memory, Window, Surface, Info };
    Kind kind = Memory;
    HWND hwnd = 0;
    BitmapObject surface;  // the target of Window and Surface DCs
    std::vector<DCState> saved;
    const Pixels *target() const { return kind == Memory ? (bitmap ? &bitmap->px : 0) : &surface.px; }
};

static std::vector<DC *> s_dcs;

static DC *dc_of(HDC h)
{
    std::lock_guard<std::mutex> lock(s_mu);
    for (DC *d : s_dcs)
        if ((HDC)d == h)
            return d;
    return 0;
}

static HDC new_dc(DC *d)
{
    if (!d->font)
        d->font = system_font();
    std::lock_guard<std::mutex> lock(s_mu);
    s_dcs.push_back(d);
    return (HDC)d;
}

// ---------------------------------------------------------------------------------------
// Pixels.

static void unpack(const Pixels &p, uint32_t v, int &r, int &g, int &b)
{
    switch (p.bpp) {
    case 32:
    case 24:
        r = (v >> 16) & 0xff;
        g = (v >> 8) & 0xff;
        b = v & 0xff;
        return;
    case 16:
        if (p.rgb565) {
            r = (v >> 11) & 0x1f;
            g = (v >> 5) & 0x3f;
            b = v & 0x1f;
            r = (r << 3) | (r >> 2);
            g = (g << 2) | (g >> 4);
        } else {
            r = (v >> 10) & 0x1f;
            g = (v >> 5) & 0x1f;
            b = v & 0x1f;
            r = (r << 3) | (r >> 2);
            g = (g << 3) | (g >> 2);
        }
        b = (b << 3) | (b >> 2);
        return;
    default: {
        const RGBQUAD &q = p.pal[v & 0xff];
        r = q.rgbRed;
        g = q.rgbGreen;
        b = q.rgbBlue;
    }
    }
}

static uint32_t nearest_index(const Pixels &p, int r, int g, int b)
{
    int n = 1 << (p.bpp > 8 ? 8 : p.bpp), best = 0, bestd = 1 << 30;
    for (int i = 0; i < n; i++) {
        int dr = p.pal[i].rgbRed - r, dg = p.pal[i].rgbGreen - g, db = p.pal[i].rgbBlue - b;
        int d = dr * dr + dg * dg + db * db;
        if (d < bestd) {
            bestd = d;
            best = i;
            if (!d)
                break;
        }
    }
    return (uint32_t)best;
}

static uint32_t pack(const Pixels &p, int r, int g, int b)
{
    switch (p.bpp) {
    case 32:
    case 24: return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    case 16:
        if (p.rgb565)
            return ((uint32_t)(r >> 3) << 11) | ((uint32_t)(g >> 2) << 5) | (uint32_t)(b >> 3);
        return ((uint32_t)(r >> 3) << 10) | ((uint32_t)(g >> 3) << 5) | (uint32_t)(b >> 3);
    default: return nearest_index(p, r, g, b);
    }
}

static uint32_t read_raw(const Pixels &p, int x, int y)
{
    const uint8_t *row = p.row(y);
    switch (p.bpp) {
    case 32: return *(const uint32_t *)(row + x * 4) & 0xffffff;
    case 24: return row[x * 3] | (row[x * 3 + 1] << 8) | ((uint32_t)row[x * 3 + 2] << 16);
    case 16: return *(const uint16_t *)(row + x * 2);
    case 8: return row[x];
    case 4: return (row[x / 2] >> ((x & 1) ? 0 : 4)) & 0xf;
    case 1: return (row[x / 8] >> (7 - (x & 7))) & 1;
    }
    return 0;
}

static void write_raw(const Pixels &p, int x, int y, uint32_t v)
{
    uint8_t *row = p.row(y);
    switch (p.bpp) {
    case 32: *(uint32_t *)(row + x * 4) = v; break;
    case 24:
        row[x * 3] = (uint8_t)v;
        row[x * 3 + 1] = (uint8_t)(v >> 8);
        row[x * 3 + 2] = (uint8_t)(v >> 16);
        break;
    case 16: *(uint16_t *)(row + x * 2) = (uint16_t)v; break;
    case 8: row[x] = (uint8_t)v; break;
    case 4: {
        int sh = (x & 1) ? 0 : 4;
        row[x / 2] = (uint8_t)((row[x / 2] & ~(0xf << sh)) | ((v & 0xf) << sh));
        break;
    }
    case 1: {
        int sh = 7 - (x & 7);
        row[x / 8] = (uint8_t)((row[x / 8] & ~(1 << sh)) | ((v & 1) << sh));
        break;
    }
    }
}

static COLORREF get_color(const Pixels &p, int x, int y)
{
    int r, g, b;
    unpack(p, read_raw(p, x, y), r, g, b);
    return RGB(r, g, b);
}

// A COLORREF in the pixels' format (palette indices taken as they are).
static uint32_t raw_of(const Pixels &p, COLORREF c)
{
    if ((c >> 24) == 1 && p.bpp <= 8)
        return c & 0xff;  // PALETTEINDEX
    return pack(p, GetRValue(c), GetGValue(c), GetBValue(c));
}

static bool clip_ok(const DC *d, const Pixels &p, int x, int y)
{
    if (x < 0 || y < 0 || x >= p.w || y >= p.h)
        return false;
    return !d->clip || (x >= d->clip_rect.left && x < d->clip_rect.right && y >= d->clip_rect.top && y < d->clip_rect.bottom);
}

static void fill_rect(DC *d, int l, int t, int r, int b, COLORREF c)
{
    const Pixels *p = d->target();
    if (!p || !p->bits)
        return;
    if (l < 0) l = 0;
    if (t < 0) t = 0;
    if (r > p->w) r = p->w;
    if (b > p->h) b = p->h;
    if (d->clip) {
        if (l < d->clip_rect.left) l = d->clip_rect.left;
        if (t < d->clip_rect.top) t = d->clip_rect.top;
        if (r > d->clip_rect.right) r = d->clip_rect.right;
        if (b > d->clip_rect.bottom) b = d->clip_rect.bottom;
    }
    uint32_t v = raw_of(*p, c);
    for (int y = t; y < b; y++)
        for (int x = l; x < r; x++)
            write_raw(*p, x, y, v);
}

static void plot(DC *d, int x, int y, COLORREF c)
{
    const Pixels *p = d->target();
    if (!p || !p->bits || !clip_ok(d, *p, x, y))
        return;
    uint32_t v = raw_of(*p, c);
    switch (d->rop2) {
    case R2_NOP: return;
    case R2_BLACK: v = raw_of(*p, 0); break;
    case R2_WHITE: v = raw_of(*p, RGB(255, 255, 255)); break;
    case R2_NOT: v = ~read_raw(*p, x, y); break;
    case R2_XORPEN: v ^= read_raw(*p, x, y); break;
    }
    write_raw(*p, x, y, v);
}

static void pen_line(DC *d, int x0, int y0, int x1, int y1, bool last)
{
    if (!d->pen || d->pen->style == PS_NULL)
        return;
    int dx = abs(x1 - x0), dy = -abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx + dy;
    int w = d->pen->width > 1 ? d->pen->width : 1;
    for (;;) {
        if (!(x0 == x1 && y0 == y1) || last)
            for (int oy = 0; oy < w; oy++)
                for (int ox = 0; ox < w; ox++)
                    plot(d, x0 + ox - w / 2, y0 + oy - w / 2, d->pen->color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// The raster operations the game uses, on 24-bit colour values.
static uint32_t rop3(DWORD rop, uint32_t src, uint32_t dst, uint32_t pat)
{
    switch (rop) {
    case SRCCOPY: return src;
    case SRCAND: return src & dst;
    case SRCPAINT: return src | dst;
    case SRCINVERT: return src ^ dst;
    case SRCERASE: return src & ~dst;
    case NOTSRCCOPY: return ~src;
    case NOTSRCERASE: return ~(src | dst);
    case MERGECOPY: return src & pat;
    case MERGEPAINT: return ~src | dst;
    case PATCOPY: return pat;
    case PATINVERT: return pat ^ dst;
    case DSTINVERT: return ~dst;
    case BLACKNESS: return 0;
    case WHITENESS: return 0xffffff;
    }
    return src;
}

// Copies (and scales) src's rectangle into dst's; mirrored when a size is negative.
static void blit(DC *d, const Pixels &dst, int dx, int dy, int dw, int dh, const Pixels *src, int sx, int sy, int sw, int sh,
                 DWORD rop)
{
    if (!dst.bits || !dw || !dh)
        return;
    bool same_format = src && src->bpp == dst.bpp && (dst.bpp != 16 || src->rgb565 == dst.rgb565) &&
                       (dst.bpp > 8 || !memcmp(src->pal, dst.pal, sizeof(RGBQUAD) << dst.bpp));
    uint32_t pat = d->brush ? d->brush->color : 0;
    pat = (GetRValue(pat) << 16) | (GetGValue(pat) << 8) | GetBValue(pat);
    // A plain copy without scaling into the same format: row copies.
    if (rop == SRCCOPY && same_format && dw == sw && dh == sh && dw > 0 && dh > 0 && dst.bpp >= 8 && !d->clip) {
        int bytes = dst.bpp / 8;
        for (int y = 0; y < dh; y++) {
            int ty = dy + y, fy = sy + y;
            if (ty < 0 || ty >= dst.h || fy < 0 || fy >= src->h)
                continue;
            int x0 = 0, x1 = dw;
            if (dx + x0 < 0) x0 = -dx;
            if (sx + x0 < 0) x0 = -sx;
            if (dx + x1 > dst.w) x1 = dst.w - dx;
            if (sx + x1 > src->w) x1 = src->w - sx;
            if (x1 > x0)
                memmove(dst.row(ty) + (dx + x0) * bytes, src->row(fy) + (sx + x0) * bytes, (size_t)(x1 - x0) * (size_t)bytes);
        }
        return;
    }
    int adw = abs(dw), adh = abs(dh), asw = abs(sw), ash = abs(sh);
    for (int y = 0; y < adh; y++) {
        int ty = dw && dh < 0 ? dy - y : dy + y;
        if (ty < 0 || ty >= dst.h)
            continue;
        int fy = ash ? (int)((long long)y * ash / adh) : 0;
        fy = (sh < 0) != (dh < 0) ? sy + ash - 1 - fy : sy + fy;
        for (int x = 0; x < adw; x++) {
            int tx = dw < 0 ? dx - x : dx + x;
            if (!clip_ok(d, dst, tx, ty))
                continue;
            int fx = asw ? (int)((long long)x * asw / adw) : 0;
            fx = (sw < 0) != (dw < 0) ? sx + asw - 1 - fx : sx + fx;
            if (same_format && rop == SRCCOPY) {
                if (fx >= 0 && fy >= 0 && fx < src->w && fy < src->h)
                    write_raw(dst, tx, ty, read_raw(*src, fx, fy));
                continue;
            }
            uint32_t s = 0;
            if (src && fx >= 0 && fy >= 0 && fx < src->w && fy < src->h) {
                int r, g, b;
                unpack(*src, read_raw(*src, fx, fy), r, g, b);
                s = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
            } else if (src) {
                continue;
            }
            int r, g, b;
            unpack(dst, read_raw(dst, tx, ty), r, g, b);
            uint32_t dv = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
            uint32_t v = rop3(rop, s, dv, pat) & 0xffffff;
            write_raw(dst, tx, ty, pack(dst, (v >> 16) & 0xff, (v >> 8) & 0xff, v & 0xff));
        }
    }
}

static int pitch_of(int w, int bpp) { return ((w * bpp + 31) / 32) * 4; }

// The pixels described by a BITMAPINFO and its bits.
static bool dib_pixels(const BITMAPINFO *bmi, const void *bits, UINT usage, Pixels &p, const DC *d)
{
    const BITMAPINFOHEADER &h = bmi->bmiHeader;
    p.w = h.biWidth;
    p.h = h.biHeight < 0 ? -h.biHeight : h.biHeight;
    p.bottom_up = h.biHeight > 0;
    p.bpp = h.biBitCount;
    p.pitch = pitch_of(p.w, p.bpp);
    p.bits = (uint8_t *)bits;
    if (p.bpp == 16 && h.biCompression == BI_BITFIELDS) {
        const DWORD *masks = (const DWORD *)((const BYTE *)bmi + h.biSize);
        p.rgb565 = masks[1] == 0x07E0;
    }
    if (p.bpp <= 8) {
        int n = h.biClrUsed ? (int)h.biClrUsed : 1 << p.bpp;
        memset(p.pal, 0, sizeof(p.pal));
        const RGBQUAD *colors = (const RGBQUAD *)((const BYTE *)bmi + h.biSize);
        for (int i = 0; i < n && i < 256; i++) {
            if (usage == DIB_PAL_COLORS) {
                WORD idx = ((const WORD *)colors)[i];
                const PaletteObject *pal = d && d->palette ? d->palette : default_palette();
                if (idx < pal->entries.size()) {
                    const PALETTEENTRY &e = pal->entries[idx];
                    p.pal[i] = {e.peBlue, e.peGreen, e.peRed, 0};
                }
            } else {
                p.pal[i] = colors[i];
            }
        }
    }
    return p.bits && p.w > 0 && p.h > 0 && h.biCompression != BI_RLE8 && h.biCompression != BI_RLE4;
}

// ---------------------------------------------------------------------------------------
// Text, through CoreText.

static void *ct_font(FontObject *f)
{
    if (!f->ct) {
        // lfHeight < 0: the character height; > 0: the cell height (about 1.15 times as large).
        double px = f->lf.lfHeight < 0 ? -f->lf.lfHeight : f->lf.lfHeight > 0 ? f->lf.lfHeight / 1.15 : 13;
        f->ct = text::font_create(f->lf.lfFaceName, px, f->lf.lfWeight >= FW_SEMIBOLD, f->lf.lfItalic != 0);
    }
    return f->ct;
}

static void text_metrics(FontObject *f, int &ascent, int &descent, int &leading)
{
    text::font_metrics(ct_font(f), &ascent, &descent, &leading);
}

static SIZE text_extent(FontObject *f, const char *s, int n)
{
    int a, de, l;
    text_metrics(f, a, de, l);
    SIZE sz = {text::width(ct_font(f), s, n), a + de};
    return sz;
}

// Draws text with its cell's top-left at (x, y) (TA_TOP | TA_LEFT).
static void draw_text(DC *d, int x, int y, const char *text, int n)
{
    const Pixels *p = d->target();
    if (!p || !p->bits || n <= 0)
        return;
    FontObject *f = d->font ? d->font : system_font();
    SIZE ext = text_extent(f, text, n);
    int a, de, l;
    text_metrics(f, a, de, l);
    if ((d->text_align & 6) == 6) // TA_CENTER
        x -= ext.cx / 2;
    else if (d->text_align & 2)   // TA_RIGHT
        x -= ext.cx;
    if (d->text_align & 8)        // TA_BOTTOM
        y -= ext.cy;
    else if ((d->text_align & 24) == 24)  // TA_BASELINE
        y -= a;
    int w = ext.cx + 2, h = ext.cy;
    if (w <= 0 || h <= 0)
        return;
    if (d->bk_mode == OPAQUE)
        fill_rect(d, x, y, x + ext.cx, y + ext.cy, d->bk);
    std::vector<uint8_t> mask((size_t)w * (size_t)h);
    bool smooth = getenv("MICKEY_TEXT_AA") != 0 && f->lf.lfQuality != NONANTIALIASED_QUALITY;
    text::render(ct_font(f), text, n, mask.data(), w, h, de, smooth);
    int tr = GetRValue(d->text), tg = GetGValue(d->text), tb = GetBValue(d->text);
    uint32_t solid = raw_of(*p, d->text);
    for (int yy = 0; yy < h; yy++) {
        const uint8_t *m = &mask[(size_t)yy * (size_t)w];
        int ty = y + yy;
        for (int xx = 0; xx < w; xx++) {
            int alpha = m[xx];
            int tx = x + xx;
            if (!alpha || !clip_ok(d, *p, tx, ty))
                continue;
            if (!smooth || alpha >= 250 || p->bpp <= 8) {
                if (smooth || alpha >= 128)
                    write_raw(*p, tx, ty, solid);
                continue;
            }
            int r, g, b;
            unpack(*p, read_raw(*p, tx, ty), r, g, b);
            r += (tr - r) * alpha / 255;
            g += (tg - g) * alpha / 255;
            b += (tb - b) * alpha / 255;
            write_raw(*p, tx, ty, pack(*p, r, g, b));
        }
    }
}

}  // namespace win32

using namespace win32;

extern "C" {

// ---------------------------------------------------------------------------------------
// DCs.

HDC port_gdi_surface_dc(void *pixels, int w, int h, int pitch, int bpp, const PALETTEENTRY *palette)
{
    DC *d = new DC;
    d->kind = DC::Surface;
    Pixels &p = d->surface.px;
    p.bits = (uint8_t *)pixels;
    p.w = w;
    p.h = h;
    p.pitch = pitch;
    p.bpp = bpp;
    p.rgb565 = true;
    d->surface.owned = false;
    if (bpp <= 8) {
        const PaletteObject *def = default_palette();
        for (int i = 0; i < 256; i++) {
            PALETTEENTRY e = palette ? palette[i] : (i < 10 ? def->entries[(size_t)i] : i >= 246 ? def->entries[(size_t)(i - 236)]
                                                                                                  : PALETTEENTRY{(BYTE)i, (BYTE)i, (BYTE)i, 0});
            p.pal[i] = {e.peBlue, e.peGreen, e.peRed, 0};
        }
    }
    return new_dc(d);
}

void port_gdi_release_surface_dc(HDC hdc) { DeleteDC(hdc); }

HDC WINAPI CreateCompatibleDC(HDC hdc)
{
    DC *d = new DC;
    d->kind = DC::Memory;
    d->bitmap = default_bitmap();
    if (DC *src = hdc ? dc_of(hdc) : 0)
        d->palette = src->palette;
    return new_dc(d);
}

HDC WINAPI CreateDCA(LPCSTR, LPCSTR, LPCSTR, const DEVMODEA *)
{
    // Printers and other devices are not available.
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}

// A window's DC draws into a backing bitmap of its client size (not shown: the game
// shows its frames through DirectDraw).
HDC WINAPI GetDC(HWND hWnd)
{
    DC *d = new DC;
    d->kind = DC::Window;
    d->hwnd = hWnd;
    Window *w = hWnd ? window(hWnd) : 0;
    Pixels &p = d->surface.px;
    p.w = w ? (w->client_w ? w->client_w : 1) : display_width();
    p.h = w ? (w->client_h ? w->client_h : 1) : display_height();
    p.bpp = 32;
    p.pitch = p.w * 4;
    static std::vector<uint8_t> backing;
    if (backing.size() < (size_t)p.pitch * (size_t)p.h)
        backing.resize((size_t)p.pitch * (size_t)p.h);
    p.bits = backing.data();
    d->surface.owned = false;
    return new_dc(d);
}

HDC WINAPI GetWindowDC(HWND hWnd) { return GetDC(hWnd); }
int WINAPI ReleaseDC(HWND, HDC hDC) { return DeleteDC(hDC); }

HDC WINAPI BeginPaint(HWND hWnd, LPPAINTSTRUCT ps)
{
    memset(ps, 0, sizeof(*ps));
    ps->hdc = GetDC(hWnd);
    GetClientRect(hWnd, &ps->rcPaint);
    ValidateRect(hWnd, 0);
    return ps->hdc;
}

BOOL WINAPI EndPaint(HWND hWnd, const PAINTSTRUCT *ps)
{
    ReleaseDC(hWnd, ps->hdc);
    return TRUE;
}

BOOL WINAPI DeleteDC(HDC hdc)
{
    std::lock_guard<std::mutex> lock(s_mu);
    for (size_t i = 0; i < s_dcs.size(); i++)
        if ((HDC)s_dcs[i] == hdc) {
            DC *d = s_dcs[i];
            if (d->bitmap)
                d->bitmap->selected--;
            s_dcs.erase(s_dcs.begin() + (long)i);
            delete d;
            return TRUE;
        }
    return FALSE;
}

int WINAPI SaveDC(HDC hdc)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    d->saved.push_back(*d);
    return (int)d->saved.size();
}

BOOL WINAPI RestoreDC(HDC hdc, int n)
{
    DC *d = dc_of(hdc);
    if (!d || d->saved.empty())
        return FALSE;
    size_t level = n < 0 ? d->saved.size() + (size_t)(n + 1) : (size_t)n;
    if (level < 1 || level > d->saved.size())
        return FALSE;
    (DCState &)*d = d->saved[level - 1];
    d->saved.resize(level - 1);
    return TRUE;
}

// The depth of the display mode, which a window DC reports, as on Windows. The DirectDraw
// layer overrides this with its current mode; this default is its desktop mode.
__attribute__((weak)) int port_display_bpp(void) { return 32; }

int WINAPI GetDeviceCaps(HDC hdc, int index)
{
    DC *d = dc_of(hdc);
    const Pixels *p = d ? d->target() : 0;
    switch (index) {
    case HORZRES: return p && d->kind != DC::Memory ? p->w : display_width();
    case VERTRES: return p && d->kind != DC::Memory ? p->h : display_height();
    case BITSPIXEL:
        if (p && d->kind != DC::Window)
            return p->bpp;
        return port_display_bpp();
    case PLANES: return 1;
    case NUMCOLORS: return -1;
    case LOGPIXELSX:
    case LOGPIXELSY: return 96;
    case RASTERCAPS: return RC_STRETCHBLT | RC_STRETCHDIB | 1;  // RC_BITBLT
    case SIZEPALETTE: return p && p->bpp == 8 ? 256 : 0;
    case NUMRESERVED: return 20;
    case HORZSIZE: return 320;
    case VERTSIZE: return 240;
    }
    return 0;
}

HGDIOBJ WINAPI GetStockObject(int i)
{
    switch (i) {
    case WHITE_BRUSH: return &s_white_brush;
    case LTGRAY_BRUSH: return &s_ltgray_brush;
    case GRAY_BRUSH: return &s_gray_brush;
    case DKGRAY_BRUSH: return &s_dkgray_brush;
    case BLACK_BRUSH: return &s_black_brush;
    case NULL_BRUSH: return &s_null_brush;
    case WHITE_PEN: return &s_white_pen;
    case BLACK_PEN: return &s_black_pen;
    case NULL_PEN: return &s_null_pen;
    case DEFAULT_PALETTE: return default_palette();
    case OEM_FIXED_FONT:
    case ANSI_FIXED_FONT:
    case ANSI_VAR_FONT:
    case SYSTEM_FONT:
    case DEVICE_DEFAULT_FONT:
    case SYSTEM_FIXED_FONT:
    case DEFAULT_GUI_FONT: return system_font();
    }
    return 0;
}

HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ h)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    if (!h)
        return 0;  // the game "deselects" with 0: nothing to do
    GdiObject *o = (GdiObject *)h;
    switch (o->kind) {
    case GdiObject::Bitmap: {
        if (d->kind != DC::Memory)
            return 0;
        BitmapObject *old = d->bitmap;
        if (old)
            old->selected--;
        d->bitmap = (BitmapObject *)o;
        d->bitmap->selected++;
        return old;
    }
    case GdiObject::Brush: {
        BrushObject *old = d->brush;
        d->brush = (BrushObject *)o;
        return old;
    }
    case GdiObject::Pen: {
        PenObject *old = d->pen;
        d->pen = (PenObject *)o;
        return old;
    }
    case GdiObject::Font: {
        FontObject *old = d->font;
        d->font = (FontObject *)o;
        return old;
    }
    case GdiObject::Region:
        SelectClipRgn(hdc, (HRGN)h);
        return (HGDIOBJ)(uintptr_t)2;  // SIMPLEREGION
    case GdiObject::Palette: break;
    }
    return 0;
}

BOOL WINAPI DeleteObject(HGDIOBJ ho)
{
    GdiObject *o = (GdiObject *)ho;
    if (!o || o->stock)
        return TRUE;
    if (o->selected > 0)
        return FALSE;  // as on Windows: a selected object stays
    delete o;
    return TRUE;
}

int WINAPI GetObjectA(HANDLE h, int c, LPVOID pv)
{
    GdiObject *o = (GdiObject *)h;
    if (!o)
        return 0;
    switch (o->kind) {
    case GdiObject::Bitmap: {
        BitmapObject *b = (BitmapObject *)o;
        BITMAP bm = {0, b->px.w, b->px.h, b->px.pitch, 1, (WORD)b->px.bpp, b->dib ? b->px.bits : 0};
        if (b->dib && c >= (int)sizeof(DIBSECTION)) {
            DIBSECTION *ds = (DIBSECTION *)pv;
            memset(ds, 0, sizeof(*ds));
            ds->dsBm = bm;
            ds->dsBmih.biSize = sizeof(BITMAPINFOHEADER);
            ds->dsBmih.biWidth = b->px.w;
            ds->dsBmih.biHeight = b->px.bottom_up ? b->px.h : -b->px.h;
            ds->dsBmih.biPlanes = 1;
            ds->dsBmih.biBitCount = (WORD)b->px.bpp;
            ds->dsBmih.biCompression = b->px.bpp == 16 && b->px.rgb565 ? BI_BITFIELDS : BI_RGB;
            ds->dsBmih.biSizeImage = (DWORD)(b->px.pitch * b->px.h);
            if (b->px.bpp == 16) {
                ds->dsBitfields[0] = b->px.rgb565 ? 0xF800 : 0x7C00;
                ds->dsBitfields[1] = b->px.rgb565 ? 0x07E0 : 0x03E0;
                ds->dsBitfields[2] = 0x001F;
            }
            return sizeof(DIBSECTION);
        }
        if (!pv)
            return sizeof(BITMAP);
        if (c < (int)sizeof(BITMAP))
            return 0;
        *(BITMAP *)pv = bm;
        return sizeof(BITMAP);
    }
    case GdiObject::Font:
        if (pv && c >= (int)sizeof(LOGFONTA))
            *(LOGFONTA *)pv = ((FontObject *)o)->lf;
        return sizeof(LOGFONTA);
    case GdiObject::Pen: {
        PenObject *p = (PenObject *)o;
        LOGPEN lp = {(UINT)p->style, {p->width, 0}, p->color};
        if (pv && c >= (int)sizeof(LOGPEN))
            *(LOGPEN *)pv = lp;
        return sizeof(LOGPEN);
    }
    case GdiObject::Brush: {
        BrushObject *b = (BrushObject *)o;
        LOGBRUSH lb = {b->style, b->color, 0};
        if (pv && c >= (int)sizeof(LOGBRUSH))
            *(LOGBRUSH *)pv = lb;
        return sizeof(LOGBRUSH);
    }
    case GdiObject::Palette:
        if (pv && c >= 2)
            *(WORD *)pv = (WORD)((PaletteObject *)o)->entries.size();
        return 2;
    case GdiObject::Region: return 0;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------
// Bitmaps.

HBITMAP WINAPI CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi, UINT usage, void **ppvBits, HANDLE, DWORD)
{
    const BITMAPINFOHEADER &h = pbmi->bmiHeader;
    BitmapObject *b = new BitmapObject;
    b->dib = true;
    Pixels tmp;
    int height = h.biHeight < 0 ? -h.biHeight : h.biHeight;
    size_t size = (size_t)pitch_of(h.biWidth, h.biBitCount) * (size_t)height;
    void *bits = calloc(1, size ? size : 1);
    dib_pixels(pbmi, bits, usage, tmp, hdc ? dc_of(hdc) : 0);
    b->px = tmp;
    if (ppvBits)
        *ppvBits = bits;
    return (HBITMAP)b;
}

HBITMAP WINAPI CreateBitmap(int nWidth, int nHeight, UINT, UINT nBitCount, const void *lpBits)
{
    BitmapObject *b = new BitmapObject;
    b->px.w = nWidth;
    b->px.h = nHeight;
    b->px.bpp = (int)(nBitCount ? nBitCount : 1);
    // A DDB's rows are word aligned.
    b->px.pitch = ((nWidth * b->px.bpp + 15) / 16) * 2;
    b->px.bits = (uint8_t *)calloc(1, (size_t)b->px.pitch * (size_t)(nHeight ? nHeight : 1));
    if (b->px.bpp == 1) {
        b->px.pal[0] = {0, 0, 0, 0};
        b->px.pal[1] = {255, 255, 255, 0};
    }
    if (lpBits)
        memcpy(b->px.bits, lpBits, (size_t)b->px.pitch * (size_t)nHeight);
    return (HBITMAP)b;
}

HBITMAP WINAPI CreateCompatibleBitmap(HDC hdc, int cx, int cy)
{
    DC *d = dc_of(hdc);
    const Pixels *p = d ? d->target() : 0;
    int bpp = p && p->bpp >= 8 ? p->bpp : 32;
    if (d && d->kind == DC::Window)
        bpp = 32;
    BitmapObject *b = new BitmapObject;
    b->px.w = cx;
    b->px.h = cy;
    b->px.bpp = bpp;
    b->px.pitch = pitch_of(cx, bpp);
    b->px.rgb565 = p ? p->rgb565 : true;
    if (p)
        memcpy(b->px.pal, p->pal, sizeof(b->px.pal));
    b->px.bits = (uint8_t *)calloc(1, (size_t)b->px.pitch * (size_t)(cy ? cy : 1));
    return (HBITMAP)b;
}

HBITMAP WINAPI CreateDIBitmap(HDC hdc, const BITMAPINFOHEADER *pbmih, DWORD flInit, const void *pjBits, const BITMAPINFO *pbmi,
                              UINT iUsage)
{
    int w = pbmih->biWidth, h = pbmih->biHeight < 0 ? -pbmih->biHeight : pbmih->biHeight;
    HBITMAP hb = CreateCompatibleBitmap(hdc, w, h);
    if ((flInit & CBM_INIT) && pjBits && pbmi) {
        BitmapObject *b = (BitmapObject *)hb;
        Pixels src;
        if (dib_pixels(pbmi, pjBits, iUsage, src, dc_of(hdc))) {
            DC tmp;
            blit(&tmp, b->px, 0, 0, w, h, &src, 0, 0, w, h, SRCCOPY);
        }
    }
    return hb;
}

UINT WINAPI GetDIBColorTable(HDC hdc, UINT iStart, UINT cEntries, RGBQUAD *prgbq)
{
    DC *d = dc_of(hdc);
    if (!d || !d->bitmap || d->bitmap->px.bpp > 8)
        return 0;
    UINT n = 0;
    for (UINT i = iStart; i < 256 && n < cEntries; i++, n++)
        prgbq[n] = d->bitmap->px.pal[i];
    return n;
}

UINT WINAPI SetDIBColorTable(HDC hdc, UINT iStart, UINT cEntries, const RGBQUAD *prgbq)
{
    DC *d = dc_of(hdc);
    if (!d || !d->bitmap || d->bitmap->px.bpp > 8)
        return 0;
    UINT n = 0;
    for (UINT i = iStart; i < 256 && n < cEntries; i++, n++)
        d->bitmap->px.pal[i] = prgbq[n];
    return n;
}

int WINAPI GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT usage)
{
    BitmapObject *b = (BitmapObject *)hbm;
    if (!b)
        return 0;
    BITMAPINFOHEADER &h = lpbmi->bmiHeader;
    if (!lpvBits) {
        // Report the format.
        h.biWidth = b->px.w;
        h.biHeight = b->px.h;
        h.biPlanes = 1;
        h.biBitCount = (WORD)b->px.bpp;
        h.biCompression = BI_RGB;
        h.biSizeImage = (DWORD)(pitch_of(b->px.w, b->px.bpp) * b->px.h);
        return (int)b->px.h;
    }
    Pixels dst;
    if (!dib_pixels(lpbmi, lpvBits, usage, dst, dc_of(hdc)))
        return 0;
    if (dst.bpp <= 8)
        memcpy(dst.pal, b->px.pal, sizeof(dst.pal));
    DC tmp;
    Pixels src = b->px;
    int lines = (int)cLines;
    blit(&tmp, dst, 0, (int)start, b->px.w, lines, &src, 0, (int)start, b->px.w, lines, SRCCOPY);
    if (dst.bpp <= 8) {
        RGBQUAD *colors = (RGBQUAD *)((BYTE *)lpbmi + h.biSize);
        memcpy(colors, b->px.pal, sizeof(RGBQUAD) << dst.bpp);
    }
    return lines;
}

// A .bmp file as a bitmap (LoadImage with LR_LOADFROMFILE): with LR_CREATEDIBSECTION a DIB
// section in the file's own depth and row order (the game reads bmBits itself), else a
// bitmap in the screen's format.
HBITMAP port_load_bitmap_file(const char *path, UINT flags)
{
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (f == INVALID_HANDLE_VALUE)
        return 0;
    DWORD size = GetFileSize(f, 0), got = 0;
    std::vector<uint8_t> data(size);
    BOOL ok = size > sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) && ReadFile(f, data.data(), size, &got, 0) && got == size;
    CloseHandle(f);
    if (!ok || data[0] != 'B' || data[1] != 'M')
        return 0;
    const BITMAPFILEHEADER *fh = (const BITMAPFILEHEADER *)data.data();
    const BITMAPINFO *bmi = (const BITMAPINFO *)(data.data() + sizeof(BITMAPFILEHEADER));
    const BITMAPINFOHEADER &h = bmi->bmiHeader;
    int rows = h.biHeight < 0 ? -h.biHeight : h.biHeight;
    size_t bytes = (size_t)pitch_of(h.biWidth, h.biBitCount) * (size_t)rows;
    if (h.biCompression != BI_RGB && h.biCompression != BI_BITFIELDS)
        return 0;
    if (fh->bfOffBits > size || size - fh->bfOffBits < bytes)
        return 0;
    const uint8_t *bits = data.data() + fh->bfOffBits;
    if (flags & LR_CREATEDIBSECTION) {
        void *dst = 0;
        HBITMAP hb = CreateDIBSection(0, bmi, DIB_RGB_COLORS, &dst, 0, 0);
        if (hb && dst)
            memcpy(dst, bits, bytes);
        return hb;
    }
    HDC screen = GetDC(0);
    HBITMAP hb = CreateDIBitmap(screen, &h, CBM_INIT, bits, bmi, DIB_RGB_COLORS);
    ReleaseDC(0, screen);
    return hb;
}

// ---------------------------------------------------------------------------------------
// Blits.

BOOL WINAPI StretchBlt(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc,
                       int hSrc, DWORD rop)
{
    DC *d = dc_of(hdcDest);
    if (!d || !d->target())
        return FALSE;
    DC *s = hdcSrc ? dc_of(hdcSrc) : 0;
    const Pixels *sp = s ? s->target() : 0;
    blit(d, *d->target(), xDest, yDest, wDest, hDest, sp, xSrc, ySrc, wSrc, hSrc, rop);
    return TRUE;
}

BOOL WINAPI BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
{
    return StretchBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, cx, cy, rop);
}

int WINAPI StretchDIBits(HDC hdc, int xDest, int yDest, int DestWidth, int DestHeight, int xSrc, int ySrc, int SrcWidth,
                         int SrcHeight, const void *lpBits, const BITMAPINFO *lpbmi, UINT iUsage, DWORD rop)
{
    DC *d = dc_of(hdc);
    if (!d || !d->target())
        return 0;
    Pixels src;
    if (!dib_pixels(lpbmi, lpBits, iUsage, src, d))
        return 0;
    blit(d, *d->target(), xDest, yDest, DestWidth, DestHeight, &src, xSrc, ySrc, SrcWidth, SrcHeight, rop);
    return SrcHeight;
}

int WINAPI SetDIBitsToDevice(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines,
                             const void *lpvBits, const BITMAPINFO *lpbmi, UINT ColorUse)
{
    (void)StartScan;
    (void)cLines;
    return StretchDIBits(hdc, xDest, yDest, (int)w, (int)h, xSrc, ySrc, (int)w, (int)h, lpvBits, lpbmi, ColorUse, SRCCOPY);
}

BOOL WINAPI SetBrushOrgEx(HDC, int, int, LPPOINT lppt)
{
    if (lppt)
        lppt->x = lppt->y = 0;
    return TRUE;
}

int WINAPI SetStretchBltMode(HDC hdc, int mode)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    int old = d->stretch_mode;
    d->stretch_mode = mode;
    return old;
}

// ---------------------------------------------------------------------------------------
// Drawing.

COLORREF WINAPI SetPixel(HDC hdc, int x, int y, COLORREF color)
{
    DC *d = dc_of(hdc);
    if (!d || !d->target())
        return CLR_INVALID;
    const Pixels &p = *d->target();
    if (!clip_ok(d, p, x, y))
        return CLR_INVALID;
    write_raw(p, x, y, raw_of(p, color));
    return get_color(p, x, y);
}

COLORREF WINAPI GetPixel(HDC hdc, int x, int y)
{
    DC *d = dc_of(hdc);
    if (!d || !d->target())
        return CLR_INVALID;
    const Pixels &p = *d->target();
    if (x < 0 || y < 0 || x >= p.w || y >= p.h || !p.bits)
        return CLR_INVALID;
    return get_color(p, x, y);
}

int WINAPI FillRect(HDC hDC, const RECT *r, HBRUSH hbr)
{
    DC *d = dc_of(hDC);
    if (!d || !r)
        return 0;
    BrushObject *b = (BrushObject *)hbr;
    if ((uintptr_t)hbr <= 31)  // COLOR_x + 1
        b = 0;
    COLORREF c = b ? b->color : GetSysColor((int)(uintptr_t)hbr - 1);
    if (b && b->style == BS_NULL)
        return 1;
    fill_rect(d, r->left, r->top, r->right, r->bottom, c);
    return 1;
}

int WINAPI FrameRect(HDC hDC, const RECT *r, HBRUSH hbr)
{
    DC *d = dc_of(hDC);
    if (!d || !r)
        return 0;
    BrushObject *b = (BrushObject *)hbr;
    COLORREF c = b ? b->color : 0;
    fill_rect(d, r->left, r->top, r->right, r->top + 1, c);
    fill_rect(d, r->left, r->bottom - 1, r->right, r->bottom, c);
    fill_rect(d, r->left, r->top, r->left + 1, r->bottom, c);
    fill_rect(d, r->right - 1, r->top, r->right, r->bottom, c);
    return 1;
}

BOOL WINAPI MoveToEx(HDC hdc, int x, int y, LPPOINT lppt)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    if (lppt)
        *lppt = d->pos;
    d->pos = {x, y};
    return TRUE;
}

BOOL WINAPI LineTo(HDC hdc, int x, int y)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    pen_line(d, d->pos.x, d->pos.y, x, y, false);
    d->pos = {x, y};
    return TRUE;
}

BOOL WINAPI Rectangle(HDC hdc, int left, int top, int right, int bottom)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    if (d->brush && d->brush->style != BS_NULL)
        fill_rect(d, left + 1, top + 1, right - 1, bottom - 1, d->brush->color);
    pen_line(d, left, top, right - 1, top, true);
    pen_line(d, right - 1, top, right - 1, bottom - 1, true);
    pen_line(d, right - 1, bottom - 1, left, bottom - 1, true);
    pen_line(d, left, bottom - 1, left, top, true);
    return TRUE;
}

// An elliptic outline (Arc) or filled ellipse, by sampling the ellipse's parametric form.
static void ellipse(DC *d, int l, int t, int r, int b, bool fill, double a0, double a1)
{
    double cx = (l + r - 1) / 2.0, cy = (t + b - 1) / 2.0, rx = (r - l - 1) / 2.0, ry = (b - t - 1) / 2.0;
    if (rx < 0 || ry < 0)
        return;
    if (fill && d->brush && d->brush->style != BS_NULL) {
        for (int y = t; y < b; y++) {
            double dy = ry > 0 ? (y - cy) / ry : 0;
            if (dy * dy > 1)
                continue;
            double half = rx * sqrt(1 - dy * dy);
            fill_rect(d, (int)lround(cx - half), y, (int)lround(cx + half) + 1, y + 1, d->brush->color);
        }
    }
    if (!d->pen || d->pen->style == PS_NULL)
        return;
    int steps = (int)(4 * (rx + ry)) + 8;
    double span = a1 - a0;
    if (span <= 0)
        span += 2 * M_PI;
    int px = 0, py = 0;
    for (int i = 0; i <= steps; i++) {
        double a = a0 + span * i / steps;
        int x = (int)lround(cx + rx * cos(a)), y = (int)lround(cy - ry * sin(a));
        if (i)
            pen_line(d, px, py, x, y, true);
        px = x;
        py = y;
    }
}

BOOL WINAPI Ellipse(HDC hdc, int left, int top, int right, int bottom)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    ellipse(d, left, top, right, bottom, true, 0, 2 * M_PI);
    return TRUE;
}

BOOL WINAPI Arc(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    double cx = (x1 + x2) / 2.0, cy = (y1 + y2) / 2.0;
    // Counterclockwise from the start radial (x3, y3) to the end radial (x4, y4).
    double a0 = atan2(cy - y3, x3 - cx), a1 = atan2(cy - y4, x4 - cx);
    if (x3 == x4 && y3 == y4)
        a1 = a0 + 2 * M_PI;
    ellipse(d, x1, y1, x2, y2, false, a0, a1);
    return TRUE;
}

int WINAPI SetROP2(HDC hdc, int rop2)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    int old = d->rop2;
    d->rop2 = rop2;
    return old;
}

int WINAPI SetBkMode(HDC hdc, int mode)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    int old = d->bk_mode;
    d->bk_mode = mode;
    return old;
}

COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color)
{
    DC *d = dc_of(hdc);
    if (!d)
        return CLR_INVALID;
    COLORREF old = d->bk;
    d->bk = color;
    return old;
}

COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color)
{
    DC *d = dc_of(hdc);
    if (!d)
        return CLR_INVALID;
    COLORREF old = d->text;
    d->text = color;
    return old;
}

UINT WINAPI SetTextAlign(HDC hdc, UINT align)
{
    DC *d = dc_of(hdc);
    if (!d)
        return GDI_ERROR;
    UINT old = d->text_align;
    d->text_align = align;
    return old;
}

HPEN WINAPI CreatePen(int iStyle, int cWidth, COLORREF color) { return (HPEN) new PenObject(iStyle, cWidth, color); }
HBRUSH WINAPI CreateSolidBrush(COLORREF color) { return (HBRUSH) new BrushObject(BS_SOLID, color); }

HBRUSH WINAPI GetSysColorBrush(int nIndex)
{
    static BrushObject *brushes[32];
    if (nIndex < 0 || nIndex >= 32)
        return 0;
    if (!brushes[nIndex]) {
        brushes[nIndex] = new BrushObject(BS_SOLID, GetSysColor(nIndex));
        brushes[nIndex]->stock = true;
    }
    return (HBRUSH)brushes[nIndex];
}

HRGN WINAPI CreateRectRgn(int x1, int y1, int x2, int y2)
{
    RegionObject *r = new RegionObject;
    r->rect = {x1, y1, x2, y2};
    return (HRGN)r;
}

int WINAPI SelectClipRgn(HDC hdc, HRGN hrgn)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    if (!hrgn) {
        d->clip = false;
        return 1;  // NULLREGION
    }
    d->clip = true;
    d->clip_rect = ((RegionObject *)hrgn)->rect;
    return 2;  // SIMPLEREGION
}

BOOL WINAPI GdiFlush(void) { return TRUE; }

// ---------------------------------------------------------------------------------------
// Palettes.

HPALETTE WINAPI CreatePalette(const LOGPALETTE *plpal)
{
    PaletteObject *p = new PaletteObject;
    p->entries.assign(plpal->palPalEntry, plpal->palPalEntry + plpal->palNumEntries);
    return (HPALETTE)p;
}

HPALETTE WINAPI SelectPalette(HDC hdc, HPALETTE hPal, BOOL)
{
    DC *d = dc_of(hdc);
    if (!d)
        return 0;
    PaletteObject *old = d->palette ? d->palette : default_palette();
    d->palette = (PaletteObject *)hPal;
    return (HPALETTE)old;
}

UINT WINAPI RealizePalette(HDC hdc)
{
    DC *d = dc_of(hdc);
    if (!d || !d->palette)
        return 0;
    // A surface DC in 8 bits takes the palette for its colour table.
    const Pixels *p = d->target();
    if (p && p->bpp == 8 && d->kind == DC::Surface)
        for (size_t i = 0; i < d->palette->entries.size() && i < 256; i++) {
            const PALETTEENTRY &e = d->palette->entries[i];
            d->surface.px.pal[i] = {e.peBlue, e.peGreen, e.peRed, 0};
        }
    return (UINT)d->palette->entries.size();
}

UINT WINAPI GetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries)
{
    PaletteObject *p = (PaletteObject *)hpal;
    if (!p)
        return 0;
    if (!pPalEntries)
        return (UINT)p->entries.size();
    UINT n = 0;
    for (UINT i = iStart; i < p->entries.size() && n < cEntries; i++, n++)
        pPalEntries[n] = p->entries[i];
    return n;
}

UINT WINAPI SetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, const PALETTEENTRY *pPalEntries)
{
    PaletteObject *p = (PaletteObject *)hpal;
    if (!p)
        return 0;
    UINT n = 0;
    for (UINT i = iStart; i < p->entries.size() && n < cEntries; i++, n++)
        p->entries[i] = pPalEntries[n];
    return n;
}

// The system palette of an 8-bit display: the 20 static colours at both ends, a gray
// ramp between (the game reads it to build its own palette around the static colours).
UINT WINAPI GetSystemPaletteEntries(HDC, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries)
{
    if (!pPalEntries)
        return 256;
    PaletteObject *def = default_palette();
    UINT n = 0;
    for (UINT i = iStart; i < 256 && n < cEntries; i++, n++) {
        if (i < 10)
            pPalEntries[n] = def->entries[i];
        else if (i >= 246)
            pPalEntries[n] = def->entries[i - 236];
        else
            pPalEntries[n] = {(BYTE)i, (BYTE)i, (BYTE)i, 0};
    }
    return n;
}

// ---------------------------------------------------------------------------------------
// Fonts and text.

HFONT WINAPI CreateFontIndirectA(const LOGFONTA *lplf) { return (HFONT) new FontObject(*lplf); }

HFONT WINAPI CreateFontA(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline,
                         DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality,
                         DWORD iPitchAndFamily, LPCSTR pszFaceName)
{
    LOGFONTA lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = cHeight;
    lf.lfWidth = cWidth;
    lf.lfEscapement = cEscapement;
    lf.lfOrientation = cOrientation;
    lf.lfWeight = cWeight;
    lf.lfItalic = (BYTE)bItalic;
    lf.lfUnderline = (BYTE)bUnderline;
    lf.lfStrikeOut = (BYTE)bStrikeOut;
    lf.lfCharSet = (BYTE)iCharSet;
    lf.lfOutPrecision = (BYTE)iOutPrecision;
    lf.lfClipPrecision = (BYTE)iClipPrecision;
    lf.lfQuality = (BYTE)iQuality;
    lf.lfPitchAndFamily = (BYTE)iPitchAndFamily;
    if (pszFaceName)
        strncpy(lf.lfFaceName, pszFaceName, LF_FACESIZE - 1);
    return CreateFontIndirectA(&lf);
}

BOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    draw_text(d, x, y, lpString, c);
    return TRUE;
}

BOOL WINAPI GetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    *psizl = text_extent(d->font ? d->font : system_font(), lpString, c);
    return TRUE;
}

BOOL WINAPI GetTextMetricsA(HDC hdc, LPTEXTMETRICA tm)
{
    DC *d = dc_of(hdc);
    if (!d)
        return FALSE;
    FontObject *f = d->font ? d->font : system_font();
    int a, de, l;
    text_metrics(f, a, de, l);
    memset(tm, 0, sizeof(*tm));
    tm->tmHeight = a + de;
    tm->tmAscent = a;
    tm->tmDescent = de;
    tm->tmExternalLeading = l;
    tm->tmAveCharWidth = text_extent(f, "x", 1).cx;
    tm->tmMaxCharWidth = text_extent(f, "W", 1).cx;
    tm->tmWeight = f->lf.lfWeight ? f->lf.lfWeight : FW_NORMAL;
    tm->tmFirstChar = 0x20;
    tm->tmLastChar = 0xFF;
    tm->tmDefaultChar = '?';
    tm->tmBreakChar = ' ';
    tm->tmItalic = f->lf.lfItalic;
    tm->tmCharSet = ANSI_CHARSET;
    tm->tmPitchAndFamily = VARIABLE_PITCH | 0x04;  // TMPF_TRUETYPE
    tm->tmDigitizedAspectX = tm->tmDigitizedAspectY = 96;
    return TRUE;
}

int WINAPI DrawTextA(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    DC *d = dc_of(hdc);
    if (!d || !lprc)
        return 0;
    if (cchText < 0)
        cchText = (int)strlen(lpchText);
    FontObject *f = d->font ? d->font : system_font();
    int width = lprc->right - lprc->left;
    // Break into lines: at newlines, and with DT_WORDBREAK at spaces to fit the width.
    std::vector<std::pair<int, int>> lines;
    int start = 0;
    while (start <= cchText) {
        int end = start;
        while (end < cchText && lpchText[end] != '\n' && lpchText[end] != '\r')
            end++;
        if ((format & DT_WORDBREAK) && !(format & DT_SINGLELINE)) {
            int s = start;
            while (s < end) {
                int fit = s, last_space = -1;
                for (int i = s; i < end; i++) {
                    if (lpchText[i] == ' ')
                        last_space = i;
                    if (text_extent(f, lpchText + s, i - s + 1).cx > width) {
                        fit = last_space > s ? last_space : (i > s ? i : i + 1);
                        break;
                    }
                    fit = i + 1;
                }
                lines.push_back({s, fit - s});
                s = fit;
                while (s < end && lpchText[s] == ' ')
                    s++;
            }
            if (start == end)
                lines.push_back({start, 0});
        } else {
            lines.push_back({start, end - start});
        }
        if (format & DT_SINGLELINE) {
            lines.resize(1);
            lines[0] = {0, cchText};
            break;
        }
        if (end >= cchText)
            break;
        start = end + ((lpchText[end] == '\r' && end + 1 < cchText && lpchText[end + 1] == '\n') ? 2 : 1);
    }
    int a, de, l;
    text_metrics(f, a, de, l);
    int lh = a + de + ((format & DT_EXTERNALLEADING) ? l : 0);
    int total = lh * (int)lines.size();
    if (format & DT_CALCRECT) {
        int maxw = 0;
        for (auto &ln : lines) {
            int cw = text_extent(f, lpchText + ln.first, ln.second).cx;
            if (cw > maxw)
                maxw = cw;
        }
        lprc->right = lprc->left + maxw;
        lprc->bottom = lprc->top + total;
        return total;
    }
    int y = lprc->top;
    if (format & DT_SINGLELINE) {
        if (format & DT_VCENTER)
            y = lprc->top + (lprc->bottom - lprc->top - lh) / 2;
        else if (format & DT_BOTTOM)
            y = lprc->bottom - lh;
    }
    UINT saved_align = d->text_align;
    d->text_align = 0;
    for (auto &ln : lines) {
        int cw = text_extent(f, lpchText + ln.first, ln.second).cx;
        int x = lprc->left;
        if (format & DT_CENTER)
            x = lprc->left + (width - cw) / 2;
        else if (format & DT_RIGHT)
            x = lprc->right - cw;
        draw_text(d, x, y, lpchText + ln.first, ln.second);
        y += lh;
    }
    d->text_align = saved_align;
    return total;
}

BOOL WINAPI TranslateCharsetInfo(DWORD *lpSrc, LPCHARSETINFO lpCs, DWORD dwFlags)
{
    memset(lpCs, 0, sizeof(*lpCs));
    UINT cp = (UINT)(uintptr_t)lpSrc;
    if (dwFlags == TCI_SRCCHARSET) {
        lpCs->ciCharset = cp;
        lpCs->ciACP = 1252;
        return TRUE;
    }
    switch (cp) {
    case 932: lpCs->ciCharset = SHIFTJIS_CHARSET; break;
    case 936: lpCs->ciCharset = GB2312_CHARSET; break;
    case 949: lpCs->ciCharset = HANGUL_CHARSET; break;
    case 950: lpCs->ciCharset = CHINESEBIG5_CHARSET; break;
    case 1250: lpCs->ciCharset = EASTEUROPE_CHARSET; break;
    case 1251: lpCs->ciCharset = RUSSIAN_CHARSET; break;
    case 1253: lpCs->ciCharset = GREEK_CHARSET; break;
    case 1254: lpCs->ciCharset = TURKISH_CHARSET; break;
    default: lpCs->ciCharset = ANSI_CHARSET;
    }
    lpCs->ciACP = cp ? cp : 1252;
    return TRUE;
}

BOOL WINAPI EnumDisplaySettingsA(LPCSTR, DWORD iModeNum, DEVMODEA *dm)
{
    // The modes the game knows: 640x480 in 8, 16 and 32 bits; the current one is 32-bit.
    static const DWORD bpps[] = {32, 16, 8};
    DWORD bpp;
    if (iModeNum == ENUM_CURRENT_SETTINGS || iModeNum == ENUM_REGISTRY_SETTINGS)
        bpp = 32;
    else if (iModeNum < 3)
        bpp = bpps[iModeNum];
    else
        return FALSE;
    memset(dm, 0, sizeof(*dm));
    dm->dmSize = sizeof(*dm);
    dm->dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
    dm->dmBitsPerPel = bpp;
    dm->dmPelsWidth = (DWORD)display_width();
    dm->dmPelsHeight = (DWORD)display_height();
    dm->dmDisplayFrequency = 60;
    return TRUE;
}

LONG WINAPI ChangeDisplaySettingsA(DEVMODEA *, DWORD) { return DISP_CHANGE_SUCCESSFUL; }

// Printing: platform/printing.cpp.

}  // extern "C"
