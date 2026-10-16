#pragma once
#include "EAIRefFrame.h"

// Camera of the scene library: a reference frame plus near/far clipping
// planes and the field of view.
class EAICamera : public EAIRefFrame {
public:
    EAICamera(const Vec3<float> &, const Vec3<float> &, const Vec3<float> &, float, float, float);
    EAICamera(const EAIRefFrame &, float, float, float);
    EAICamera(const EAIRefFrame &);
    EAICamera(const EAICamera &);
    EAICamera();
    ~EAICamera();
    void setNear(float n) { m_Near = n; }
    float getNear() const { return m_Near; }
    void setFar(float f) { m_Far = f; }
    float getFar() const { return m_Far; }
    void setFOV(float fov) { m_FOV = fov; }
    float getFOV() const { return m_FOV; }
    void setAutoClip(bool enable) { m_AutoClippingIsEnabled = enable; }
    bool getAutoClip() const { return m_AutoClippingIsEnabled; }
    void updateClipPlanes(const Vec3<float> &, float);
    void viewAll(const Vec3<float> &, const Vec3<float> &);
    void viewAll(const Vec3<float> &, float);
    virtual void operator=(const EAICamera &);
    virtual int operator==(const EAICamera &) const;
    EAICamera slerpTo(const EAICamera &, float) const;
    EAICamera interp(const EAICamera &, float) const;
private:
    float m_Near;
    float m_Far;
    float m_FOV;
    bool m_AutoClippingIsEnabled;
};
