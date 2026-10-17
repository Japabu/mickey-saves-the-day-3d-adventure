// ddraw.cpp - the DirectDraw object (IDirectDraw7, with IDirectDraw and IDirect3D7 as
// its other interfaces), surface creation, and the DirectDraw entry points.
#include "dx.h"

#include <string.h>

#define DX_EXPORT extern "C"

// the interface identifiers of DirectX 7
DX_EXPORT const GUID IID_IDirectDraw = {0x6c14db80, 0xa733, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}};
DX_EXPORT const GUID IID_IDirectDraw7 = {0x15e65ec0, 0x3b9c, 0x11d2, {0xb9, 0x2f, 0x00, 0x60, 0x97, 0x97, 0xea, 0x5b}};
DX_EXPORT const GUID IID_IDirectDrawSurface7 = {0x06675a80, 0x3b9b, 0x11d2, {0xb9, 0x2f, 0x00, 0x60, 0x97, 0x97, 0xea, 0x5b}};
DX_EXPORT const GUID IID_IDirect3D7 = {0xf5049e77, 0x4861, 0x11d2, {0xa4, 0x07, 0x00, 0xa0, 0xc9, 0x06, 0x29, 0xa8}};
DX_EXPORT const GUID IID_IDirect3DRefDevice = {0x50936643, 0x13e9, 0x11d1, {0x89, 0xaa, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29}};
DX_EXPORT const GUID IID_IDirect3DHALDevice = {0x84e63de0, 0x46aa, 0x11cf, {0x81, 0x6f, 0x00, 0x00, 0xc0, 0x20, 0x15, 0x6e}};
DX_EXPORT const GUID IID_IDirect3DRGBDevice = {0xa4665c60, 0x2673, 0x11cf, {0xa3, 0x1a, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56}};
DX_EXPORT const GUID IID_IDirect3DTnLHalDevice = {0xf5049e78, 0x4861, 0x11d2, {0xa4, 0x07, 0x00, 0xa0, 0xc9, 0x06, 0x29, 0xa8}};

namespace dx {

static const DWORD VIDEO_MEMORY = 64u << 20;

// The display modes offered; the desktop's size is added in front.
static const int MODE_SIZES[][2] = {{640, 480}, {800, 600}, {1024, 768}};
static const int MODE_DEPTHS[] = {8, 16, 32};

static void mode_desc(DDSURFACEDESC2 *d, int w, int h, int bpp)
{
    DWORD size = d->dwSize;
    memset(d, 0, sizeof(*d));
    d->dwSize = size ? size : sizeof(*d);
    d->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_REFRESHRATE;
    d->dwWidth = w;
    d->dwHeight = h;
    d->lPitch = w * (bpp / 8);
    d->dwRefreshRate = 60;
    d->ddpfPixelFormat = display_format(bpp);
}

// The most recently created DirectDraw object: its display mode is the display's.
static DirectDraw *s_current;

DirectDraw::DirectDraw()
{
    s_current = this;
    d3d.dd = this;
    dd1.dd = this;
    modeW = GetSystemMetrics(SM_CXSCREEN);
    modeH = GetSystemMetrics(SM_CYSCREEN);
    if (modeW <= 0 || modeH <= 0)
        modeW = 1024, modeH = 768;
    modeBpp = 32;
}

DirectDraw::~DirectDraw()
{
    if (s_current == this)
        s_current = 0;
}

HRESULT DirectDraw::QueryInterface(REFIID iid, void **out)
{
    if (iid == IID_IUnknown || iid == IID_IDirectDraw7)
        *out = static_cast<IDirectDraw7 *>(this);
    else if (iid == IID_IDirectDraw)
        *out = &dd1;
    else if (iid == IID_IDirect3D7)
        *out = &d3d;
    else {
        *out = 0;
        return E_NOINTERFACE;
    }
    AddRef();
    return DD_OK;
}

HRESULT DirectDraw::Compact() { return DD_OK; }

HRESULT DirectDraw::CreateClipper(DWORD, IDirectDrawClipper **out, IUnknown *)
{
    *out = new Clipper;
    return DD_OK;
}

HRESULT DirectDraw::CreatePalette(DWORD caps, PALETTEENTRY *entries, IDirectDrawPalette **out, IUnknown *)
{
    Palette *p = new Palette;
    p->caps = caps;
    memset(p->entries, 0, sizeof(p->entries));
    int n = (caps & DDPCAPS_4BIT) ? 16 : (caps & DDPCAPS_8BIT) ? 256 : 0;
    if (!n) {
        p->Release();
        *out = 0;
        return DDERR_INVALIDPARAMS;
    }
    if (entries)
        memcpy(p->entries, entries, n * sizeof(PALETTEENTRY));
    *out = p;
    return DD_OK;
}

// Memory placement: everything is in memory; a surface reports where it was asked to be.
static DWORD placement(DWORD caps)
{
    if (!(caps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_VIDEOMEMORY)))
        caps |= DDSCAPS_VIDEOMEMORY;
    if ((caps & DDSCAPS_VIDEOMEMORY) && !(caps & (DDSCAPS_LOCALVIDMEM | DDSCAPS_NONLOCALVIDMEM)))
        caps |= DDSCAPS_LOCALVIDMEM;
    return caps;
}

