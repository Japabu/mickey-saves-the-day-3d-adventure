#include "Gfx_Ray3f.h"
#include "MathConstants.h"
#include "Gfx_Plane3f.h"
#include "Gfx_Polygon3f.h"
#include "Gfx_BoundingVolume.h"
#include "Gfx_Assert.h"
#include "AVec.h"
#include <float.h>

// the original reports the build machine's __FILE__
#define kThisFile "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\Geometry.cpp"
#pragma optimize("a", on) // evidence: operator= and Get load m_Offset before the normal copy (no-alias code)

// 0x10105250 ??0Ray3f@Gfx@@QAE@XZ
Gfx::Ray3f::Ray3f() : m_Origin(0.0f, 0.0f, 0.0f), m_Direction(0.0f, 0.0f, 0.0f)
{
}

// 0x10105270 ??0Ray3f@Gfx@@QAE@ABV?$Vec3@M@@0@Z
Gfx::Ray3f::Ray3f(const Vec3<float> &origin, const Vec3<float> &direction) : m_Origin(origin), m_Direction(direction)
{
}

// 0x101052b0 ??0Ray3f@Gfx@@QAE@ABVLineSegment3f@1@@Z
Gfx::Ray3f::Ray3f(const Gfx::LineSegment3f &segment)
    : m_Origin(segment.GetEndPoint1()), m_Direction(segment.GetEndPoint2() - segment.GetEndPoint1())
{
}

// 0x10105330 ?GetNearestPoint@Ray3f@Gfx@@QBE?AV?$Vec3@M@@ABV3@@Z
Vec3<float> Gfx::Ray3f::GetNearestPoint(const Vec3<float> &point) const
{
    Vec3<float> normal = m_Direction;
    normal.normalize();
    Gfx::Plane3f plane(normal, -point.dot(normal));
    float t;
    if (plane.Intersects(*this, t))
        return GetAt(t);
    return m_Origin;
}

// 0x10105460 ?Normalize@Ray3f@Gfx@@QAEXXZ
void Gfx::Ray3f::Normalize()
{
    m_Direction.normalize();
}

// 0x101054a0 ??0LineSegment3f@Gfx@@QAE@XZ
Gfx::LineSegment3f::LineSegment3f() : m_EndPoint1(0.0f, 0.0f, 0.0f), m_EndPoint2(0.0f, 0.0f, 0.0f)
{
}

// 0x101054c0 ??0LineSegment3f@Gfx@@QAE@ABV?$Vec3@M@@0@Z
Gfx::LineSegment3f::LineSegment3f(const Vec3<float> &point1, const Vec3<float> &point2)
    : m_EndPoint1(point1), m_EndPoint2(point2)
{
}

// 0x10105500 ?FindClosestPoint@LineSegment3f@Gfx@@QBE?AV?$Vec3@M@@ABV3@@Z
Vec3<float> Gfx::LineSegment3f::FindClosestPoint(const Vec3<float> &point) const
{
    Vec3<float> normal = m_EndPoint2 - m_EndPoint1;
    normal.normalize();
    Gfx::Plane3f plane(normal, -point.dot(normal));
    float t;
    plane.Intersects(*this, t);
    if (t < 0.0f)
        return m_EndPoint1;
    if (t > 1.0f)
        return m_EndPoint2;
    return GetAt(t);
}

