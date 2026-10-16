// dx.h - the software DirectDraw 7 / Direct3D 7 of the macOS port: the objects behind
// the game's IDirectDraw7, IDirectDrawSurface7, IDirectDrawPalette, IDirectDrawClipper,
// IDirect3D7 and IDirect3DDevice7 pointers.
//
// The interfaces are the game's own declarations (SteamboatApp/d3d7.h; MBEngine's
// CMBDirect3DScreen.h and CMBDirectDrawSurface.h declare prefixes of the same vtables).
// Every surface lives in ordinary memory; the primary surface is handed to the window
// layer (port_present) when it changes. Methods the game never calls return
// DDERR_UNSUPPORTED and log once on the "dx" channel (PORT_LOG=dx).
#ifndef PORT_DX_H
#define PORT_DX_H

#include "../../../src/SteamboatApp/d3d7.h"
#include "../port.h"
#include "../win32/internal.h"

#include <stdint.h>
#include <atomic>
#include <mutex>
#include <vector>

// Interface identifiers the game's headers do not declare (defined in ddraw.cpp).
extern "C" const GUID IID_IDirectDraw;
extern "C" const GUID IID_IDirectDrawSurface7;
extern "C" const GUID IID_IDirect3DHALDevice;
extern "C" const GUID IID_IDirect3DTnLHalDevice;
extern "C" const GUID IID_IDirect3DRGBDevice;

namespace dx {

#define DX_LOG(...) win32::log("dx", __VA_ARGS__)
// Logs the first call of a method that is not implemented.
#define DX_UNSUPPORTED(name)                                         \
    do {                                                             \
        static bool once;                                            \
        if (!once) {                                                 \
            once = true;                                             \
            win32::log("dx", "%s is not implemented\n", name);       \
        }                                                            \
        return DDERR_UNSUPPORTED;                                    \
    } while (0)

// One lock for the whole layer (DDSCL_MULTITHREADED): the game's cursor thread blits
// to the front buffer while the main thread renders and flips.
std::recursive_mutex &layer_lock();
#define DX_GUARD std::lock_guard<std::recursive_mutex> dx_guard_(::dx::layer_lock())

// ---- pixel formats

// A DDPIXELFORMAT reduced to what the converters need: bytes per pixel and, for each
// RGB(A) channel, its shift and width.
struct PixFmt {
    int bytes;      // 1, 2, 3 or 4
    bool palette;   // 8-bit palettized
    bool zbuffer;
    uint32_t mask[4]; // a, r, g, b
    int shift[4], bits[4];
    void set(const DDPIXELFORMAT &pf);
};

uint32_t read_pixel(const uint8_t *p, int bytes);
void write_pixel(uint8_t *p, int bytes, uint32_t v);
// Raw pixel <-> 0xAARRGGBB. Formats without alpha read as opaque.
uint32_t to_argb(const PixFmt &f, uint32_t raw, const PALETTEENTRY *palette);
uint32_t from_argb(const PixFmt &f, uint32_t argb, const PALETTEENTRY *palette);

DDPIXELFORMAT rgb_format(int bpp, uint32_t r, uint32_t g, uint32_t b, uint32_t a);
DDPIXELFORMAT display_format(int bpp);
DDPIXELFORMAT z_format(int bits);

// ---- objects

struct DirectDraw;
struct Surface;
struct TexCache;

// Reference counting shared by every object.
template <class I> struct Object : I {
    std::atomic<int> refs{1};
    virtual ~Object() {}
    ULONG __stdcall AddRef() override { return ++refs; }
    ULONG __stdcall Release() override
    {
        int n = --refs;
        if (n == 0)
            delete this;
        return n;
    }
};

struct Palette : Object<IDirectDrawPalette> {
    PALETTEENTRY entries[256];
    DWORD caps;
    unsigned version = 0; // bumped by SetEntries
    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    HRESULT __stdcall GetCaps(DWORD *) override;
    HRESULT __stdcall GetEntries(DWORD, DWORD, DWORD, PALETTEENTRY *) override;
    HRESULT __stdcall Initialize(IDirectDraw *, DWORD, PALETTEENTRY *) override;
    HRESULT __stdcall SetEntries(DWORD, DWORD, DWORD, PALETTEENTRY *) override;
};

struct Clipper : Object<IDirectDrawClipper> {
    HWND hwnd = 0;
    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    HRESULT __stdcall GetClipList(RECT *, struct _RGNDATA *, DWORD *) override;
    HRESULT __stdcall GetHWnd(HWND *) override;
    HRESULT __stdcall Initialize(IDirectDraw *, DWORD) override;
    HRESULT __stdcall IsClipListChanged(BOOL *) override;
    HRESULT __stdcall SetClipList(struct _RGNDATA *, DWORD) override;
    HRESULT __stdcall SetHWnd(DWORD, HWND) override;
};

struct Surface : Object<IDirectDrawSurface7> {
    DirectDraw *dd;            // holds a reference
    DDSURFACEDESC2 desc;       // size, caps and pixel format; lpSurface is 0
    PixFmt fmt;
    uint8_t *bits = 0;         // swapped between the buffers of a flip chain by Flip
    int pitch = 0;
    std::vector<Surface *> attached; // each holds a reference
    std::vector<Surface *> chain;    // complex surface: the members it created (back buffers, mip levels)
    Surface *owner = 0;              // a member of another surface's chain
    Palette *palette = 0;
    Clipper *clipper = 0;
    DDCOLORKEY srcKey{}, destKey{};
    bool hasSrcKey = false, hasDestKey = false;
    int locks = 0;
    HDC dc = 0;
    uint8_t *dcShadow = 0;     // 32-bit copy behind the DC for formats GDI cannot draw into
    unsigned version = 0;      // bumped on every change of the pixels (texture caches)
    TexCache *texCache = 0;    // the converted copy for sampling (render.h)