// Pixel formats a surface can have: 8-bit palettized, RGB(A) with 16, 24 or 32 bits (an
// RGB format without masks gets the display's masks for its depth), z-buffers of 16 or
// 32 bits.
static bool valid_format(DDPIXELFORMAT &pf)
{
    DWORD bits = pf.dwRGBBitCount;
    if (pf.dwFlags & DDPF_ZBUFFER)
        return bits == 16 || bits == 32;
    if (pf.dwFlags & DDPF_PALETTEINDEXED8)
        return bits == 8;
    if (!(pf.dwFlags & DDPF_RGB) || (bits != 16 && bits != 24 && bits != 32))
        return false;
    if (!pf.dwRBitMask && !pf.dwGBitMask && !pf.dwBBitMask) {
        DDPIXELFORMAT d = display_format((int)bits);
        pf.dwRBitMask = d.dwRBitMask;
        pf.dwGBitMask = d.dwGBitMask;
        pf.dwBBitMask = d.dwBBitMask;
    }
    return true;
}

HRESULT DirectDraw::CreateSurface(DDSURFACEDESC2 *in, IDirectDrawSurface7 **out, IUnknown *)
{
    if (!in || !out)
        return DDERR_INVALIDPARAMS;
    *out = 0;
    DDSURFACEDESC2 d = *in;
    d.dwSize = sizeof(d);
    DWORD caps = (d.dwFlags & DDSD_CAPS) ? d.ddsCaps.dwCaps : DDSCAPS_OFFSCREENPLAIN;
    if (caps & DDSCAPS_PRIMARYSURFACE) {
        if (primary)
            return DDERR_INVALIDPARAMS;
        d.dwWidth = modeW;
        d.dwHeight = modeH;
        d.ddpfPixelFormat = display_format(modeBpp);
        d.dwFlags |= DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;
        caps |= DDSCAPS_VISIBLE;
        if (caps & DDSCAPS_FLIP)
            caps |= DDSCAPS_FRONTBUFFER;
        d.ddsCaps.dwCaps = placement(caps & ~DDSCAPS_SYSTEMMEMORY);
        Surface *front = new Surface(this, d);
        primary = front;
        int backs = (caps & DDSCAPS_FLIP) && (d.dwFlags & DDSD_BACKBUFFERCOUNT) ? (int)d.dwBackBufferCount : 0;
        for (int i = 0; i < backs; i++) {
            DDSURFACEDESC2 b = d;
            b.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
            b.dwBackBufferCount = 0;
            b.ddsCaps.dwCaps = (d.ddsCaps.dwCaps & ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER | DDSCAPS_VISIBLE))
                | (i == 0 ? DDSCAPS_BACKBUFFER : 0);
            Surface *s = new Surface(this, b);
            s->owner = front;
            front->chain.push_back(s);
            if (i == 0)
                front->attached.push_back(s); // the front holds the chain's one reference
            else {
                front->chain[i - 1]->attached.push_back(s);
            }
        }
        *out = front;
        return DD_OK;
    }
    if (!(d.dwFlags & DDSD_WIDTH) || !(d.dwFlags & DDSD_HEIGHT) || !d.dwWidth || !d.dwHeight)
        return DDERR_INVALIDPARAMS;
    if (!(d.dwFlags & DDSD_PIXELFORMAT)) {
        if (caps & DDSCAPS_ZBUFFER)
            return DDERR_INVALIDPARAMS;
        d.ddpfPixelFormat = display_format(modeBpp);
        d.dwFlags |= DDSD_PIXELFORMAT;
    }
    if (!valid_format(d.ddpfPixelFormat)) {
        DX_LOG("CreateSurface: unsupported pixel format (flags %#x, %u bits)\n", (unsigned)d.ddpfPixelFormat.dwFlags,
               (unsigned)d.ddpfPixelFormat.dwRGBBitCount);
        return DDERR_INVALIDPIXELFORMAT;
    }
    d.dwFlags |= DDSD_CAPS;
    d.ddsCaps.dwCaps = placement(caps);
    if ((caps & DDSCAPS_MIPMAP) && (caps & DDSCAPS_COMPLEX)) {
        int levels = 1;
        while ((d.dwWidth >> levels) || (d.dwHeight >> levels))
            levels++;
        if ((d.dwFlags & DDSD_MIPMAPCOUNT) && d.dwMipMapCount && (int)d.dwMipMapCount < levels)
            levels = (int)d.dwMipMapCount;
        d.dwFlags |= DDSD_MIPMAPCOUNT;
        d.dwMipMapCount = levels;
        Surface *top = new Surface(this, d), *prev = top;
        for (int i = 1; i < levels; i++) {
            DDSURFACEDESC2 m = d;
            m.dwWidth = d.dwWidth >> i ? d.dwWidth >> i : 1;
            m.dwHeight = d.dwHeight >> i ? d.dwHeight >> i : 1;
            m.dwMipMapCount = levels - i;
            m.ddsCaps.dwCaps2 |= 0x00010000; // DDSCAPS2_MIPMAPSUBLEVEL
            Surface *s = new Surface(this, m);
            s->owner = top;
            prev->attached.push_back(s);
            prev = s;
        }
        *out = top;
        return DD_OK;
    }
    *out = new Surface(this, d);
    return DD_OK;
}