// Closest points of two segments: the point of this segment is returned, the
// one of the other segment goes to pointOnSegment and their difference to
// difference (the common normal where both lie inside their segments).
// Skew segments use the determinant solution of the two lines; parallel ones
// are resolved by projecting end points onto planes across the direction.
// TODO 0x101056d0 ?FindClosestPoint@LineSegment3f@Gfx@@QBE?AV?$Vec3@M@@ABV12@AAV3@1@Z: inline budget only: the original inlines Vec3::operator-= inside the inlined operator- (VC6 here calls it), calls mag2 out of line in the parallel normalize and calls operator- out of line in the last branches, so its frame is 0x18 bytes larger; the branch structure, calls and comparisons are the same (with the per-component operator- of Vec3.h this reaches 56%, but LineSegment3f::FindClosestPoint(point) then differs) (46% similar)
Vec3<float> Gfx::LineSegment3f::FindClosestPoint(const Gfx::LineSegment3f &segment, Vec3<float> &pointOnSegment, Vec3<float> &difference) const
{
    Vec3<float> closest;
    Vec3<float> direction = m_EndPoint2 - m_EndPoint1;
    Vec3<float> segmentDirection = segment.m_EndPoint2 - segment.m_EndPoint1;
    double cosAngle = direction.dot(segmentDirection) / (direction.mag() * segmentDirection.mag());
    if (fabs(cosAngle) < 1.0 - FLT_EPSILON) {
        Vec3<float> normal = direction.cross(segmentDirection);
        double scale = 1.0 / normal.mag2();
        TMatrix3<float> m;
        m(0, 0) = segment.m_EndPoint1[0] - m_EndPoint1[0];
        m(0, 1) = segment.m_EndPoint1[1] - m_EndPoint1[1];
        m(0, 2) = segment.m_EndPoint1[2] - m_EndPoint1[2];
        m(1, 0) = segmentDirection[0];
        m(1, 1) = segmentDirection[1];
        m(1, 2) = segmentDirection[2];
        m(2, 0) = normal[0];
        m(2, 1) = normal[1];
        m(2, 2) = normal[2];
        double t = m.determinant() * scale;
        m(1, 0) = direction[0];
        m(1, 1) = direction[1];
        m(1, 2) = direction[2];
        double s = m.determinant() * scale;
        if (t > 0.0 && t < 1.0) {
            if (s < 0.0) {
                pointOnSegment = segment.m_EndPoint1;
                closest = FindClosestPoint(pointOnSegment);
                difference = pointOnSegment - closest;
            } else if (s > 1.0) {
                pointOnSegment = segment.m_EndPoint2;
                closest = FindClosestPoint(pointOnSegment);
                difference = pointOnSegment - closest;
            } else {
                pointOnSegment = segment.GetAt(s);
                closest = GetAt(t);
                difference = normal;
            }
        } else if (s > 0.0 && s < 1.0) {
            if (t < 0.0) {
                pointOnSegment = segment.FindClosestPoint(m_EndPoint1);
                closest = m_EndPoint1;
            } else {
                closest = m_EndPoint2;
                pointOnSegment = segment.FindClosestPoint(closest);
            }
            difference = pointOnSegment - closest;
        } else {
            if (t < 0.0) {
                pointOnSegment = segment.FindClosestPoint(m_EndPoint1);
                closest = m_EndPoint1;
            } else {
                pointOnSegment = segment.FindClosestPoint(m_EndPoint2);
                closest = m_EndPoint2;
            }
            difference = pointOnSegment - closest;
        }
    } else {
        Vec3<float> normal = direction;
        normal.normalize();
        Gfx::Plane3f plane(normal, -normal.dot(m_EndPoint1));
        float t;
        plane.Intersects(segment, t);
        if (cosAngle > 0.0) {
            if (t > 1.0f) {
                pointOnSegment = segment.m_EndPoint2;
                closest = m_EndPoint1;
            } else if (t < 0.0f) {
                plane.Set(normal, -normal.dot(m_EndPoint2));
                plane.Intersects(segment, t);
                if (t < 0.0f) {
                    pointOnSegment = segment.m_EndPoint1;
                    closest = m_EndPoint2;
                } else if (t > 1.0f) {
                    plane.Set(normal, -normal.dot(segment.m_EndPoint1));
                    plane.Intersects(*this, t);
                    pointOnSegment = segment.m_EndPoint1;
                    closest = GetAt(t);
                } else {
                    pointOnSegment = segment.GetAt(t);
                    closest = m_EndPoint2;
                }
            } else {
                pointOnSegment = segment.GetAt(t);
                closest = m_EndPoint1;
            }
        } else {
            if (t < 0.0f) {
                pointOnSegment = segment.m_EndPoint1;
                closest = m_EndPoint1;
            } else if (t > 1.0f) {
                plane.Set(normal, -normal.dot(m_EndPoint2));
                plane.Intersects(segment, t);
                if (t > 1.0f) {
                    pointOnSegment = segment.m_EndPoint2;
                    closest = m_EndPoint2;
                } else if (t < 0.0f) {
                    plane.Set(normal, -normal.dot(segment.m_EndPoint1));
                    plane.Intersects(*this, t);
                    pointOnSegment = segment.m_EndPoint1;
                    closest = GetAt(t);
                } else {
                    pointOnSegment = segment.GetAt(t);
                    closest = m_EndPoint2;
                }
            } else {
                pointOnSegment = segment.GetAt(t);
                closest = m_EndPoint1;
            }
        }
        difference = pointOnSegment - closest;
    }
    return closest;
}

