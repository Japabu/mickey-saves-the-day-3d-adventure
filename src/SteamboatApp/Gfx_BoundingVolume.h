#pragma once
#include "Vec3.h"
template <class T0> class TMatrix4;
template <class T0> class RVec;
template <class T0> class AVec;
namespace Gfx { class Ray3f; }
namespace Gfx { class LineSegment3f; }
namespace Gfx { class Polygon3f; }
namespace Gfx { class BoundingSphere; }
namespace Gfx { class BoundingBox; }
namespace Gfx { class Plane3f; }
namespace Gfx { template <class T0, int N> class MatrixStack; }

// Tolerance and largest value of a number type.
template <class T>
class VisTData {
public:
    T epsilon;
    T max;
    VisTData();
};

template <>
inline VisTData<float>::VisTData()
{
    epsilon = 1e-4f;
    max = 3.402823466e+38f;
}

namespace Gfx {
// Base of the bounding volumes: a cached centre and the intersection tests.
class BoundingVolume {
public:
    virtual ~BoundingVolume() {}
    virtual const Vec3<float> &GetCenter() const { return m_Center; }
    virtual float GetRadius() const = 0;
    virtual bool Contains(const Vec3<float> &) const = 0;
    virtual bool Touches(const Vec3<float> &) const = 0;
    virtual bool Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const = 0;
    virtual bool IsValid() const = 0;
    virtual void Reset() = 0;
    static void PushLeftIntersectionMatrix(const TMatrix4<float> &);
    static void PushRightIntersectionMatrix(const TMatrix4<float> &);
    static void PopLeftIntersectionMatrix();
    static void PopRightIntersectionMatrix();
    static void SwapIntersectionOperands();
    static const TMatrix4<float> &GetLeftIntersectionMatrix();
    static const TMatrix4<float> &GetRightIntersectionMatrix();
    static const TMatrix4<float> &GetIntersectionMatrix();
    static bool IntersectionOperandsAreSwapped();
protected:
    BoundingVolume(const Gfx::BoundingVolume &volume) : m_Center(volume.m_Center) {}
    BoundingVolume(const Vec3<float> &center) : m_Center(center) {}
    BoundingVolume() : m_Center(0.0f, 0.0f, 0.0f) {}
    void SetCenter(const Vec3<float> &center) const { ((BoundingVolume *)this)->m_Center = center; }
    float GetEpsilon() const { return m_TData.epsilon; }
    float GetMaximum() const { return m_TData.max; }
    static const Gfx::MatrixStack<float, 32> &GetIntersectionMatrixStack() { return s_IntersectionMatrixStack; }
private:
    Vec3<float> m_Center;
    static VisTData<float> m_TData;
    static Gfx::MatrixStack<float, 32> s_LeftIntersectionMatrixStack;
    static Gfx::MatrixStack<float, 32> s_RightIntersectionMatrixStack;
    static Gfx::MatrixStack<float, 32> s_IntersectionMatrixStack;
    static bool s_SwapIntersectionOperands;
};

// Sphere around the centre of the base class.
class BoundingSphere : public Gfx::BoundingVolume {
public:
    BoundingSphere(const Gfx::BoundingSphere &sphere) : BoundingVolume(sphere), m_Radius(sphere.m_Radius) {}
    BoundingSphere(const RVec<float> &, int, int);
    BoundingSphere(const Gfx::BoundingBox &, bool);
    BoundingSphere(const Gfx::BoundingVolume &);
    BoundingSphere(const Vec3<float> &, float);
    BoundingSphere();
    virtual ~BoundingSphere() {}
    virtual float GetRadius() const { return m_Radius; }
    float GetSurfaceArea() const;
    float ComputeDistanceSquared(const Gfx::Polygon3f &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::BoundingBox &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::BoundingSphere &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::Polygon3f &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::Polygon3f &, Vec3<float> &, Vec3<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::Plane3f &, Vec3<float> &, Vec3<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::LineSegment3f &, Vec3<float> &, Vec3<float> &) const;
    Vec3<float> FindClosestPoint(const Vec3<float> &, Vec3<float> &) const;
    virtual bool Contains(const Vec3<float> &) const;
    bool Contains(const Gfx::BoundingSphere &) const;
    virtual bool Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const;
    bool Intersects(const Gfx::Polygon3f &, const TMatrix4<float> &) const;
    bool Intersects(const Gfx::Polygon3f &) const;
    bool Intersects(const Gfx::BoundingSphere &) const;
    bool IntersectsTransformed(const Gfx::BoundingSphere &, const TMatrix4<float> &) const;
    bool IntersectsTransformed(const Gfx::BoundingSphere &) const;
    virtual bool Touches(const Vec3<float> &) const;
    Gfx::BoundingSphere operator+(const Gfx::BoundingSphere &) const;
    Gfx::BoundingSphere operator+(const Vec3<float> &) const;
    Gfx::BoundingSphere &operator+=(const Gfx::BoundingSphere &);
    Gfx::BoundingSphere &operator+=(const Vec3<float> &);
    bool operator==(const Gfx::BoundingSphere &);
    Gfx::BoundingSphere TransformAffine(const TMatrix4<float> &) const;
    Gfx::BoundingSphere TransformInverseAffine(const TMatrix4<float> &) const;
    virtual bool IsValid() const;
    virtual void Reset();
private:
    float m_Radius;
};

// Upright cylinder around the centre of the base class (no PDB declaration;
// names chosen from the use).
class BoundingCylinder : public Gfx::BoundingVolume {
public:
    BoundingCylinder();
    BoundingCylinder(const Vec3<float> &, float, float);
    BoundingCylinder(const Gfx::BoundingCylinder &);
    BoundingCylinder(const Gfx::BoundingBox &, bool);
    virtual ~BoundingCylinder() {}
    virtual float GetRadius() const { return m_Radius; }
    float GetHeight() const { return m_Height; }
    virtual bool Contains(const Vec3<float> &) const;
    bool Contains(const Gfx::BoundingCylinder &) const;
    bool out(const Gfx::BoundingCylinder &) const;
    bool Intersects(const Gfx::BoundingCylinder &) const;
    virtual bool Touches(const Vec3<float> &) const;
    virtual bool Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const;
    Gfx::BoundingCylinder operator+(const Vec3<float> &) const;
    Gfx::BoundingCylinder operator+(const Gfx::BoundingCylinder &) const;
    Gfx::BoundingCylinder &operator+=(const Vec3<float> &);
    Gfx::BoundingCylinder &operator+=(const Gfx::BoundingCylinder &);
    bool operator==(const Gfx::BoundingCylinder &);
    virtual bool IsValid() const;
    virtual void Reset();
private:
    float m_Radius;
    float m_Height;
};

// Axis-aligned box; an empty box has its minima above its maxima.
class BoundingBox : public Gfx::BoundingVolume {
public:
    BoundingBox(const float *minima, const float *maxima) : m_Minima(minima), m_Maxima(maxima) {}
    BoundingBox(const Vec3<float> minima, const Vec3<float> maxima) : m_Minima(minima), m_Maxima(maxima) {}
    BoundingBox(const Gfx::BoundingSphere &);
    BoundingBox(const Gfx::BoundingBox &box) : m_Minima(box.m_Minima), m_Maxima(box.m_Maxima) {}
    BoundingBox() : m_Minima(GetMaximum(), GetMaximum(), GetMaximum()),
        m_Maxima(-GetMaximum(), -GetMaximum(), -GetMaximum()) {}
    virtual ~BoundingBox() {}
    Gfx::BoundingBox TransformAffine(const TMatrix4<float> &) const;
    Gfx::BoundingBox TransformInverseAffine(const TMatrix4<float> &) const;
    void Transform4(const TMatrix4<float> &);
    void Transform4X(const TMatrix4<float> &);
    const Vec3<float> &GetMinima() const { return m_Minima; }
    const Vec3<float> &GetMaxima() const { return m_Maxima; }
    Vec3<float> GetCorner(unsigned int) const;
    Gfx::LineSegment3f GetEdge(unsigned int) const;
    virtual const Vec3<float> &GetCenter() const;
    Vec3<float> GetDiagonal() const { return m_Maxima - m_Minima; }
    virtual float GetRadius() const;
    float GetSurfaceArea() const;
    Vec3<float> FindClosestPoint(const Gfx::BoundingSphere &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::BoundingBox &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::Polygon3f &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::LineSegment3f &, Vec3<float> &, Vec3<float> &) const;
    Vec3<float> FindClosestPoint(const Vec3<float> &, Vec3<float> &) const;
    float ComputeDistanceSquared(const Gfx::BoundingBox &, const TMatrix4<float> &) const;
    float ComputeDistanceSquared(const Gfx::BoundingBox &) const;
    float ComputeDistanceSquared(const Gfx::Polygon3f &, const TMatrix4<float> &) const;
    float ComputeDistanceSquared(const Gfx::LineSegment3f &) const;
    float ComputeDistanceSquared(const Vec3<float> &) const;
    bool Intersects(const Gfx::LineSegment3f &) const;
    bool Intersects(const Gfx::BoundingSphere &) const;
    bool Intersects(const Gfx::BoundingBox &) const;
    bool Intersects(const Gfx::Polygon3f &, const TMatrix4<float> &) const;
    bool Intersects(const Gfx::Polygon3f &) const;
    bool Intersects(const Gfx::LineSegment3f &, float &, float &, Vec3<float> &) const;
    virtual bool Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const;
    bool IntersectsTransformed(const Gfx::BoundingSphere &, const TMatrix4<float> &) const;
    bool IntersectsTransformed(const Gfx::BoundingSphere &) const;
    bool IntersectsTransformed(const Gfx::BoundingBox &, const TMatrix4<float> &) const;
    bool IntersectsTransformed(const Gfx::BoundingBox &) const;
    bool IntersectX(float, float);
    Gfx::BoundingBox operator+(const Gfx::BoundingSphere &) const;
    Gfx::BoundingBox operator+(const Gfx::BoundingBox &) const;
    Gfx::BoundingBox operator+(const Vec3<float> &) const;
    Gfx::BoundingBox &operator+=(const Gfx::BoundingSphere &);
    Gfx::BoundingBox &operator+=(const Gfx::BoundingBox &);
    Gfx::BoundingBox &operator+=(const Vec3<float> &);
    bool operator==(const Gfx::BoundingBox &);
    void ExtendBy(const RVec<float> &, int, int);
    void ExtendBy(AVec<Vec3<float> > &);
    void Scale(const Vec3<float> &);
    virtual bool Contains(const Vec3<float> &) const;
    virtual bool Touches(const Vec3<float> &) const;
    virtual bool IsValid() const;
    virtual void Reset();
protected:
    Vec3<float> m_Minima;
    Vec3<float> m_Maxima;
};
}