HRESULT DirectDraw::DuplicateSurface(IDirectDrawSurface7 *, IDirectDrawSurface7 **) { DX_UNSUPPORTED("IDirectDraw7::DuplicateSurface"); }

HRESULT DirectDraw::EnumDisplayModes(DWORD, DDSURFACEDESC2 *filter, void *ctx, HRESULT(__stdcall *cb)(DDSURFACEDESC2 *, void *))
{
    if (!cb)
        return DDERR_INVALIDPARAMS;
    std::vector<std::pair<int, int>> sizes;
    for (auto &s : MODE_SIZES)
        sizes.push_back({s[0], s[1]});
    int dw = GetSystemMetrics(SM_CXSCREEN), dh = GetSystemMetrics(SM_CYSCREEN);
    if (dw > 0 && dh > 0) {
        bool listed = false;
        for (auto &s : sizes)
            listed |= s.first == dw && s.second == dh;
        if (!listed)
            sizes.push_back({dw, dh});
    }
    for (auto &s : sizes) {
        for (int bpp : MODE_DEPTHS) {
            if (filter) {
                if ((filter->dwFlags & DDSD_WIDTH) && filter->dwWidth != (DWORD)s.first)
                    continue;
                if ((filter->dwFlags & DDSD_HEIGHT) && filter->dwHeight != (DWORD)s.second)
                    continue;
                if ((filter->dwFlags & DDSD_PIXELFORMAT) && filter->ddpfPixelFormat.dwRGBBitCount != (DWORD)bpp)
                    continue;
            }
            DDSURFACEDESC2 d;
            d.dwSize = sizeof(d);
            mode_desc(&d, s.first, s.second, bpp);
            if (cb(&d, ctx) == DDENUMRET_CANCEL)
                return DD_OK;
        }
    }
    return DD_OK;
}

HRESULT DirectDraw::EnumSurfaces(DWORD, DDSURFACEDESC2 *, void *, LPDDENUMSURFACESCALLBACK7) { DX_UNSUPPORTED("IDirectDraw7::EnumSurfaces"); }
HRESULT DirectDraw::FlipToGDISurface() { return DD_OK; }

static void fill_caps(_DDCAPS_DX7 *c, bool hal)
{
    if (!c)
        return;
    memset(c, 0, sizeof(*c));
    c->dwSize = sizeof(*c);
    c->dwCaps = DDCAPS_COLORKEY | (hal ? DDCAPS_3D : 0);
    c->dwCaps2 = DDCAPS2_CANRENDERWINDOWED;
    c->dwCKeyCaps = 0x00000200; // DDCKEYCAPS_SRCBLT
    c->dwPalCaps = DDPCAPS_8BIT;
    c->dwZBufferBitDepths = DDBD_16 | DDBD_32;
    c->dwVidMemTotal = hal ? VIDEO_MEMORY : 0;
    c->dwVidMemFree = hal ? VIDEO_MEMORY : 0;
    c->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_FRONTBUFFER | DDSCAPS_OFFSCREENPLAIN
        | DDSCAPS_PALETTE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE | DDSCAPS_3DDEVICE
        | DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER | DDSCAPS_MIPMAP | DDSCAPS_LOCALVIDMEM;
}

