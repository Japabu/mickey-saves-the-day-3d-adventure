#include "DXGfx_DisplayD3D.h"
#include "DXGfx_Surface.h"
#include "DX_Direct3D.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "MBNew.h"
#include "DX_DirectDrawSurface.h"
#include "EG2_Bitmap.h"
#include "StdString.h"
#include "DX_Exception.h"
#include <string.h>

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\DXGraphics\\Display.cpp"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d4b30 ?Create@DisplayD3D@DXGfx@@SAXPAUHWND__@@PAUIDirectDraw7@@PAU_GUID@@@Z
void DXGfx::DisplayD3D::Create(HWND hwnd, IDirectDraw7 *directDraw, GUID *guid)
{
    if (!mInstance)
        mInstance = new DisplayD3D(hwnd, directDraw, guid);
}

// TODO 0x100d4bb0 ?SetSurfaceProperties@DisplayD3D@DXGfx@@UAEXABUSurfaceDesc@Gfx@@@Z: (50% similar) the original loads height, width and depth into three saved registers before the byte stores (no-alias scheduling); plain, chained and /Oa forms all keep them in edx
void DXGfx::DisplayD3D::SetSurfaceProperties(const Gfx::SurfaceDesc &desc)
{
    m_PrimarySurfaceDesc.systemMemory = desc.systemMemory;
    m_PrimarySurfaceDesc.flip = desc.flip;
    m_PrimarySurfaceDesc.backBufferCount = desc.backBufferCount;
    m_PrimarySurfaceDesc.is3D = desc.is3D;
    m_dwPreferredHeight = m_PrimarySurfaceDesc.height = desc.height;
    m_dwPreferredWidth = m_PrimarySurfaceDesc.width = desc.width;
    m_dwPreferredDepth = m_PrimarySurfaceDesc.depth = desc.depth;
}

// TODO 0x100d4c00 ?Initialize@DisplayD3D@DXGfx@@UAEXXZ: (97% similar) matches only with #pragma optimize("a", on) for the whole unit (the inlined ValList lookup stores cnum after loading next), which breaks six other functions here; a per-function pragma has no effect
void DXGfx::DisplayD3D::Initialize()
{
    Gfx::CurrentTransformationMatrix::Create();
    DX::Direct3D *direct3D = m_pDirect3D;
    direct3D->SetEmulation(m_bEmulation);
    direct3D->Create(0, m_dwPreferredWidth, m_dwPreferredHeight, m_dwPreferredDepth);
    if (m_FrontBuffer && m_BackBuffer) {
        AssignBuffers(m_FrontBuffer, m_BackBuffer);
        CheckForPalettized();
        return;
    }
    if (m_bFullscreen) {
        SetCoopLevel(1);
        SetDisplayMode(m_pDirect3D->GetDisplayMode());
    } else {
        SetCoopLevel(0);
    }
    CreateFrontBuffer();
    CheckForPalettized();
}

// 0x100d4ce0 ?DDDeviceSelectionCallback@DisplayD3D@DXGfx@@CAHPAVDirectDraw@DX@@PAU_GUID@@ABU_DDCAPS_DX7@@PAX@Z
int DXGfx::DisplayD3D::DDDeviceSelectionCallback(DX::DirectDraw *, GUID *guid, const DDCAPS_DX7 &caps, void *context)
{
    DisplayD3D *display = (DisplayD3D *)context;
    if (display->m_bFullscreen) {
        if (guid && (caps.dwCaps & DDCAPS_3D))
            return 1;
    } else {
        if (!guid)
            return 1;
    }
    return 0;
}

// 0x100d4d10 ?SetFullScreen@DisplayD3D@DXGfx@@UAEXH@Z
void DXGfx::DisplayD3D::SetFullScreen(int bFullScreen)
{
    m_bFullscreen = bFullScreen;
}

