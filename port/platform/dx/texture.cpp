// texture.cpp - textures for the rasterizer: a 32-bit copy of every mip level, kept
// up to date with the surfaces' change counters, and the sampler (address modes, point
// and bilinear filtering, mip selection, colour key).
#include "render.h"

#include <math.h>

namespace dx {

static Surface *next_level(Surface *s)
{
    for (Surface *a : s->attached)
        if ((a->desc.ddsCaps.dwCaps & (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP)) == (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP))
            return a;
    return 0;
}

void free_texture_cache(TexCache *c) { delete c; }

static bool current(const TexCache *c, Surface *s)
{
    if (c->palette != s->palette || (s->palette && c->paletteVersion != s->palette->version))
        return false;
    if (c->hasKey != s->hasSrcKey || (s->hasSrcKey && memcmp(&c->key, &s->srcKey, sizeof(c->key))))
        return false;
    size_t i = 0;
    for (Surface *l = s; l; l = next_level(l), i++)
        if (i >= c->versions.size() || c->versions[i] != l->version)
            return false;
    return i == c->versions.size();
}

TexCache *texture_cache(Surface *s)
{
    TexCache *c = s->texCache;
    if (c && current(c, s))
        return c;
    if (!c)
        c = s->texCache = new TexCache;
    c->palette = s->palette;
    c->paletteVersion = s->palette ? s->palette->version : 0;
    c->hasKey = s->hasSrcKey;
    c->key = s->srcKey;
    c->alpha = s->fmt.bits[0] != 0;
    c->versions.clear();
    c->levels.clear();
    const PALETTEENTRY *pal = s->pal();
    for (Surface *l = s; l; l = next_level(l)) {
        c->versions.push_back(l->version);
        TexLevel lv;
        lv.w = l->width();
        lv.h = l->height();
        lv.argb.resize((size_t)lv.w * lv.h);
        lv.keyed.assign((size_t)lv.w * lv.h, 0);
        for (int y = 0; y < lv.h; y++) {
            const uint8_t *p = l->row(y);
            for (int x = 0; x < lv.w; x++, p += l->fmt.bytes) {
                uint32_t raw = read_pixel(p, l->fmt.bytes);
                lv.argb[(size_t)y * lv.w + x] = to_argb(l->fmt, raw, pal);
                if (c->hasKey && raw >= c->key.dwColorSpaceLowValue && raw <= c->key.dwColorSpaceHighValue)
                    lv.keyed[(size_t)y * lv.w + x] = 1;
            }
        }
        c->levels.push_back(std::move(lv));
    }
    return c;
}

static inline f4 texel(uint32_t v)
{
    return f4{(float)(v >> 16 & 0xff), (float)(v >> 8 & 0xff), (float)(v & 0xff), (float)(v >> 24)} * (1.0f / 255.0f);
}

// Texel coordinate i of a level n texels wide under an address mode; -1 = border.
static inline int address(int i, int n, DWORD mode)
{
    switch (mode) {
    case D3DTADDRESS_CLAMP:
        return i < 0 ? 0 : i >= n ? n - 1 : i;
    case D3DTADDRESS_MIRROR: {
        int p = i % (2 * n);
        if (p < 0)
            p += 2 * n;
        return p < n ? p : 2 * n - 1 - p;
    }
    case D3DTADDRESS_BORDER:
        return i < 0 || i >= n ? -1 : i;
    default: { // wrap
        int p = i % n;
        return p < 0 ? p + n : p;
    }
    }
}

static f4 fetch(const StageState &s, const TexLevel &l, int x, int y, bool key, bool *keyed)
{
    x = address(x, l.w, s.addressU);
    y = address(y, l.h, s.addressV);
    if (x < 0 || y < 0) {
        *keyed = false;
        return s.border;
    }
    size_t i = (size_t)y * l.w + x;
    *keyed = key && l.keyed[i];
    return texel(l.argb[i]);
}

static f4 filter(const StageState &s, const TexLevel &l, float u, float v, bool linear, bool key, bool *keyed)
{
    if (!linear)
        return fetch(s, l, (int)floorf(u * l.w), (int)floorf(v * l.h), key, keyed);
    float fx = u * l.w - 0.5f, fy = v * l.h - 0.5f;
    float x0 = floorf(fx), y0 = floorf(fy);
    float ax = fx - x0, ay = fy - y0;
    int ix = (int)x0, iy = (int)y0;
    bool k[4];
    f4 t[4] = {fetch(s, l, ix, iy, key, &k[0]), fetch(s, l, ix + 1, iy, key, &k[1]), fetch(s, l, ix, iy + 1, key, &k[2]),
               fetch(s, l, ix + 1, iy + 1, key, &k[3])};
    float w[4] = {(1 - ax) * (1 - ay), ax * (1 - ay), (1 - ax) * ay, ax * ay};
    if (!key) {
        *keyed = false;
        return t[0] * w[0] + t[1] * w[1] + t[2] * w[2] + t[3] * w[3];
    }
    // keyed texels drop out of the blend; the sample is keyed where they dominate
    f4 sum = 0;
    float total = 0;
    for (int i = 0; i < 4; i++)
        if (!k[i]) {
            sum += t[i] * w[i];
            total += w[i];
        }
    *keyed = total < 0.5f;
    return total > 0 ? sum / total : f4(0);
}

f4 sample(const StageState &s, float u, float v, float dudx, float dvdx, float dudy, float dvdy, bool key, bool *keyed)
{
    const TexCache &c = *s.tex;
    const TexLevel &top = c.levels[0];
    bool minify = false;
    float lod = 0;
    if (s.min != s.mag || (s.mip != D3DTFP_NONE && c.levels.size() > 1)) {
        float ax = dudx * top.w, ay = dvdx * top.h, bx = dudy * top.w, by = dvdy * top.h;
        float rho2 = fmaxf(ax * ax + ay * ay, bx * bx + by * by);
        lod = rho2 > 0 ? 0.5f * log2f(rho2) + s.lodBias : -100.0f;
        minify = lod > 0;
    }
    bool linear = minify ? s.min != D3DTFN_POINT : s.mag != D3DTFG_POINT;
    if (!minify || s.mip == D3DTFP_NONE || c.levels.size() == 1)
        return filter(s, top, u, v, linear, key, keyed);
    float maxLevel = (float)(c.levels.size() - 1);
    if (lod > maxLevel)
        lod = maxLevel;
    if (s.mip == D3DTFP_POINT)
        return filter(s, c.levels[(int)(lod + 0.5f)], u, v, linear, key, keyed);
    int l0 = (int)lod;
    int l1 = l0 + 1 < (int)c.levels.size() ? l0 + 1 : l0;
    float f = lod - l0;
    bool k0, k1;
    f4 a = filter(s, c.levels[l0], u, v, linear, key, &k0);
    f4 b = filter(s, c.levels[l1], u, v, linear, key, &k1);
    *keyed = f < 0.5f ? k0 : k1;
    return a + (b - a) * f;
}

} // namespace dx