VisTData<float> Gfx::Plane3f::m_TData;

// 0x10106470 ??0Plane3f@Gfx@@QAE@XZ
Gfx::Plane3f::Plane3f()
{
    m_Normal[0] = 0.0f;
    m_Normal[1] = 0.0f;
    m_Normal[2] = 0.0f;
    m_Offset = 0.0f;
}

// 0x10106480 ??0Plane3f@Gfx@@QAE@ABV?$Vec3@M@@M@Z
Gfx::Plane3f::Plane3f(const Vec3<float> &normal, float offset) : m_Normal(normal), m_Offset(offset)
{
}

// 0x101064b0 ??4Plane3f@Gfx@@QAEAAV01@ABV01@@Z
Gfx::Plane3f &Gfx::Plane3f::operator=(const Gfx::Plane3f &plane)
{
    m_Normal = plane.m_Normal;
    m_Offset = plane.m_Offset;
    return *this;
}

// 0x101064e0 ?Set@Plane3f@Gfx@@QAEXABV?$Vec3@M@@M@Z
void Gfx::Plane3f::Set(const Vec3<float> &normal, float offset)
{
    m_Normal = normal;
    m_Offset = offset;
}

// TODO 0x10106510 ?Set@Plane3f@Gfx@@QAEXPAPAV?$Vec3@M@@H@Z: x87 register allocation of the Newell loop only: the original keeps center[0] in memory and loads each sum before the difference; same operations and stores (83% similar)
void Gfx::Plane3f::Set(Vec3<float> **points, int nPoints)
{
    GFX_ASSERT(nPoints >= 3, kThisFile, 376);
    Vec3<float> normal;
    Vec3<float> center;
    normal.zero();
    center.zero();
    for (int i = 0, j = nPoints - 1; i < nPoints; j = i, i++) {
        Vec3<float> &u = *points[j];
        Vec3<float> &v = *points[i];
        normal[0] += (u[1] - v[1]) * (u[2] + v[2]);
        normal[1] += (u[2] - v[2]) * (u[0] + v[0]);
        normal[2] += (u[0] - v[0]) * (u[1] + v[1]);
        center += v;
    }
    normal.normalize();
    m_Normal = normal;
    center /= (float)nPoints;
    m_Offset = -center.dot(normal);
}

// The same Newell normal and centroid for points kept in a vector.
// TODO 0x101066f0 ?Set@Plane3f@Gfx@@QAEXAAV?$AVec@V?$Vec3@M@@@@H@Z: operand order of two commutative fadds only (the original loads v[2] and v[1] before adding u[2]/u[1]; written orders make no difference) (91% similar)
void Gfx::Plane3f::Set(AVec<Vec3<float> > &points, int nPoints)
{
    GFX_ASSERT(nPoints >= 3, kThisFile, 409);
    Vec3<float> normal;
    Vec3<float> center;
    normal.zero();
    center.zero();
    for (int i = 0, j = nPoints - 1; i < nPoints; j = i, i++) {
        Vec3<float> &u = points[j];
        Vec3<float> &v = points[i];
        normal[0] += (u[1] - v[1]) * (u[2] + v[2]);
        normal[1] += (u[2] - v[2]) * (u[0] + v[0]);
        normal[2] += (u[0] - v[0]) * (u[1] + v[1]);
        center += v;
    }
    normal.normalize();
    m_Normal = normal;
    center /= (float)nPoints;
    m_Offset = -center.dot(normal);
}