// 0x100d4d20 ?GetDisplaySurface@DisplayD3D@DXGfx@@UAEPAVSurface@Gfx@@XZ
Gfx::Surface * DXGfx::DisplayD3D::GetDisplaySurface()
{
    return m_pBackBuffer;
}

// 0x100d4d30 ?CreateSurface@DisplayD3D@DXGfx@@UAEPAVSurface@Gfx@@ABUSurfaceDesc@4@@Z
Gfx::Surface *DXGfx::DisplayD3D::CreateSurface(const Gfx::SurfaceDesc &desc)
{
    DXGfx::Surface *surface = new DXGfx::Surface(m_pDirect3D);
    surface->Create(desc, false);
    return surface;
}

// 0x100d4da0 ?CreateSurfaceFromResource@DisplayD3D@DXGfx@@UAEPAVSurface@Gfx@@PAVResource@EG2@@@Z
Gfx::Surface *DXGfx::DisplayD3D::CreateSurfaceFromResource(EG2::Resource *resource)
{
    EG2::Bitmap bitmap;
    if (!bitmap.LoadFromResource(resource))
        return 0;
    Gfx::SurfaceDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.width = bitmap.GetBitmapInfo()->biWidth;
    desc.height = bitmap.GetBitmapInfo()->biHeight;
    desc.depth = 0;
    desc.backBufferCount = 0;
    desc.systemMemory = true;
    desc.flip = false;
    desc.is3D = false;
    Gfx::Surface *surface = CreateSurface(desc);
    DXGfx::Surface *dxSurface = dynamic_cast<DXGfx::Surface *>(surface);
    if (!dxSurface)
        return 0;
    HDC hdc;
    dxSurface->GetDirectDrawSurface()->GetDC(&hdc);
    HBITMAP hbm = CreateDIBitmap(hdc, bitmap.GetBitmapInfo(), CBM_INIT, bitmap.GetBits(),
                                 (BITMAPINFO *)bitmap.GetBitmapInfo(), DIB_RGB_COLORS);
    dxSurface->GetDirectDrawSurface()->ReleaseDC(hdc);
    if (!CopyBitmap(dxSurface->GetDirectDrawSurface()->GetCOMInterface(), hbm))
        return 0;
    DeleteObject(hbm);
    return surface;
}

// 0x100d4f10 ?CreateSurfaceFromFile@DisplayD3D@DXGfx@@UAEPAVSurface@Gfx@@V?$_SS@D@@@Z
Gfx::Surface *DXGfx::DisplayD3D::CreateSurfaceFromFile(_SS<char> fileName)
{
    EG2::Bitmap bitmap;
    if (!bitmap.LoadFromFile(fileName))
        return 0;
    Gfx::SurfaceDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.width = bitmap.GetBitmapInfo()->biWidth;
    desc.height = bitmap.GetBitmapInfo()->biHeight;
    desc.depth = 0;
    desc.backBufferCount = 0;
    desc.systemMemory = true;
    desc.flip = false;
    desc.is3D = false;
    Gfx::Surface *surface = CreateSurface(desc);
    DXGfx::Surface *dxSurface = dynamic_cast<DXGfx::Surface *>(surface);
    if (!dxSurface)
        return 0;
    HDC hdc;
    dxSurface->GetDirectDrawSurface()->GetDC(&hdc);
    HBITMAP hbm = CreateDIBitmap(hdc, bitmap.GetBitmapInfo(), CBM_INIT, bitmap.GetBits(),
                                 (BITMAPINFO *)bitmap.GetBitmapInfo(), DIB_RGB_COLORS);
    dxSurface->GetDirectDrawSurface()->ReleaseDC(hdc);
    if (!CopyBitmap(dxSurface->GetDirectDrawSurface()->GetCOMInterface(), hbm))
        return 0;
    DeleteObject(hbm);
    return surface;
}

