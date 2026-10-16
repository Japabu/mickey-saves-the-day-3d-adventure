#pragma once
#include "Gfx_Display.h"
#include "Gfx_Surface.h"
#include "d3d7.h"
template <class T0> class _SS;
namespace DX { class DirectDraw; class Direct3D; }
namespace DXGfx { class Surface; }
namespace EG2 { class Resource; }
struct IDirectDraw7;
struct _DDCAPS_DX7;

namespace DXGfx {
// The Direct3D display: the primary surface, its back buffer and the
// Direct3D object everything is created from (declaration from
// SteamboatApp.pdb).
class DisplayD3D : public Gfx::Display {
public:
    static void Create(HWND, IDirectDraw7 *, GUID *);
    DX::Direct3D *GetDirect3D() { return m_pDirect3D; }
    IDirectDrawPalette *GetDDPalette() { return m_lpDDPalette; }
    int GetFullScreen() { return m_bFullscreen; }
    int GetSingleStepMode() { return m_bSingleStepMode; }
    HWND GetMainWnd() { return mhMainHwnd; }
    const Gfx::SurfaceDesc &GetPrimarySurfaceDesc() { return m_PrimarySurfaceDesc; }
    void CreateFrontBuffer();
    int SetDisplayMode(int);
    int SetCoopLevel(int);
    void SetClearAll(int clearAll) { m_bClearAll = clearAll; }
    int GetClearAll() { return m_bClearAll; }
    virtual Gfx::Surface *GetDisplaySurface();
    virtual void Flip();
    virtual void SetSurfaceProperties(const Gfx::SurfaceDesc &);
    virtual void Initialize();
    virtual Gfx::Surface *CreateSurface(const Gfx::SurfaceDesc &);
    virtual Gfx::Surface *CreateSurfaceFromResource(EG2::Resource *);
    virtual Gfx::Surface *CreateSurfaceFromFile(_SS<char>);
protected:
    virtual ~DisplayD3D();
public:
    virtual void SetFullScreen(int);
    int IsFullScreen() { return m_bFullscreen; }
    void SetEmulation(int emulation) { m_bEmulation = emulation; }
    int IsEmulation() { return m_bEmulation; }
    void SetFrontBuffer(IDirectDrawSurface7 *);
    void SetBackBuffer(DXGfx::Surface *);
    void SetBackBuffer(IDirectDrawSurface7 *);
    void SetDisplaySurface(DXGfx::Surface *);
    void CheckForPalettized();
    int IgnoreWM_SIZE() { return m_bIgnoreWM_SIZE; }
    void SetIgnoreWM_SIZE(int ignore) { m_bIgnoreWM_SIZE = ignore; }
    int InSingleStepMode() { return m_bSingleStepMode; }
    int m_bClearsOn;
    IDirectDrawSurface7 *m_lpddsFrontBuffer;
    int m_bPaused;
    int m_bPrimaryPalettized;
    int m_bPaletteActivate;
    IDirectDrawPalette *m_lpDDPalette;
    int m_bTexturesDisabled;
    int m_bFullscreen;
    int m_bEmulation;
protected:
    DisplayD3D(HWND, IDirectDraw7 *, GUID *);
    int InitDirect3D();
    void AssignBuffers(IDirectDrawSurface7 *, IDirectDrawSurface7 *);
    Gfx::SurfaceDesc m_PrimarySurfaceDesc;
    DXGfx::Surface *m_pDisplaySurface;
    DXGfx::Surface *m_pBackBuffer;
private:
    bool CopyBitmap(IDirectDrawSurface7 *, HBITMAP);
    static int DDDeviceSelectionCallback(DX::DirectDraw *, GUID *, const _DDCAPS_DX7 &, void *);
    HWND mhMainHwnd;
    DX::Direct3D *m_pDirect3D;
    IDirectDrawClipper *m_lpDDClipper;
    int m_bIgnoreWM_SIZE;
    int m_bAppActive;
    int m_bOnlySystemMemory;
    int m_bInitialized;
    SIZE m_szLastClient;
    int m_CallbackRefCount;
    int m_bSingleStepMode;
    int m_bDrawAFrame;
    int m_bClearAll;
    int m_bResizingDisabled;
    int m_bResized;
    int m_iPreferredDriver;
    int m_iPreferredMode;
    unsigned long m_dwPreferredWidth;
    unsigned long m_dwPreferredHeight;
    unsigned long m_dwPreferredDepth;
    RECT m_rcWindowLast;
    IDirectDrawSurface7 *m_FrontBuffer;
    IDirectDrawSurface7 *m_BackBuffer;
};
}
