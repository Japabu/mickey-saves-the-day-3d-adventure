#pragma once
#include "Vec3.h"
template <class T0> class TMatrix4;
namespace Gfx { class LineSegment3f; }

namespace Gfx {
// Half line from an origin along a direction.
class Ray3f {
public:
    Ray3f(const Gfx::LineSegment3f &);
    Ray3f(const Vec3<float> &, const Vec3<float> &);
    Ray3f();
    void SetOrigin(const Vec3<float> &origin) { m_Origin = origin; }
    void SetDirection(const Vec3<float> &direction) { m_Direction = direction; }
    const Vec3<float> &GetOrigin() const { return m_Origin; }
    const Vec3<float> &GetDirection() const { return m_Direction; }
    Vec3<float> GetAt(float t) const { return m_Origin + m_Direction * t; }
    void Normalize();
    Vec3<float> GetNearestPoint(const Vec3<float> &) const;
    Gfx::Ray3f Transform(const TMatrix4<float> &) const;
    Gfx::Ray3f TransformInverseAffine(const TMatrix4<float> &) const;
protected:
    Vec3<float> m_Origin;
    Vec3<float> m_Direction;
};

// Segment between two end points.
class LineSegment3f {
public:
    LineSegment3f(const Vec3<float> &, const Vec3<float> &);
    LineSegment3f();
    void Set(const Vec3<float> &, const Vec3<float> &);
    const Vec3<float> &GetEndPoint1() const { return m_EndPoint1; }
    const Vec3<float> &GetEndPoint2() const { return m_EndPoint2; }
    Vec3<float> GetAt(float t) const { return m_EndPoint1 + (m_EndPoint2 - m_EndPoint1) * t; }
    Vec3<float> FindClosestPoint(const Vec3<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::LineSegment3f &, Vec3<float> &, Vec3<float> &) const;
    Gfx::LineSegment3f Transform(const TMatrix4<float> &) const;
    Gfx::LineSegment3f TransformInverseAffine(const TMatrix4<float> &) const;
protected:
    Vec3<float> m_EndPoint1;
    Vec3<float> m_EndPoint2;
};
}

#include "TMatrix4.h"

// The segment's end points through a projective matrix (no out-of-line copy
// in the binary: inline).
inline Gfx::LineSegment3f Gfx::LineSegment3f::Transform(const TMatrix4<float> &matrix) const
{
    return Gfx::LineSegment3f(Vec3<float>(matrix.multVec3PointLeft(m_EndPoint1)),
        Vec3<float>(matrix.multVec3PointLeft(m_EndPoint2)));
}

// The end points into the frame of an affine matrix (inline as well).
inline Gfx::LineSegment3f Gfx::LineSegment3f::TransformInverseAffine(const TMatrix4<float> &matrix) const
{
    return Gfx::LineSegment3f(matrix.multVec3PointLeftInverseAffine(m_EndPoint1),
        matrix.multVec3PointLeftInverseAffine(m_EndPoint2));
}
