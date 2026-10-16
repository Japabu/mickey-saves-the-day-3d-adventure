#pragma once
#include "Gfx_Light.h"
#include "StdBase.h"
#include "d3d7.h"
namespace DXGfx { class ViewD3D; }

namespace DXGfx {
// A Gfx::Light kept in one of the light slots of its view's Direct3D device.
class LightD3D : public Gfx::Light {
    friend class DXGfx::ViewD3D;
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    LightD3D(int);
    virtual ~LightD3D();
    virtual const Vec3<float> GetPosition();
    virtual void SetPosition(const Vec3<float> &);
    virtual void SetDirection(const Vec3<float> &);
    virtual const Vec3<float> GetDirection();
    virtual Gfx::Light::LightType GetType();
    virtual void SetType(Gfx::Light::LightType);
    virtual void SetColor(const Vec4<float> &);
    virtual const Vec4<float> GetColor();
    virtual void SetAmbient(const Vec4<float> &);
    virtual const Vec4<float> GetAmbient();
    virtual void TurnOn();
    virtual void TurnOff();
private:
    int m_Index;
    DXGfx::ViewD3D *m_pView;
    D3DLIGHT7 m_Light;
};
}
