#pragma once
#include "Gfx_Plane3f.h"
#include "Gfx_Ray3f.h"

namespace Gfx {
// Planar polygon given by pointers to its corner points (shared with the
// caller or owned) and its plane; the two coordinates with the largest
// extent are used for the inside tests.
class Polygon3f {
    enum VP3Status {
        Null = 0,
        Shared = 1,
        Mine = 2
    };
public:
    Polygon3f(Vec3<float> **, int);
    Polygon3f();
    ~Polygon3f();
    void Share(Vec3<float> **, int);
    void Set(Vec3<float> **, int);
    Vec3<float> **const GetPoints() const { return m_vPoints; }
    Vec3<float> **GetPoints() { return m_vPoints; }
    Gfx::LineSegment3f GetEdge(int) const;
    int GetNumberOfPoints() const { return m_NumberOfPoints; }
    const Gfx::Plane3f &GetPlane() const { return m_Plane; }
    bool IsAround(const Vec3<float> &) const;
    bool Intersects(const Gfx::Polygon3f &) const;
    bool Intersects(const Gfx::LineSegment3f &, float &) const;
    bool Intersects(const Gfx::Ray3f &, float &) const;
    Vec3<float> FindClosestPoint(const Gfx::LineSegment3f &, Vec3<float> &, Vec3<float> &) const;
    float ComputeDistanceSquared(const Gfx::LineSegment3f &) const;
    float ComputeApproximateDistance(const Gfx::Polygon3f &) const;
    float ComputeDistance(const Gfx::Polygon3f &) const;
protected:
    Gfx::Plane3f m_Plane;
    Vec3<float> **m_vPoints;
    int m_NumberOfPoints;
    VP3Status m_PointStatus;
    unsigned int _projCoordX;
    unsigned int _projCoordY;
    void _setProjCoords();
    void _coniderEdgeDistance(Vec3<float> **[][2], float &) const;
};
}