// 0x100d50e0 ??0DisplayD3D@DXGfx@@IAE@PAUHWND__@@PAUIDirectDraw7@@PAU_GUID@@@Z
DXGfx::DisplayD3D::DisplayD3D(HWND hwnd, IDirectDraw7 *directDraw, GUID *guid)
{
    mhMainHwnd = hwnd;
    m_iPreferredDriver = -25;
    m_iPreferredMode = -25;
    m_bIgnoreWM_SIZE = 1;
    m_bAppActive = 0;
    m_bClearsOn = 1;
    m_bClearAll = 0;
    m_bDrawAFrame = 0;
    m_bFullscreen = 0;
    m_bEmulation = 0;
    m_bInitialized = 0;
    m_bOnlySystemMemory = 0;
    m_bPaletteActivate = 0;
    m_bPaused = 0;
    m_bPrimaryPalettized = 0;
    m_bResized = 0;
    m_bResizingDisabled = 0;
    m_bSingleStepMode = 0;
    m_bTexturesDisabled = 0;
    m_CallbackRefCount = 0;
    m_lpDDPalette = 0;
    m_lpDDClipper = 0;
    m_lpddsFrontBuffer = 0;
    m_pDisplaySurface = 0;
    m_pBackBuffer = 0;
    m_dwPreferredWidth = (unsigned long)-1;
    m_dwPreferredHeight = (unsigned long)-1;
    m_dwPreferredDepth = (unsigned long)-1;
    m_FrontBuffer = 0;
    m_BackBuffer = 0;
    m_pDirect3D = new DX::Direct3D(directDraw, guid);
    m_pDirect3D->SetDeviceSelectionCallback(DDDeviceSelectionCallback, this);
}

// 0x100d51f0 ??1Display@Gfx@@MAE@XZ

// 0x100d5200 ??_GDisplay@Gfx@@MAEPAXI@Z

// 0x100d5230 ??_GDisplayD3D@DXGfx@@MAEPAXI@Z

// TODO 0x100d5260 ??1DisplayD3D@DXGfx@@MAE@XZ: (91% similar) identical except that VC6 here pops edi (the CurrentTransformationMatrix pointer) before the buffer compare, the original after it
DXGfx::DisplayD3D::~DisplayD3D()
{
    if (m_lpDDClipper) {
        m_lpDDClipper->Release();
        m_lpDDClipper = 0;
    }
    if (m_lpDDPalette) {
        m_lpDDPalette->Release();
        m_lpDDPalette = 0;
    }
    Gfx::CurrentTransformationMatrix::Destroy();
    if (m_pBackBuffer != m_pDisplaySurface) {
        if (m_bFullscreen)
            m_pBackBuffer->Detach();
        else
            m_pBackBuffer->Release();
        delete m_pBackBuffer;
    }
    delete m_pDisplaySurface;
    if (m_pDirect3D)
        m_pDirect3D->Release();
    delete m_pDirect3D;
}

