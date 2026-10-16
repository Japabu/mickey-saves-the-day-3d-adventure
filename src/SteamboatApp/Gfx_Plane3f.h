#pragma once
#include "Vec3.h"
template <class T0> class AVec;
template <class T0> class VisTData;
namespace Gfx { class LineSegment3f; }
namespace Gfx { class Ray3f; }

namespace Gfx {
// Plane n.x + d = 0 with unit normal n.
class Plane3f {
public:
    Plane3f(const Vec3<float> &, float);
    Plane3f();
    Gfx::Plane3f &operator=(const Gfx::Plane3f &);
    bool operator==(const Gfx::Plane3f &) const;
    bool operator!=(const Gfx::Plane3f &) const;
    void Set(Vec3<float> &, Vec3<float> &, Vec3<float> &);
    void Set(Vec3<float> **, int);
    void Set(AVec<Vec3<float> > &, int);
    void Set(const Vec3<float> &, float);
    void Get(Vec3<float> &, float &);
    const Vec3<float> &GetNormal() const { return m_Normal; }
    float GetOffset() const { return m_Offset; }
    bool Intersects(const Gfx::LineSegment3f &, float &) const;
    bool Intersects(const Gfx::Ray3f &, float &) const;
    float ComputeDistance(Vec3<float> point) const { return m_Normal.dot(point) + m_Offset; }
protected:
    Vec3<float> m_Normal;
    float m_Offset;
    static VisTData<float> m_TData;
};
}
