// d3d_device.cpp - IDirect3D7 (one device, its z-buffer formats) and the state side of
// IDirect3DDevice7: render and texture stage states, transforms, viewport, material,
// lights, clip status, Clear and Load.
#include "device.h"

#include <string.h>

namespace dx {

// ---- the device the enumeration reports

static D3DDEVICEDESC7 make_desc(bool hardware)
{
    D3DDEVICEDESC7 d;
    memset(&d, 0, sizeof(d));
    d.dwDevCaps = (hardware ? D3DDEVCAPS_HWRASTERIZATION | 0x00010000 /* HWTRANSFORMANDLIGHT */ : 0) | 0x00000010 /* FLOATTLVERTEX */ | 0x00000100 /* TEXTURESYSTEMMEMORY */
        | 0x00000200 /* TEXTUREVIDEOMEMORY */ | 0x00000400 /* DRAWPRIMTLVERTEX */ | 0x00000800 /* CANRENDERAFTERFLIP */
        | 0x00002000 /* DRAWPRIMITIVES2 */;
    D3DPRIMCAPS p;
    memset(&p, 0, sizeof(p));
    p.dwSize = sizeof(p);
    p.dwMiscCaps = 0x00000002 /* MASKZ */ | 0x00000010 /* CULLNONE */ | 0x00000020 /* CULLCW */ | 0x00000040 /* CULLCCW */;
    p.dwRasterCaps = 0x00000010 /* ZTEST */ | 0x00000080 /* FOGVERTEX */ | 0x00000100 /* FOGTABLE */
        | 0x00002000 /* MIPMAPLODBIAS */;
    p.dwZCmpCaps = 0xff;
    p.dwAlphaCmpCaps = 0xff;
    p.dwSrcBlendCaps = 0x1fff;
    p.dwDestBlendCaps = 0x1fff;
    p.dwShadeCaps = 0x00000002 | 0x00000008 /* COLOR FLAT/GOURAUD RGB */ | 0x00000080 | 0x00000200 /* SPECULAR FLAT/GOURAUD RGB */
        | 0x00001000 | 0x00004000 /* ALPHA FLAT/GOURAUD BLEND */ | 0x00080000 /* FOGGOURAUD */;
    p.dwTextureCaps = D3DPTEXTURECAPS_PERSPECTIVE | 0x00000004 /* ALPHA */ | 0x00000008 /* TRANSPARENCY */;
    p.dwTextureFilterCaps = 0x0000003f | 0x00000100 | 0x00000200 | 0x00010000 | 0x00020000 | 0x01000000 | 0x02000000;
    p.dwTextureBlendCaps = 0x000000ff;
    p.dwTextureAddressCaps = 0x00000001 /* WRAP */ | 0x00000004 /* CLAMP */ | 0x00000010 /* INDEPENDENTUV */;
    d.dpcLineCaps = p;
    d.dpcTriCaps = p;
    d.dwDeviceRenderBitDepth = DDBD_16 | DDBD_32;
    d.dwDeviceZBufferBitDepth = DDBD_16 | DDBD_32;
    d.dwMinTextureWidth = d.dwMinTextureHeight = 1;
    d.dwMaxTextureWidth = d.dwMaxTextureHeight = 2048;
    d.dwMaxTextureRepeat = 2048;
    d.dwMaxTextureAspectRatio = 2048;
    d.dwMaxAnisotropy = 1;
    d.dvGuardBandLeft = d.dvGuardBandTop = -4096;
    d.dvGuardBandRight = d.dvGuardBandBottom = 4096;
    d.dwFVFCaps = 8;
    d.dwTextureOpCaps = 0x0000ffff; // DISABLE .. BLENDCURRENTALPHA
    d.wMaxTextureBlendStages = MAX_STAGES;
    d.wMaxSimultaneousTextures = MAX_STAGES;
    d.dwMaxActiveLights = 8;
    d.dvMaxVertexW = 1e10f;
    d.deviceGUID = hardware ? IID_IDirect3DHALDevice : IID_IDirect3DRGBDevice;
    d.dwVertexProcessingCaps = 0x3f;
    return d;
}

// The game keeps a pointer to the enumerated description's GUID, so they are static.
const D3DDEVICEDESC7 &device_desc(bool hardware)
{
    static D3DDEVICEDESC7 hal = make_desc(true), rgb = make_desc(false);
    return hardware ? hal : rgb;
}

// ---- IDirect3D7

HRESULT Direct3D::QueryInterface(REFIID iid, void **out) { return dd->QueryInterface(iid, out); }
ULONG Direct3D::AddRef() { return dd->AddRef(); }
ULONG Direct3D::Release() { return dd->Release(); }

HRESULT Direct3D::EnumDevices(LPD3DENUMDEVICESCALLBACK7 cb, void *ctx)
{
    if (!cb)
        return DDERR_INVALIDPARAMS;
    // Both render with the software rasterizer. The game asks for the RGB (software)
    // device when its back buffer is in system memory (the windowed screen) and for a
    // hardware one otherwise, and it takes the first matching one: RGB comes first, as
    // in DirectX 7's own list.
    if (cb((char *)"Microsoft Direct3D RGB Software Emulation", (char *)"RGB Emulation",
           const_cast<D3DDEVICEDESC7 *>(&device_desc(false)), ctx) == D3DENUMRET_CANCEL)
        return D3D_OK;
    cb((char *)"Microsoft Direct3D Hardware acceleration through Direct3D HAL", (char *)"Direct3D HAL",
       const_cast<D3DDEVICEDESC7 *>(&device_desc(true)), ctx);
    return D3D_OK;
}

HRESULT Direct3D::CreateDevice(const GUID &guid, IDirectDrawSurface7 *surface, IDirect3DDevice7 **out)
{
    if (!out || !surface)
        return DDERR_INVALIDPARAMS;
    return create_device(dd, static_cast<Surface *>(surface), guid != IID_IDirect3DRGBDevice, out);
}

HRESULT Direct3D::CreateVertexBuffer(struct _D3DVERTEXBUFFERDESC *, IDirect3DVertexBuffer7 **, DWORD) { DX_UNSUPPORTED("IDirect3D7::CreateVertexBuffer"); }

HRESULT Direct3D::EnumZBufferFormats(const GUID &, LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx)
{
    if (!cb)
        return DDERR_INVALIDPARAMS;
    DDPIXELFORMAT f16 = z_format(16), f32 = z_format(32);
    if (cb(&f16, ctx) == D3DENUMRET_OK)
        cb(&f32, ctx);
    return D3D_OK;
}

HRESULT Direct3D::EvictManagedTextures() { return D3D_OK; }

// ---- the device

HRESULT create_device(DirectDraw *dd, Surface *target, bool hardware, IDirect3DDevice7 **out)
{
    // any 16- to 32-bit surface will do (DDSCAPS_3DDEVICE is not required)
    if (target->fmt.palette || target->fmt.bytes < 2 || target->fmt.zbuffer) {
        *out = 0;
        return DDERR_INVALIDPARAMS;
    }
    Device *d = new Device(dd, target);
    d->hardware = hardware;
    *out = d;
    return D3D_OK;
}

static void identity(D3DMATRIX &m)
{
    memset(&m, 0, sizeof(m));
    m._11 = m._22 = m._33 = m._44 = 1;
}

Device::Device(DirectDraw *dd, Surface *target) : dd(dd), target(target)
{
    dd->AddRef();
    target->AddRef();
    memset(rs, 0, sizeof(rs));
    rs[D3DRENDERSTATE_TEXTUREPERSPECTIVE] = TRUE;
    rs[D3DRENDERSTATE_ZENABLE] = zbuffer() ? 1 : 0;
    rs[D3DRENDERSTATE_FILLMODE] = D3DFILL_SOLID;
    rs[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_GOURAUD;
    rs[D3DRENDERSTATE_LINEPATTERN] = 0;
    rs[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
    rs[D3DRENDERSTATE_LASTPIXEL] = TRUE;
    rs[D3DRENDERSTATE_SRCBLEND] = D3DBLEND_ONE;
    rs[D3DRENDERSTATE_DESTBLEND] = D3DBLEND_ZERO;
    rs[D3DRENDERSTATE_CULLMODE] = D3DCULL_CCW;
    rs[D3DRENDERSTATE_ZFUNC] = D3DCMP_LESSEQUAL;
    rs[D3DRENDERSTATE_ALPHAFUNC] = D3DCMP_ALWAYS;
    rs[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_NONE;
    rs[D3DRENDERSTATE_FOGTABLEEND] = 0x3f800000; // 1.0f
    rs[D3DRENDERSTATE_FOGTABLEDENSITY] = 0x3f800000;
    rs[D3DRENDERSTATE_TEXTUREFACTOR] = 0xffffffff;
    rs[D3DRENDERSTATE_LIGHTING] = TRUE;
    rs[RS_CLIPPING] = TRUE;
    rs[RS_COLORVERTEX] = TRUE;
    rs[RS_LOCALVIEWER] = TRUE;
    rs[RS_DIFFUSEMATERIALSOURCE] = 1; // D3DMCS_COLOR1
    rs[RS_SPECULARMATERIALSOURCE] = 2; // D3DMCS_COLOR2
    memset(tss, 0, sizeof(tss));
    for (int s = 0; s < MAX_STAGES; s++) {
        tss[s][D3DTSS_COLOROP] = s ? D3DTOP_DISABLE : D3DTOP_MODULATE;
        tss[s][D3DTSS_COLORARG1] = D3DTA_TEXTURE;
        tss[s][D3DTSS_COLORARG2] = D3DTA_CURRENT;
        tss[s][D3DTSS_ALPHAOP] = s ? D3DTOP_DISABLE : D3DTOP_SELECTARG1;
        tss[s][D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
        tss[s][D3DTSS_ALPHAARG2] = D3DTA_CURRENT;
        tss[s][D3DTSS_TEXCOORDINDEX] = s;
        tss[s][D3DTSS_ADDRESS] = tss[s][D3DTSS_ADDRESSU] = tss[s][D3DTSS_ADDRESSV] = D3DTADDRESS_WRAP;
        tss[s][D3DTSS_MAGFILTER] = D3DTFG_POINT;
        tss[s][D3DTSS_MINFILTER] = D3DTFN_POINT;
        tss[s][D3DTSS_MIPFILTER] = D3DTFP_NONE;
    }
    identity(world);
    identity(view);
    identity(proj);
    viewport = {0, 0, (DWORD)target->width(), (DWORD)target->height(), 0, 1};
    memset(&material, 0, sizeof(material));
    memset(&clipStatus, 0, sizeof(clipStatus));
}

Device::~Device()
{
    for (Surface *t : textures)
        if (t)
            t->Release();
    target->Release();
    dd->Release();
}

Surface *Device::zbuffer() const
{
    for (Surface *s : target->attached)
        if (s->desc.ddsCaps.dwCaps & DDSCAPS_ZBUFFER)
            return s;
    return 0;
}

HRESULT Device::QueryInterface(REFIID, void **out)
{
    *out = 0;
    return E_NOINTERFACE;
}

HRESULT Device::GetCaps(_D3DDeviceDesc7 *d)
{
    if (!d)
        return DDERR_INVALIDPARAMS;
    *d = device_desc(hardware);
    return D3D_OK;
}

HRESULT Device::EnumTextureFormats(HRESULT(__stdcall *cb)(DDPIXELFORMAT *, void *), void *ctx)
{
    if (!cb)
        return DDERR_INVALIDPARAMS;
    DDPIXELFORMAT formats[] = {
        rgb_format(16, 0xf800, 0x07e0, 0x001f, 0),
        rgb_format(16, 0x7c00, 0x03e0, 0x001f, 0x8000),
        rgb_format(16, 0x0f00, 0x00f0, 0x000f, 0xf000),
        rgb_format(32, 0xff0000, 0x00ff00, 0x0000ff, 0xff000000),
        rgb_format(32, 0xff0000, 0x00ff00, 0x0000ff, 0),
        display_format(8),
    };
    for (DDPIXELFORMAT &f : formats)
        if (cb(&f, ctx) == D3DENUMRET_CANCEL)
            break;
    return D3D_OK;
}

HRESULT Device::BeginScene() { return D3D_OK; }
HRESULT Device::EndScene() { return D3D_OK; }

HRESULT Device::GetDirect3D(IDirect3D7 **out)
{
    dd->AddRef();
    *out = &dd->d3d;
    return D3D_OK;
}

HRESULT Device::SetRenderTarget(IDirectDrawSurface7 *s, DWORD)
{
    if (!s)
        return DDERR_INVALIDPARAMS;
    s->AddRef();
    target->Release();
    target = static_cast<Surface *>(s);
    return D3D_OK;
}

HRESULT Device::GetRenderTarget(IDirectDrawSurface7 **out)
{
    target->AddRef();
    *out = target;
    return D3D_OK;
}

HRESULT Device::Clear(DWORD count, D3DRECT *rects, DWORD flags, D3DCOLOR color, D3DVALUE z, DWORD)
{
    DX_GUARD;
    RECT vp = {(LONG)viewport.dwX, (LONG)viewport.dwY, (LONG)(viewport.dwX + viewport.dwWidth),
               (LONG)(viewport.dwY + viewport.dwHeight)};
    Surface *zb = (flags & D3DCLEAR_ZBUFFER) ? zbuffer() : 0;
    uint32_t pixel = from_argb(target->fmt, color, target->pal());
    uint32_t depth = 0;
    if (zb) {
        uint32_t max = zb->desc.ddpfPixelFormat.dwZBitMask;
        float c = z < 0 ? 0 : z > 1 ? 1 : z;
        depth = (uint32_t)((double)c * max + 0.5);
    }
    for (DWORD i = 0; i < (rects ? count : 1); i++) {
        RECT r = vp;
        if (rects) {
            r.left = rects[i].x1 > vp.left ? rects[i].x1 : vp.left;
            r.top = rects[i].y1 > vp.top ? rects[i].y1 : vp.top;
            r.right = rects[i].x2 < vp.right ? rects[i].x2 : vp.right;
            r.bottom = rects[i].y2 < vp.bottom ? rects[i].y2 : vp.bottom;
        }
        if (r.right <= r.left || r.bottom <= r.top)
            continue;
        if (flags & D3DCLEAR_TARGET)
            target->fill(&r, pixel);
        if (zb)
            zb->fill(&r, depth);
    }
    return D3D_OK;
}

HRESULT Device::SetTransform(D3DTRANSFORMSTATETYPE t, D3DMATRIX *m)
{
    if (!m)
        return DDERR_INVALIDPARAMS;
    switch (t) {
    case D3DTRANSFORMSTATE_WORLD: world = *m; break;
    case D3DTRANSFORMSTATE_VIEW: view = *m; break;
    case D3DTRANSFORMSTATE_PROJECTION: proj = *m; break;
    default: return DDERR_INVALIDPARAMS;
    }
    return D3D_OK;
}

HRESULT Device::GetTransform(D3DTRANSFORMSTATETYPE t, D3DMATRIX *m)
{
    if (!m)
        return DDERR_INVALIDPARAMS;
    switch (t) {
    case D3DTRANSFORMSTATE_WORLD: *m = world; break;
    case D3DTRANSFORMSTATE_VIEW: *m = view; break;
    case D3DTRANSFORMSTATE_PROJECTION: *m = proj; break;
    default: return DDERR_INVALIDPARAMS;
    }
    return D3D_OK;
}

HRESULT Device::MultiplyTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX *) { DX_UNSUPPORTED("IDirect3DDevice7::MultiplyTransform"); }

HRESULT Device::SetViewport(D3DVIEWPORT7 *v)
{
    if (!v || v->dwX + v->dwWidth > (DWORD)target->width() || v->dwY + v->dwHeight > (DWORD)target->height())
        return DDERR_INVALIDPARAMS;
    viewport = *v;
    return D3D_OK;
}

HRESULT Device::GetViewport(D3DVIEWPORT7 *v)
{
    *v = viewport;
    return D3D_OK;
}

HRESULT Device::SetMaterial(D3DMATERIAL7 *m)
{
    material = *m;
    return D3D_OK;
}

HRESULT Device::GetMaterial(D3DMATERIAL7 *m)
{
    *m = material;
    return D3D_OK;
}

HRESULT Device::SetLight(DWORD i, D3DLIGHT7 *l)
{
    if (!l)
        return DDERR_INVALIDPARAMS;
    if (i >= lights.size())
        lights.resize(i + 1, Light{{}, false});
    lights[i].light = *l;
    return D3D_OK;
}

HRESULT Device::GetLight(DWORD i, D3DLIGHT7 *l)
{
    if (i >= lights.size())
        return DDERR_INVALIDPARAMS;
    *l = lights[i].light;
    return D3D_OK;
}

HRESULT Device::LightEnable(DWORD i, BOOL on)
{
    if (i >= lights.size()) {
        // enabling a light that was never set gives the default light
        D3DLIGHT7 def;
        memset(&def, 0, sizeof(def));
        def.dltType = D3DLIGHT_DIRECTIONAL;
        def.dcvDiffuse.r = def.dcvDiffuse.g = def.dcvDiffuse.b = 1;
        def.dvDirection.z = 1;
        lights.resize(i + 1, Light{def, false});
    }
    lights[i].enabled = on != 0;
    return D3D_OK;
}

HRESULT Device::GetLightEnable(DWORD i, BOOL *on)
{
    if (i >= lights.size())
        return DDERR_INVALIDPARAMS;
    *on = lights[i].enabled;
    return D3D_OK;
}

HRESULT Device::SetRenderState(D3DRENDERSTATETYPE s, DWORD v)
{
    if ((unsigned)s >= RS_COUNT)
        return DDERR_INVALIDPARAMS;
    rs[s] = v;
    if (s == D3DRENDERSTATE_TEXTUREMAPBLEND)
        texture_map_blend(v);
    return D3D_OK;
}

// The DirectX 5 texture blend as stage 0 operations, as the DirectX 7 runtime maps it.
void Device::texture_map_blend(DWORD mode)
{
    DWORD *t = tss[0];
    t[D3DTSS_COLORARG1] = t[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
    t[D3DTSS_COLORARG2] = t[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
    legacyModulateAlpha = false;
    switch (mode) {
    case D3DTBLEND_DECAL:
    case D3DTBLEND_COPY:
        t[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
        t[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
        break;
    case D3DTBLEND_MODULATE:
        t[D3DTSS_COLOROP] = D3DTOP_MODULATE;
        t[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
        legacyModulateAlpha = true; // the texture's alpha if it has one, else the diffuse alpha
        break;
    case D3DTBLEND_MODULATEALPHA:
        t[D3DTSS_COLOROP] = D3DTOP_MODULATE;
        t[D3DTSS_ALPHAOP] = D3DTOP_MODULATE;
        break;
    case D3DTBLEND_DECALALPHA:
        t[D3DTSS_COLOROP] = D3DTOP_BLENDTEXTUREALPHA;
        t[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
        break;
    case D3DTBLEND_ADD:
        t[D3DTSS_COLOROP] = D3DTOP_ADD;
        t[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
        break;
    default:
        DX_LOG("TEXTUREMAPBLEND %u is not implemented\n", (unsigned)mode);
        break;
    }
}

HRESULT Device::GetRenderState(D3DRENDERSTATETYPE s, DWORD *v)
{
    if ((unsigned)s >= RS_COUNT || !v)
        return DDERR_INVALIDPARAMS;
    *v = rs[s];
    return D3D_OK;
}

HRESULT Device::BeginStateBlock() { DX_UNSUPPORTED("IDirect3DDevice7::BeginStateBlock"); }
HRESULT Device::EndStateBlock(DWORD *) { DX_UNSUPPORTED("IDirect3DDevice7::EndStateBlock"); }
HRESULT Device::PreLoad(IDirectDrawSurface7 *) { return D3D_OK; }

HRESULT Device::SetClipStatus(D3DCLIPSTATUS *c)
{
    clipStatus = *c;
    return D3D_OK;
}

HRESULT Device::GetClipStatus(D3DCLIPSTATUS *c)
{
    *c = clipStatus;
    static int budget = getenv("PORT_DX_TRACE") ? atoi(getenv("PORT_DX_TRACE")) : 0;
    if (budget > 0 && budget--)
        DX_LOG("GetClipStatus flags %#x extents x %g..%g y %g..%g\n", (unsigned)c->dwFlags, c->minx, c->maxx, c->miny,
               c->maxy);
    return D3D_OK;
}

HRESULT Device::DrawPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::DrawPrimitiveVB"); }
HRESULT Device::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE, IDirect3DVertexBuffer7 *, DWORD, DWORD, WORD *, DWORD, DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::DrawIndexedPrimitiveVB"); }
HRESULT Device::ComputeSphereVisibility(D3DVECTOR *, D3DVALUE *, DWORD, DWORD, DWORD *) { DX_UNSUPPORTED("IDirect3DDevice7::ComputeSphereVisibility"); }

HRESULT Device::GetTexture(DWORD stage, IDirectDrawSurface7 **out)
{
    if (stage >= MAX_STAGES)
        return DDERR_INVALIDPARAMS;
    *out = textures[stage];
    if (*out)
        (*out)->AddRef();
    return D3D_OK;
}

HRESULT Device::SetTexture(DWORD stage, IDirectDrawSurface7 *t)
{
    if (stage >= MAX_STAGES)
        return DDERR_INVALIDPARAMS;
    if (t)
        t->AddRef();
    if (textures[stage])
        textures[stage]->Release();
    textures[stage] = static_cast<Surface *>(t);
    return D3D_OK;
}

HRESULT Device::GetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE s, DWORD *v)
{
    if (stage >= MAX_STAGES || (unsigned)s >= TSS_COUNT || !v)
        return DDERR_INVALIDPARAMS;
    *v = tss[stage][s];
    return D3D_OK;
}

HRESULT Device::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE s, DWORD v)
{
    if (stage >= MAX_STAGES || (unsigned)s >= TSS_COUNT)
        return DDERR_INVALIDPARAMS;
    tss[stage][s] = v;
    if (stage == 0 && (s == D3DTSS_ALPHAOP || s == D3DTSS_ALPHAARG1 || s == D3DTSS_ALPHAARG2))
        legacyModulateAlpha = false;
    if (s == D3DTSS_ADDRESS)
        tss[stage][D3DTSS_ADDRESSU] = tss[stage][D3DTSS_ADDRESSV] = v;
    return D3D_OK;
}

HRESULT Device::ValidateDevice(DWORD *passes)
{
    if (passes)
        *passes = 1;
    return D3D_OK;
}

HRESULT Device::ApplyStateBlock(DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::ApplyStateBlock"); }
HRESULT Device::CaptureStateBlock(DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::CaptureStateBlock"); }
HRESULT Device::DeleteStateBlock(DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::DeleteStateBlock"); }
HRESULT Device::CreateStateBlock(D3DSTATEBLOCKTYPE, DWORD *) { DX_UNSUPPORTED("IDirect3DDevice7::CreateStateBlock"); }

static Surface *next_mip(Surface *s)
{
    for (Surface *a : s->attached)
        if ((a->desc.ddsCaps.dwCaps & (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP)) == (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP))
            return a;
    return 0;
}

// Copies a texture (with its mip levels when no rectangle is given) into another one.
HRESULT Device::Load(IDirectDrawSurface7 *idst, POINT *pt, IDirectDrawSurface7 *isrc, RECT *rect, DWORD)
{
    DX_GUARD;
    Surface *dst = static_cast<Surface *>(idst), *src = static_cast<Surface *>(isrc);
    if (!dst || !src)
        return DDERR_INVALIDPARAMS;
    if (src->palette && !dst->palette)
        dst->SetPalette(src->palette);
    else if (src->palette && dst->palette && dst->palette != src->palette) {
        memcpy(dst->palette->entries, src->palette->entries, sizeof(dst->palette->entries));
        dst->palette->version++;
    }
    if (src->hasSrcKey) {
        dst->srcKey = src->srcKey;
        dst->hasSrcKey = true;
    }
    // every mip level both have, the rectangle and point halved per level (the game
    // passes the whole top level's rectangle for its mipmapped textures)
    RECT r = rect ? *rect : RECT{0, 0, src->width(), src->height()};
    POINT p = pt ? *pt : POINT{0, 0};
    for (int level = 0; dst && src; level++, dst = next_mip(dst), src = next_mip(src)) {
        RECT s = {r.left >> level, r.top >> level, r.right >> level, r.bottom >> level};
        if (s.right <= s.left)
            s.right = s.left + 1;
        if (s.bottom <= s.top)
            s.bottom = s.top + 1;
        RECT d = {p.x >> level, p.y >> level, (p.x >> level) + (s.right - s.left), (p.y >> level) + (s.bottom - s.top)};
        HRESULT hr = dst->blit(d, src, s, false);
        if (hr != DD_OK)
            return hr;
    }
    return D3D_OK;
}

HRESULT Device::SetClipPlane(DWORD, D3DVALUE *) { DX_UNSUPPORTED("IDirect3DDevice7::SetClipPlane"); }
HRESULT Device::GetClipPlane(DWORD, D3DVALUE *) { DX_UNSUPPORTED("IDirect3DDevice7::GetClipPlane"); }
HRESULT Device::GetInfo(DWORD, void *, DWORD) { DX_UNSUPPORTED("IDirect3DDevice7::GetInfo"); }

} // namespace dx
