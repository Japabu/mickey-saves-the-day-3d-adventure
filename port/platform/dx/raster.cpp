// raster.cpp - the rasterizer: triangles (edge functions in 28.4 fixed point, the
// top-left fill rule, pixel centres on integer coordinates as in Direct3D 7), lines and
// points, and the per-pixel stages: depth test, texture stages, specular, fog, alpha
// test, colour key, blending, and the writes.
#include "render.h"

#include <math.h>
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace dx {

// Interpolated attributes, in this order; all but z are multiplied by rhw.
enum { A_Z, A_RHW, A_DIFF, A_SPEC = A_DIFF + 4, A_TEX = A_SPEC + 4, A_MAX = A_TEX + 2 * MAX_TEXSETS };

static inline f4 clamp01(f4 v)
{
    return __builtin_elementwise_min(__builtin_elementwise_max(v, f4(0)), f4(1));
}

static inline f4 unpack(uint32_t v)
{
    return f4{(float)(v >> 16 & 0xff), (float)(v >> 8 & 0xff), (float)(v & 0xff), (float)(v >> 24)} * (1.0f / 255.0f);
}

static inline uint32_t pack(f4 c)
{
    c = clamp01(c) * 255.0f + 0.5f;
    return (uint32_t)c.w << 24 | (uint32_t)c.x << 16 | (uint32_t)c.y << 8 | (uint32_t)c.z;
}

static inline f4 from_d3dcolor(DWORD c) { return unpack(c); }

// ---- setup from the device state

void setup_raster(Device *d, Raster &r)
{
    const DWORD *rs = d->rs;
    r.target = d->target;
    r.tf = d->target->fmt;
    r.zb = d->zbuffer();
    r.zenable = rs[D3DRENDERSTATE_ZENABLE] && r.zb;
    r.zwrite = r.zenable && rs[D3DRENDERSTATE_ZWRITEENABLE];
    r.zfunc = rs[D3DRENDERSTATE_ZFUNC];
    if (r.zb) {
        r.zmax = r.zb->desc.ddpfPixelFormat.dwZBitMask;
        r.zbytes = r.zb->fmt.bytes;
    }
    const D3DVIEWPORT7 &vp = d->viewport;
    r.clip = {(LONG)vp.dwX, (LONG)vp.dwY, (LONG)(vp.dwX + vp.dwWidth), (LONG)(vp.dwY + vp.dwHeight)};
    if (r.clip.right > r.target->width())
        r.clip.right = r.target->width();
    if (r.clip.bottom > r.target->height())
        r.clip.bottom = r.target->height();
    r.target->make_hi();
    if (r.zb && r.target->hi)
        r.zb->make_hi();
    r.k = r.target->hi && (!r.zb || r.zb->hi) ? Surface::scale() : 1;
    bool sharp = r.k > 1;
    r.cbits = sharp ? r.target->hi : r.target->bits;
    r.cpitch = sharp ? r.target->hiPitch : r.target->pitch;
    r.zbits = r.zb ? (sharp ? r.zb->hi : r.zb->bits) : 0;
    r.zpitch = r.zb ? (sharp ? r.zb->hiPitch : r.zb->pitch) : 0;
    r.pclip = {r.clip.left * r.k, r.clip.top * r.k, r.clip.right * r.k, r.clip.bottom * r.k};
    r.dirty = {r.pclip.right, r.pclip.bottom, r.pclip.left, r.pclip.top};
    r.cull = rs[D3DRENDERSTATE_CULLMODE];
    r.fill = rs[D3DRENDERSTATE_FILLMODE];
    r.gouraud = rs[D3DRENDERSTATE_SHADEMODE] != D3DSHADE_FLAT;
    r.nstages = 0;
    r.nsets = 0;
    for (int s = 0; s < MAX_STAGES && d->tss[s][D3DTSS_COLOROP] != D3DTOP_DISABLE; s++) {
        StageState &st = r.stages[s];
        const DWORD *t = d->tss[s];
        st.colorop = t[D3DTSS_COLOROP];
        st.carg1 = t[D3DTSS_COLORARG1];
        st.carg2 = t[D3DTSS_COLORARG2];
        st.alphaop = t[D3DTSS_ALPHAOP];
        st.aarg1 = t[D3DTSS_ALPHAARG1];
        st.aarg2 = t[D3DTSS_ALPHAARG2];
        st.tex = d->textures[s] ? texture_cache(d->textures[s]) : 0;
        if (s == 0 && d->legacyModulateAlpha) {
            // TEXTUREMAPBLEND MODULATE: the texture's alpha if it has one, else the diffuse alpha
            st.alphaop = st.tex && st.tex->alpha ? D3DTOP_MODULATE : D3DTOP_SELECTARG2;
        }
        st.texset = (int)(t[D3DTSS_TEXCOORDINDEX] & 0xffff) % MAX_TEXSETS;
        st.addressU = t[D3DTSS_ADDRESSU];
        st.addressV = t[D3DTSS_ADDRESSV];
        st.mag = t[D3DTSS_MAGFILTER];
        st.min = t[D3DTSS_MINFILTER];
        st.mip = t[D3DTSS_MIPFILTER];
        float bias;
        memcpy(&bias, &t[D3DTSS_MIPMAPLODBIAS], 4);
        st.lodBias = bias;
        st.border = from_d3dcolor(t[D3DTSS_BORDERCOLOR]);
        if (st.tex && st.texset + 1 > r.nsets)
            r.nsets = st.texset + 1;
        r.nstages = s + 1;
    }
    r.colorKey = rs[D3DRENDERSTATE_COLORKEYENABLE] != 0;
    r.specular = rs[D3DRENDERSTATE_SPECULARENABLE] != 0;
    r.fog = rs[D3DRENDERSTATE_FOGENABLE] != 0;
    r.fogMode = rs[D3DRENDERSTATE_FOGTABLEMODE];
    r.tableFog = r.fog && r.fogMode != D3DFOG_NONE;
    memcpy(&r.fogStart, &rs[D3DRENDERSTATE_FOGTABLESTART], 4);
    memcpy(&r.fogEnd, &rs[D3DRENDERSTATE_FOGTABLEEND], 4);
    memcpy(&r.fogDensity, &rs[D3DRENDERSTATE_FOGTABLEDENSITY], 4);
    r.fogColor = from_d3dcolor(rs[D3DRENDERSTATE_FOGCOLOR]);
    r.tfactor = from_d3dcolor(rs[D3DRENDERSTATE_TEXTUREFACTOR]);
    r.alphaTest = rs[D3DRENDERSTATE_ALPHATESTENABLE] != 0;
    r.alphaFunc = rs[D3DRENDERSTATE_ALPHAFUNC];
    r.alphaRef = (int)(rs[D3DRENDERSTATE_ALPHAREF] & 0xff);
    r.blend = rs[D3DRENDERSTATE_ALPHABLENDENABLE] != 0;
    r.srcBlend = rs[D3DRENDERSTATE_SRCBLEND];
    r.destBlend = rs[D3DRENDERSTATE_DESTBLEND];
    if (r.srcBlend == D3DBLEND_BOTHSRCALPHA) {
        r.srcBlend = D3DBLEND_SRCALPHA;
        r.destBlend = D3DBLEND_INVSRCALPHA;
    } else if (r.srcBlend == D3DBLEND_BOTHINVSRCALPHA) {
        r.srcBlend = D3DBLEND_INVSRCALPHA;
        r.destBlend = D3DBLEND_SRCALPHA;
    }
    r.extents = false;
}

// ---- per pixel

static inline bool compare(DWORD func, uint32_t a, uint32_t b)
{
    switch (func) {
    case D3DCMP_NEVER: return false;
    case D3DCMP_LESS: return a < b;
    case D3DCMP_EQUAL: return a == b;
    case D3DCMP_LESSEQUAL: return a <= b;
    case D3DCMP_GREATER: return a > b;
    case D3DCMP_NOTEQUAL: return a != b;
    case D3DCMP_GREATEREQUAL: return a >= b;
    default: return true;
    }
}

static inline f4 argument(DWORD a, f4 cur, f4 diff, f4 spec, f4 tex, f4 tf)
{
    f4 v;
    switch (a & D3DTA_SELECTMASK) {
    case D3DTA_DIFFUSE: v = diff; break;
    case D3DTA_CURRENT: v = cur; break;
    case D3DTA_TEXTURE: v = tex; break;
    case D3DTA_TFACTOR: v = tf; break;
    case D3DTA_SPECULAR: v = spec; break;
    default: v = cur; break;
    }
    if (a & D3DTA_COMPLEMENT)
        v = 1.0f - v;
    if (a & D3DTA_ALPHAREPLICATE)
        v = v.wwww;
    return v;
}

// A texture operation on whole colours; the caller takes rgb or alpha from the result.
static inline f4 operation(DWORD op, f4 a1, f4 a2, f4 diff, f4 cur, f4 tex, f4 tf)
{
    switch (op) {
    case D3DTOP_SELECTARG1: return a1;
    case D3DTOP_SELECTARG2: return a2;
    case D3DTOP_MODULATE: return a1 * a2;
    case D3DTOP_MODULATE2X: return a1 * a2 * 2.0f;
    case D3DTOP_MODULATE4X: return a1 * a2 * 4.0f;
    case D3DTOP_ADD: return a1 + a2;
    case D3DTOP_ADDSIGNED: return a1 + a2 - 0.5f;
    case D3DTOP_ADDSIGNED2X: return (a1 + a2 - 0.5f) * 2.0f;
    case D3DTOP_SUBTRACT: return a1 - a2;
    case D3DTOP_ADDSMOOTH: return a1 + a2 - a1 * a2;
    case D3DTOP_BLENDDIFFUSEALPHA: return a1 * diff.w + a2 * (1.0f - diff.w);
    case D3DTOP_BLENDTEXTUREALPHA: return a1 * tex.w + a2 * (1.0f - tex.w);
    case D3DTOP_BLENDFACTORALPHA: return a1 * tf.w + a2 * (1.0f - tf.w);
    case D3DTOP_BLENDTEXTUREALPHAPM: return a1 + a2 * (1.0f - tex.w);
    case D3DTOP_BLENDCURRENTALPHA: return a1 * cur.w + a2 * (1.0f - cur.w);
    case D3DTOP_MODULATEALPHA_ADDCOLOR: return a1 + a1.wwww * a2;
    case D3DTOP_MODULATECOLOR_ADDALPHA: return a1 * a2 + a1.wwww;
    case D3DTOP_MODULATEINVALPHA_ADDCOLOR: return (1.0f - a1.wwww) * a2 + a1;
    case D3DTOP_MODULATEINVCOLOR_ADDALPHA: return (1.0f - a1) * a2 + a1.wwww;
    case D3DTOP_DOTPRODUCT3: {
        f4 p = (a1 - 0.5f) * (a2 - 0.5f) * 4.0f;
        return f4(p.x + p.y + p.z);
    }
    default: return a1; // PREMODULATE and the bump-mapping operations: not used by the game
    }
}

static inline f4 blend_factor(DWORD f, f4 src, f4 dst)
{
    switch (f) {
    case D3DBLEND_ZERO: return f4(0);
    case D3DBLEND_ONE: return f4(1);
    case D3DBLEND_SRCCOLOR: return src;
    case D3DBLEND_INVSRCCOLOR: return 1.0f - src;
    case D3DBLEND_SRCALPHA: return src.wwww;
    case D3DBLEND_INVSRCALPHA: return 1.0f - src.wwww;
    case D3DBLEND_DESTALPHA: return dst.wwww;
    case D3DBLEND_INVDESTALPHA: return 1.0f - dst.wwww;
    case D3DBLEND_DESTCOLOR: return dst;
    case D3DBLEND_INVDESTCOLOR: return 1.0f - dst;
    case D3DBLEND_SRCALPHASAT: {
        float s = fminf(src.w, 1.0f - dst.w);
        return f4{s, s, s, 1};
    }
    default: return f4(1);
    }
}

static inline uint32_t read_target(const PixFmt &f, const uint8_t *p)
{
    if (f.bytes == 2 && f.mask[1] == 0xf800 && f.mask[2] == 0x07e0) {
        uint32_t v = *(const uint16_t *)p, r = v >> 11, g = v >> 5 & 63, b = v & 31;
        return 0xff000000u | (r << 3 | r >> 2) << 16 | (g << 2 | g >> 4) << 8 | (b << 3 | b >> 2);
    }
    if (f.bytes == 4 && f.mask[1] == 0xff0000 && f.mask[2] == 0xff00)
        return *(const uint32_t *)p | (f.mask[0] ? 0 : 0xff000000u);
    return to_argb(f, read_pixel(p, f.bytes), 0);
}

static inline void write_target(const PixFmt &f, uint8_t *p, uint32_t argb)
{
    if (f.bytes == 2 && f.mask[1] == 0xf800 && f.mask[2] == 0x07e0) {
        *(uint16_t *)p = (uint16_t)((argb >> 8 & 0xf800) | (argb >> 5 & 0x07e0) | (argb >> 3 & 0x1f));
        return;
    }
    if (f.bytes == 4 && f.mask[1] == 0xff0000 && f.mask[2] == 0xff00) {
        *(uint32_t *)p = f.mask[0] ? argb : argb & 0xffffff;
        return;
    }
    write_pixel(p, f.bytes, from_argb(f, argb, 0));
}

void Raster::pixel(int x, int y, float z, float w, const float *a, const float *dadx, const float *dady)
{
    // depth
    uint8_t *zp = 0;
    uint32_t zv = 0;
    if (zenable) {
        float zc = z < 0 ? 0 : z > 1 ? 1 : z;
        zv = (uint32_t)((double)zc * zmax + 0.5);
        zp = zbits + (size_t)y * zpitch + x * zbytes;
        uint32_t old = zbytes == 2 ? *(uint16_t *)zp : zbytes == 4 ? *(uint32_t *)zp : read_pixel(zp, zbytes);
        if (!compare(zfunc, zv, old))
            return;
    }
    f4 diff = f4{a[A_DIFF], a[A_DIFF + 1], a[A_DIFF + 2], a[A_DIFF + 3]} * w;
    f4 spec = f4{a[A_SPEC], a[A_SPEC + 1], a[A_SPEC + 2], a[A_SPEC + 3]} * w;
    diff = clamp01(diff);
    spec = clamp01(spec);

    // texture stages
    f4 cur = diff;
    for (int s = 0; s < nstages; s++) {
        const StageState &st = stages[s];
        f4 tex = f4(1);
        if (st.tex) {
            int k = A_TEX + 2 * st.texset;
            float u = a[k] * w, v = a[k + 1] * w;
            // derivatives of u = U / R: (dU - u dR) / R
            float dudx = (dadx[k] - u * dadx[A_RHW]) * w, dvdx = (dadx[k + 1] - v * dadx[A_RHW]) * w;
            float dudy = (dady[k] - u * dady[A_RHW]) * w, dvdy = (dady[k + 1] - v * dady[A_RHW]) * w;
            bool keyed = false;
            tex = sample(st, u, v, dudx, dvdx, dudy, dvdy, colorKey && st.tex->hasKey, &keyed);
            if (keyed)
                return; // colour-keyed texel: no pixel
        }
        f4 c1 = argument(st.carg1, cur, diff, spec, tex, tfactor);
        f4 c2 = argument(st.carg2, cur, diff, spec, tex, tfactor);
        f4 rgb = operation(st.colorop, c1, c2, diff, cur, tex, tfactor);
        float alpha = cur.w;
        if (st.alphaop != D3DTOP_DISABLE) {
            f4 a1 = argument(st.aarg1, cur, diff, spec, tex, tfactor);
            f4 a2 = argument(st.aarg2, cur, diff, spec, tex, tfactor);
            alpha = operation(st.alphaop, a1.wwww, a2.wwww, diff, cur, tex, tfactor).w;
        }
        cur = clamp01(f4{rgb.x, rgb.y, rgb.z, alpha});
    }
    if (specular)
        cur = clamp01(cur + f4{spec.x, spec.y, spec.z, 0});

    // fog: 1 = none
    if (fog) {
        float f;
        if (tableFog) {
            float d = z;
            switch (fogMode) {
            case D3DFOG_LINEAR: f = fogEnd != fogStart ? (fogEnd - d) / (fogEnd - fogStart) : 1; break;
            case D3DFOG_EXP: f = expf(-fogDensity * d); break;
            default: f = expf(-(fogDensity * d) * (fogDensity * d)); break;
            }
            f = f < 0 ? 0 : f > 1 ? 1 : f;
        } else {
            f = spec.w;
        }
        f4 c = cur * f + fogColor * (1.0f - f);
        cur = f4{c.x, c.y, c.z, cur.w};
    }

    if (alphaTest && !compare(alphaFunc, (uint32_t)(cur.w * 255.0f + 0.5f), (uint32_t)alphaRef))
        return;

    uint8_t *p = cbits + (size_t)y * cpitch + x * tf.bytes;
    if (blend) {
        f4 dst = unpack(read_target(tf, p));
        cur = cur * blend_factor(srcBlend, cur, dst) + dst * blend_factor(destBlend, cur, dst);
    }
    write_target(tf, p, pack(cur));
    if (zwrite) {
        if (zbytes == 2)
            *(uint16_t *)zp = (uint16_t)zv;
        else if (zbytes == 4)
            *(uint32_t *)zp = zv;
        else
            write_pixel(zp, zbytes, zv);
    }
}

// ---- primitives

static inline int attribute_count(int nsets) { return A_TEX + 2 * nsets; }

static void load(const SVert *v, int n, float *a)
{
    a[A_Z] = v->z;
    a[A_RHW] = v->rhw;
    for (int i = 0; i < 4; i++) {
        a[A_DIFF + i] = v->diffuse[i] * v->rhw;
        a[A_SPEC + i] = v->specular[i] * v->rhw;
    }
    for (int k = A_TEX; k < n; k += 2) {
        int set = (k - A_TEX) / 2;
        a[k] = v->tex[set][0] * v->rhw;
        a[k + 1] = v->tex[set][1] * v->rhw;
    }
}

void Raster::note_extent(const SVert *v)
{
    if (!extents)
        return;
    float x = v->x < clip.left ? clip.left : v->x > clip.right - 1 ? clip.right - 1 : v->x;
    float y = v->y < clip.top ? clip.top : v->y > clip.bottom - 1 ? clip.bottom - 1 : v->y;
    minx = fminf(minx, x);
    maxx = fmaxf(maxx, x);
    miny = fminf(miny, y);
    maxy = fmaxf(maxy, y);
}

static inline int64_t fixed(float v) { return (int64_t)lrintf(v * 16.0f); }

void Raster::draw_triangle(const SVert *v0, const SVert *v1, const SVert *v2)
{
    int64_t X0 = fixed(v0->x), Y0 = fixed(v0->y), X1 = fixed(v1->x), Y1 = fixed(v1->y), X2 = fixed(v2->x), Y2 = fixed(v2->y);
    int64_t area = (X1 - X0) * (Y2 - Y0) - (X2 - X0) * (Y1 - Y0);
    if (area == 0)
        return;
    if (area < 0) {
        std::swap(v1, v2);
        std::swap(X1, X2);
        std::swap(Y1, Y2);
        area = -area;
    }
    // bounding box in pixels, inside the viewport
    auto floor16 = [](int64_t v) { return (int)(v >= 0 ? v / 16 : -((-v + 15) / 16)); };
    auto ceil16 = [](int64_t v) { return (int)(v >= 0 ? (v + 15) / 16 : -((-v) / 16)); };
    int xmin = ceil16((std::min)({X0, X1, X2})), xmax = floor16((std::max)({X0, X1, X2}));
    int ymin = ceil16((std::min)({Y0, Y1, Y2})), ymax = floor16((std::max)({Y0, Y1, Y2}));
    xmin = (std::max)(xmin, (int)pclip.left);
    ymin = (std::max)(ymin, (int)pclip.top);
    xmax = (std::min)(xmax, (int)pclip.right - 1);
    ymax = (std::min)(ymax, (int)pclip.bottom - 1);
    if (xmin > xmax || ymin > ymax)
        return;

    // edges a->b: E(p) = (Xb - Xa)(Py - Ya) - (Yb - Ya)(Px - Xa) >= 0 inside; on the
    // edge only for top (horizontal, going right) and left (going up) edges
    struct Edge {
        int64_t dx, dy, row;
        int64_t bias;
    } e[3];
    const int64_t X[3] = {X0, X1, X2}, Y[3] = {Y0, Y1, Y2};
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        int64_t ex = X[j] - X[i], ey = Y[j] - Y[i];
        bool topleft = (ey == 0 && ex > 0) || ey < 0;
        e[i].dx = -ey * 16; // per pixel step in x
        e[i].dy = ex * 16;  // per pixel step in y
        e[i].row = ex * ((int64_t)ymin * 16 - Y[i]) - ey * ((int64_t)xmin * 16 - X[i]);
        e[i].bias = topleft ? 0 : -1;
    }

    // attribute planes: a = a0 + dadx (x - x0) + dady (y - y0)
    int n = attribute_count(nsets);
    float A0[A_MAX], A1[A_MAX], A2[A_MAX], dadx[A_MAX], dady[A_MAX], arow[A_MAX], a[A_MAX];
    load(v0, n, A0);
    load(v1, n, A1);
    load(v2, n, A2);
    if (!gouraud) {
        // flat: the first vertex's colours (the pipeline passes the provoking vertex first)
        for (int i = A_DIFF; i < A_TEX; i++) {
            A1[i] = A0[i] / v0->rhw * v1->rhw;
            A2[i] = A0[i] / v0->rhw * v2->rhw;
        }
    }
    float x0 = X0 / 16.0f, y0 = Y0 / 16.0f;
    float x10 = (X1 - X0) / 16.0f, y10 = (Y1 - Y0) / 16.0f, x20 = (X2 - X0) / 16.0f, y20 = (Y2 - Y0) / 16.0f;
    float det = x10 * y20 - x20 * y10;
    for (int i = 0; i < n; i++) {
        float d1 = A1[i] - A0[i], d2 = A2[i] - A0[i];
        dadx[i] = (d1 * y20 - d2 * y10) / det;
        dady[i] = (d2 * x10 - d1 * x20) / det;
    }
    for (int y = ymin; y <= ymax; y++) {
        int64_t e0 = e[0].row, e1 = e[1].row, e2 = e[2].row;
        e[0].row += e[0].dy;
        e[1].row += e[1].dy;
        e[2].row += e[2].dy;
        for (int i = 0; i < n; i++)
            arow[i] = A0[i] + dadx[i] * (xmin - x0) + dady[i] * (y - y0);
        bool inside = false;
        for (int x = xmin; x <= xmax; x++) {
            if ((e0 + e[0].bias) >= 0 && (e1 + e[1].bias) >= 0 && (e2 + e[2].bias) >= 0) {
                inside = true;
                float off = (float)(x - xmin);
                for (int i = 0; i < n; i++)
                    a[i] = arow[i] + dadx[i] * off;
                float w = a[A_RHW] > 0 ? 1.0f / a[A_RHW] : 0;
                pixel(x, y, a[A_Z], w, a, dadx, dady);
            } else if (inside) {
                break; // convex: the span is over
            }
            e0 += e[0].dx;
            e1 += e[1].dx;
            e2 += e[2].dx;
        }
    }
}

