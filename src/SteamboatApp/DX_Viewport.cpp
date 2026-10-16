#include "DX_Viewport.h"
#include "DX_Device.h"
#include "DX_Exception.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXViewport.cpp"

// 0x1012de80 ??0Viewport@DX@@QAE@PAVDirect3D@1@PAVDevice@1@@Z
DX::Viewport::Viewport(DX::Direct3D *direct3D, DX::Device *device) : m_pDirect3D(direct3D), m_pDevice(device)
{
    memset(&m_Material, 0, sizeof(m_Material));
    m_Material.diffuse.r = 1.0f;
    m_Material.diffuse.g = 1.0f;
    m_Material.diffuse.b = 1.0f;
    m_Material.diffuse.a = 1.0f;
    m_Material.ambient.r = 1.0f;
    m_Material.ambient.g = 1.0f;
    m_Material.ambient.b = 1.0f;
    SetBackground();
}

// 0x1012def0 ??1Viewport@DX@@UAE@XZ
DX::Viewport::~Viewport()
{
    Release();
}

// 0x1012df00 ?SetBackgroundColor@Viewport@DX@@QAEXMMM@Z
void DX::Viewport::SetBackgroundColor(float red, float green, float blue)
{
    m_Material.diffuse.r = red;
    m_Material.diffuse.g = green;
    m_Material.diffuse.b = blue;
    SetBackground();
}

// 0x1012df20 ?SetBackground@Viewport@DX@@IAEXXZ
void DX::Viewport::SetBackground()
{
    D3DCOLOR color = (int)(m_Material.diffuse.a * 255.0f);
    color = (color << 8) | (int)(m_Material.diffuse.r * 255.0f);
    color = (color << 8) | (int)(m_Material.diffuse.g * 255.0f);
    color = (color << 8) | (int)(m_Material.diffuse.b * 255.0f);
    m_BackgroundColor = color;
}

// 0x1012df80 ?Create@Viewport@DX@@QAEXHH@Z
void DX::Viewport::Create(int width, int height)
{
    D3DVIEWPORT7 viewport;
    memset(&viewport, 0, sizeof(viewport));
    viewport.dwX = 0;
    viewport.dwY = 0;
    viewport.dwWidth = width;
    viewport.dwHeight = height;
    viewport.dvMinZ = 0.0f;
    viewport.dvMaxZ = 1.0f;
    m_pDevice->SetViewport(&viewport);
}

// 0x1012dfe0 ?Release@Viewport@DX@@QAEXXZ
void DX::Viewport::Release()
{
}

// 0x1012dff0 ?Clear@Viewport@DX@@QAEXXZ
void DX::Viewport::Clear()
{
    if (!m_pDevice->RenderingIsEnabled())
        throw DX::Exception("Cannot call clear while bRenderingIsOK is FALSE");
    DWORD flags = D3DCLEAR_TARGET;
    if (m_pDevice->m_D3DRenderState.m_bZEnable)
        flags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
    D3DVIEWPORT7 viewport;
    memset(&viewport, 0, sizeof(viewport));
    m_pDevice->GetViewport(&viewport);
    HRESULT hr = m_pDevice->Clear(0, 0, flags, m_BackgroundColor, 1.0f, 0);
    if (hr != D3D_OK)
        throw DX::Exception(hr, THIS_FILE, 139, "Viewport clear failed");
}

// 0x1012e0a0 ?SetSize@Viewport@DX@@QAEXHH@Z
void DX::Viewport::SetSize(int width, int height)
{
    D3DVIEWPORT7 viewport;
    memset(&viewport, 0, sizeof(viewport));
    m_pDevice->GetViewport(&viewport);
    viewport.dwWidth = width;
    viewport.dwHeight = height;
    m_pDevice->SetViewport(&viewport);
}

// 0x1012e0f0 ?SetPosition@Viewport@DX@@QAEXHH@Z
void DX::Viewport::SetPosition(int x, int y)
{
    D3DVIEWPORT7 viewport;
    memset(&viewport, 0, sizeof(viewport));
    m_pDevice->GetViewport(&viewport);
    viewport.dwX = x;
    viewport.dwY = y;
    m_pDevice->SetViewport(&viewport);
}

// 0x1012e140 ?BeginScene@Viewport@DX@@QAEJXZ
long DX::Viewport::BeginScene()
{
    try {
        m_pDevice->CheckForLostSurfaces();
    } catch (DX::Exception) {
        return 0;
    }
    return m_pDevice->BeginScene();
}

// 0x1012e1b0 ?EndScene@Viewport@DX@@QAEJXZ
long DX::Viewport::EndScene()
{
    m_pDevice->ResetRenderState();
    m_pDevice->ResetTextureStageState();
    return m_pDevice->EndScene();
}

// 0x1012ded0 ??_GViewport@DX@@UAEPAXI@Z
// 0x1012e1e0 _$E25
// 0x1012e210 _$E21
// 0x1012e220 _$E24