// TODO 0x101068e0 ?Set@Plane3f@Gfx@@QAEXAAV?$Vec3@M@@00@Z: x87 stack use of the inlined cross product differs (the original reloads v.x/v.y from memory and pops two values at the end)
void Gfx::Plane3f::Set(Vec3<float> &a, Vec3<float> &b, Vec3<float> &c)
{
    Vec3<float> u = b - a;
    Vec3<float> v = c - a;
    m_Normal = u.cross(v);
    m_Normal.normalize();
    m_Offset = -a.dot(m_Normal);
}

// 0x10106a00 ?Get@Plane3f@Gfx@@QAEXAAV?$Vec3@M@@AAM@Z
void Gfx::Plane3f::Get(Vec3<float> &normal, float &offset)
{
    normal = m_Normal;
    offset = m_Offset;
}

// TODO 0x10106a30 ?Intersects@Plane3f@Gfx@@QBE_NABVRay3f@2@AAM@Z: the original shares the "t = max; return false" tail between the t < 0 and the parallel case and reloads &t in each path; here t stays in ebx (early-return form: separate tails)
bool Gfx::Plane3f::Intersects(const Gfx::Ray3f &ray, float &t) const
{
    float denom = m_Normal.dot(ray.GetDirection());
    if (Abs(denom) > m_TData.epsilon) {
        t = -(ray.GetOrigin().dot(m_Normal) + m_Offset) / denom;
        if (t >= 0.0f)
            return true;
    } else if (Abs(ray.GetOrigin().dot(m_Normal) + m_Offset) < m_TData.epsilon) {
        t = 0.0f;
        return true;
    }
    t = m_TData.max;
    return false;
}

// 0x10106b20 ?Intersects@Plane3f@Gfx@@QBE_NABVLineSegment3f@2@AAM@Z
bool Gfx::Plane3f::Intersects(const Gfx::LineSegment3f &segment, float &t) const
{
    Vec3<float> direction = segment.GetEndPoint2() - segment.GetEndPoint1();
    float denom = m_Normal.dot(direction);
    if (Abs(denom) > m_TData.epsilon) {
        t = -(segment.GetEndPoint1().dot(m_Normal) + m_Offset) / denom;
        if (t < 0.0f || t > 1.0f)
            return false;
        return true;
    }
    if (Abs(segment.GetEndPoint1().dot(m_Normal) + m_Offset) < m_TData.epsilon) {
        t = 0.0f;
        return true;
    }
    t = m_TData.max;
    return false;
}

// 0x10106c80 ??0Polygon3f@Gfx@@QAE@XZ
Gfx::Polygon3f::Polygon3f()
{
    m_PointStatus = Null;
    m_NumberOfPoints = 0;
}

// 0x10106ca0 ??0Polygon3f@Gfx@@QAE@PAPAV?$Vec3@M@@H@Z
Gfx::Polygon3f::Polygon3f(Vec3<float> **points, int n)
{
    Share(points, n);
}

// 0x10106cc0 ??1Polygon3f@Gfx@@QAE@XZ
Gfx::Polygon3f::~Polygon3f()
{
}

// 0x10106cd0 ?Share@Polygon3f@Gfx@@QAEXPAPAV?$Vec3@M@@H@Z
void Gfx::Polygon3f::Share(Vec3<float> **points, int n)
{
    m_Plane.Set(points, n);
    m_vPoints = points;
    m_NumberOfPoints = n;
    m_PointStatus = Shared;
    _setProjCoords();
}

// 0x10106d00 ?Set@Polygon3f@Gfx@@QAEXPAPAV?$Vec3@M@@H@Z
void Gfx::Polygon3f::Set(Vec3<float> **points, int n)
{
    m_Plane.Set(points, n);
    m_vPoints = points;
    m_NumberOfPoints = n;
    m_PointStatus = Mine;
    _setProjCoords();
}

