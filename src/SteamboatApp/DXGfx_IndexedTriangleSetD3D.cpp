#include "DXGfx_IndexedTriangleSetD3D.h"
#include "MathConstants.h"
#include "Gfx_IndexedTriangleSet.h"
#include "Graph_Stream.h"
#include "StdBase.h"
#include "MBNew.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_TextureAttribute.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_MaterialAttribute.h"
#include "Gfx_VertexClusterAttribute.h"
#include "Gfx_AttributeStack.h"
#include "Gfx_View.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d7440 ?GetTypeInfo@IndexedTriangleSetD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::IndexedTriangleSetD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d7450 ?GetTypeName@IndexedTriangleSetD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::IndexedTriangleSetD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100d7460 ??0IndexedTriangleSetD3D@DXGfx@@QAE@XZ
DXGfx::IndexedTriangleSetD3D::IndexedTriangleSetD3D() : m_Textured(true), m_nTriangles(0), m_FVF(0), m_DrawFlags(0)
{
}

// 0x100d74c0 ??_GIndexedTriangleSetD3D@DXGfx@@UAEPAXI@Z

// (the three converting constructors leave m_Textured uninitialized)
// 0x100d74f0 ??0IndexedTriangleSetD3D@DXGfx@@QAE@AAVPackedTriStripSet@Gfx@@@Z
DXGfx::IndexedTriangleSetD3D::IndexedTriangleSetD3D(Gfx::PackedTriStripSet &set)
    : IndexedTriangleSet(set), m_nTriangles(0), m_FVF(0), m_DrawFlags(0)
{
}

// 0x100d7550 ??0IndexedTriangleSetD3D@DXGfx@@QAE@ABVBoundingBox@Gfx@@@Z
DXGfx::IndexedTriangleSetD3D::IndexedTriangleSetD3D(const Gfx::BoundingBox &box)
    : IndexedTriangleSet(box), m_nTriangles(0), m_FVF(0), m_DrawFlags(0)
{
}

// 0x100d75b0 ??0IndexedTriangleSetD3D@DXGfx@@QAE@ABVBoundingSphere@Gfx@@@Z
DXGfx::IndexedTriangleSetD3D::IndexedTriangleSetD3D(const Gfx::BoundingSphere &sphere)
    : IndexedTriangleSet(sphere), m_nTriangles(0), m_FVF(0), m_DrawFlags(0)
{
}

// 0x100d7610 ??1IndexedTriangleSetD3D@DXGfx@@UAE@XZ
DXGfx::IndexedTriangleSetD3D::~IndexedTriangleSetD3D()
{
}

// Hands the changed matrices to the device, sets the cull mode from the
// current material and draws; in a multi-texture pass (single-texture
// hardware) the pass's texture coordinates are drawn without z writes.
// TODO 0x100d7650 ?Render@IndexedTriangleSetD3D@DXGfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@Gfx@@@Z: register allocation and scheduling only (66% similar): the original builds the view-matrix argument in ecx before loading the COM interface and keeps the camera/texture-stage values in other registers; same calls, arguments and stores
Graph::NodeTraverser::Response DXGfx::IndexedTriangleSetD3D::Render(Gfx::RenderTraverser *traverser)
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
    if (Gfx::MaterialAttribute::s_Stack.size() != 0 && Gfx::MaterialAttribute::s_Stack.top()->m_BackfaceCulling != 0) {
        if (m_CullCW)
            hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
        else
            hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
    } else {
        hr = device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    }
    if (hr != DD_OK)
        return Graph::NodeTraverser::Terminate;
    if (Gfx::VertexClusterAttribute::GetStack() && Gfx::VertexClusterAttribute::GetStack()->GetTop())
        Gfx::VertexClusterAttribute::HandleClusterStack(this, traverser->GetFrameTime());
    if (m_EnvironmentMapping)
        ApplySphereMap(view->GetCamera(), m_TCoords.length() - 1);
    unsigned long zWrite, zFunc;
    if (traverser->GetMultiTexture() == 0 && traverser->GetMultiTexturePass() > 0) {
        int pass = traverser->GetMultiTexturePass();
        device->GetRenderState(D3DRENDERSTATE_ZFUNC, &zFunc);
        device->GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &zWrite);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        if (GetTextureBinding() != BindNone) {
            m_StridedData.textureCoords[0].lpvData = &m_TCoords[pass][0];
            m_StridedData.textureCoords[0].dwStride = sizeof(Vec3<float>);
        }
    }
    RenderDrawPrimitive(traverser);
    if (traverser->GetMultiTexture() == 0 && traverser->GetMultiTexturePass() > 0) {
        device->SetRenderState(D3DRENDERSTATE_ZFUNC, zFunc);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, zWrite);
        if (GetTextureBinding() != BindNone)
            m_StridedData.textureCoords[0].lpvData = &m_TCoords[0][0];
    }
    return Graph::NodeTraverser::Continue;
}

