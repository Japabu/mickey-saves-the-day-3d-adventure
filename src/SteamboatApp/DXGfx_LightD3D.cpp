// evidence: the setters load the last two components before storing them (no-alias code, as in
// the other Graphics.lib units)
#pragma optimize("a", on)
#include "DXGfx_LightD3D.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "MBNew.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d6280 ?GetTypeInfo@LightD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::LightD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100d6290 ?GetTypeName@LightD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::LightD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100d62a0 ??0LightD3D@DXGfx@@QAE@H@Z
DXGfx::LightD3D::LightD3D(int index)
    : m_Index(index), m_pView(0)
{
    memset(&m_Light, 0, sizeof(m_Light));
    m_Light.dcvDiffuse.r = 1.0f;
    m_Light.dcvDiffuse.g = 1.0f;
    m_Light.dcvDiffuse.b = 1.0f;
    m_Light.dcvDiffuse.a = 1.0f;
    m_Light.dcvAmbient.r = 1.0f;
    m_Light.dcvAmbient.g = 1.0f;
    m_Light.dcvAmbient.b = 1.0f;
    m_Light.dcvAmbient.a = 1.0f;
    m_Light.dvAttenuation0 = 1.0f;
    m_Light.dvDirection.x = 1.0f;
    m_Light.dvPosition.x = 0.0f;
    m_Light.dvPosition.y = 0.0f;
    m_Light.dvPosition.z = 0.0f;
    m_Light.dvAttenuation1 = 0.0f;
    m_Light.dvAttenuation2 = 0.0f;
    m_Light.dvDirection.y = 0.0f;
    m_Light.dvDirection.z = 0.0f;
    m_Light.dltType = D3DLIGHT_DIRECTIONAL;
}

// 0x100d6350 ??_GLightD3D@DXGfx@@UAEPAXI@Z
// 0x100d6380 ??1LightD3D@DXGfx@@UAE@XZ
DXGfx::LightD3D::~LightD3D()
{
}

// 0x100d6390 ??_GLight@Gfx@@UAEPAXI@Z
// 0x100d63c0 ??1Light@Gfx@@UAE@XZ

// 0x100d63d0 ?SetPosition@LightD3D@DXGfx@@UAEXABV?$Vec3@M@@@Z
void DXGfx::LightD3D::SetPosition(const Vec3<float> &position)
{
    m_Light.dvPosition.x = position.e[0];
    m_Light.dvPosition.y = position.e[1];
    m_Light.dvPosition.z = position.e[2];
}

// 0x100d6400 ?GetPosition@LightD3D@DXGfx@@UAE?BV?$Vec3@M@@XZ
const Vec3<float> DXGfx::LightD3D::GetPosition()
{
    return Vec3<float>(m_Light.dvPosition.x, m_Light.dvPosition.y, m_Light.dvPosition.z);
}

// 0x100d6450 ?SetDirection@LightD3D@DXGfx@@UAEXABV?$Vec3@M@@@Z
void DXGfx::LightD3D::SetDirection(const Vec3<float> &direction)
{
    m_Light.dvDirection.x = direction.e[0];
    m_Light.dvDirection.y = direction.e[1];
    m_Light.dvDirection.z = direction.e[2];
}

// 0x100d6480 ?GetDirection@LightD3D@DXGfx@@UAE?BV?$Vec3@M@@XZ
const Vec3<float> DXGfx::LightD3D::GetDirection()
{
    return Vec3<float>(m_Light.dvDirection.x, m_Light.dvDirection.y, m_Light.dvDirection.z);
}

// 0x100d64d0 ?SetType@LightD3D@DXGfx@@UAEXW4LightType@Light@Gfx@@@Z
void DXGfx::LightD3D::SetType(Gfx::Light::LightType type)
{
    if (type == Point)
        m_Light.dltType = D3DLIGHT_POINT;
    else if (type == ParallelPoint)
        m_Light.dltType = D3DLIGHT_PARALLELPOINT;
    else if (type == Directional)
        m_Light.dltType = D3DLIGHT_DIRECTIONAL;
}

// 0x100d6510 ?GetType@LightD3D@DXGfx@@UAE?AW4LightType@Light@Gfx@@XZ
Gfx::Light::LightType DXGfx::LightD3D::GetType()
{
    Gfx::Light::LightType type;
    if (m_Light.dltType == D3DLIGHT_POINT)
        type = Point;
    else if (m_Light.dltType == D3DLIGHT_PARALLELPOINT)
        type = ParallelPoint;
    else if (m_Light.dltType == D3DLIGHT_DIRECTIONAL)
        type = Directional;
    return type;
}

// 0x100d6540 ?SetColor@LightD3D@DXGfx@@UAEXABV?$Vec4@M@@@Z
void DXGfx::LightD3D::SetColor(const Vec4<float> &color)
{
    m_Light.dcvDiffuse.r = color.e[0];
    m_Light.dcvDiffuse.g = color.e[1];
    m_Light.dcvDiffuse.b = color.e[2];
    m_Light.dcvDiffuse.a = color.e[3];
}

// 0x100d6570 ?GetColor@LightD3D@DXGfx@@UAE?BV?$Vec4@M@@XZ
const Vec4<float> DXGfx::LightD3D::GetColor()
{
    return Vec4<float>(m_Light.dcvDiffuse.r, m_Light.dcvDiffuse.g, m_Light.dcvDiffuse.b, m_Light.dcvDiffuse.a);
}

// 0x100d65d0 ?SetAmbient@LightD3D@DXGfx@@UAEXABV?$Vec4@M@@@Z
void DXGfx::LightD3D::SetAmbient(const Vec4<float> &color)
{
    m_Light.dcvAmbient.r = color.e[0];
    m_Light.dcvAmbient.g = color.e[1];
    m_Light.dcvAmbient.b = color.e[2];
    m_Light.dcvAmbient.a = color.e[3];
}

// 0x100d6600 ?GetAmbient@LightD3D@DXGfx@@UAE?BV?$Vec4@M@@XZ
const Vec4<float> DXGfx::LightD3D::GetAmbient()
{
    return Vec4<float>(m_Light.dcvAmbient.r, m_Light.dcvAmbient.g, m_Light.dcvAmbient.b, m_Light.dcvAmbient.a);
}

// 0x100d6660 ?TurnOn@LightD3D@DXGfx@@UAEXXZ
void DXGfx::LightD3D::TurnOn()
{
    m_pView->GetDevice()->LightEnable(m_Index, TRUE);
    m_pView->GetDevice()->SetLight(m_Index, &m_Light);
    m_pView->GetDevice()->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x33333333);
}

// 0x100d66c0 ?TurnOff@LightD3D@DXGfx@@UAEXXZ
void DXGfx::LightD3D::TurnOff()
{
    m_pView->GetDevice()->LightEnable(m_Index, FALSE);
}

// 0x100d6190 _$E40
// 0x100d61a0 _$E39
// 0x100d61c0 _$E43
// 0x100d61d0 _$E42
// 0x100d61f0 _$E46
// 0x100d6200 _$E45
// 0x100d6220 _$E49
// 0x100d6230 _$E48
// 0x100d6250 _$E52
// 0x100d6260 _$E51
// 0x100d66f0 _$E84
// 0x100d6720 _$E80
// 0x100d6730 _$E83

// the run-time type of DXGfx::LightD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::LightD3D::s_TypeInfo = { "DXGfx::LightD3D", &Gfx::Light::s_TypeInfo };
