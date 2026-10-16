#include "DXGfx_PackedTriStripSetD3D.h"
#include "MathConstants.h"
#include "Graph_Stream.h"
#include "StdBase.h"
#include "MBNew.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_TextureAttribute.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_MaterialAttribute.h"
#include "Gfx_View.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d6830 ?GetTypeInfo@PackedTriStripSetD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::PackedTriStripSetD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d6840 ?GetTypeName@PackedTriStripSetD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::PackedTriStripSetD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100d6850 ??0PackedTriStripSetD3D@DXGfx@@QAE@XZ
DXGfx::PackedTriStripSetD3D::PackedTriStripSetD3D() : m_Textured(true), m_nTriangles(0), m_FVF(0), m_DrawFlags(0)
{
}

// 0x100d68f0 ??_GPackedTriStripSetD3D@DXGfx@@UAEPAXI@Z

// 0x100d6920 ??1PackedTriStripSetD3D@DXGfx@@UAE@XZ
DXGfx::PackedTriStripSetD3D::~PackedTriStripSetD3D()
{
}

// TODO 0x100d6960 ?Render@PackedTriStripSetD3D@DXGfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@Gfx@@@Z: 58% - no-alias code: the original reuses the CurrentTransformationMatrix pointer after the changed-flag stores and keeps the strided-data pointers in registers; a unit-wide #pragma optimize("a", on) gives 94% but breaks the ctor, dtor, Clean and the Vec instances; a per-function pragma has no effect
Graph::NodeTraverser::Response DXGfx::PackedTriStripSetD3D::Render(Gfx::RenderTraverser *traverser)
{
    if (traverser->GetMultiTexturePass() > 0 && traverser->GetMultiTexturePass() >= m_TCoords.length())
        return Graph::NodeTraverser::Continue;
    Gfx::View *view = traverser->GetView();
    DX::Device *device = ((DXGfx::ViewD3D *)view)->GetDevice();
    if (Gfx::CurrentTransformationMatrix::GetInstance()->m_ProjectionChanged
        && device->GetCOMInterface()->SetTransform(D3DTRANSFORMSTATE_PROJECTION,
               (D3DMATRIX *)(float *)*Gfx::CurrentTransformationMatrix::GetInstance()) == DD_OK)
        Gfx::CurrentTransformationMatrix::GetInstance()->m_ProjectionChanged = false;
    if (Gfx::CurrentTransformationMatrix::GetInstance()->m_ViewChanged
        && device->GetCOMInterface()->SetTransform(D3DTRANSFORMSTATE_VIEW,
               (D3DMATRIX *)(const float *)Gfx::CurrentTransformationMatrix::GetInstance()->GetViewMatrix()) == DD_OK)
        Gfx::CurrentTransformationMatrix::GetInstance()->m_ViewChanged = false;
    if (Gfx::CurrentTransformationMatrix::GetInstance()->m_ModelChanged
        && device->GetCOMInterface()->SetTransform(D3DTRANSFORMSTATE_WORLD,
               (D3DMATRIX *)(const float *)Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix()) == DD_OK)
        Gfx::CurrentTransformationMatrix::GetInstance()->m_ModelChanged = false;
    HRESULT hr;
    if (Gfx::MaterialAttribute::s_Stack.top()->m_BackfaceCulling != 0) {
        if (m_CullCW)
            hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
        else
            hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
    } else {
        hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    }
    if (hr != DD_OK)
        return Graph::NodeTraverser::Terminate;
    if (m_EnvironmentMapping)
        ApplySphereMap(view->GetCamera(), m_TCoords.length() - 1);
    DWORD zWrite, zFunc;
    int i;
    if (traverser->GetMultiTexture() == 0 && traverser->GetMultiTexturePass() > 0) {
        int pass = traverser->GetMultiTexturePass();
        device->GetRenderState(D3DRENDERSTATE_ZFUNC, &zFunc);
        device->GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &zWrite);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        if (GetTextureBinding() != BindNone) {
            for (i = 0; i < m_Prim.length() - 1; i++) {
                m_StridedData[i].textureCoords[0].lpvData = &m_TCoords[pass][m_Prim[i]];
                m_StridedData[i].textureCoords[0].dwStride = sizeof(Vec3<float>);
            }
        }
    }
    RenderDrawPrimitive(traverser);
    if (traverser->GetMultiTexture() == 0 && traverser->GetMultiTexturePass() > 0) {
        device->SetRenderState(D3DRENDERSTATE_ZFUNC, zFunc);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, zWrite);
        if (GetTextureBinding() != BindNone) {
            for (i = 0; i < m_Prim.length() - 1; i++) {
                m_StridedData[i].textureCoords[0].lpvData = &m_TCoords[0][m_Prim[i]];
                m_StridedData[i].textureCoords[0].dwStride = sizeof(Vec3<float>);
            }
        }
    }
    return Graph::NodeTraverser::Continue;
}