// 0x100d7880 ?Read@IndexedTriangleSetD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::IndexedTriangleSetD3D::Read(Graph::Stream * stream)
{
    IndexedTriangleSet::Read(stream);
}

// 0x100d7890 ?Refresh@IndexedTriangleSetD3D@DXGfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void DXGfx::IndexedTriangleSetD3D::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    BuildVertexData();
    IndexedTriangleSet::Refresh(traverser);
}

// 0x100d78b0 ?Clean@IndexedTriangleSetD3D@DXGfx@@IAEXXZ
void DXGfx::IndexedTriangleSetD3D::Clean()
{
    m_D3DIndices.setLength(0);
    m_nVerts = 0;
    m_nTriangles = 0;
    m_FVF = 0;
}

// The strided vertex description and the 16-bit index copy for Direct3D.
// TODO 0x100d7970 ?BuildVertexData@IndexedTriangleSetD3D@DXGfx@@IAEXXZ: no-alias code: the inlined initElements is rep stosd/stosw and the copy loop keeps its pointers in registers only with the pragma at the top of the unit (68% then), which breaks 7 other claims of the unit; a pragma around the function changes nothing
void DXGfx::IndexedTriangleSetD3D::BuildVertexData()
{
    if (m_D3DIndices.length() != m_Indices.length())
        m_D3DIndices.setLength(m_Indices.length());
    for (int i = 0; i < m_Indices.length(); i++)
        m_D3DIndices[i] = (unsigned short)m_Indices[i];
    m_nTriangles = m_Prim.length() - 1;
    if (m_nTriangles < 1)
        return;
    m_StridedData.position.lpvData = (Vec3<float> *)m_Coords;
    m_StridedData.position.dwStride = sizeof(Vec3<float>);
    m_FVF = D3DFVF_XYZ;
    if (GetColorBinding() != BindNone) {
        m_StridedData.diffuse.lpvData = (unsigned long *)m_DiffuseColors;
        m_StridedData.diffuse.dwStride = sizeof(unsigned long);
        m_StridedData.specular.lpvData = (unsigned long *)m_SpecularColors;
        m_StridedData.specular.dwStride = sizeof(unsigned long);
        m_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
    } else if (GetNormalBinding() != BindNone) {
        m_StridedData.normal.lpvData = (Vec3<float> *)m_Normals;
        m_StridedData.normal.dwStride = sizeof(Vec3<float>);
        m_FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
    }
    if (GetTextureBinding() == BindNone)
        return;
    for (int stage = 0; stage < m_TCoords.length(); stage++) {
        m_StridedData.textureCoords[stage].lpvData = (Vec3<float> *)m_TCoords[stage];
        m_StridedData.textureCoords[stage].dwStride = sizeof(Vec3<float>);
    }
    switch (m_TCoords.length()) {
    case 0:
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

// 0x100d7bd0 ?CopyVertices@IndexedTriangleSetD3D@DXGfx@@IAEXXZ
void DXGfx::IndexedTriangleSetD3D::CopyVertices()
{
}

// Draws the triangle list; when the shape wants its screen rectangle the
// clip-status extents are collected around the call (initialized inverted:
// minimum = viewport size, maximum = viewport origin).
// TODO 0x100d7be0 ?RenderDrawPrimitive@IndexedTriangleSetD3D@DXGfx@@IAE_NPAVRenderTraverser@Gfx@@@Z: register allocation of the draw-call arguments (the original spills m_FVF to a stack slot and reuses esi for &m_StridedData) and the texture-stack size test (sar vs test); same calls, arguments and stores (83% similar)
bool DXGfx::IndexedTriangleSetD3D::RenderDrawPrimitive(Gfx::RenderTraverser *traverser)
{
    if (Gfx::TextureAttribute::GetStack(0).size() != 0
        && m_Textured != Gfx::TextureAttribute::GetStack(0).top()->WantTexturing()) {
        Clean();
        BuildVertexData();
    }
    traverser->AddTris(m_nTriangles);
    DX::Device *device = ((DXGfx::ViewD3D *)traverser->GetView())->GetDevice();
    if (!GetCalculateScreenRect()) {
        if (device->GetCOMInterface()->DrawIndexedPrimitiveStrided(D3DPT_TRIANGLELIST, m_FVF, &m_StridedData,
                m_Coords.length(), &m_D3DIndices[0], m_D3DIndices.length(), m_DrawFlags) != DD_OK)
            return false;
    } else {
    device->SetRenderState(D3DRENDERSTATE_EXTENTS, TRUE);
    IDirect3DDevice7 *d3dDevice = device->GetCOMInterface();
    D3DVIEWPORT7 viewport;
    memset(&viewport, 0, sizeof(viewport));
    d3dDevice->GetViewport(&viewport);
    D3DCLIPSTATUS clipStatus;
    memset(&clipStatus, 0, sizeof(clipStatus));
    clipStatus.dwFlags = D3DCLIPSTATUS_EXTENTS2;
    clipStatus.minx = (float)viewport.dwWidth;
    clipStatus.miny = (float)viewport.dwHeight;
    clipStatus.maxx = (float)viewport.dwX;
    clipStatus.maxy = (float)viewport.dwY;
    d3dDevice->SetClipStatus(&clipStatus);
    HRESULT hr = d3dDevice->DrawIndexedPrimitiveStrided(D3DPT_TRIANGLELIST, m_FVF, &m_StridedData,
        m_Coords.length(), &m_D3DIndices[0], m_D3DIndices.length(), m_DrawFlags);
    memset(&clipStatus, 0, sizeof(clipStatus));
    clipStatus.dwFlags = D3DCLIPSTATUS_EXTENTS2;
    d3dDevice->GetClipStatus(&clipStatus);
    mScreenRect.left = (long)clipStatus.minx;
    mScreenRect.top = (long)clipStatus.miny;
    mScreenRect.right = (long)(clipStatus.maxx + 1.0f);
    mScreenRect.bottom = (long)(clipStatus.maxy + 1.0f);
    device->SetRenderState(D3DRENDERSTATE_EXTENTS, FALSE);
    if (hr != DD_OK)
        return false;
    mScreenRectValid = true;
    }
    return true;
}

// 0x100d7e10 ?SetDrawFunction@IndexedTriangleSetD3D@DXGfx@@UAEXXZ
void DXGfx::IndexedTriangleSetD3D::SetDrawFunction()
{
}

// 0x100d7e20 ??_G?$Vec@G@@UAEPAXI@Z
// 0x100d7e50 ??1?$Vec@G@@UAE@XZ
// 0x100d7e80 ?reallocMem@?$Vec@G@@IAEXHHH@Z
// 0x100d7350 _$E50
// 0x100d7360 _$E49
// 0x100d7380 _$E53
// 0x100d7390 _$E52
// 0x100d73b0 _$E56
// 0x100d73c0 _$E55
// 0x100d73e0 _$E59
// 0x100d73f0 _$E58
// 0x100d7410 _$E62
// 0x100d7420 _$E61
// 0x100d7f40 _$E123
// 0x100d7f70 _$E119
// 0x100d7f80 _$E122