void Raster::draw_line(const SVert *v0, const SVert *v1, bool last)
{
    float dx = v1->x - v0->x, dy = v1->y - v0->y;
    int n = attribute_count(nsets);
    float A0[A_MAX], A1[A_MAX], a[A_MAX], dadx[A_MAX] = {}, dady[A_MAX] = {};
    load(v0, n, A0);
    load(v1, n, A1);
    bool xmajor = fabsf(dx) >= fabsf(dy);
    float len = xmajor ? dx : dy;
    if (len == 0) {
        if (last)
            draw_point(v1);
        return;
    }
    // gradients along the line for the texture level
    for (int i = 0; i < n; i++) {
        float g = (A1[i] - A0[i]) / len;
        (xmajor ? dadx : dady)[i] = g;
    }
    float s0 = xmajor ? v0->x : v0->y, s1 = xmajor ? v1->x : v1->y;
    int i0 = (int)floorf(s0 + 0.5f), i1 = (int)floorf(s1 + 0.5f);
    int step = i1 >= i0 ? 1 : -1;
    int end = last ? i1 + step : i1;
    for (int i = i0; i != end; i += step) {
        float t = (i - s0) / (s1 - s0);
        t = t < 0 ? 0 : t > 1 ? 1 : t;
        float other = xmajor ? v0->y + dy * t : v0->x + dx * t;
        int x = xmajor ? i : (int)floorf(other + 0.5f);
        int y = xmajor ? (int)floorf(other + 0.5f) : i;
        if (x < pclip.left || x >= pclip.right || y < pclip.top || y >= pclip.bottom)
            continue;
        for (int k = 0; k < n; k++)
            a[k] = A0[k] + (A1[k] - A0[k]) * t;
        if (!gouraud)
            for (int k = A_DIFF; k < A_TEX; k++)
                a[k] = A0[k] / v0->rhw * a[A_RHW];
        float w = a[A_RHW] > 0 ? 1.0f / a[A_RHW] : 0;
        pixel(x, y, a[A_Z], w, a, dadx, dady);
    }
}