// 0x100d6be0 ?ReadVis@PackedTriStripSetD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::PackedTriStripSetD3D::ReadVis(Graph::Stream *pStream)
{
    PackedTriStripSet::ReadVis(pStream);
}

// 0x100d6bf0 ?Read@PackedTriStripSetD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::PackedTriStripSetD3D::Read(Graph::Stream *pStream)
{
    PackedTriStripSet::Read(pStream);
}

// 0x100d6c00 ?Refresh@PackedTriStripSetD3D@DXGfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void DXGfx::PackedTriStripSetD3D::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    BuildVertexData();
    PackedTriStripSet::Refresh(traverser);
}

// 0x100d6c20 ?Clean@PackedTriStripSetD3D@DXGfx@@IAEXXZ
void DXGfx::PackedTriStripSetD3D::Clean()
{
    m_StridedData.setLength(0);
    m_nTriangles = 0;
    m_FVF = 0;
    m_DrawFlags = 0;
}

// TODO 0x100d6d20 ?BuildVertexData@PackedTriStripSetD3D@DXGfx@@IAEXXZ: 73% - scheduling only: the original loads ColorShift before the m_DrawFlags/m_FVF stores, and keeps case 0 in the jump table at a different base; same stores and cases
void DXGfx::PackedTriStripSetD3D::BuildVertexData()
{
    SetupStrips();
    if (m_nTriangles < 1)
        return;
    m_FVF = D3DFVF_XYZ;
    m_DrawFlags = D3DDP_DONOTUPDATEEXTENTS;
    if (GetColorBinding() != BindNone) {
        m_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
        m_DrawFlags = D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTLIGHT;
    } else if (GetNormalBinding() != BindNone) {
        m_FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
    }
    if (GetTextureBinding() == BindNone)
        return;
    switch (m_TCoords.length()) {
    case 0:
        m_FVF |= D3DFVF_TEX0;
        break;
    case 1:
        m_FVF |= D3DFVF_TEX1;
        break;
    case 2:
        m_FVF |= D3DFVF_TEX2;
        break;
    case 3:
        m_FVF |= D3DFVF_TEX3;
        break;
    case 4:
        m_FVF |= D3DFVF_TEX4;
        break;
    case 5:
        m_FVF |= D3DFVF_TEX5;
        break;
    case 6:
        m_FVF |= D3DFVF_TEX6;
        break;
    case 7:
        m_FVF |= D3DFVF_TEX7;
        break;
    case 8:
        m_FVF |= D3DFVF_TEX8;
        break;
    }
}