// 0x10106d30 ?GetEdge@Polygon3f@Gfx@@QBE?AVLineSegment3f@2@H@Z
Gfx::LineSegment3f Gfx::Polygon3f::GetEdge(int i) const
{
    return Gfx::LineSegment3f(*m_vPoints[i % m_NumberOfPoints], *m_vPoints[(i + 1) % m_NumberOfPoints]);
}


// Inside test in the projection plane: barycentric coordinates for a
// triangle, the crossings test (Haines) otherwise.
// TODO 0x10106d80 ?IsAround@Polygon3f@Gfx@@QBE_NABV?$Vec3@M@@@Z: the crossings loop matches; in the triangle branch the original keeps px/py in slots of their own and the inverse in y's slot (frame 8 bytes larger, the triangle-fail path returns through its own tail), one product loads its other operand; same arithmetic and compares (75% similar)
bool Gfx::Polygon3f::IsAround(const Vec3<float> &point) const
{
    int X = _projCoordX;
    int Y = _projCoordY;
    float x = point(X);
    float y = point(Y);
    bool inside = false;
    if (m_NumberOfPoints == 3) {
        const Vec3<float> &p0 = *m_vPoints[0];
        const Vec3<float> &p1 = *m_vPoints[1];
        const Vec3<float> &p2 = *m_vPoints[2];
        float e1x = p1(X) - p0(X);
        float e1y = p1(Y) - p0(Y);
        float e2x = p2(X) - p0(X);
        float e2y = p2(Y) - p0(Y);
        float px = x - p0(X);
        float py = y - p0(Y);
        float inverse = 1.0 / (e1x * e2y - e2x * e1y);
        float u = (px * e2y - py * e2x) * inverse;
        if (u >= 0.0 && u <= 1.0) {
            float v = (py * e1x - e1y * px) * inverse;
            if (v >= 0.0 && v <= 1.0) {
                if (u + v <= 1.0)
                    return true;
                return false;
            }
        }
    } else {
        // the first vertex is read with a Vec3 stride (sic): *m_vPoints[n - 1][y]
        int yflag0 = *m_vPoints[m_NumberOfPoints - 1][Y] > y;
        for (int j = 0, i = m_NumberOfPoints - 1; j < m_NumberOfPoints; i = j, j++) {
            const Vec3<float> &vtx1 = *m_vPoints[j];
            int yflag1 = vtx1(Y) > y;
            if (yflag0 != yflag1) {
                const Vec3<float> &vtx0 = *m_vPoints[i];
                if (((vtx0(X) - vtx1(X)) * (vtx1(Y) - y)
                        >= (vtx0(Y) - vtx1(Y)) * (vtx1(X) - x)) == yflag1)
                    inside = !inside;
            }
            yflag0 = yflag1;
        }
    }
    return inside;
}

// 0x10106fa0 ?Intersects@Polygon3f@Gfx@@QBE_NABVRay3f@2@AAM@Z
bool Gfx::Polygon3f::Intersects(const Gfx::Ray3f &ray, float &t) const
{
    if (m_Plane.Intersects(ray, t) && IsAround(ray.GetAt(t)))
        return true;
    return false;
}

// 0x10107030 ?Intersects@Polygon3f@Gfx@@QBE_NABVLineSegment3f@2@AAM@Z
bool Gfx::Polygon3f::Intersects(const Gfx::LineSegment3f &segment, float &t) const
{
    if (m_Plane.Intersects(segment, t) && IsAround(segment.GetAt(t)))
        return true;
    return false;
}

