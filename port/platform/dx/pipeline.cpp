// pipeline.cpp - the fixed-function vertex pipeline of the Direct3D 7 device: vertex
// fetch (flexible vertex formats, interleaved or strided), world/view/projection
// transform, lighting, vertex fog, clipping in homogeneous space, the viewport
// mapping, and primitive assembly for the rasterizer.
#include "render.h"

#include <math.h>

namespace dx {

// ---- vertex formats

enum {
    FVF_POSITION_MASK = 0x00e,
    FVF_XYZ = 0x002,
    FVF_XYZRHW = 0x004,
    FVF_NORMAL = 0x010,
    FVF_RESERVED1 = 0x020,
    FVF_DIFFUSE = 0x040,
    FVF_SPECULAR = 0x080,
    FVF_TEXCOUNT_SHIFT = 8,
};

// One component of the vertices: where the first one is and the distance between them.
struct Stream {
    const uint8_t *p = 0;
    size_t stride = 0;
    const float *f(DWORD i) const { return (const float *)(p + i * stride); }
    DWORD u(DWORD i) const { return *(const uint32_t *)(p + i * stride); }
};

struct Streams {
    Stream pos, normal, diffuse, specular, tex[MAX_TEXSETS];
    int texSize[MAX_TEXSETS]; // floats per coordinate set
    int ntex;
    bool transformed;
};

static int tex_size(DWORD fvf, int set)
{
    switch (fvf >> (16 + 2 * set) & 3) {
    case 1: return 3;
    case 2: return 4;
    case 3: return 1;
    default: return 2;
    }
}

static void interleaved(DWORD fvf, const void *data, Streams &s)
{
    const uint8_t *p = (const uint8_t *)data;
    size_t off = 0;
    DWORD pos = fvf & FVF_POSITION_MASK;
    s.transformed = pos == FVF_XYZRHW;
    s.pos.p = p;
    off = pos == FVF_XYZRHW ? 16 : pos == FVF_XYZ ? 12 : 12 + 4 * ((pos - 4) / 2); // XYZB1..B5
    if (fvf & FVF_NORMAL) {
        s.normal.p = p + off;
        off += 12;
    }
    if (fvf & FVF_RESERVED1)
        off += 4;
    if (fvf & FVF_DIFFUSE) {
        s.diffuse.p = p + off;
        off += 4;
    }
    if (fvf & FVF_SPECULAR) {
        s.specular.p = p + off;
        off += 4;
    }
    s.ntex = (int)(fvf >> FVF_TEXCOUNT_SHIFT & 0xf);
    if (s.ntex > MAX_TEXSETS)
        s.ntex = MAX_TEXSETS;
    for (int i = 0; i < s.ntex; i++) {
        s.texSize[i] = tex_size(fvf, i);
        s.tex[i].p = p + off;
        off += 4 * s.texSize[i];
    }
    for (Stream *st : {&s.pos, &s.normal, &s.diffuse, &s.specular})
        st->stride = off;
    for (int i = 0; i < s.ntex; i++)
        s.tex[i].stride = off;
}

static void strided(DWORD fvf, const _D3DDRAWPRIMITIVESTRIDEDDATA *d, Streams &s)
{
    s.transformed = (fvf & FVF_POSITION_MASK) == FVF_XYZRHW;
    s.pos.p = (const uint8_t *)d->position.lpvData;
    s.pos.stride = d->position.dwStride;
    if (fvf & FVF_NORMAL) {
        s.normal.p = (const uint8_t *)d->normal.lpvData;
        s.normal.stride = d->normal.dwStride;
    }
    if (fvf & FVF_DIFFUSE) {
        s.diffuse.p = (const uint8_t *)d->diffuse.lpvData;
        s.diffuse.stride = d->diffuse.dwStride;
    }
    if (fvf & FVF_SPECULAR) {
        s.specular.p = (const uint8_t *)d->specular.lpvData;
        s.specular.stride = d->specular.dwStride;
    }
    s.ntex = (int)(fvf >> FVF_TEXCOUNT_SHIFT & 0xf);
    if (s.ntex > MAX_TEXSETS)
        s.ntex = MAX_TEXSETS;
    for (int i = 0; i < s.ntex; i++) {
        s.texSize[i] = tex_size(fvf, i);
        s.tex[i].p = (const uint8_t *)d->textureCoords[i].lpvData;
        s.tex[i].stride = d->textureCoords[i].dwStride;
    }
}

// ---- matrices (row vectors: v' = v M)

struct M4 {
    float m[4][4];
};

static M4 mat(const D3DMATRIX &d)
{
    M4 r;
    memcpy(r.m, &d, sizeof(r.m));
    return r;
}

static M4 mul(const M4 &a, const M4 &b)
{
    M4 r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
    return r;
}

static f4 xform(const M4 &m, float x, float y, float z, float w)
{
    return f4{x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0],
              x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1],
              x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2],
              x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3]};
}