// TODO 0x100d6e80 ?SetupStrips@PackedTriStripSetD3D@DXGfx@@IAEXXZ: 31% - no-alias code: the original reads the three bindings once before the strip loop (short stack slots) and keeps m_Prim/m_StridedData pointers in registers; a unit-wide #pragma optimize("a", on) gives 69%; locals for the bindings give 35-42%
void DXGfx::PackedTriStripSetD3D::SetupStrips()
{
    m_StridedData.setLength(m_Prim.length() - 1);
    m_nTriangles = 0;
    for (int i = 0; i < m_Prim.length() - 1; i++) {
        m_nTriangles += m_Prim[i + 1] - m_Prim[i] - 2;
        D3DDRAWPRIMITIVESTRIDEDDATA &data = m_StridedData[i];
        data.position.lpvData = &m_Coords[m_Prim[i]];
        data.position.dwStride = sizeof(Vec3<float>);
        if (GetColorBinding() != BindNone) {
            data.diffuse.lpvData = &m_DiffuseColors[m_Prim[i]];
            data.diffuse.dwStride = sizeof(unsigned long);
            data.specular.lpvData = &m_SpecularColors[m_Prim[i]];
            data.specular.dwStride = sizeof(unsigned long);
        }
        if (GetNormalBinding() != BindNone) {
            data.normal.lpvData = &m_Normals[m_Prim[i]];
            data.normal.dwStride = sizeof(Vec3<float>);
        }
        if (GetTextureBinding() != BindNone) {
            for (int stage = 0; stage < m_TCoords.length(); stage++) {
                data.textureCoords[stage].lpvData = &m_TCoords[stage][m_Prim[i]];
                data.textureCoords[stage].dwStride = sizeof(Vec3<float>);
            }
        }
    }
}

// TODO 0x100d7050 ?RenderDrawPrimitive@PackedTriStripSetD3D@DXGfx@@IAE_NPAVRenderTraverser@Gfx@@@Z: 49% - no-alias code: the original keeps the strip count, m_FVF and m_DrawFlags in stack slots across the draw calls; a unit-wide #pragma optimize("a", on) gives 87%
bool DXGfx::PackedTriStripSetD3D::RenderDrawPrimitive(Gfx::RenderTraverser *traverser)
{
    if (Gfx::TextureAttribute::GetStack(0).size() != 0
        && m_Textured != Gfx::TextureAttribute::GetStack(0).top()->WantTexturing()) {
        Clean();
        BuildVertexData();
    }
    traverser->AddPackedTriStripSet(1);
    traverser->AddTriStrips(m_Prim.length() - 1);
    traverser->AddTris(m_nTriangles);
    DX::Device *device = ((DXGfx::ViewD3D *)traverser->GetView())->GetDevice();
    for (int i = 0; i < m_Prim.length() - 1; i++) {
        if (device->GetCOMInterface()->DrawPrimitiveStrided(D3DPT_TRIANGLESTRIP, m_FVF, &m_StridedData[i],
                                                           m_Prim[i + 1] - m_Prim[i], m_DrawFlags) != DD_OK)
            return false;
    }
    return true;
}

// 0x100d7140 ?SetDrawFunction@PackedTriStripSetD3D@DXGfx@@UAEXXZ
void DXGfx::PackedTriStripSetD3D::SetDrawFunction()
{
}

// 0x100d6740 _$E50
// 0x100d6750 _$E49
// 0x100d6770 _$E53
// 0x100d6780 _$E52
// 0x100d67a0 _$E56
// 0x100d67b0 _$E55
// 0x100d67d0 _$E59
// 0x100d67e0 _$E58
// 0x100d6800 _$E62
// 0x100d6810 _$E61

// 0x100d7300 _$E117
// 0x100d7330 _$E113
// 0x100d7340 _$E116

// Template instances this unit also emits:
// 0x100d7150 ??_G?$Vec@U_D3DDRAWPRIMITIVESTRIDEDDATA@@@@UAEPAXI@Z
// 0x100d7180 ??1?$Vec@U_D3DDRAWPRIMITIVESTRIDEDDATA@@@@UAE@XZ
// 0x100d71b0 ?reallocMem@?$Vec@U_D3DDRAWPRIMITIVESTRIDEDDATA@@@@IAEXHHH@Z
// TODO 0x100d72b0 ?initElements@?$Vec@U_D3DDRAWPRIMITIVESTRIDEDDATA@@@@IAEXHH@Z: 46% - rep movsd copy of the zero element only with /Oa at the end of the unit (template bodies), which breaks reallocMem
