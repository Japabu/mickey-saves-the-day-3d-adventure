#include "DXGfx_ViewD3D.h"
#include "MathConstants.h"
#include "DX_Viewport.h"
#include "DX_Device.h"
#include "DX_Exception.h"
#include "DXGfx_DisplayD3D.h"
#include "DXGfx_LightD3D.h"
#include "DXGfx_Surface.h"
#include "DX_Direct3D.h"
#include "MBNew.h"
#include "Gfx_TextureException.h"
#include "StdBase.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d3f70 ?GetTypeInfo@ViewD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::ViewD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d3f80 ?GetTypeName@ViewD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::ViewD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\DXGraphics\\View.cpp"

// 0x100d3f90 ??0ViewD3D@DXGfx@@QAE@PAVSurface@1@@Z
DXGfx::ViewD3D::ViewD3D(DXGfx::Surface *surface)
    : m_pViewport(0), m_pSurface(surface), m_pDevice(0), m_LightInUse(0)
{
    Initialize();
    DXGfx::DisplayD3D *display = dynamic_cast<DXGfx::DisplayD3D *>(Gfx::Display::GetInstance());
    m_pDevice = new DX::Device(display->GetDirect3D());
    m_pViewport = new DX::Viewport(display->GetDirect3D(), m_pDevice);
}

// 0x100d4080 ?DrawBox@View@Gfx@@MAEXABVBoundingBox@2@K@Z
// 0x100d4090 ??_GViewD3D@DXGfx@@UAEPAXI@Z

// 0x100d40c0 ??1ViewD3D@DXGfx@@UAE@XZ
DXGfx::ViewD3D::~ViewD3D()
{
    Release();
    delete m_pViewport;
    delete m_pDevice;
}

// TODO 0x100d4130 ?Create@ViewD3D@DXGfx@@UAEXHH@Z: as Device::Create: the original calls ValList<DDDeviceInfo>::operator[] out of line inside GetCurrentDevice(), VC6 here inlines it (78%)
void DXGfx::ViewD3D::Create(int width, int height)
{
    m_pDevice->m_bEnableRendering = FALSE;
    Release();
    DXGfx::DisplayD3D *display = dynamic_cast<DXGfx::DisplayD3D *>(Gfx::Display::GetInstance());
    m_pDevice->Create(display->GetDirect3D()->Driver(), m_pSurface->GetDirectDrawSurface(), width, height);
    D3DDEVICEDESC7 caps;
    m_pDevice->m_IDirect3DDevice7->GetCaps(&caps);
    m_MaxLights = caps.dwMaxActiveLights;
    if (m_MaxLights > 10)
        m_MaxLights = 10;
    if (!m_LightInUse)
        m_LightInUse = (bool *)MBmalloc_release(m_MaxLights);
    for (unsigned int i = 0; i < m_MaxLights; i++)
        m_LightInUse[i] = false;
    m_pViewport->Create(width, height);
    m_pDevice->SetRenderState();
    SetAspectRatio((float)width / height);
    m_pDevice->m_bEnableRendering = TRUE;
}

// 0x100d4260 ?Release@ViewD3D@DXGfx@@UAEXXZ
void DXGfx::ViewD3D::Release()
{
    if (m_LightInUse) {
        MBfree_release(m_LightInUse);
        m_LightInUse = 0;
    }
    m_pViewport->Release();
    m_pDevice->Release();
}

// 0x100d42a0 ?CreateLight@ViewD3D@DXGfx@@UAEPAVLight@Gfx@@XZ
Gfx::Light *DXGfx::ViewD3D::CreateLight()
{
    int index = 0;
    while (m_LightInUse[index])
        index++;
    m_LightInUse[index] = true;
    DXGfx::LightD3D *light = new DXGfx::LightD3D(index);
    light->m_pView = this;
    m_pDevice->LightEnable(index, TRUE);
    return light;
}

// 0x100d4340 ?DeleteLight@ViewD3D@DXGfx@@UAEXPAVLight@Gfx@@@Z
void DXGfx::ViewD3D::DeleteLight(Gfx::Light *light)
{
    DXGfx::LightD3D *lightD3D = dynamic_cast<DXGfx::LightD3D *>(light);
    int index = lightD3D->m_Index;
    m_pDevice->LightEnable(index, FALSE);
    m_LightInUse[index] = false;
    delete lightD3D;
}

// 0x100d43a0 ?Clear@ViewD3D@DXGfx@@UAEXXZ
void DXGfx::ViewD3D::Clear()
{
    m_pViewport->Clear();
}

// 0x100d43b0 ?SetBackgroundColor@ViewD3D@DXGfx@@UAEXMMM@Z
void DXGfx::ViewD3D::SetBackgroundColor(float red, float green, float blue)
{
    m_pViewport->SetBackgroundColor(red, green, blue);
}

// 0x100d43d0 ?Refresh@ViewD3D@DXGfx@@UAEXXZ
void DXGfx::ViewD3D::Refresh()
{
    if (m_pDevice->RenderingIsEnabled()) {
        HRESULT hr = m_pViewport->BeginScene();
        if (hr)
            throw DX::Exception(hr, THIS_FILE, 172, "Error calling BeginScene");
        try {
            View::Refresh();
        } catch (Gfx::TextureException &) {
            hr = m_pViewport->EndScene();
            if (hr)
                throw DX::Exception(hr, THIS_FILE, 181, "Error calling EndScene");
            throw;
        }
        hr = m_pViewport->EndScene();
        if (hr)
            throw DX::Exception(hr, THIS_FILE, 186, "Error calling EndScene");
    }
}

// 0x100d3e80 _$E10
// 0x100d3e90 _$E9
// 0x100d3eb0 _$E13
// 0x100d3ec0 _$E12
// 0x100d3ee0 _$E16
// 0x100d3ef0 _$E15
// 0x100d3f10 _$E19
// 0x100d3f20 _$E18
// 0x100d3f40 _$E22
// 0x100d3f50 _$E21

// The initializers below are named by VC6's counter: DXGfx::Surface's three static members
// (needed by Create) move it by 3 from the original's _$E88/_$E84/_$E87; the bytes match.
// 0x100d44f0 _$E91
// 0x100d4520 _$E87
// 0x100d4530 _$E90

// the run-time type of DXGfx::ViewD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::ViewD3D::s_TypeInfo = { "DXGfx::ViewD3D", &Gfx::View::s_TypeInfo };
