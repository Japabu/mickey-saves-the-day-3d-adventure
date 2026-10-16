#pragma once
#include "Gfx_Node.h"
#include "StdBase.h"
#include "EAICamera.h"
#include "TMatrix4.h"
template <class T0> class Vec2;
template <class T0> class Quaternion;
namespace Gfx { class Ray3f; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class BasicNode; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Camera node: an EAICamera in the coordinates of its parent and the matrix
// that takes those coordinates to the world.
class Camera : public Gfx::Node {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    Camera(const Gfx::Camera &);
    Camera();
    virtual ~Camera();
    void SetAt(const Vec3<float> &at) { m_EAICamera.setAtPoint(at); }
    void SetFrom(const Vec3<float> &from) { m_EAICamera.setFromPoint(from); }
    void SetUp(const Vec3<float> &up) { m_EAICamera.setUpVec(up); }
    void LockPitch(bool);
    void LockRoll(bool);
    void LockYaw(bool);
    void SetNearClip(float nearClip) { m_EAICamera.setNear(nearClip); }
    void SetFarClip(float farClip) { m_EAICamera.setFar(farClip); }
    void SetFOV(float fov) { m_EAICamera.setFOV(fov); }
    void EnableAutoClipping(bool enable) { m_EAICamera.setAutoClip(enable); }
    const Vec3<float> &GetAt() const { return m_EAICamera.getAtPoint(); }
    const Vec3<float> &GetFrom() const { return m_EAICamera.getFromPoint(); }
    const Vec3<float> &GetUp() const { return m_EAICamera.getUpVec(); }
    bool IsPitchLocked() const { return m_PitchLocked; }
    bool IsRollLocked() const { return m_RollLocked; }
    bool IsYawLocked() const { return m_YawLocked; }
    float GetNearClip() const { return m_EAICamera.getNear(); }
    float GetFarClip() const { return m_EAICamera.getFar(); }
    float GetFOV() const { return m_EAICamera.getFOV(); }
    bool AutoClippingIsEnabled() const { return m_EAICamera.getAutoClip(); }
    void SetWorldMatrix(const TMatrix4<float> &matrix) { m_WorldMatrix.setValues(matrix); }
    const TMatrix4<float> &GetWorldMatrix() const { return m_WorldMatrix; }
    Vec3<float> GetWorldFrom() const { return m_WorldMatrix.multVec3PointLeftAffine(GetFrom()); }
    Vec3<float> GetWorldAt() const { return m_WorldMatrix.multVec3PointLeftAffine(GetAt()); }
    Vec3<float> GetWorldUp() const { return m_WorldMatrix.multVector3Left(GetUp()); }
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    Gfx::Ray3f const CalcRayThruScreenPoint(const Vec2<float> &);
    int CalcWorldPointFromScreen(const Vec2<float> &, Vec3<float> &);
    void Interpolate(Gfx::Camera *, int, int);
    void UpdateClippingPlanes(const Vec3<float> &, float);
    void CalcScreenAxis(Vec3<float> &, Vec3<float> &, Vec3<float> &) const;
    Gfx::Camera &operator=(const Gfx::Camera &);
    void ScreenTranslate(const Vec3<float> &);
    void Translate(const Vec3<float> &);
    void AtScreenRotate(const Vec3<float> &, float);
    void AtRotate(const Quaternion<float> &);
    void FromScreenRotate(const Vec3<float> &, float);
    void FromRotate(const Quaternion<float> &);
    void Twist(float);
protected:
    virtual void ParentAdded(Graph::BasicNode *);
    virtual void ParentRemoved(Graph::BasicNode *);
    virtual void Refresh(Graph::BasicRefreshTraverser &);
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
private:
    static int const Version;
    EAICamera m_EAICamera;
    TMatrix4<float> m_WorldMatrix;
    bool m_PitchLocked;
    bool m_RollLocked;
    bool m_YawLocked;
    bool m_Interpolating;
    int m_InterpolateStartTime;
    int m_InterpolateDuration;
    EAICamera m_InterpolateFromEAICamera;
    Gfx::Camera *m_pInterpolateToCamera;
};
}
