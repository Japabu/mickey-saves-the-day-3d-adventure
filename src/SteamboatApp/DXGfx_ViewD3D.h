#pragma once
#include "Gfx_View.h"
#include "StdBase.h"
namespace DX { class Device; class Viewport; }
namespace DXGfx { class Surface; }
namespace Gfx { class Light; }

namespace DXGfx {
// A Gfx::View drawn with Direct3D: its own device and viewport on a
// surface, and the light slots of the device in use.
class ViewD3D : public Gfx::View {
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    ViewD3D(DXGfx::Surface *);
    virtual ~ViewD3D();
    virtual void SetBackgroundColor(float, float, float);
    virtual Gfx::Light * CreateLight();
    virtual void DeleteLight(Gfx::Light *);
    virtual void Create(int, int);
    virtual void Release();
    virtual void Refresh();
    virtual void Clear();
    DX::Device *GetDevice() { return m_pDevice; }
private:
    DX::Viewport *m_pViewport;
    DXGfx::Surface *m_pSurface;
    DX::Device *m_pDevice;
    unsigned int m_MaxLights;
    bool *m_LightInUse;
};
}
