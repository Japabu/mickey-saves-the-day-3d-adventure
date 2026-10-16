#include "Gfx_View.h"
#include "MathConstants.h"
#include "Gfx_Camera.h"
#include "Gfx_Node.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_BoundingVolume.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "StdBase.h"

// 0x100ecd70 ?GetTypeInfo@View@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::View::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100ecd80 ?GetTypeName@View@Gfx@@UBEPBDXZ
char const * Gfx::View::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100ecd90 ??1View@Gfx@@UAE@XZ
Gfx::View::~View()
{
    if (m_pScene)
        m_pScene->Unref();
}

// 0x100ecde0 ?SetBackgroundColor@View@Gfx@@UAEXMMM@Z
void Gfx::View::SetBackgroundColor(float, float, float)
{
}

// 0x100ecdf0 ??_GView@Gfx@@UAEPAXI@Z

// 0x100ece10 ?Initialize@View@Gfx@@QAEXXZ
void Gfx::View::Initialize()
{
}

// 0x100ece20 ?SetRenderTraverser@View@Gfx@@QAEXPAVRenderTraverser@2@@Z
void Gfx::View::SetRenderTraverser(Gfx::RenderTraverser * traverser)
{
    m_RenderTraverser = traverser;
}

// 0x100ece30 ?GetScene@View@Gfx@@QAEPAVNode@2@XZ
Gfx::Node * Gfx::View::GetScene()
{
    return m_pScene;
}

// 0x100ece40 ?SetScene@View@Gfx@@QAEXPAVNode@2@@Z
void Gfx::View::SetScene(Gfx::Node *scene)
{
    if (m_pScene)
        m_pScene->Unref();
    m_pScene = scene;
    scene->Ref();
}

// 0x100ece60 ?GetCamera@View@Gfx@@QAEPAVCamera@2@XZ
Gfx::Camera * Gfx::View::GetCamera()
{
    return m_pCamera;
}

// 0x100ece70 ?SetCamera@View@Gfx@@QAEXPAVCamera@2@@Z
void Gfx::View::SetCamera(Gfx::Camera * camera)
{
    m_pCamera = camera;
    CalcViewFrustumPlanes();
}

// 0x100ece80 ?CopyCamera@View@Gfx@@QAEXABVCamera@2@@Z
void Gfx::View::CopyCamera(const Gfx::Camera &camera)
{
    *m_pCamera = camera;
    CalcViewFrustumPlanes();
}

// 0x100ecea0 ?GetBackgroundColor@View@Gfx@@QAEKXZ
unsigned long Gfx::View::GetBackgroundColor()
{
    return m_BackgroundColor;
}

// 0x100eceb0 ?SetBackgroundColor@View@Gfx@@UAEXK@Z
void Gfx::View::SetBackgroundColor(unsigned long color)
{
    m_BackgroundColor = color;
}

// TODO 0x100ecec0 ?SetupMatrices@View@Gfx@@QAEXXZ: the original recomputes &camera->m_WorldMatrix for every product, here it is kept in a register
void Gfx::View::SetupMatrices()
{
    Gfx::Camera *camera = m_pCamera;
    Gfx::CurrentTransformationMatrix::GetInstance()->SetView(camera->GetWorldFrom(), camera->GetWorldAt(),
                                                           camera->GetWorldUp());
    Gfx::CurrentTransformationMatrix::GetInstance()->SetPerspective(camera->GetFOV(), m_AspectRatio,
                                                                  camera->GetNearClip(), camera->GetFarClip());
}

// TODO 0x100ecfa0 ?Refresh@View@Gfx@@UAEXXZ: as SetupMatrices
void Gfx::View::Refresh()
{
    Gfx::Node *scene = m_pScene;
    if (scene) {
        Gfx::Camera *camera = m_pCamera;
        if (camera) {
            Gfx::CurrentTransformationMatrix::GetInstance()->SetView(camera->GetWorldFrom(), camera->GetWorldAt(),
                                                                   camera->GetWorldUp());
            Gfx::CurrentTransformationMatrix::GetInstance()->SetPerspective(camera->GetFOV(), m_AspectRatio,
                                                                          camera->GetNearClip(),
                                                                          camera->GetFarClip());
            m_RenderTraverser->SetView(this);
            m_RenderTraverser->SetFrameTime(m_CurrentTime);
            m_RenderTraverser->Traverse(*scene);
        }
    }
}

// 0x100ed0a0 ?IsBoundingBoxInViewFrustum@View@Gfx@@QAE?AW4ViewFrustumIntersect@2@ABVBoundingBox@2@@Z
Gfx::ViewFrustumIntersect Gfx::View::IsBoundingBoxInViewFrustum(const Gfx::BoundingBox &box)
{
    Gfx::ViewFrustumIntersect result;
    bool inside = true;

    result = OverlapPlane(m_LeftPlane, box.GetMinima(), box.GetMaxima());
    if (result == Gfx::Out)
        return Gfx::Out;
    if (result == Gfx::Partial)
        inside = false;
    result = OverlapPlane(m_RightPlane, box.GetMinima(), box.GetMaxima());
    if (result == Gfx::Out)
        return Gfx::Out;
    if (result == Gfx::Partial)
        inside = false;
    result = OverlapPlane(m_TopPlane, box.GetMinima(), box.GetMaxima());
    if (result == Gfx::Out)
        return Gfx::Out;
    if (result == Gfx::Partial)
        inside = false;
    result = OverlapPlane(m_BottomPlane, box.GetMinima(), box.GetMaxima());
    if (result == Gfx::Out)
        return Gfx::Out;
    if (result != Gfx::Partial && inside)
        return Gfx::In;
    return Gfx::Partial;
}

