#include "DXGfx_PolyLineSetD3D.h"
#include "MathConstants.h"
#include "Graph_Stream.h"
#include "StdBase.h"
#include "MBNew.h"
#include "Gfx_RenderTraverser.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_MaterialAttribute.h"
#include "Gfx_View.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d8080 ?GetTypeInfo@PolyLineSetD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::PolyLineSetD3D::GetTypeInfo() const
{
    return &DXGfx::PolyLineSetD3D::s_TypeInfo;
}

// 0x100d8090 ?GetTypeName@PolyLineSetD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::PolyLineSetD3D::GetTypeName() const
{
    return DXGfx::PolyLineSetD3D::s_TypeInfo.typeName;
}

// EQUIV 0x100d80a0 ??0PolyLineSetD3D@DXGfx@@QAE@XZ: reviewed: identical code; only the EH unwind funclets address the base and member objects at other ebp offsets (stack slots)
DXGfx::PolyLineSetD3D::PolyLineSetD3D()
    : m_VerticesChanged(true), m_Dirty(true), m_IgnoreMaterial(false), m_nVertices(0), m_nLineSegments(0)
{
}

// 0x100d8190 ?SetDirty@PolyLineSetD3D@DXGfx@@UAEXXZ
void DXGfx::PolyLineSetD3D::SetDirty()
{
    m_Dirty = true;
}

// 0x100d81a0 ??_GPolyLineSetD3D@DXGfx@@UAEPAXI@Z

// 0x100d81d0 ??1PolyLineSetD3D@DXGfx@@UAE@XZ
DXGfx::PolyLineSetD3D::~PolyLineSetD3D()
{
}

// TODO 0x100d8260 ?Render@PolyLineSetD3D@DXGfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@Gfx@@@Z: 91% - the original reuses the CurrentTransformationMatrix pointer after each changed-flag store (no-alias style), ours reloads s_pInstance; a unit-wide /Oa gets 87% and breaks Clean, SetupStrips stays; same calls and arguments
Graph::NodeTraverser::Response DXGfx::PolyLineSetD3D::Render(Gfx::RenderTraverser *traverser)
{
    DX::Device *device = ((DXGfx::ViewD3D *)traverser->GetView())->GetDevice();
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
    if (device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE) != DD_OK)
        return Graph::NodeTraverser::Terminate;
    RenderDrawPrimitive(traverser);
    return Graph::NodeTraverser::Continue;
}

// 0x100d8320 ?ReadVis@PolyLineSetD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::PolyLineSetD3D::ReadVis(Graph::Stream * stream)
{
    PolyLineSet::ReadVis(stream);
}

// 0x100d8330 ?Read@PolyLineSetD3D@DXGfx@@UAEXPAVStream@Graph@@@Z
void DXGfx::PolyLineSetD3D::Read(Graph::Stream * stream)
{
    PolyLineSet::Read(stream);
}

// 0x100d8340 ?Refresh@PolyLineSetD3D@DXGfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void DXGfx::PolyLineSetD3D::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    m_Dirty = true;
    PolyLineSet::Refresh(traverser);
}

// 0x100d8360 ?Rebuild@PolyLineSetD3D@DXGfx@@IAEXXZ
void DXGfx::PolyLineSetD3D::Rebuild()
{
    int current = m_UnlitVertices.length();
    if (current <= 0)
        current = m_LitVertices.length();
    if (current == 0) {
        BuildVertexData();
    } else {
        int n = m_CList.length() / m_Stride;
        if (n > current)
            AppendVertices(n - current);
        else if (n < current)
            ChopVertices(current - n);
        else
            CopyVertices();
    }
    m_Dirty = false;
}

// 0x100d83e0 ?Clean@PolyLineSetD3D@DXGfx@@IAEXXZ
void DXGfx::PolyLineSetD3D::Clean()
{
    m_StripLengths.setLength(0);
    m_LitVertices.setLength(0);
    m_UnlitVertices.setLength(0);
    m_nStrips = 0;
    m_nVertices = 0;
    m_nLineSegments = 0;
    m_Dirty = true;
}

// EQUIV 0x100d8550 ?BuildVertexData@PolyLineSetD3D@DXGfx@@IAEXXZ: reviewed: register choice in the inlined setLength copy loops (the original adds the element offset straight into esi/edi, VC6 here goes through ecx/ebp with one more mov; ebp is dead there) and the place of one inc; same calls, stores and control flow
void DXGfx::PolyLineSetD3D::BuildVertexData()
{
    const Vec4<float> &diffuse = Gfx::MaterialAttribute::s_Stack.top()->m_Diffuse;
    m_IgnoreMaterial = false;
    SetupStrips();
    int i;
    if (GetColorBinding() != 1) {
        m_LitVertices.setLength(m_nVertices);
        for (i = 0; i < m_nVertices; i++)
            CopyLitVertex(i, diffuse);
    } else {
        m_UnlitVertices.setLength(m_nVertices);
        for (i = 0; i < m_nVertices; i++)
            CopyUnlitVertex(i);
    }
}