    Surface(DirectDraw *dd, const DDSURFACEDESC2 &d);
    ~Surface();
    bool primary() const { return (desc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) != 0; }
    int width() const { return (int)desc.dwWidth; }
    int height() const { return (int)desc.dwHeight; }
    uint8_t *row(int y) { return bits + (size_t)y * pitch; }
    const PALETTEENTRY *pal() const { return palette ? palette->entries : 0; }
    // The pixels changed: drop cached copies and show the primary.
    void changed();
    // Copies a rectangle of src (stretched, colour keyed, converted) into this surface.
    HRESULT blit(const RECT &dst, Surface *src, const RECT &srcRect, bool key);
    HRESULT fill(const RECT *dst, uint32_t raw);

    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    // IDirectDrawSurface7
    HRESULT __stdcall AddAttachedSurface(IDirectDrawSurface7 *) override;
    HRESULT __stdcall AddOverlayDirtyRect(RECT *) override;
    HRESULT __stdcall Blt(RECT *, IDirectDrawSurface7 *, RECT *, DWORD, DDBLTFX *) override;
    HRESULT __stdcall BltBatch(DDBLTBATCH *, DWORD, DWORD) override;
    HRESULT __stdcall BltFast(DWORD, DWORD, IDirectDrawSurface7 *, RECT *, DWORD) override;
    HRESULT __stdcall DeleteAttachedSurface(DWORD, IDirectDrawSurface7 *) override;
    HRESULT __stdcall EnumAttachedSurfaces(void *, LPDDENUMSURFACESCALLBACK7) override;
    HRESULT __stdcall EnumOverlayZOrders(DWORD, void *, LPDDENUMSURFACESCALLBACK7) override;
    HRESULT __stdcall Flip(IDirectDrawSurface7 *, DWORD) override;
    HRESULT __stdcall GetAttachedSurface(DDSCAPS2 *, IDirectDrawSurface7 **) override;
    HRESULT __stdcall GetBltStatus(DWORD) override;
    HRESULT __stdcall GetCaps(DDSCAPS2 *) override;
    HRESULT __stdcall GetClipper(IDirectDrawClipper **) override;
    HRESULT __stdcall GetColorKey(DWORD, DDCOLORKEY *) override;
    HRESULT __stdcall GetDC(HDC *) override;
    HRESULT __stdcall GetFlipStatus(DWORD) override;
    HRESULT __stdcall GetOverlayPosition(LONG *, LONG *) override;
    HRESULT __stdcall GetPalette(IDirectDrawPalette **) override;
    HRESULT __stdcall GetPixelFormat(DDPIXELFORMAT *) override;
    HRESULT __stdcall GetSurfaceDesc(DDSURFACEDESC2 *) override;
    HRESULT __stdcall Initialize(IDirectDraw *, DDSURFACEDESC2 *) override;
    HRESULT __stdcall IsLost() override;
    HRESULT __stdcall Lock(RECT *, DDSURFACEDESC2 *, DWORD, HANDLE) override;
    HRESULT __stdcall ReleaseDC(HDC) override;
    HRESULT __stdcall Restore() override;
    HRESULT __stdcall SetClipper(IDirectDrawClipper *) override;
    HRESULT __stdcall SetColorKey(DWORD, DDCOLORKEY *) override;
    HRESULT __stdcall SetOverlayPosition(LONG, LONG) override;
    HRESULT __stdcall SetPalette(IDirectDrawPalette *) override;
    HRESULT __stdcall Unlock(RECT *) override;
    HRESULT __stdcall UpdateOverlay(RECT *, IDirectDrawSurface7 *, RECT *, DWORD, DDOVERLAYFX *) override;
    HRESULT __stdcall UpdateOverlayDisplay(DWORD) override;
    HRESULT __stdcall UpdateOverlayZOrder(DWORD, IDirectDrawSurface7 *) override;
    HRESULT __stdcall GetDDInterface(void **) override;
    HRESULT __stdcall PageLock(DWORD) override;
    HRESULT __stdcall PageUnlock(DWORD) override;
    HRESULT __stdcall SetSurfaceDesc(DDSURFACEDESC2 *, DWORD) override;
    HRESULT __stdcall SetPrivateData(REFGUID, void *, DWORD, DWORD) override;
    HRESULT __stdcall GetPrivateData(REFGUID, void *, DWORD *) override;
    HRESULT __stdcall FreePrivateData(REFGUID) override;
    HRESULT __stdcall GetUniquenessValue(DWORD *) override;
    HRESULT __stdcall ChangeUniquenessValue() override;
    HRESULT __stdcall SetPriority(DWORD) override;
    HRESULT __stdcall GetPriority(DWORD *) override;
    HRESULT __stdcall SetLOD(DWORD) override;
    HRESULT __stdcall GetLOD(DWORD *) override;
};

// Hands the primary surface (or the part of it under the clipper's window) to the
// window layer.
void present(Surface *primary);

// The Direct3D 7 interface of a DirectDraw object (QueryInterface(IID_IDirect3D7)).
struct Direct3D : IDirect3D7 {
    DirectDraw *dd;
    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall EnumDevices(LPD3DENUMDEVICESCALLBACK7, void *) override;
    HRESULT __stdcall CreateDevice(const GUID &, IDirectDrawSurface7 *, IDirect3DDevice7 **) override;
    HRESULT __stdcall CreateVertexBuffer(struct _D3DVERTEXBUFFERDESC *, IDirect3DVertexBuffer7 **, DWORD) override;
    HRESULT __stdcall EnumZBufferFormats(const GUID &, LPD3DENUMPIXELFORMATSCALLBACK, void *) override;
    HRESULT __stdcall EvictManagedTextures() override;
};

// The DirectDraw 1 interface of a DirectDraw object (DirectDrawCreate); the game only
// asks it for IDirectDraw7.
struct DirectDraw1 : IDirectDraw {
    DirectDraw *dd;
    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall Compact() override;
    HRESULT __stdcall CreateClipper(DWORD, IDirectDrawClipper **, IUnknown *) override;
    HRESULT __stdcall CreatePalette(DWORD, PALETTEENTRY *, IDirectDrawPalette **, IUnknown *) override;
    HRESULT __stdcall CreateSurface(struct _DDSURFACEDESC *, IDirectDrawSurface **, IUnknown *) override;
    HRESULT __stdcall DuplicateSurface(IDirectDrawSurface *, IDirectDrawSurface **) override;
    HRESULT __stdcall EnumDisplayModes(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMMODESCALLBACK) override;
    HRESULT __stdcall EnumSurfaces(DWORD, struct _DDSURFACEDESC *, LPVOID, LPDDENUMSURFACESCALLBACK) override;
    HRESULT __stdcall FlipToGDISurface() override;
    HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS) override;
    HRESULT __stdcall GetDisplayMode(struct _DDSURFACEDESC *) override;
    HRESULT __stdcall GetFourCCCodes(DWORD *, DWORD *) override;
    HRESULT __stdcall GetGDISurface(IDirectDrawSurface **) override;
    HRESULT __stdcall GetMonitorFrequency(DWORD *) override;
    HRESULT __stdcall GetScanLine(DWORD *) override;
    HRESULT __stdcall GetVerticalBlankStatus(BOOL *) override;
    HRESULT __stdcall Initialize(GUID *) override;
    HRESULT __stdcall RestoreDisplayMode() override;
    HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) override;
    HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD) override;
    HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE) override;
};

