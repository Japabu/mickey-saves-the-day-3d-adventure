#pragma once
#include "Gfx_TraversalPath.h"
#include "Gfx_BoundingVolume.h"
#include "TMatrix4.h"
#include "Vec.h"
#include "Vec3.h"

namespace Gfx {
// One collision found by the intersection traversal: the paths of the two
// shapes (and of their group), their matrices, the atoms hit, the contact
// points and normal and the two bounding spheres (declaration from
// SteamboatApp.pdb).
class IntersectionRecord {
public:
    IntersectionRecord(const Gfx::TraversalPath &, const Gfx::TraversalPath &, const Gfx::TraversalPath &,
                       const TMatrix4<float> &, const TMatrix4<float> &, const Vec<int> &, const Vec3<float> &,
                       const Vec3<float> &, const Vec3<float> &, const Gfx::BoundingVolume &,
                       const Gfx::BoundingVolume &);
    IntersectionRecord(const Gfx::IntersectionRecord &);
    IntersectionRecord();
    ~IntersectionRecord();
    void Set(const Gfx::TraversalPath &, const Gfx::TraversalPath &, const Gfx::TraversalPath &,
             const TMatrix4<float> &, const TMatrix4<float> &, const Vec<int> &, const Vec3<float> &,
             const Vec3<float> &, const Vec3<float> &, const Gfx::BoundingVolume &, const Gfx::BoundingVolume &);
    const Gfx::TraversalPath &GetGroupPath() const { return m_GroupPath; }
    const Gfx::TraversalPath &GetPath1() const { return m_Path1; }
    const Gfx::TraversalPath &GetPath2() const { return m_Path2; }
    const TMatrix4<float> &GetMatrix1() const { return m_Matrix1; }
    const TMatrix4<float> &GetMatrix2() const { return m_Matrix2; }
    const Vec<int> &GetAtomList() const { return m_AtomList; }
    const Vec3<float> &GetPoint1() const { return m_Point1; }
    const Vec3<float> &GetPoint2() const { return m_Point2; }
    const Vec3<float> &GetNormal() const { return m_Normal; }
    const Gfx::BoundingSphere &GetSphere1() const { return m_Sphere1; }
    const Gfx::BoundingSphere &GetSphere2() const { return m_Sphere2; }
    Gfx::IntersectionRecord &operator=(const Gfx::IntersectionRecord &);
private:
    Gfx::TraversalPath m_GroupPath;
    Gfx::TraversalPath m_Path1;
    Gfx::TraversalPath m_Path2;
    TMatrix4<float> m_Matrix1;
    TMatrix4<float> m_Matrix2;
    Vec<int> m_AtomList;
    Vec3<float> m_Point1;
    Vec3<float> m_Point2;
    Vec3<float> m_Normal;
    Gfx::BoundingSphere m_Sphere1;
    Gfx::BoundingSphere m_Sphere2;
};

// The intersections found by one traversal, last found on top.
class IntersectionStack {
public:
    IntersectionStack(const Gfx::IntersectionStack &);
    IntersectionStack();
    ~IntersectionStack();
    Gfx::IntersectionRecord &GetTop() { return m_Intersections[m_Intersections.length() - 1]; }
    Gfx::IntersectionRecord &GetIntersection(int i) { return m_Intersections[i]; }
    int GetNumberOfIntersections() { return m_Intersections.length(); }
    bool IsEmpty() const { return m_Intersections.length() == 0; }
    void Push(const Gfx::IntersectionRecord &record) { m_Intersections.append(record); }
    Gfx::IntersectionRecord Pop();
    void Clear() { m_Intersections.setLength(0); }
    Gfx::IntersectionStack &operator=(const Gfx::IntersectionStack &);
private:
    Vec<Gfx::IntersectionRecord> m_Intersections;
};
}