void Raster::draw_point(const SVert *v)
{
    int x = (int)floorf(v->x + 0.5f), y = (int)floorf(v->y + 0.5f);
    if (x < pclip.left || x >= pclip.right || y < pclip.top || y >= pclip.bottom)
        return;
    int n = attribute_count(nsets);
    float a[A_MAX], d[A_MAX] = {};
    load(v, n, a);
    pixel(x, y, a[A_Z], v->rhw > 0 ? 1.0f / v->rhw : 0, a, d, d);
}

// ---- recording and the banded drawing

// Pixel centres are at whole coordinates: the centre of pixel x of the game's screen is
// the middle of the k pixels that show it.
SVert Raster::to_pixels(const SVert *v)
{
    SVert p = *v;
    if (k > 1) {
        p.x = (v->x + 0.5f) * k - 0.5f;
        p.y = (v->y + 0.5f) * k - 0.5f;
    }
    return p;
}

void Raster::touch(const SVert *v, int n)
{
    for (int i = 0; i < n; i++) {
        int x0 = (int)floorf(v[i].x) - 1, x1 = (int)ceilf(v[i].x) + 2;
        int y0 = (int)floorf(v[i].y) - 1, y1 = (int)ceilf(v[i].y) + 2;
        dirty.left = (std::min)(dirty.left, (LONG)(std::max)(x0, (int)pclip.left));
        dirty.top = (std::min)(dirty.top, (LONG)(std::max)(y0, (int)pclip.top));
        dirty.right = (std::max)(dirty.right, (LONG)(std::min)(x1, (int)pclip.right));
        dirty.bottom = (std::max)(dirty.bottom, (LONG)(std::min)(y1, (int)pclip.bottom));
    }
}

