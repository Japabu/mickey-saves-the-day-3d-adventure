#pragma once
#include "StdBase.h"
#include "enums.h"
#include "Gfx_Plane3f.h"
template <class T0> class Vec3;
namespace Gfx { class BoundingBox; }
namespace Gfx { class Camera; }
namespace Gfx { class Light; }
namespace Gfx { class Node; }
namespace Gfx { class RenderTraverser; }

namespace Gfx {
// A window onto a scene: the camera, the scene root, the render traverser
// and the view frustum planes in camera coordinates.
class View : public StdBase {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~View();
    virtual void SetBackgroundColor(unsigned long);
    virtual void SetBackgroundColor(float, float, float);
    virtual Gfx::Light * CreateLight() = 0;
    virtual void DeleteLight(Gfx::Light *) = 0;
    virtual void Create(int, int) = 0;
    virtual void Release() = 0;
    virtual void Refresh();
    virtual void Clear() = 0;
protected:
    virtual void DrawBox(Gfx::BoundingBox const &, unsigned long) {}
public:
    void SetAspectRatio(float ratio) { m_AspectRatio = ratio; }
    float GetAspectRatio() { return m_AspectRatio; }
    Gfx::RenderTraverser *GetRenderTraverser() { return m_RenderTraverser; }
    void SetTime(int time)
    {
        m_LastTime = m_CurrentTime;
        m_CurrentTime = time;
    }
    int GetTime() { return m_CurrentTime; }
    Gfx::Camera * GetCamera();
    Gfx::Node * GetScene();
    Gfx::ViewFrustumIntersect IsBoundingBoxInViewFrustum(Gfx::BoundingBox const &);
    unsigned long GetBackgroundColor();
    void CopyCamera(Gfx::Camera const &);
    void Initialize();
    void SetCamera(Gfx::Camera *);
    void SetRenderTraverser(Gfx::RenderTraverser *);
    void SetScene(Gfx::Node *);
    void SetupMatrices();
protected:
    Gfx::ViewFrustumIntersect OverlapPlane(Gfx::Plane3f &, Vec3<float> const &, Vec3<float> const &);
    View();
    void CalcViewFrustumPlanes();
private:
    float m_AspectRatio;
    Gfx::Camera *m_pCamera;
    Gfx::Node *m_pScene;
    Gfx::RenderTraverser *m_RenderTraverser;
    unsigned long m_BackgroundColor;
    unsigned long m_LastTime;
    unsigned long m_CurrentTime;
    Gfx::Plane3f m_NearPlane;
    Gfx::Plane3f m_TopPlane;
    Gfx::Plane3f m_BottomPlane;
    Gfx::Plane3f m_RightPlane;
    Gfx::Plane3f m_LeftPlane;
};
}