// The matrix that takes normals to camera space: the inverse transpose of the upper
// 3x3 of world * view, as a row-vector matrix.
static void normal_matrix(const M4 &wv, float n[3][3])
{
    const float(*a)[4] = wv.m;
    float c00 = a[1][1] * a[2][2] - a[1][2] * a[2][1];
    float c01 = a[1][2] * a[2][0] - a[1][0] * a[2][2];
    float c02 = a[1][0] * a[2][1] - a[1][1] * a[2][0];
    float det = a[0][0] * c00 + a[0][1] * c01 + a[0][2] * c02;
    float inv = det != 0 ? 1.0f / det : 0;
    // cofactor matrix / det = (M^-1)^T
    n[0][0] = c00 * inv;
    n[0][1] = c01 * inv;
    n[0][2] = c02 * inv;
    n[1][0] = (a[0][2] * a[2][1] - a[0][1] * a[2][2]) * inv;
    n[1][1] = (a[0][0] * a[2][2] - a[0][2] * a[2][0]) * inv;
    n[1][2] = (a[0][1] * a[2][0] - a[0][0] * a[2][1]) * inv;
    n[2][0] = (a[0][1] * a[1][2] - a[0][2] * a[1][1]) * inv;
    n[2][1] = (a[0][2] * a[1][0] - a[0][0] * a[1][2]) * inv;
    n[2][2] = (a[0][0] * a[1][1] - a[0][1] * a[1][0]) * inv;
}

// ---- lighting