void Raster::triangle(const SVert *v0, const SVert *v1, const SVert *v2)
{
    int64_t X0 = fixed(v0->x), Y0 = fixed(v0->y), X1 = fixed(v1->x), Y1 = fixed(v1->y), X2 = fixed(v2->x), Y2 = fixed(v2->y);
    int64_t area = (X1 - X0) * (Y2 - Y0) - (X2 - X0) * (Y1 - Y0);
    if (area == 0)
        return;
    // area > 0: clockwise on the screen (y down)
    if ((cull == D3DCULL_CW && area > 0) || (cull == D3DCULL_CCW && area < 0))
        return;
    if (fill == D3DFILL_WIREFRAME) {
        line(v0, v1, false);
        line(v1, v2, false);
        line(v2, v0, false);
        return;
    }
    if (fill == D3DFILL_POINT) {
        point(v0);
        point(v1);
        point(v2);
        return;
    }
    note_extent(v0);
    note_extent(v1);
    note_extent(v2);
    Cmd c;
    c.kind = Cmd::TRIANGLE;
    c.last = false;
    c.v[0] = to_pixels(v0);
    c.v[1] = to_pixels(v1);
    c.v[2] = to_pixels(v2);
    c.y0 = (int)floorf(fminf(c.v[0].y, fminf(c.v[1].y, c.v[2].y))) - 1;
    c.y1 = (int)ceilf(fmaxf(c.v[0].y, fmaxf(c.v[1].y, c.v[2].y))) + 2;
    if (c.y1 <= pclip.top || c.y0 >= pclip.bottom)
        return;
    touch(c.v, 3);
    cmds.push_back(c);
    work += (area < 0 ? -area : area) * k * k / 512 + 16; // half the bounding area, in pixels
}