// TODO 0x100d87e0 ?AppendVertices@PolyLineSetD3D@DXGfx@@IAEXH@Z: 59% - inline budget: same structure (setLength with out-of-line reallocMem), but the clamp temporary is spilled and registers differ
void DXGfx::PolyLineSetD3D::AppendVertices(int n)
{
    SetupStrips();
    int i;
    if (GetColorBinding() == 1) {
        int start = m_UnlitVertices.length();
        int end = start + n;
        m_UnlitVertices.setLength(end);
        for (i = start; i < end; i++)
            CopyUnlitVertex(i);
    } else {
        int start = m_LitVertices.length();
        int end = start + n;
        m_LitVertices.setLength(end);
        const Vec4<float> &diffuse = Gfx::MaterialAttribute::s_Stack.top()->m_Diffuse;
        for (i = start; i < end; i++)
            CopyLitVertex(i, diffuse);
    }
}

// TODO 0x100d8950 ?ChopVertices@PolyLineSetD3D@DXGfx@@IAEXH@Z: 41% - inline budget: our copy inlines Vec::reallocMem into the setLength, the original calls it and inlines only initElements
void DXGfx::PolyLineSetD3D::ChopVertices(int n)
{
    SetupStrips();
    if (GetColorBinding() == 1)
        m_UnlitVertices.setLength(m_UnlitVertices.length() - n);
    else
        m_LitVertices.setLength(m_LitVertices.length() - n);
}

// TODO 0x100d8a40 ?SetupStrips@PolyLineSetD3D@DXGfx@@IAEXXZ: 77% - matches with a unit-wide #pragma optimize("a", on) (the strip loop keeps m_Prim/m_StripLengths in registers), which breaks Clean, BuildVertexData and the Vec instances; a per-function pragma has no effect
void DXGfx::PolyLineSetD3D::SetupStrips()
{
    m_nStrips = m_Prim.length() - 1;
    m_StripLengths.setLength(m_nStrips);
    m_nVertices = m_CList.length() / m_Stride;
    m_nLineSegments = 0;
    for (int i = 0; i < m_nStrips; i++) {
        m_StripLengths[i] = m_Prim[i + 1] - m_Prim[i];
        m_nLineSegments += m_StripLengths[i] - 1;
    }
}

// TODO 0x100d8af0 ?CopyVertices@PolyLineSetD3D@DXGfx@@IAEXXZ: 85% - register allocation: the original keeps 0 in ecx for the compares and the m_ChangedVertices reset, ours uses immediates
void DXGfx::PolyLineSetD3D::CopyVertices()
{
    int start = 0;
    int end = m_nVertices;
    if (m_ChangedVertices > 0)
        end = m_ChangedVertices;
    else if (m_ChangedVertices < 0)
        start = m_ChangedVertices + end;
    m_ChangedVertices = 0;
    int i;
    if ((D3DVERTEX *)m_UnlitVertices) {
        for (i = start; i < end; i++)
            CopyUnlitVertex(i);
    } else {
        const Vec4<float> &diffuse = Gfx::MaterialAttribute::s_Stack.top()->m_Diffuse;
        for (i = start; i < end; i++)
            CopyLitVertex(i, diffuse);
    }
}

// EQUIV 0x100d8b70 ?CopyUnlitVertex@PolyLineSetD3D@DXGfx@@IAEXH@Z: reviewed: the original reads m_Binding for the texture test before the position and normal stores into the vertex (independent memory); otherwise register choice and the order of the final pops
void DXGfx::PolyLineSetD3D::CopyUnlitVertex(int i)
{
    int base = m_Stride * i;
    float *c = &m_CList[0];
    D3DVERTEX &v = m_UnlitVertices[i];
    v.x = c[base];
    v.y = c[base + 1];
    v.z = c[base + 2];
    int n = m_NormalOffset + base;
    v.nx = -c[n];
    v.ny = -c[n + 1];
    v.nz = -c[n + 2];
    if (GetTextureBinding() != 1) {
        int t = m_TextureOffset + base;
        v.tu = c[t];
        v.tv = c[t + 1];
    } else {
        v.tu = 1.0f;
        v.tv = 1.0f;
    }
}

// TODO 0x100d8c20 ?CopyLitVertex@PolyLineSetD3D@DXGfx@@IAEXHABV?$Vec4@M@@@Z: 57% - register allocation: the original keeps the colour offset in a stack slot and the vertex pointer in edi; same expression order and calls
void DXGfx::PolyLineSetD3D::CopyLitVertex(int i, const Vec4<float> &diffuse)
{
    int base = m_Stride * i;
    float *c = &m_CList[0];
    D3DLVERTEX &v = m_LitVertices[i];
    v.x = c[base];
    v.y = c[base + 1];
    v.z = c[base + 2];
    int k = m_ColorOffset + base;
    if (m_IgnoreMaterial)
        v.color = ((((int)(c[k + 3] * 255.0f) << 8 | (int)(c[k] * 255.0f)) << 8
                    | (int)(c[k + 1] * 255.0f)) << 8)
            | (int)(c[k + 2] * 255.0f);
    else
        v.color = ((((int)(c[k + 3] * diffuse.e[3] * 255.0f) << 8 | (int)(c[k] * diffuse.e[0] * 255.0f)) << 8
                    | (int)(c[k + 1] * diffuse.e[1] * 255.0f)) << 8)
            | (int)(c[k + 2] * diffuse.e[2] * 255.0f);
    v.specular = 0xff000000;
    if (GetTextureBinding() != 1) {
        int t = m_TextureOffset + base;
        v.tu = c[t];
        v.tv = c[t + 1];
    } else {
        v.tu = 1.0f;
        v.tv = 1.0f;
    }
}