// The closest points of a segment and the polygon: the end points dropped
// onto the plane (where they land inside), then every edge against the
// segment. Note the roles: for the end points the segment point is returned
// and the polygon point stored, for the edges the other way round.
// TODO 0x10107100 ?FindClosestPoint@Polygon3f@Gfx@@QBE?AV?$Vec3@M@@ABVLineSegment3f@2@AAV3@1@Z: instruction scheduling only, in the second end point's update (the original loads closest[0]/[1] after the pointOnPolygon stores; every statement order tried is further off) (98% similar)
Vec3<float> Gfx::Polygon3f::FindClosestPoint(const Gfx::LineSegment3f &segment, Vec3<float> &pointOnPolygon, Vec3<float> &difference) const
{
    difference = Vec3<float>(0.0f, 0.0f, 0.0f);
    float minDistance = FLT_MAX;
    Vec3<float> closest;
    float distance = m_Plane.ComputeDistance(segment.GetEndPoint1());
    float distanceSq = distance * distance;
    Gfx::Ray3f ray;
    ray.SetOrigin(segment.GetEndPoint1());
    if (distance > 0.0f)
        ray.SetDirection(-m_Plane.GetNormal());
    else
        ray.SetDirection(m_Plane.GetNormal());
    float t;
    if (m_Plane.Intersects(ray, t)) {
        Vec3<float> point = ray.GetAt(t);
        if (IsAround(point)) {
            closest = segment.GetEndPoint1();
            pointOnPolygon = point;
            minDistance = distanceSq;
            difference = -ray.GetDirection();
        }
    }
    distance = m_Plane.ComputeDistance(segment.GetEndPoint2());
    distanceSq = distance * distance;
    ray.SetOrigin(segment.GetEndPoint2());
    if (distance > 0.0f)
        ray.SetDirection(-m_Plane.GetNormal());
    else
        ray.SetDirection(m_Plane.GetNormal());
    if (m_Plane.Intersects(ray, t)) {
        Vec3<float> point = ray.GetAt(t);
        if (IsAround(point) && distanceSq < minDistance) {
            closest = segment.GetEndPoint2();
            pointOnPolygon = point;
            minDistance = distanceSq;
            difference = -ray.GetDirection();
        }
    }
    for (int i = 0; i < m_NumberOfPoints; i++) {
        Vec3<float> onSegment, edgeDifference;
        Vec3<float> onEdge = GetEdge(i).FindClosestPoint(segment, onSegment, edgeDifference);
        distanceSq = (onEdge - onSegment).mag2();
        if (distanceSq < minDistance) {
            closest = onEdge;
            pointOnPolygon = onSegment;
            minDistance = distanceSq;
            difference = edgeDifference;
        }
    }
    return closest;
}

// The squared distance of a segment to the polygon (the same steps as
// FindClosestPoint without the points).
// 0x10107620 ?ComputeDistanceSquared@Polygon3f@Gfx@@QBEMABVLineSegment3f@2@@Z
float Gfx::Polygon3f::ComputeDistanceSquared(const Gfx::LineSegment3f &segment) const
{
    float minDistance = FLT_MAX;
    float distance = m_Plane.ComputeDistance(segment.GetEndPoint1());
    float distanceSq = distance * distance;
    Gfx::Ray3f ray;
    ray.SetOrigin(segment.GetEndPoint1());
    if (distance > 0.0f)
        ray.SetDirection(-m_Plane.GetNormal());
    else
        ray.SetDirection(m_Plane.GetNormal());
    float t;
    if (m_Plane.Intersects(ray, t)) {
        Vec3<float> point = ray.GetAt(t);
        if (IsAround(point))
            minDistance = distanceSq;
    }
    distance = m_Plane.ComputeDistance(segment.GetEndPoint2());
    distanceSq = distance * distance;
    ray.SetOrigin(segment.GetEndPoint2());
    if (distance > 0.0f)
        ray.SetDirection(-m_Plane.GetNormal());
    else
        ray.SetDirection(m_Plane.GetNormal());
    if (m_Plane.Intersects(ray, t)) {
        Vec3<float> point = ray.GetAt(t);
        if (IsAround(point) && distanceSq < minDistance)
            minDistance = distanceSq;
    }
    for (int i = 0; i < m_NumberOfPoints; i++) {
        Vec3<float> onSegment, edgeDifference;
        Vec3<float> onEdge = GetEdge(i).FindClosestPoint(segment, onSegment, edgeDifference);
        distanceSq = (onEdge - onSegment).mag2();
        if (distanceSq < minDistance)
            minDistance = distanceSq;
    }
    return minDistance;
}

