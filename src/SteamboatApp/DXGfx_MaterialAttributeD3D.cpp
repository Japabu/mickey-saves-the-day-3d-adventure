#include "DXGfx_MaterialAttributeD3D.h"
#include "Graph_PassiveTraverser.h"
#include "Gfx_RenderTraverser.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "MBNew.h"
#include <string.h>
#include "MathConstants.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d9440 ?GetTypeInfo@MaterialAttributeD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::MaterialAttributeD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d9450 ?GetTypeName@MaterialAttributeD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::MaterialAttributeD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100d9460 ??0MaterialAttributeD3D@DXGfx@@QAE@XZ
DXGfx::MaterialAttributeD3D::MaterialAttributeD3D()
{
    MaterialAttributeD3D::SetMaterial();
}

// 0x100d94b0 ??_GMaterialAttributeD3D@DXGfx@@MAEPAXI@Z

// 0x100d94e0 ??1MaterialAttributeD3D@DXGfx@@MAE@XZ
DXGfx::MaterialAttributeD3D::~MaterialAttributeD3D()
{
}

// 0x100d94f0 ?SetMaterial@MaterialAttributeD3D@DXGfx@@UAEXXZ
void DXGfx::MaterialAttributeD3D::SetMaterial()
{
    memcpy(&m_Material.ambient, &m_Ambient, sizeof(D3DCOLORVALUE));
    memcpy(&m_Material.diffuse, &m_Diffuse, sizeof(D3DCOLORVALUE));
    memcpy(&m_Material.specular, &m_Specular, sizeof(D3DCOLORVALUE));
    memcpy(&m_Material.emissive, &m_Emissive, sizeof(D3DCOLORVALUE));
    m_Material.power = GetPower();
}

// 0x100d9590 ?Apply@MaterialAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::MaterialAttributeD3D::Apply(Graph::PassiveTraverser *traverser) const
{
    Gfx::RenderTraverser *renderTraverser = StdBaseCast(traverser, (Gfx::RenderTraverser *)0);
    if (!renderTraverser)
        return;
    DX::Device *device = ((DXGfx::ViewD3D *)renderTraverser->GetView())->GetDevice();
    renderTraverser->AddMaterial(1);
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    DWORD zBuffer = m_ZBuffer;
    device->SetRenderState(D3DRENDERSTATE_ZENABLE, zBuffer);
    device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, zBuffer);
    device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    device->GetCOMInterface()->SetMaterial((D3DMATERIAL7 *)&m_Material);
}

// 0x100d9630 ?Push@MaterialAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::MaterialAttributeD3D::Push(Graph::PassiveTraverser *traverser) const
{
    Apply(traverser);
    MaterialAttribute::Push(traverser);
}

// 0x100d9650 ?Pop@MaterialAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::MaterialAttributeD3D::Pop(Graph::PassiveTraverser * traverser) const
{
    MaterialAttribute::Pop(traverser);
}

// 0x100d9660 ?ReadVis@MaterialAttributeD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::MaterialAttributeD3D::ReadVis(Graph::Stream *stream)
{
    MaterialAttribute::ReadVis(stream);
    SetMaterial();
}

// 0x100d9680 ?Read@MaterialAttributeD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::MaterialAttributeD3D::Read(Graph::Stream *stream)
{
    MaterialAttribute::Read(stream);
    SetMaterial();
}

// 0x100d9350 _$E40
// 0x100d9360 _$E39
// 0x100d9380 _$E43
// 0x100d9390 _$E42
// 0x100d93b0 _$E46
// 0x100d93c0 _$E45
// 0x100d93e0 _$E49
// 0x100d93f0 _$E48
// 0x100d9410 _$E52
// 0x100d9420 _$E51

// 0x100d96a0 _$E76
// 0x100d96d0 _$E72
// 0x100d96e0 _$E75