// EQUIV 0x100d8d90 ?RenderDrawPrimitive@PolyLineSetD3D@DXGfx@@IAE_NPAVRenderTraverser@Gfx@@@Z: reviewed: the original keeps 0 in ebx for the flag tests and stores (immediates here), keeps the strip index and count in the other of the two stack slots and stores i = 0 before the count test; same calls, arguments and control flow
bool DXGfx::PolyLineSetD3D::RenderDrawPrimitive(Gfx::RenderTraverser *traverser)
{
    if (m_Dirty)
        Rebuild();
    else if (m_VerticesChanged)
        CopyVertices();
    m_VerticesChanged = false;
    DX::Device *device = ((DXGfx::ViewD3D *)traverser->GetView())->GetDevice();
    int i;
    int nStrips;
    if ((D3DVERTEX *)m_UnlitVertices) {
        nStrips = m_nStrips;
        if (nStrips > 0) {
            int *prim = &m_Prim[0];
            D3DVERTEX *vertices = &m_UnlitVertices[0];
            int *lengths = &m_StripLengths[0];
            IDirect3DDevice7 *d3d = device->GetCOMInterface();
            for (i = 0; i < nStrips; i++) {
                if (d3d->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_VERTEX, &vertices[prim[i]], lengths[i], D3DDP_DONOTUPDATEEXTENTS)
                    != DD_OK)
                    return false;
            }
        }
    } else {
        nStrips = m_nStrips;
        if (nStrips > 0) {
            int *prim = &m_Prim[0];
            D3DLVERTEX *vertices = &m_LitVertices[0];
            int *lengths = &m_StripLengths[0];
            IDirect3DDevice7 *d3d = device->GetCOMInterface();
            for (i = 0; i < nStrips; i++) {
                if (d3d->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_LVERTEX, &vertices[prim[i]], lengths[i], D3DDP_DONOTUPDATEEXTENTS)
                    != DD_OK)
                    return false;
            }
        }
    }
    return true;
}

// 0x100d8ec0 ?SetDrawFunction@PolyLineSetD3D@DXGfx@@UAEXXZ
void DXGfx::PolyLineSetD3D::SetDrawFunction()
{
    m_VerticesChanged = true;
}

// 0x100d7f90 _$E50
// 0x100d7fa0 _$E49
// 0x100d7fc0 _$E53
// 0x100d7fd0 _$E52
// 0x100d7ff0 _$E56
// 0x100d8000 _$E55
// 0x100d8020 _$E59
// 0x100d8030 _$E58
// 0x100d8050 _$E62
// 0x100d8060 _$E61

// 0x100d9300 _$E111
// 0x100d9330 _$E107
// 0x100d9340 _$E110

// Template and inline instances this unit also emits:
// 0x100d8ed0 ??1?$Vec@H@@UAE@XZ
// 0x100d8f00 ??1?$Vec@U_D3DVERTEX@@@@UAE@XZ
// 0x100d8f30 ??_G?$Vec@H@@UAEPAXI@Z
// 0x100d8f60 ??_G?$Vec@U_D3DVERTEX@@@@UAEPAXI@Z
// 0x100d8f90 ??_G?$Vec@U_D3DLVERTEX@@@@UAEPAXI@Z
// 0x100d8fc0 ??1?$Vec@U_D3DLVERTEX@@@@UAE@XZ
// 0x100d8ff0 ?reallocMem@?$Vec@H@@IAEXHHH@Z
// TODO 0x100d90b0 ?initElements@?$Vec@H@@IAEXHH@Z: 56% - rep stosd only with /Oa at the end of the unit (template bodies), which breaks the reallocMem instances
// 0x100d90d0 ?reallocMem@?$Vec@U_D3DVERTEX@@@@IAEXHHH@Z
// TODO 0x100d91c0 ?reallocMem@?$Vec@U_D3DLVERTEX@@@@IAEXHHH@Z: not emitted: our copy inlines Vec<D3DLVERTEX>::reallocMem at every call (inline budget), and emits initElements<D3DVERTEX> instead
// TODO 0x100d92b0 ?initElements@?$Vec@U_D3DLVERTEX@@@@IAEXHH@Z: 62% - rep stosd only with /Oa at the end of the unit (template bodies), which breaks the reallocMem instances

// the run-time type of DXGfx::PolyLineSetD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::PolyLineSetD3D::s_TypeInfo = { "DXGfx::PolyLineSetD3D", &Gfx::PolyLineSet::s_TypeInfo };