// 0x100d5340 ?CreateFrontBuffer@DisplayD3D@DXGfx@@QAEXXZ
void DXGfx::DisplayD3D::CreateFrontBuffer()
{
    HRESULT hr;
    if (m_lpDDClipper) {
        m_lpDDClipper->Release();
        m_lpDDClipper = 0;
    }
    if (m_lpddsFrontBuffer) {
        m_lpddsFrontBuffer->Release();
        m_lpddsFrontBuffer = 0;
    }
    if (m_bFullscreen) {
        m_pDisplaySurface = new DXGfx::Surface(m_pDirect3D);
        m_pDisplaySurface->Create(m_PrimarySurfaceDesc, true);
        m_lpddsFrontBuffer = m_pDisplaySurface->GetDirectDrawSurface()->GetCOMInterface();
        if (m_PrimarySurfaceDesc.backBufferCount > 0) {
            DDSCAPS2 caps;
            IDirectDrawSurface7 *backBuffer;
            caps.dwCaps = DDSCAPS_BACKBUFFER;
            m_lpddsFrontBuffer->GetAttachedSurface(&caps, &backBuffer);
            m_pBackBuffer = new DXGfx::Surface(m_pDirect3D);
            DX::DirectDrawSurface *surface = new DX::DirectDrawSurface(m_pDirect3D);
            surface->Attach(backBuffer);
            m_pBackBuffer->Attach(surface);
        } else {
            m_pBackBuffer = m_pDisplaySurface;
        }
    } else {
        Gfx::SurfaceDesc desc = m_PrimarySurfaceDesc;
        desc.backBufferCount = 0;
        desc.flip = false;
        m_pDisplaySurface = new DXGfx::Surface(m_pDirect3D);
        m_pDisplaySurface->Create(desc, true);
        m_lpddsFrontBuffer = m_pDisplaySurface->GetDirectDrawSurface()->GetCOMInterface();
        if (m_PrimarySurfaceDesc.backBufferCount > 0) {
            Gfx::SurfaceDesc backDesc = m_PrimarySurfaceDesc;
            backDesc.is3D = true;
            backDesc.depth = 0;
            backDesc.backBufferCount = 0;
            backDesc.flip = false;
            backDesc.hasColorKey = false;
            m_pBackBuffer = new DXGfx::Surface(m_pDirect3D);
            m_pBackBuffer->Create(backDesc, false);
        } else {
            m_pBackBuffer = m_pDisplaySurface;
        }
        IDirectDraw7 *directDraw = m_pDirect3D->GetCOMInterface();
        if (directDraw) {
            hr = directDraw->CreateClipper(0, &m_lpDDClipper, 0);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 408, "CreateClipper failed");
        }
        hr = m_lpDDClipper->SetHWnd(0, ((DisplayD3D *)Gfx::Display::GetInstance())->GetMainWnd());
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 409, "Error setting clipper window");
        hr = m_lpddsFrontBuffer->SetClipper(m_lpDDClipper);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 411, "Attaching clipper to front buffer failed");
    }
    if (m_bFullscreen && m_lpddsFrontBuffer) {
        DDSURFACEDESC2 ddsd;
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        hr = m_lpddsFrontBuffer->GetSurfaceDesc(&ddsd);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 421, "Failure getting the surface description of the front buffer before clearing");
        DDBLTFX fx;
        memset(&fx, 0, sizeof(fx));
        fx.dwSize = sizeof(fx);
        RECT rect;
        SetRect(&rect, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
        hr = m_lpddsFrontBuffer->Blt(&rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &fx);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 428, "Clearing the front buffer failed");
    }
    m_bInitialized = 1;
}

// TODO 0x100d5750 ?AssignBuffers@DisplayD3D@DXGfx@@IAEXPAUIDirectDrawSurface7@@0@Z: (91% similar) the clipper block differs in register choice only (edx/eax/ecx rotated); the same block in CreateFrontBuffer matches
void DXGfx::DisplayD3D::AssignBuffers(IDirectDrawSurface7 *frontBuffer, IDirectDrawSurface7 *backBuffer)
{
    HRESULT hr;
    m_pDisplaySurface = new DXGfx::Surface(m_pDirect3D);
    DX::DirectDrawSurface *surface = new DX::DirectDrawSurface(m_pDirect3D);
    surface->Attach(frontBuffer);
    m_pDisplaySurface->Attach(surface);
    m_lpddsFrontBuffer = frontBuffer;
    if (backBuffer && backBuffer != frontBuffer) {
        m_pBackBuffer = new DXGfx::Surface(m_pDirect3D);
        surface = new DX::DirectDrawSurface(m_pDirect3D);
        surface->Attach(backBuffer);
        m_pBackBuffer->Attach(surface);
    } else {
        m_pBackBuffer = m_pDisplaySurface;
    }
    if (!m_bFullscreen) {
        IDirectDraw7 *directDraw = m_pDirect3D->GetCOMInterface();
        if (directDraw) {
            hr = directDraw->CreateClipper(0, &m_lpDDClipper, 0);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 463, "CreateClipper failed");
        }
        hr = m_lpDDClipper->SetHWnd(0, ((DisplayD3D *)Gfx::Display::GetInstance())->GetMainWnd());
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 464, "Error setting clipper window");
        hr = m_lpddsFrontBuffer->SetClipper(m_lpDDClipper);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 465, "Attaching clipper to front buffer failed");
    }
    if (m_bFullscreen && m_lpddsFrontBuffer) {
        DDSURFACEDESC2 ddsd;
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        hr = m_lpddsFrontBuffer->GetSurfaceDesc(&ddsd);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 476, "Failure getting the surface description of the front buffer before clearing");
        DDBLTFX fx;
        memset(&fx, 0, sizeof(fx));
        fx.dwSize = sizeof(fx);
        RECT rect;
        SetRect(&rect, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
        hr = m_lpddsFrontBuffer->Blt(&rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &fx);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 480, "Error clearing buffer");
    }
    m_bInitialized = 1;
}