void Raster::line(const SVert *v0, const SVert *v1, bool last)
{
    note_extent(v0);
    note_extent(v1);
    Cmd c;
    c.kind = Cmd::LINE;
    c.last = last;
    c.v[0] = to_pixels(v0);
    c.v[1] = to_pixels(v1);
    c.y0 = (int)floorf(fminf(c.v[0].y, c.v[1].y)) - 1;
    c.y1 = (int)ceilf(fmaxf(c.v[0].y, c.v[1].y)) + 2;
    if (c.y1 <= pclip.top || c.y0 >= pclip.bottom)
        return;
    touch(c.v, 2);
    cmds.push_back(c);
    work += (int64_t)(fabsf(c.v[1].x - c.v[0].x) + fabsf(c.v[1].y - c.v[0].y)) + 16;
}

void Raster::point(const SVert *v)
{
    note_extent(v);
    Cmd c;
    c.kind = Cmd::POINT;
    c.last = false;
    c.v[0] = to_pixels(v);
    c.y0 = (int)floorf(c.v[0].y) - 1;
    c.y1 = (int)ceilf(c.v[0].y) + 2;
    touch(c.v, 1);
    cmds.push_back(c);
    work += 16;
}

void Raster::run(const std::vector<Cmd> &list, int y0, int y1)
{
    pclip.top = y0;
    pclip.bottom = y1;
    for (const Cmd &c : list) {
        if (c.y1 <= y0 || c.y0 >= y1)
            continue;
        switch (c.kind) {
        case Cmd::TRIANGLE: draw_triangle(&c.v[0], &c.v[1], &c.v[2]); break;
        case Cmd::LINE: draw_line(&c.v[0], &c.v[1], c.last); break;
        case Cmd::POINT: draw_point(&c.v[0]); break;
        }
    }
}