static inline f4 color(const D3DCOLORVALUE &c) { return f4{c.r, c.g, c.b, c.a}; }
static inline f4 unpack_color(DWORD v)
{
    return f4{(float)(v >> 16 & 0xff), (float)(v >> 8 & 0xff), (float)(v & 0xff), (float)(v >> 24)} * (1.0f / 255.0f);
}
static inline float dot3(f4 a, f4 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline f4 norm3(f4 v)
{
    float l = sqrtf(dot3(v, v));
    return l > 0 ? v / l : v;
}

// A light in camera space for one draw.
struct CamLight {
    const D3DLIGHT7 *l;
    f4 pos, dir; // dir: the direction towards the light for directional lights
    float cosTheta, cosPhi;
};

struct Lighting {
    std::vector<CamLight> lights;
    f4 ambient; // the global ambient
    const D3DMATERIAL7 *mat;
    DWORD diffuseSrc, specularSrc, ambientSrc, emissiveSrc;
    bool colorVertex, localViewer, specular;
};

// Material colour from its source: 0 the material, 1 the vertex diffuse, 2 the vertex specular.
static inline f4 source(DWORD src, f4 mat, bool hasD, f4 d, bool hasS, f4 s)
{
    if (src == 1 && hasD)
        return d;
    if (src == 2 && hasS)
        return s;
    return mat;
}

static void light_vertex(const Lighting &L, f4 p, f4 n, bool hasD, f4 vd, bool hasS, f4 vs, f4 &outD, f4 &outS)
{
    const D3DMATERIAL7 &m = *L.mat;
    bool cv = L.colorVertex;
    f4 md = cv ? source(L.diffuseSrc, color(m.diffuse), hasD, vd, hasS, vs) : color(m.diffuse);
    f4 ms = cv ? source(L.specularSrc, color(m.specular), hasD, vd, hasS, vs) : color(m.specular);
    f4 ma = cv ? source(L.ambientSrc, color(m.ambient), hasD, vd, hasS, vs) : color(m.ambient);
    f4 me = cv ? source(L.emissiveSrc, color(m.emissive), hasD, vd, hasS, vs) : color(m.emissive);
    f4 amb = 0, dif = 0, spe = 0;
    f4 view = L.localViewer ? norm3(-p) : f4{0, 0, -1, 0};
    for (const CamLight &cl : L.lights) {
        const D3DLIGHT7 &l = *cl.l;
        f4 ldir;
        float att = 1;
        if (l.dltType == D3DLIGHT_DIRECTIONAL || l.dltType == D3DLIGHT_PARALLELPOINT) {
            ldir = cl.dir;
        } else {
            f4 d = cl.pos - p;
            float dist = sqrtf(dot3(d, d));
            if (l.dvRange > 0 && dist > l.dvRange)
                continue;
            ldir = dist > 0 ? d / dist : f4{0, 0, 1, 0};
            float q = l.dvAttenuation0 + l.dvAttenuation1 * dist + l.dvAttenuation2 * dist * dist;
            att = q > 0 ? 1.0f / q : 1.0f;
            if (l.dltType == D3DLIGHT_SPOT) {
                float rho = dot3(-ldir, cl.dir);
                if (rho <= cl.cosPhi)
                    continue;
                if (rho < cl.cosTheta) {
                    float t = (rho - cl.cosPhi) / (cl.cosTheta - cl.cosPhi);
                    att *= l.dvFalloff == 1.0f ? t : powf(t, l.dvFalloff);
                }
            }
        }
        amb += color(l.dcvAmbient) * att;
        float nl = dot3(n, ldir);
        if (nl > 0) {
            dif += color(l.dcvDiffuse) * (nl * att);
            if (L.specular && m.power > 0) {
                f4 h = norm3(ldir + view);
                float nh = dot3(n, h);
                if (nh > 0)
                    spe += color(l.dcvSpecular) * (powf(nh, m.power) * att);
            }
        }
    }
    f4 c = me + ma * (L.ambient + amb) + md * dif;
    c.w = md.w;
    outD = c;
    f4 s = ms * spe;
    outS = f4{s.x, s.y, s.z, 1};
}

// ---- clipping

// Planes of the clip volume, as distances that are >= 0 inside. x and y have a guard
// band: the rasterizer scissors to the viewport, so they only keep coordinates small.
enum { P_NEAR, P_FAR, P_LEFT, P_RIGHT, P_BOTTOM, P_TOP, P_W, P_COUNT };
static const float GUARD = 8.0f;

static inline float plane(int p, f4 v)
{
    switch (p) {
    case P_NEAR: return v.z;
    case P_FAR: return v.w - v.z;
    case P_LEFT: return v.x + GUARD * v.w;
    case P_RIGHT: return GUARD * v.w - v.x;
    case P_BOTTOM: return v.y + GUARD * v.w;
    case P_TOP: return GUARD * v.w - v.y;
    default: return v.w - 1e-6f;
    }
}

static inline unsigned outcode(f4 v)
{
    unsigned c = 0;
    for (int p = 0; p < P_COUNT; p++)
        if (plane(p, v) < 0)
            c |= 1u << p;
    return c;
}

static CVert lerp(const CVert &a, const CVert &b, float t)
{
    CVert r;
    r.pos = a.pos + (b.pos - a.pos) * t;
    r.v.diffuse = a.v.diffuse + (b.v.diffuse - a.v.diffuse) * t;
    r.v.specular = a.v.specular + (b.v.specular - a.v.specular) * t;
    for (int i = 0; i < MAX_TEXSETS; i++)
        for (int j = 0; j < 2; j++)
            r.v.tex[i][j] = a.v.tex[i][j] + (b.v.tex[i][j] - a.v.tex[i][j]) * t;
    r.code = 0;
    return r;
}

// ---- the draw

struct Draw {
    Device *dev;
    Raster r;
    std::vector<CVert> verts;
    std::vector<SVert> screen; // projected vertices (valid where code == 0)
    float vx, vy, vw, vh, vminz, vmaxz;
    bool transformed;

    void project(const CVert &c, SVert &s) const
    {
        s = c.v;
        if (transformed)
            return;
        float rhw = 1.0f / c.pos.w;
        s.x = vx + (1.0f + c.pos.x * rhw) * vw * 0.5f;
        s.y = vy + (1.0f - c.pos.y * rhw) * vh * 0.5f;
        s.z = vminz + c.pos.z * rhw * (vmaxz - vminz);
        s.rhw = rhw;
    }

    void triangle(DWORD i0, DWORD i1, DWORD i2)
    {
        const CVert *v[3] = {&verts[i0], &verts[i1], &verts[i2]};
        if (transformed || !(v[0]->code | v[1]->code | v[2]->code)) {
            r.triangle(&screen[i0], &screen[i1], &screen[i2]);
            return;
        }
        if (v[0]->code & v[1]->code & v[2]->code)
            return;
        // Sutherland-Hodgman against the planes the triangle crosses
        CVert buf[2][16];
        int n = 3;
        for (int i = 0; i < 3; i++)
            buf[0][i] = *v[i];
        if (!r.gouraud) {
            for (int i = 1; i < 3; i++) {
                buf[0][i].v.diffuse = v[0]->v.diffuse;
                buf[0][i].v.specular = v[0]->v.specular;
            }
        }
        unsigned crossed = v[0]->code | v[1]->code | v[2]->code;
        int cur = 0;
        for (int p = 0; p < P_COUNT && n >= 3; p++) {
            if (!(crossed & (1u << p)))
                continue;
            CVert *in = buf[cur], *out = buf[cur ^ 1];
            int m = 0;
            for (int i = 0; i < n; i++) {
                const CVert &a = in[i], &b = in[(i + 1) % n];
                float da = plane(p, a.pos), db = plane(p, b.pos);
                if (da >= 0)
                    out[m++] = a;
                if ((da >= 0) != (db >= 0) && m < 16)
                    out[m++] = lerp(a, b, da / (da - db));
            }
            n = m;
            cur ^= 1;
        }
        if (n < 3)
            return;
        SVert s[16];
        for (int i = 0; i < n; i++)
            project(buf[cur][i], s[i]);
        for (int i = 1; i + 1 < n; i++)
            r.triangle(&s[0], &s[i], &s[i + 1]);
    }

    void line(DWORD i0, DWORD i1, bool last)
    {
        const CVert &a = verts[i0], &b = verts[i1];
        if (transformed || !(a.code | b.code)) {
            r.line(&screen[i0], &screen[i1], last);
            return;
        }
        if (a.code & b.code)
            return;
        float t0 = 0, t1 = 1;
        for (int p = 0; p < P_COUNT; p++) {
            float da = plane(p, a.pos), db = plane(p, b.pos);
            if (da < 0 && db < 0)
                return;
            if (da < 0)
                t0 = fmaxf(t0, da / (da - db));
            else if (db < 0)
                t1 = fminf(t1, da / (da - db));
        }
        if (t0 >= t1)
            return;
        SVert s0, s1;
        project(lerp(a, b, t0), s0);
        project(lerp(a, b, t1), s1);
        if (!r.gouraud) {
            s1.diffuse = s0.diffuse = a.v.diffuse;
            s1.specular = s0.specular = a.v.specular;
        }
        r.line(&s0, &s1, last && t1 == 1);
    }

    void point(DWORD i)
    {
        if (transformed || !verts[i].code)
            r.point(&screen[i]);
    }
};

static float rs_float(const DWORD *rs, int s)
{
    float f;
    memcpy(&f, &rs[s], 4);
    return f;
}

HRESULT Device::draw(D3DPRIMITIVETYPE type, DWORD fvf, const void *data, const _D3DDRAWPRIMITIVESTRIDEDDATA *sdata,
                     DWORD nverts, const WORD *idx, DWORD nidx, DWORD flags)
{
    if (!nverts || (!data && !sdata))
        return D3D_OK;
    Streams s;
    memset(&s, 0, sizeof(s));
    if (data)
        interleaved(fvf, data, s);
    else
        strided(fvf, sdata, s);

    Draw d;
    d.dev = this;
    setup_raster(this, d.r);
    d.transformed = s.transformed;
    d.vx = (float)viewport.dwX;
    d.vy = (float)viewport.dwY;
    d.vw = (float)viewport.dwWidth;
    d.vh = (float)viewport.dwHeight;
    d.vminz = viewport.dvMinZ;
    d.vmaxz = viewport.dvMaxZ;
    if (rs[D3DRENDERSTATE_EXTENTS] && (clipStatus.dwFlags & D3DCLIPSTATUS_EXTENTS2) && !(flags & D3DDP_DONOTUPDATEEXTENTS)) {
        d.r.extents = true;
        d.r.minx = clipStatus.minx;
        d.r.maxx = clipStatus.maxx;
        d.r.miny = clipStatus.miny;
        d.r.maxy = clipStatus.maxy;
    }

    // ---- vertices
    M4 wv = mul(mat(world), mat(view));
    M4 wvp = mul(wv, mat(proj));
    // Lighting needs normals: vertices without them are pre-lit (the game draws its
    // coloured shapes so, some with D3DDP_DONOTLIGHT and some without).
    bool light = !s.transformed && rs[D3DRENDERSTATE_LIGHTING] && s.normal.p && !(flags & D3DDP_DONOTLIGHT);
    Lighting L;
    float nm[3][3];
    if (light) {
        normal_matrix(wv, nm);
        M4 v = mat(view);
        f4 origin = xform(wv, 0, 0, 0, 1);
        for (const Light &li : lights) {
            if (!li.enabled)
                continue;
            const D3DLIGHT7 &l = li.light;
            CamLight cl;
            cl.l = &l;
            cl.pos = xform(v, l.dvPosition.x, l.dvPosition.y, l.dvPosition.z, 1);
            f4 dir = norm3(xform(v, l.dvDirection.x, l.dvDirection.y, l.dvDirection.z, 0));
            if (l.dltType == D3DLIGHT_DIRECTIONAL)
                cl.dir = -dir;
            else if (l.dltType == D3DLIGHT_PARALLELPOINT)
                cl.dir = norm3(cl.pos - origin);
            else
                cl.dir = dir;
            cl.cosTheta = cosf(l.dvTheta * 0.5f);
            cl.cosPhi = cosf(l.dvPhi * 0.5f);
            L.lights.push_back(cl);
        }
        L.ambient = unpack_color(rs[D3DRENDERSTATE_AMBIENT]);
        L.mat = &material;
        L.colorVertex = rs[RS_COLORVERTEX] != 0;
        L.diffuseSrc = rs[RS_DIFFUSEMATERIALSOURCE];
        L.specularSrc = rs[RS_SPECULARMATERIALSOURCE];
        L.ambientSrc = rs[RS_AMBIENTMATERIALSOURCE];
        L.emissiveSrc = rs[RS_EMISSIVEMATERIALSOURCE];
        L.localViewer = rs[RS_LOCALVIEWER] != 0;
        L.specular = rs[D3DRENDERSTATE_SPECULARENABLE] != 0;
    }
    bool normalize = rs[RS_NORMALIZENORMALS] != 0;
    // vertex fog (table fog is per pixel in the rasterizer)
    DWORD vfog = rs[D3DRENDERSTATE_FOGENABLE] && !d.r.tableFog && !s.transformed ? rs[RS_FOGVERTEXMODE] : 0;
    float fstart = rs_float(rs, D3DRENDERSTATE_FOGTABLESTART), fend = rs_float(rs, D3DRENDERSTATE_FOGTABLEEND);
    float fdens = rs_float(rs, D3DRENDERSTATE_FOGTABLEDENSITY);
    bool rangeFog = rs[D3DRENDERSTATE_RANGEFOGENABLE] != 0;

    d.verts.resize(nverts);
    d.screen.resize(nverts);
    for (DWORD i = 0; i < nverts; i++) {
        CVert &c = d.verts[i];
        SVert &v = c.v;
        bool hasD = s.diffuse.p != 0, hasS = s.specular.p != 0;
        f4 vd = hasD ? unpack_color(s.diffuse.u(i)) : f4(1);
        f4 vs = hasS ? unpack_color(s.specular.u(i)) : f4(0);
        for (int k = 0; k < MAX_TEXSETS; k++) {
            if (k < s.ntex && s.tex[k].p) {
                const float *t = s.tex[k].f(i);
                v.tex[k][0] = t[0];
                v.tex[k][1] = s.texSize[k] > 1 ? t[1] : 0;
            } else {
                v.tex[k][0] = v.tex[k][1] = 0;
            }
        }
        const float *p = s.pos.f(i);
        if (s.transformed) {
            v.x = p[0];
            v.y = p[1];
            v.z = p[2];
            v.rhw = p[3];
            v.diffuse = vd;
            v.specular = f4{vs.x, vs.y, vs.z, hasS ? vs.w : 1};
            c.code = 0;
            d.screen[i] = v;
            continue;
        }
        c.pos = xform(wvp, p[0], p[1], p[2], 1);
        c.code = outcode(c.pos);
        f4 cam = 0;
        if (light || vfog)
            cam = xform(wv, p[0], p[1], p[2], 1);
        if (light) {
            const float *n = s.normal.f(i);
            f4 nc = f4{n[0] * nm[0][0] + n[1] * nm[1][0] + n[2] * nm[2][0], n[0] * nm[0][1] + n[1] * nm[1][1] + n[2] * nm[2][1],
                       n[0] * nm[0][2] + n[1] * nm[1][2] + n[2] * nm[2][2], 0};
            if (normalize)
                nc = norm3(nc);
            light_vertex(L, cam, nc, hasD, vd, hasS, vs, v.diffuse, v.specular);
        } else {
            v.diffuse = vd;
            v.specular = f4{vs.x, vs.y, vs.z, hasS ? vs.w : 1};
        }
        if (vfog) {
            float dist = rangeFog ? sqrtf(dot3(cam, cam)) : fabsf(cam.z);
            float f;
            switch (vfog) {
            case D3DFOG_LINEAR: f = fend != fstart ? (fend - dist) / (fend - fstart) : 1; break;
            case D3DFOG_EXP: f = expf(-fdens * dist); break;
            default: f = expf(-(fdens * dist) * (fdens * dist)); break;
            }
            v.specular.w = f < 0 ? 0 : f > 1 ? 1 : f;
        }
        v.diffuse = __builtin_elementwise_min(__builtin_elementwise_max(v.diffuse, f4(0)), f4(1));
        v.specular = __builtin_elementwise_min(__builtin_elementwise_max(v.specular, f4(0)), f4(1));
        if (!c.code)
            d.project(c, d.screen[i]);
    }

    // ---- primitives
    DWORD count = idx ? nidx : nverts;
    auto at = [&](DWORD k) -> DWORD { return idx ? idx[k] : k; };
    if (idx)
        for (DWORD k = 0; k < nidx; k++)
            if (idx[k] >= nverts)
                return DDERR_INVALIDPARAMS;
    switch (type) {
    case D3DPT_TRIANGLELIST:
        for (DWORD k = 0; k + 2 < count; k += 3)
            d.triangle(at(k), at(k + 1), at(k + 2));
        break;
    case D3DPT_TRIANGLESTRIP:
        for (DWORD k = 0; k + 2 < count; k++) {
            if (k & 1)
                d.triangle(at(k), at(k + 2), at(k + 1)); // the first vertex leads (flat shading)
            else
                d.triangle(at(k), at(k + 1), at(k + 2));
        }
        break;
    case D3DPT_TRIANGLEFAN:
        for (DWORD k = 1; k + 1 < count; k++)
            d.triangle(at(k), at(k + 1), at(0));
        break;
    case D3DPT_LINELIST:
        for (DWORD k = 0; k + 1 < count; k += 2)
            d.line(at(k), at(k + 1), false);
        break;
    case D3DPT_LINESTRIP:
        for (DWORD k = 0; k + 1 < count; k++)
            d.line(at(k), at(k + 1), k + 2 == count);
        break;
    case D3DPT_POINTLIST:
        for (DWORD k = 0; k < count; k++)
            d.point(at(k));
        break;
    default:
        return DDERR_INVALIDPARAMS;
    }
    if (d.r.extents) {
        clipStatus.minx = d.r.minx;
        clipStatus.maxx = d.r.maxx;
        clipStatus.miny = d.r.miny;
        clipStatus.maxy = d.r.maxy;
    }
    return D3D_OK;
}

HRESULT Device::DrawPrimitive(D3DPRIMITIVETYPE type, DWORD fvf, void *v, DWORD n, DWORD flags)
{
    return draw(type, fvf, v, 0, n, 0, 0, flags);
}

HRESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, DWORD fvf, void *v, DWORD n, WORD *idx, DWORD nidx, DWORD flags)
{
    return draw(type, fvf, v, 0, n, idx, nidx, flags);
}

HRESULT Device::DrawPrimitiveStrided(D3DPRIMITIVETYPE type, DWORD fvf, _D3DDRAWPRIMITIVESTRIDEDDATA *s, DWORD n, DWORD flags)
{
    return draw(type, fvf, 0, s, n, 0, 0, flags);
}

HRESULT Device::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE type, DWORD fvf, _D3DDRAWPRIMITIVESTRIDEDDATA *s, DWORD n, WORD *idx,
                                            DWORD nidx, DWORD flags)
{
    return draw(type, fvf, 0, s, n, idx, nidx, flags);
}

} // namespace dx