// TODO 0x100d5a80 ?SetCoopLevel@DisplayD3D@DXGfx@@QAEHH@Z: (92% similar) the windowed branch differs in register choice only (ecx/edx swapped around the SetCooperativeLevel call); the full-screen branch matches
int DXGfx::DisplayD3D::SetCoopLevel(int fullScreen)
{
    if (fullScreen) {
        m_bIgnoreWM_SIZE = 1;
        IDirectDraw7 *directDraw = m_pDirect3D->GetCOMInterface();
        if (directDraw) {
            HRESULT hr = directDraw->SetCooperativeLevel(mhMainHwnd, DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 502, "Error setting cooperative level");
        }
        m_bIgnoreWM_SIZE = 0;
        m_bFullscreen = 1;
    } else {
        if (m_bFullscreen)
            MoveWindow(mhMainHwnd, m_rcWindowLast.left, m_rcWindowLast.bottom, m_rcWindowLast.right - m_rcWindowLast.left,
                       m_rcWindowLast.bottom - m_rcWindowLast.top, FALSE);
        m_bIgnoreWM_SIZE = 1;
        IDirectDraw7 *directDraw = m_pDirect3D->GetCOMInterface();
        if (directDraw) {
            HRESULT hr = directDraw->SetCooperativeLevel(mhMainHwnd, DDSCL_MULTITHREADED | DDSCL_NORMAL);
            if (hr != DD_OK)
                throw DX::Exception(hr, THIS_FILE, 518, "Error setting cooperative level");
        }
        m_bIgnoreWM_SIZE = 0;
        m_bFullscreen = 0;
    }
    return 1;
}

// TODO 0x100d5ba0 ?SetDisplayMode@DisplayD3D@DXGfx@@QAEHH@Z: (91% similar) same as Initialize: matches only with a unit-wide /Oa pragma
int DXGfx::DisplayD3D::SetDisplayMode(int mode)
{
    m_bIgnoreWM_SIZE = 1;
    DX::Direct3D *direct3D = m_pDirect3D;
    int device = direct3D->GetCurrentDeviceIndex();
    unsigned long width = direct3D->GetDeviceInfoPtr(device)->DisplayModes[mode].dwWidth;
    unsigned long height = direct3D->GetDeviceInfoPtr(device)->DisplayModes[mode].dwHeight;
    unsigned long depth = direct3D->GetDeviceInfoPtr(device)->DisplayModes[mode].dwRGBBitCount;
    IDirectDraw7 *directDraw = direct3D->GetCOMInterface();
    if (directDraw) {
        HRESULT hr = directDraw->SetDisplayMode(width, height, depth, 0, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 535, "Error setting display mode");
    }
    m_bIgnoreWM_SIZE = 0;
    return 1;
}

