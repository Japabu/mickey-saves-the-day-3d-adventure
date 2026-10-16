#pragma optimize("a", on) // evidence: the copy constructor and the (frame, fov, near, far) constructor match only with no-alias code
#include "EAICamera.h"
#include "MathConstants.h"
#include <math.h>
#include <float.h>

// 0x1012f6a0 ??0EAICamera@@QAE@XZ
EAICamera::EAICamera() : m_Near(1.0f), m_Far(1000.0f), m_FOV(90.0f), m_AutoClippingIsEnabled(true)
{
}

// 0x1012f6f0 ??0EAICamera@@QAE@ABV0@@Z
EAICamera::EAICamera(const EAICamera &camera) : EAIRefFrame(camera), m_Near(camera.m_Near), m_Far(camera.m_Far),
    m_FOV(camera.m_FOV), m_AutoClippingIsEnabled(camera.m_AutoClippingIsEnabled)
{
}

// 0x1012f730 ??0EAICamera@@QAE@ABVEAIRefFrame@@@Z
EAICamera::EAICamera(const EAIRefFrame &frame) : EAIRefFrame(frame), m_Near(1.0f), m_Far(1000.0f), m_FOV(90.0f),
    m_AutoClippingIsEnabled(true)
{
}

// 0x1012f770 ??0EAICamera@@QAE@ABVEAIRefFrame@@MMM@Z
EAICamera::EAICamera(const EAIRefFrame &frame, float fov, float nearClip, float farClip) : EAIRefFrame(frame),
    m_Near(nearClip), m_Far(farClip), m_FOV(fov), m_AutoClippingIsEnabled(true)
{
}

// 0x1012f7b0 ??0EAICamera@@QAE@ABV?$Vec3@M@@00MMM@Z
EAICamera::EAICamera(const Vec3<float> &at, const Vec3<float> &from, const Vec3<float> &up, float fov, float nearClip,
                     float farClip)
    : EAIRefFrame(at, from, up), m_Near(nearClip), m_Far(farClip), m_FOV(fov), m_AutoClippingIsEnabled(true)
{
}

// 0x1012f830 ??1EAICamera@@QAE@XZ
EAICamera::~EAICamera()
{
}

// 0x1012f5b0 _$E3
// 0x1012f5c0 _$E2
// 0x1012f5e0 _$E6
// 0x1012f5f0 _$E5
// 0x1012f610 _$E9
// 0x1012f620 _$E8
// 0x1012f640 _$E12
// 0x1012f650 _$E11
// 0x1012f670 _$E15
// 0x1012f680 _$E14

// 0x1012f840 ?interp@EAICamera@@QBE?AV1@ABV1@M@Z
EAICamera EAICamera::interp(const EAICamera &camera, float t) const
{
    EAICamera result(EAIRefFrame::slerpTo(camera, t));
    float s = 1.0f - t;
    result.m_Near = s * m_Near + t * camera.m_Near;
    result.m_Far = s * m_Far + t * camera.m_Far;
    result.m_FOV = s * m_FOV + t * camera.m_FOV;
    if (t < 1.0f && m_AutoClippingIsEnabled != camera.m_AutoClippingIsEnabled)
        result.m_AutoClippingIsEnabled = false;
    else
        result.m_AutoClippingIsEnabled = camera.m_AutoClippingIsEnabled;
    return result;
}

// 0x1012f920 ?updateClipPlanes@EAICamera@@QAEXABV?$Vec3@M@@M@Z
void EAICamera::updateClipPlanes(const Vec3<float> &center, float radius)
{
    Vec3<float> toCenter = center - From;
    Vec3<float> viewVec = getViewVec();
    float distance = toCenter.dot(viewVec) / getViewVec().mag();
    float farClip = distance + radius;
    m_Far = farClip;
    m_Near = distance - radius;
    if (m_Near < FLT_EPSILON)
        m_Near = farClip * 0.001f;
}

// 0x1012fa90 ?viewAll@EAICamera@@QAEXABV?$Vec3@M@@M@Z
void EAICamera::viewAll(const Vec3<float> &center, float radius)
{
    translate(center - At);
    setFromDist(radius / (float)sin(m_FOV * DEG_TO_RAD));
    updateClipPlanes(center, radius);
}

// 0x1012fb40 ?viewAll@EAICamera@@QAEXABV?$Vec3@M@@0@Z
void EAICamera::viewAll(const Vec3<float> &minima, const Vec3<float> &maxima)
{
    Vec3<float> center = (minima + maxima) * 0.5f;
    viewAll(center, (maxima - center).mag());
}

// 0x1012fc30 ??4EAICamera@@UAEXABV0@@Z
void EAICamera::operator=(const EAICamera &camera)
{
    EAIRefFrame::operator=(camera);
    m_FOV = camera.m_FOV;
    m_Near = camera.m_Near;
    m_Far = camera.m_Far;
    m_AutoClippingIsEnabled = camera.m_AutoClippingIsEnabled;
}

// 0x1012fc70 ??8EAICamera@@UBEHABV0@@Z
int EAICamera::operator==(const EAICamera &camera) const
{
    const EAIRefFrame *frame = &camera;
    return EAIRefFrame::operator==(*frame) && m_FOV == camera.m_FOV && m_Near == camera.m_Near && m_Far == camera.m_Far
        && m_AutoClippingIsEnabled == camera.m_AutoClippingIsEnabled;
}