// Worker threads for the bands: run(n, job) calls job(0..n-1), spread over the
// workers and the calling thread, and returns when all are done.
namespace {
class Pool {
public:
    static Pool &get()
    {
        static Pool *pool = new Pool; // never destroyed: workers outlive static destructors
        return *pool;
    }
    int threads() const { return (int)workers.size() + 1; }
    void run(int n, const std::function<void(int)> &job)
    {
        std::unique_lock<std::mutex> lock(m);
        this->job = &job;
        count = n;
        next = 0;
        pending = (int)workers.size();
        generation++;
        wake.notify_all();
        lock.unlock();
        work();
        lock.lock();
        done.wait(lock, [this] { return pending == 0; });
        this->job = 0;
    }

private:
    Pool()
    {
        unsigned n = std::thread::hardware_concurrency();
        n = n < 1 ? 1 : n > 8 ? 8 : n;
        for (unsigned i = 1; i < n; i++)
            workers.emplace_back([this] { loop(); });
        for (std::thread &t : workers)
            t.detach();
    }
    void work()
    {
        for (int i; (i = next++) < count;)
            (*job)(i);
    }
    void loop()
    {
        unsigned seen = 0;
        for (;;) {
            std::unique_lock<std::mutex> lock(m);
            wake.wait(lock, [&] { return generation != seen; });
            seen = generation;
            lock.unlock();
            work();
            lock.lock();
            if (--pending == 0)
                done.notify_one();
        }
    }
    std::vector<std::thread> workers;
    std::mutex m;
    std::condition_variable wake, done;
    const std::function<void(int)> *job = 0;
    std::atomic<int> next{0};
    int count = 0, pending = 0;
    unsigned generation = 0;
};
} // namespace

void Raster::flush()
{
    if (cmds.empty())
        return;
    RECT full = pclip;
    enum { BAND = 32, PARALLEL_WORK = 20000 };
    int rows = full.bottom - full.top;
    int bands = (rows + BAND - 1) / BAND;
    Pool *pool = work >= PARALLEL_WORK && bands > 1 ? &Pool::get() : 0;
    std::vector<Cmd> list;
    list.swap(cmds); // copies of the state below do not copy the list
    if (!pool || pool->threads() == 1) {
        run(list, full.top, full.bottom);
    } else {
        // each band on its own copy of the state: the bands share no pixels
        pool->run(bands, [&](int b) {
            Raster band = *this;
            int y0 = full.top + b * BAND;
            band.run(list, y0, y0 + BAND < full.bottom ? y0 + BAND : full.bottom);
        });
    }
    cmds.swap(list);
    pclip = full;
    cmds.clear();
    work = 0;
    // the game's own copy of what was drawn
    if (k > 1 && dirty.left < dirty.right && dirty.top < dirty.bottom) {
        target->downsample(dirty);
        if (zwrite)
            zb->downsample(dirty);
    }
    dirty = {pclip.right, pclip.bottom, pclip.left, pclip.top};
    target->version++;
}

} // namespace dx