// 0x100d5dc0 ?Flip@DisplayD3D@DXGfx@@UAEXXZ
void DXGfx::DisplayD3D::Flip()
{
    IDirectDrawSurface7 *front = m_lpddsFrontBuffer;
    if (front->IsLost() == DDERR_SURFACELOST)
        front->Restore();
    if (m_bFullscreen) {
        front->Flip(0, DDFLIP_WAIT);
        return;
    }
    if (m_pBackBuffer != m_pDisplaySurface) {
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = m_PrimarySurfaceDesc.width;
        rect.bottom = m_PrimarySurfaceDesc.height;
        m_pDisplaySurface->BitBlt(rect, m_pBackBuffer, rect);
    }
}

// 0x100d5e30 ?CheckForPalettized@DisplayD3D@DXGfx@@QAEXXZ
void DXGfx::DisplayD3D::CheckForPalettized()
{
    DDSURFACEDESC2 ddsd;
    m_pDisplaySurface->GetDirectDrawSurface()->GetSurfDesc(&ddsd);
    m_bPrimaryPalettized = (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) ? 1 : 0;
    if (!m_bPrimaryPalettized)
        return;
    HDC hdc = GetDC(0);
    DX::Direct3D *direct3D = m_pDirect3D;
    GetSystemPaletteEntries(hdc, 0, 256, direct3D->GetPalette());
    ReleaseDC(0, hdc);
    int i;
    if (!m_bFullscreen) {
        for (i = 0; i < 10; i++)
            direct3D->GetPalette()[i].peFlags = D3DPAL_READONLY;
        for (i = 10; i < 246; i++)
            direct3D->GetPalette()[i].peFlags = PC_RESERVED;
        for (i = 246; i < 256; i++)
            direct3D->GetPalette()[i].peFlags = D3DPAL_READONLY;
    } else {
        direct3D->GetPalette()[0].peFlags = D3DPAL_READONLY;
        for (i = 1; i < 255; i++)
            direct3D->GetPalette()[i].peFlags = PC_RESERVED;
        direct3D->GetPalette()[255].peFlags = D3DPAL_READONLY;
    }
    HRESULT hr;
    IDirectDraw7 *directDraw = direct3D->GetCOMInterface();
    if (directDraw) {
        hr = directDraw->CreatePalette(DDPCAPS_8BIT, direct3D->GetPalette(), &m_lpDDPalette, 0);
        if (hr != DD_OK)
            throw DX::Exception(hr, THIS_FILE, 598, "CreatePalette failed");
    }
    hr = m_lpddsFrontBuffer->SetPalette(m_lpDDPalette);
    if (hr != DD_OK)
        throw DX::Exception(hr, THIS_FILE, 602, "SetPalette failed on front buffer");
    m_bPaletteActivate = 1;
}

// 0x100d5f90 ?CopyBitmap@DisplayD3D@DXGfx@@AAE_NPAUIDirectDrawSurface7@@PAUHBITMAP__@@@Z
bool DXGfx::DisplayD3D::CopyBitmap(IDirectDrawSurface7 *surface, HBITMAP hbm)
{
    HDC hdcImage = CreateCompatibleDC(0);
    if (!hdcImage)
        return false;
    SetStretchBltMode(hdcImage, HALFTONE);
    SetBrushOrgEx(hdcImage, 0, 0, 0);
    SelectObject(hdcImage, hbm);
    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    surface->GetSurfaceDesc(&ddsd);
    HDC hdc;
    if (surface->GetDC(&hdc) != DD_OK) {
        DeleteDC(hdcImage);
        return false;
    }
    StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    surface->ReleaseDC(hdc);
    DeleteDC(hdcImage);
    return true;
}

// 0x100d6140 _$E33
// 0x100d6170 _$E29
// 0x100d6180 _$E32

// Template and inline instances this unit also emits:
// 0x10008b10 ?id@?$collate@D@std@@$E
// 0x100d6080 ??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@PBDABV?$allocator@D@1@@Z

// The unit's <locale> initializers (linked apart from the rest of Display.obj):
// 0x100e6550 _$E33
// 0x100e6580 _$E29
// 0x100e6590 _$E32
