#include "DXGfx_ShadowAttributeD3D.h"
#include "StdBase.h"
#include "Graph_PassiveTraverser.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "d3d7.h"
#include "MBNew.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d96f0 ?GetTypeInfo@ShadowAttributeD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::ShadowAttributeD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d9700 ?GetTypeName@ShadowAttributeD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::ShadowAttributeD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100d9710 ??0ShadowAttributeD3D@DXGfx@@QAE@XZ
DXGfx::ShadowAttributeD3D::ShadowAttributeD3D()
{
    m_Matrix.identity();
    Vec4<float> black(0.0f, 0.0f, 0.0f, 0.0f);
    Vec4<float> shadowColor(0.0f, 0.0f, 0.0f, 0.7f);
    m_Ambient = black;
    m_Diffuse = shadowColor;
    m_Power = 0.0f;
    m_Specular = black;
    m_Emissive = black;
}

// 0x100d98a0 ??_GShadowAttributeD3D@DXGfx@@MAEPAXI@Z

// 0x100d98d0 ??1ShadowAttributeD3D@DXGfx@@MAE@XZ
DXGfx::ShadowAttributeD3D::~ShadowAttributeD3D()
{
}

// TODO 0x100d98e0 ?Push@ShadowAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z: 92% similar: only the TMatrix4 temporaries differ: the original builds the results of operator* and operator- with an out-of-line setValues straight into the result, VC6 copies them through a temporary (rep movsd plus the copy constructor); SquareMatrix(const T *) calling setValues changes nothing
void DXGfx::ShadowAttributeD3D::Push(Graph::PassiveTraverser *traverser) const
{
    Vec4<float> plane(m_Plane.GetNormal(), m_Plane.GetOffset());
    Vec4<float> light(1.0f, 1.0f, 1.0f, 0.0f);
    TMatrix4<float> shadow;
    shadow.outerProduct(plane, light);
    float scale = 1.0f / plane.dot(light);
    shadow = m_Matrix - shadow * scale;
    Gfx::CurrentTransformationMatrix::GetInstance()->PushRight(shadow);
    Gfx::RenderTraverser *renderTraverser = StdBaseCast(traverser, (Gfx::RenderTraverser *)0);
    if (renderTraverser) {
        DX::Device *device = ((DXGfx::ViewD3D *)renderTraverser->GetView())->GetDevice();
        renderTraverser->AddMaterial(1);
        device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
        device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, 0x66000000);
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
        device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        device->GetCOMInterface()->SetMaterial((D3DMATERIAL7 *)&m_Diffuse);
    }
    ShadowAttribute::Push(traverser);
}

// 0x100d9b10 ?Pop@ShadowAttributeD3D@DXGfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void DXGfx::ShadowAttributeD3D::Pop(Graph::PassiveTraverser *traverser) const
{
    Gfx::CurrentTransformationMatrix::GetInstance()->Pop();
    ShadowAttribute::Pop(traverser);
}

// Matched by this code (template and inline instances):
// 0x100d9820 ??9ShadowAttribute@Gfx@@UAE_NABV01@@Z
// 0x100d9840 ??8ShadowAttribute@Gfx@@UBE_NABV01@@Z
// 0x100d9b30 ?subtractMatrix@?$SquareMatrix@M$03@@QAEXABV1@@Z
// 0x100d9b50 ?multScalar@?$SquareMatrix@M$03@@QAEXABM@Z

// 0x100d9b80 _$E63
// 0x100d9bb0 _$E59
// 0x100d9bc0 _$E62

// the run-time type of DXGfx::ShadowAttributeD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::ShadowAttributeD3D::s_TypeInfo = { "DXGfx::ShadowAttributeD3D", &Gfx::ShadowAttribute::s_TypeInfo };