struct DirectDraw : Object<IDirectDraw7> {
    Direct3D d3d;
    DirectDraw1 dd1;
    HWND hwnd = 0;
    DWORD coopLevel = 0;
    int modeW, modeH, modeBpp; // the display mode (the desktop's until SetDisplayMode)
    Surface *primary = 0;      // not a reference; cleared by the primary's destructor

    DirectDraw();
    ~DirectDraw();
    bool fullscreen() const { return (coopLevel & DDSCL_FULLSCREEN) != 0; }

    HRESULT __stdcall QueryInterface(REFIID, void **) override;
    HRESULT __stdcall Compact() override;
    HRESULT __stdcall CreateClipper(DWORD, IDirectDrawClipper **, IUnknown *) override;
    HRESULT __stdcall CreatePalette(DWORD, PALETTEENTRY *, IDirectDrawPalette **, IUnknown *) override;
    HRESULT __stdcall CreateSurface(DDSURFACEDESC2 *, IDirectDrawSurface7 **, IUnknown *) override;
    HRESULT __stdcall DuplicateSurface(IDirectDrawSurface7 *, IDirectDrawSurface7 **) override;
    HRESULT __stdcall EnumDisplayModes(DWORD, DDSURFACEDESC2 *, void *, HRESULT(__stdcall *)(DDSURFACEDESC2 *, void *)) override;
    HRESULT __stdcall EnumSurfaces(DWORD, DDSURFACEDESC2 *, void *, LPDDENUMSURFACESCALLBACK7) override;
    HRESULT __stdcall FlipToGDISurface() override;
    HRESULT __stdcall GetCaps(_DDCAPS_DX7 *, _DDCAPS_DX7 *) override;
    HRESULT __stdcall GetDisplayMode(DDSURFACEDESC2 *) override;
    HRESULT __stdcall GetFourCCCodes(DWORD *, DWORD *) override;
    HRESULT __stdcall GetGDISurface(IDirectDrawSurface7 **) override;
    HRESULT __stdcall GetMonitorFrequency(DWORD *) override;
    HRESULT __stdcall GetScanLine(DWORD *) override;
    HRESULT __stdcall GetVerticalBlankStatus(BOOL *) override;
    HRESULT __stdcall Initialize(GUID *) override;
    HRESULT __stdcall RestoreDisplayMode() override;
    HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) override;
    HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD) override;
    HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE) override;
    HRESULT __stdcall GetAvailableVidMem(DDSCAPS2 *, DWORD *, DWORD *) override;
    HRESULT __stdcall GetSurfaceFromDC(HDC, IDirectDrawSurface7 **) override;
    HRESULT __stdcall RestoreAllSurfaces() override;
    HRESULT __stdcall TestCooperativeLevel() override;
    HRESULT __stdcall GetDeviceIdentifier(tagDDDEVICEIDENTIFIER2 *, DWORD) override;
    HRESULT __stdcall StartModeTest(SIZE *, DWORD, DWORD) override;
    HRESULT __stdcall EvaluateMode(DWORD, DWORD *) override;
};

// Creates the Direct3D device that renders into target (d3d_device.cpp).
HRESULT create_device(DirectDraw *dd, Surface *target, bool hardware, IDirect3DDevice7 **out);
// The devices the enumeration reports: the HAL one and the RGB (software) one.
const D3DDEVICEDESC7 &device_desc(bool hardware);

} // namespace dx

#endif