// Two polygons intersect when an edge of one, taken as a ray from the
// previous corner, meets the other within the edge.
// TODO 0x10107990 ?Intersects@Polygon3f@Gfx@@QBE_NABV12@@Z: scheduling of the six array stores at the top only (the original reads the other polygon before this one's point list); the loops match (89% similar)
bool Gfx::Polygon3f::Intersects(const Gfx::Polygon3f &other) const
{
    int counts[2];
    Vec3<float> **points[2];
    const Gfx::Polygon3f *polygons[2];
    counts[0] = m_NumberOfPoints;
    polygons[0] = this;
    counts[1] = other.m_NumberOfPoints;
    polygons[1] = &other;
    points[0] = m_vPoints;
    // the other polygon's first corner pointer is taken for its list (sic)
    points[1] = (Vec3<float> **)other.m_vPoints[0];
    bool hit = false;
    for (int k = 0, j = 1; k < 2 && !hit; j = k, k++) {
        int n = counts[k];
        Vec3<float> **corner = points[k];
        Vec3<float> **previous = &corner[n - 1];
        for (int i = 0; i < n && !hit; i++) {
            Gfx::Ray3f ray(**previous, **corner - **previous);
            float t;
            if (polygons[j]->Intersects(ray, t))
                hit = t >= 0.0f && t <= 1.0;
            previous = corner;
            corner++;
        }
    }
    return hit;
}

// The smallest distance between corners of the two polygons (the other
// polygon's corners are read as one array behind its first pointer).
// TODO 0x10107ad0 ?ComputeApproximateDistance@Polygon3f@Gfx@@QBEMABV12@@Z: register allocation only (eax/edx swapped for the other polygon and the point lists) (87% similar)
float Gfx::Polygon3f::ComputeApproximateDistance(const Gfx::Polygon3f &other) const
{
    float minDistance = FLT_MAX;
    int otherCount = other.m_NumberOfPoints;
    for (int i = 0; i < m_NumberOfPoints; i++) {
        for (int j = 0; j < otherCount; j++) {
            float distance = (*m_vPoints[i] - other.m_vPoints[0][j]).mag();
            if (distance < minDistance)
                minDistance = distance;
        }
    }
    return minDistance;
}

// The distance of two polygons: corners of each dropped onto the other's
// plane where they land inside it, then every pair of edges.
// TODO 0x10107bc0 ?ComputeDistance@Polygon3f@Gfx@@QBEMABV12@@Z: instruction order only: the original loads the edge pointer into ebx before the loop test, this tree after it; a load without side effects, the logic is the same (97% similar)
float Gfx::Polygon3f::ComputeDistance(const Gfx::Polygon3f &other) const
{
    const Gfx::Polygon3f *polygons[2] = { this, &other };
    int counts[2] = { m_NumberOfPoints, other.m_NumberOfPoints };
    Vec3<float> **points[2] = { m_vPoints, other.m_vPoints };
    const Gfx::Plane3f *planes[2] = { &m_Plane, &other.m_Plane };
    float minDistance = FLT_MAX;
    int i;
    for (int k = 0, j = 1; k < 2; j = k, k++) {
        for (i = 0; i < counts[k]; i++) {
            Vec3<float> &point = *points[k][i];
            float d = planes[j]->ComputeDistance(point);
            float distance = Abs(d);
            if (distance < minDistance && polygons[j]->IsAround(point - planes[j]->GetNormal() * d))
                minDistance = distance;
        }
    }
    if (minDistance > 0.0) {
        Vec3<float> **edges[2][2];
        edges[0][0] = &points[0][counts[0] - 1];
        edges[0][1] = points[0];
        for (i = 0; i < counts[0]; i++) {
            edges[1][0] = &points[1][counts[1] - 1];
            edges[1][1] = points[1];
            for (int m = 0; m < counts[1]; m++) {
                _coniderEdgeDistance(edges, minDistance);
                edges[1][0] = edges[1][1];
                edges[1][1]++;
            }
            edges[0][0] = edges[0][1];
            edges[0][1]++;
        }
    }
    return minDistance;
}