HRESULT DirectDraw::GetCaps(_DDCAPS_DX7 *hal, _DDCAPS_DX7 *hel)
{
    fill_caps(hal, true);
    fill_caps(hel, false);
    return DD_OK;
}

HRESULT DirectDraw::GetDisplayMode(DDSURFACEDESC2 *d)
{
    mode_desc(d, modeW, modeH, modeBpp);
    return DD_OK;
}

HRESULT DirectDraw::GetFourCCCodes(DWORD *n, DWORD *)
{
    if (n)
        *n = 0;
    return DD_OK;
}

HRESULT DirectDraw::GetGDISurface(IDirectDrawSurface7 **out)
{
    *out = primary;
    if (!primary)
        return DDERR_NOTFOUND;
    primary->AddRef();
    return DD_OK;
}

HRESULT DirectDraw::GetMonitorFrequency(DWORD *f)
{
    *f = 60;
    return DD_OK;
}

HRESULT DirectDraw::GetScanLine(DWORD *) { DX_UNSUPPORTED("IDirectDraw7::GetScanLine"); }

HRESULT DirectDraw::GetVerticalBlankStatus(BOOL *b)
{
    *b = TRUE;
    return DD_OK;
}

HRESULT DirectDraw::Initialize(GUID *) { return DD_OK; }

HRESULT DirectDraw::RestoreDisplayMode()
{
    modeW = GetSystemMetrics(SM_CXSCREEN);
    modeH = GetSystemMetrics(SM_CYSCREEN);
    if (modeW <= 0 || modeH <= 0)
        modeW = 1024, modeH = 768;
    modeBpp = 32;
    return DD_OK;
}

HRESULT DirectDraw::SetCooperativeLevel(HWND h, DWORD flags)
{
    hwnd = h;
    coopLevel = flags;
    if ((flags & (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)) == (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE))
        port_set_fullscreen(1);
    else if (flags & DDSCL_NORMAL)
        port_set_fullscreen(0);
    return DD_OK;
}

HRESULT DirectDraw::SetDisplayMode(DWORD w, DWORD h, DWORD bpp, DWORD, DWORD)
{
    if (!w || !h || (bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32))
        return DDERR_INVALIDPARAMS;
    modeW = (int)w;
    modeH = (int)h;
    modeBpp = (int)bpp;
    return DD_OK;
}

HRESULT DirectDraw::WaitForVerticalBlank(DWORD, HANDLE)
{
    port_wait_vblank();
    return DD_OK;
}

HRESULT DirectDraw::GetAvailableVidMem(DDSCAPS2 *, DWORD *total, DWORD *free)
{
    if (total)
        *total = VIDEO_MEMORY;
    if (free)
        *free = VIDEO_MEMORY;
    return DD_OK;
}

HRESULT DirectDraw::GetSurfaceFromDC(HDC, IDirectDrawSurface7 **) { DX_UNSUPPORTED("IDirectDraw7::GetSurfaceFromDC"); }
HRESULT DirectDraw::RestoreAllSurfaces() { return DD_OK; }
HRESULT DirectDraw::TestCooperativeLevel() { return DD_OK; }

HRESULT DirectDraw::GetDeviceIdentifier(tagDDDEVICEIDENTIFIER2 *id, DWORD)
{
    if (!id)
        return DDERR_INVALIDPARAMS;
    memset(id, 0, sizeof(*id));
    strcpy(id->szDriver, "display");
    strcpy(id->szDescription, "Software DirectDraw (port)");
    return DD_OK;
}

HRESULT DirectDraw::StartModeTest(SIZE *, DWORD, DWORD) { DX_UNSUPPORTED("IDirectDraw7::StartModeTest"); }
HRESULT DirectDraw::EvaluateMode(DWORD, DWORD *) { DX_UNSUPPORTED("IDirectDraw7::EvaluateMode"); }

// ---- the DirectDraw 1 interface: only a way to IDirectDraw7

