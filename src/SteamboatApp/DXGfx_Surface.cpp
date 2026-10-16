#include "DXGfx_Surface.h"
#include "DX_Exception.h"
#include "MBNew.h"
#include "ddutil.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\DXGraphics\\Surface.cpp"

// 0x100da9c0 ?GetTypeInfo@Surface@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::Surface::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100da9d0 ?GetTypeName@Surface@DXGfx@@UBEPBDXZ
char const * DXGfx::Surface::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100da9e0 ??0Surface@DXGfx@@QAE@PAVDirectDraw@DX@@@Z
DXGfx::Surface::Surface(DX::DirectDraw *directDraw) : mbLocked(false), mbColorKey(false)
{
    m_pDirectDrawSurface = new DX::DirectDrawSurface(directDraw);
}

// 0x100daaa0 ?Create@Surface@DXGfx@@QAEXXZ
void DXGfx::Surface::Create()
{
    DDSURFACEDESC2 ddsd;
    ddsd.dwWidth = 640;
    ddsd.dwHeight = 480;
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_16;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwSize = sizeof(ddsd);
    m_pDirectDrawSurface->Create(ddsd);
}

// 0x100dab00 ?Create@Surface@DXGfx@@QAEXABUSurfaceDesc@Gfx@@_N@Z
void DXGfx::Surface::Create(const Gfx::SurfaceDesc &desc, bool primary)
{
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    if (!primary) {
        if (desc.hasColorKey)
            mbColorKey = desc.hasColorKey;
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.dwWidth = desc.width;
        ddsd.dwHeight = desc.height;
        if (desc.depth) {
            ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
            ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
            ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
            switch (desc.depth) {
            case 4:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_4;
                break;
            case 8:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_8;
                break;
            case 16:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_16;
                break;
            case 24:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_24;
                break;
            case 32:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_32;
                break;
            default:
                ddsd.ddpfPixelFormat.dwRGBBitCount = DDBD_16;
                break;
            }
        }
    }
    if (desc.systemMemory)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    else if (!primary)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
    if (desc.flip && desc.backBufferCount > 0) {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
    }
    if (desc.is3D)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
    if (primary)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_PRIMARYSURFACE;
    ddsd.dwBackBufferCount = desc.backBufferCount;
    m_pDirectDrawSurface->Create(ddsd);
    if (!desc.is3D && desc.hasColorKey)
        DDSetColorKey(m_pDirectDrawSurface->GetCOMInterface(), desc.colorKey);
}

// 0x100daca0 ?Release@Surface@DXGfx@@QAEXXZ
void DXGfx::Surface::Release()
{
    m_pDirectDrawSurface->Release();
}

// 0x100dacc0 ?Attach@Surface@DXGfx@@QAEXPAVDirectDrawSurface@DX@@@Z
void DXGfx::Surface::Attach(DX::DirectDrawSurface *surface)
{
    delete m_pDirectDrawSurface;
    m_pDirectDrawSurface = surface;
}

// 0x100dacf0 ?Detach@Surface@DXGfx@@QAEXXZ
void DXGfx::Surface::Detach()
{
    m_pDirectDrawSurface = new DX::DirectDrawSurface(m_pDirectDrawSurface->GetDirectDraw());
}

// 0x100dad60 ??1Surface@DXGfx@@UAE@XZ
DXGfx::Surface::~Surface()
{
    delete m_pDirectDrawSurface;
}

// 0x100dadb0 ?GetPitch@Surface@DXGfx@@UAEJXZ
long DXGfx::Surface::GetPitch()
{
    return mPitch;
}

// 0x100dadc0 ?GetWidth@Surface@DXGfx@@UAEHXZ
int DXGfx::Surface::GetWidth()
{
    return m_pDirectDrawSurface->Width();
}

// 0x100dadd0 ?GetHeight@Surface@DXGfx@@UAEHXZ
int DXGfx::Surface::GetHeight()
{
    return m_pDirectDrawSurface->Height();
}

// 0x100dade0 ?HasColorKey@Surface@DXGfx@@QAE_NXZ
bool DXGfx::Surface::HasColorKey()
{
    return mbColorKey;
}