// TODO 0x100ed1c0 ?CalcViewFrustumPlanes@View@Gfx@@IAEXXZ: same calls, constants and order; the original divides 1 by the length in the inlined normalize with fld 1.0 / fdiv st(1) (keeping the length on the FPU stack), VC6 here uses fdivr from memory, and the stack slots of the temporaries differ (57% similar)
void Gfx::View::CalcViewFrustumPlanes()
{
    // The side planes in camera coordinates: each passes through the eye
    // and two corners of the view window at distance 1 down -z.
    Vec3<float> eye, up, side;
    eye = Vec3<float>(0.0f, 0.0f, 0.0f);
    up = Vec3<float>(0.0f, 1.0f, 0.0f);
    side = Vec3<float>(1.0f, 0.0f, 0.0f);
    side.normalize();
    Vec3<float> dir(0.0f, 0.0f, -1.0f);

    double t = tan(m_pCamera->GetFOV() * DEG_TO_RAD * 0.5);
    float right = (float)t;
    float left = -right;
    float top = (float)(t / m_AspectRatio);
    float bottom = -top;

    Vec3<float> topRight = side * right + up * top + dir;
    Vec3<float> topLeft = side * left + up * top + dir;
    Vec3<float> bottomRight = side * right + up * bottom + dir;
    Vec3<float> bottomLeft = side * left + up * bottom + dir;

    m_TopPlane.Set(eye, topLeft, topRight);
    m_LeftPlane.Set(eye, bottomLeft, topLeft);
    m_BottomPlane.Set(eye, bottomRight, bottomLeft);
    m_RightPlane.Set(eye, topRight, bottomRight);
}

// 0x100ed640 ?OverlapPlane@View@Gfx@@IAE?AW4ViewFrustumIntersect@2@AAVPlane3f@2@ABV?$Vec3@M@@1@Z
Gfx::ViewFrustumIntersect Gfx::View::OverlapPlane(Gfx::Plane3f &plane, const Vec3<float> &minima,
                                                   const Vec3<float> &maxima)
{
    float x = plane.GetNormal()(0);
    float y = plane.GetNormal()(1);
    float z = plane.GetNormal()(2);
    Vec3<float> nearest, farthest;

    if (x > 0.0f) {
        if (y > 0.0f) {
            if (z > 0.0f) {
                farthest.setValues(maxima(0), maxima(1), maxima(2));
                nearest.setValues(minima(0), minima(1), minima(2));
            } else {
                farthest.setValues(maxima(0), maxima(1), minima(2));
                nearest.setValues(minima(0), minima(1), maxima(2));
            }
        } else {
            if (z > 0.0f) {
                farthest.setValues(maxima(0), minima(1), maxima(2));
                nearest.setValues(minima(0), maxima(1), minima(2));
            } else {
                farthest.setValues(maxima(0), minima(1), minima(2));
                nearest.setValues(minima(0), maxima(1), maxima(2));
            }
        }
    } else {
        if (y > 0.0f) {
            if (z > 0.0f) {
                farthest.setValues(minima(0), maxima(1), maxima(2));
                nearest.setValues(maxima(0), minima(1), minima(2));
            } else {
                farthest.setValues(minima(0), maxima(1), minima(2));
                nearest.setValues(maxima(0), minima(1), maxima(2));
            }
        } else {
            if (z > 0.0f) {
                farthest.setValues(minima(0), minima(1), maxima(2));
                nearest.setValues(maxima(0), maxima(1), minima(2));
            } else {
                farthest.setValues(minima(0), minima(1), minima(2));
                nearest.setValues(maxima(0), maxima(1), maxima(2));
            }
        }
    }

    if (plane.ComputeDistance(nearest) > 0.0f)
        return Gfx::In;
    if (plane.ComputeDistance(farthest) <= 0.0f)
        return Gfx::Out;
    return Gfx::Partial;
}

// 0x100ed140 ??0View@Gfx@@IAE@XZ
Gfx::View::View()
    : m_pCamera(0), m_pScene(0), m_BackgroundColor(0), m_LastTime(0), m_CurrentTime(0)
{
}

// 0x100ecc80 _$E7
// 0x100ecc90 _$E6
// 0x100eccb0 _$E10
// 0x100eccc0 _$E9
// 0x100ecce0 _$E13
// 0x100eccf0 _$E12
// 0x100ecd10 _$E16
// 0x100ecd20 _$E15
// 0x100ecd40 _$E19
// 0x100ecd50 _$E18

// Header instances of this object:
// 0x100ed810 ??D?$Vec3@M@@QBE?AV0@M@Z
// TODO 0x100ed860 ?multVector3Left@?$TMatrix4@M@@QBE?AV?$Vec3@M@@ABV2@@Z: the terms are now summed in the original order (through a local, see TMatrix4.h); the original still keeps the partial sum on the FPU (fadd st(1) ... fstp st(0)), VC6 here adds with faddp (84% similar)