HRESULT DirectDraw1::QueryInterface(REFIID iid, void **out) { return dd->QueryInterface(iid, out); }
ULONG DirectDraw1::AddRef() { return dd->AddRef(); }
ULONG DirectDraw1::Release() { return dd->Release(); }
HRESULT DirectDraw1::Compact() { return DD_OK; }
HRESULT DirectDraw1::CreateClipper(DWORD f, IDirectDrawClipper **out, IUnknown *u) { return dd->CreateClipper(f, out, u); }
HRESULT DirectDraw1::CreatePalette(DWORD f, PALETTEENTRY *e, IDirectDrawPalette **out, IUnknown *u) { return dd->CreatePalette(f, e, out, u); }
HRESULT DirectDraw1::CreateSurface(struct _DDSURFACEDESC *, IDirectDrawSurface **, IUnknown *) { DX_UNSUPPORTED("IDirectDraw::CreateSurface"); }
HRESULT DirectDraw1::DuplicateSurface(IDirectDrawSurface *, IDirectDrawSurface **) { DX_UNSUPPORTED("IDirectDraw::DuplicateSurface"); }
HRESULT DirectDraw1::EnumDisplayModes(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMMODESCALLBACK) { DX_UNSUPPORTED("IDirectDraw::EnumDisplayModes"); }
HRESULT DirectDraw1::EnumSurfaces(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMSURFACESCALLBACK) { DX_UNSUPPORTED("IDirectDraw::EnumSurfaces"); }
HRESULT DirectDraw1::FlipToGDISurface() { return DD_OK; }
HRESULT DirectDraw1::GetCaps(LPDDCAPS, LPDDCAPS) { DX_UNSUPPORTED("IDirectDraw::GetCaps"); }
HRESULT DirectDraw1::GetDisplayMode(struct _DDSURFACEDESC *) { DX_UNSUPPORTED("IDirectDraw::GetDisplayMode"); }
HRESULT DirectDraw1::GetFourCCCodes(DWORD *n, DWORD *c) { return dd->GetFourCCCodes(n, c); }
HRESULT DirectDraw1::GetGDISurface(IDirectDrawSurface **) { DX_UNSUPPORTED("IDirectDraw::GetGDISurface"); }
HRESULT DirectDraw1::GetMonitorFrequency(DWORD *f) { return dd->GetMonitorFrequency(f); }
HRESULT DirectDraw1::GetScanLine(DWORD *l) { return dd->GetScanLine(l); }
HRESULT DirectDraw1::GetVerticalBlankStatus(BOOL *b) { return dd->GetVerticalBlankStatus(b); }
HRESULT DirectDraw1::Initialize(GUID *g) { return dd->Initialize(g); }
HRESULT DirectDraw1::RestoreDisplayMode() { return dd->RestoreDisplayMode(); }
HRESULT DirectDraw1::SetCooperativeLevel(HWND h, DWORD f) { return dd->SetCooperativeLevel(h, f); }
HRESULT DirectDraw1::SetDisplayMode(DWORD w, DWORD h, DWORD bpp) { return dd->SetDisplayMode(w, h, bpp, 0, 0); }
HRESULT DirectDraw1::WaitForVerticalBlank(DWORD f, HANDLE e) { return dd->WaitForVerticalBlank(f, e); }

} // namespace dx

// ---- entry points (ddraw.dll)

// The display's bits per pixel for GDI (GetDeviceCaps(BITSPIXEL)): the DirectDraw
// mode, or the desktop's 32 while no DirectDraw object exists.
DX_EXPORT int port_display_bpp(void)
{
    return dx::s_current ? dx::s_current->modeBpp : 32;
}

static const char DRIVER_DESC[] = "Primary Display Driver";
static const char DRIVER_NAME[] = "display";

DX_EXPORT HRESULT __stdcall DirectDrawCreate(GUID *, LPDIRECTDRAW *out, IUnknown *)
{
    if (!out)
        return DDERR_INVALIDPARAMS;
    dx::DirectDraw *dd = new dx::DirectDraw;
    *out = &dd->dd1;
    return DD_OK;
}

DX_EXPORT HRESULT __stdcall DirectDrawCreateEx(GUID *, void **out, REFIID iid, IUnknown *)
{
    if (!out)
        return DDERR_INVALIDPARAMS;
    *out = 0;
    if (iid != IID_IDirectDraw7)
        return DDERR_INVALIDPARAMS;
    *out = static_cast<IDirectDraw7 *>(new dx::DirectDraw);
    return DD_OK;
}

DX_EXPORT HRESULT __stdcall DirectDrawEnumerateA(LPDDENUMCALLBACKA cb, LPVOID ctx)
{
    if (cb)
        cb(0, (LPSTR)DRIVER_DESC, (LPSTR)DRIVER_NAME, ctx);
    return DD_OK;
}

DX_EXPORT HRESULT __stdcall DirectDrawEnumerateExA(BOOL(__stdcall *cb)(GUID *, LPSTR, LPSTR, LPVOID, HMONITOR), LPVOID ctx, DWORD)
{
    if (cb)
        cb(0, (LPSTR)DRIVER_DESC, (LPSTR)DRIVER_NAME, ctx, 0);
    return DD_OK;
}