// 0x100dadf0 ?SetColorKey@Surface@DXGfx@@QAEXK@Z
void DXGfx::Surface::SetColorKey(unsigned long colorKey)
{
    DDSetColorKey(m_pDirectDrawSurface->GetCOMInterface(), colorKey);
    mbColorKey = true;
}

// 0x100dae10 ?IsLocked@Surface@DXGfx@@UAE_NXZ
bool DXGfx::Surface::IsLocked()
{
    return mbLocked;
}

// 0x100dae20 ?IsLost@Surface@DXGfx@@QAE_NXZ
bool DXGfx::Surface::IsLost()
{
    return m_pDirectDrawSurface->IsLost() == DDERR_SURFACELOST;
}

// 0x100dae40 ?Lock@Surface@DXGfx@@UAEPAXXZ
void *DXGfx::Surface::Lock()
{
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if (m_pDirectDrawSurface->GetCOMInterface()->Lock(0, &ddsd, DDLOCK_WAIT, 0) == DD_OK)
        mbLocked = true;
    mPitch = ddsd.lPitch;
    return ddsd.lpSurface;
}

// 0x100daea0 ?Unlock@Surface@DXGfx@@UAEXXZ
void DXGfx::Surface::Unlock()
{
    mbLocked = m_pDirectDrawSurface->GetCOMInterface()->Unlock(0) != DD_OK;
}

// 0x100daec0 ?BitBlt@Surface@DXGfx@@UAEXABUtagRECT@@PAV1Gfx@@0@Z
void DXGfx::Surface::BitBlt(const tagRECT &destRect, Gfx::Surface *source, const tagRECT &sourceRect)
{
    DDBLTFX fx;
    memset(&fx, 0, sizeof(fx));
    fx.dwSize = sizeof(fx);
    DXGfx::Surface *surface = (DXGfx::Surface *)source;
    HRESULT hr = m_pDirectDrawSurface->GetCOMInterface()->Blt((RECT *)&destRect,
        surface->m_pDirectDrawSurface->GetCOMInterface(), (RECT *)&sourceRect,
        (surface->mbColorKey ? DDBLT_KEYSRC : 0) + DDBLT_WAIT, &fx);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 283, "Error bltting to surface");
}

// 0x100daf60 ?FastBlt@Surface@DXGfx@@UAEXABUtagPOINT@@PAV1Gfx@@ABUtagRECT@@@Z
void DXGfx::Surface::FastBlt(const tagPOINT &point, Gfx::Surface *source, const tagRECT &sourceRect)
{
    DXGfx::Surface *surface = (DXGfx::Surface *)source;
    HRESULT hr = m_pDirectDrawSurface->GetCOMInterface()->BltFast(point.x, point.y,
        surface->m_pDirectDrawSurface->GetCOMInterface(), (RECT *)&sourceRect,
        surface->mbColorKey ? DDBLT_KEYSRC | DDBLT_WAIT : 0);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 291, "Error fast bltting to surface");
}

// 0x100dafe0 ?CopyBitmap@Surface@DXGfx@@QAE_NPAUHBITMAP__@@@Z
bool DXGfx::Surface::CopyBitmap(HBITMAP bitmap)
{
    HDC hdcImage = CreateCompatibleDC(0);
    if (!hdcImage)
        return false;
    SetStretchBltMode(hdcImage, HALFTONE);
    SetBrushOrgEx(hdcImage, 0, 0, 0);
    SelectObject(hdcImage, bitmap);
    BITMAP bm;
    GetObject(bitmap, sizeof(bm), &bm);
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    m_pDirectDrawSurface->GetCOMInterface()->GetSurfaceDesc(&ddsd);
    HDC hdc;
    HRESULT hr = m_pDirectDrawSurface->GetCOMInterface()->GetDC(&hdc);
    if (hr == DD_OK) {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        m_pDirectDrawSurface->GetCOMInterface()->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);
    return hr == DD_OK;
}

// 0x100daa60 ??1Surface@Gfx@@UAE@XZ
// 0x100daa70 ??_GSurface@DXGfx@@UAEPAXI@Z
// 0x100db0d0 _$E31
// 0x100db100 _$E27
// 0x100db110 _$E30