// 0x10107dd0 ?_setProjCoords@Polygon3f@Gfx@@IAEXXZ
void Gfx::Polygon3f::_setProjCoords()
{
    const Vec3<float> &normal = m_Plane.GetNormal();
    if (Abs(normal(0)) > Abs(normal(1))) {
        if (Abs(normal(0)) > Abs(normal(2))) {
            _projCoordX = 1;
            _projCoordY = 2;
            return;
        }
    } else if (Abs(normal(1)) > Abs(normal(2))) {
        _projCoordX = 0;
        _projCoordY = 2;
        return;
    }
    _projCoordX = 0;
    _projCoordY = 1;
}

// The distance of two edges (edges[k][0] to edges[k][1]) if it is smaller
// than minDistance: the distance of the two lines where each edge crosses
// the other's line, else the distance of the closest end points.
// TODO 0x10107e90 ?_coniderEdgeDistance@Polygon3f@Gfx@@IBEXQAY01PAPAV?$Vec3@M@@AAM@Z: inline budget: the original inlines all three operator- (with -=) at the top and operator-/mag2 in the end-point loop, VC6 here calls some of them out of line (frame 4 bytes larger, other slots); the branch structure (shared success tail, loop tests k < 2 at the bottom, unsigned i), the operations and the compares are the original's (51% similar)
void Gfx::Polygon3f::_coniderEdgeDistance(Vec3<float> **edges[][2], float &minDistance) const
{
    Vec3<float> offset = **edges[1][0] - **edges[0][0];
    Vec3<float> direction[2] = { **edges[0][1] - **edges[0][0], **edges[1][1] - **edges[1][0] };
    direction[0].normalize();
    direction[1].normalize();
    Vec3<float> normal = direction[0].cross(direction[1]);
    float length = normal.mag();
    float distance;
    bool found;
    if (Abs(length) > 0.0001) {
        distance = Abs(normal.dot(offset)) / length;
        if (!(distance < minDistance))
            return;
        found = true;
        for (int k = 0, j = 1; k < 2 && found; j = k, k++) {
            Vec3<float> side = direction[k].cross(normal);
            float base = side.dot(**edges[k][0]);
            float first = side.dot(**edges[j][0]) - base;
            found = (side.dot(**edges[j][1]) - base) * first <= 0.0;
        }
    } else {
        Vec3<float> between = offset.cross(direction[0]);
        distance = between.mag();
        if (!(distance < minDistance))
            return;
        Vec3<float> axis = direction[0];
        float start = axis.dot(**edges[0][0]);
        bool beyondStart = false;
        unsigned int i;
        for (i = 0; i < 2 && !beyondStart; i++)
            beyondStart = axis.dot(**edges[1][i]) - start >= 0.0;
        float end = axis.dot(**edges[0][1]);
        bool beforeEnd = false;
        for (i = 0; i < 2 && !beforeEnd; i++)
            beforeEnd = axis.dot(**edges[1][i]) - end <= 0.0;
        found = beyondStart && beforeEnd;
    }
    if (found) {
        minDistance = distance;
        return;
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float endDistance = (**edges[0][i] - **edges[1][j]).mag();
            if (endDistance < minDistance)
                minDistance = endDistance;
        }
    }
}

// 0x10105160 _$E2
// 0x10105170 _$E1
// 0x10105190 _$E5
// 0x101051a0 _$E4
// 0x101051c0 _$E8
// 0x101051d0 _$E7
// 0x101051f0 _$E11
// 0x10105200 _$E10
// 0x10105220 _$E14
// 0x10105230 _$E13
// 0x10106440 _$E33
// 0x10106450 _$E32
