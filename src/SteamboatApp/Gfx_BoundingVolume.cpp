#pragma optimize("a", on) // evidence: the constructors that copy a centre, Reset and the operators match only with no-alias code
#include "Gfx_BoundingVolume.h"
#include "MathConstants.h"
#include "Gfx_MatrixStack.h"
#include "Gfx_Ray3f.h"
#include "Gfx_Plane3f.h"
#include "Gfx_Polygon3f.h"
#include "AVec.h"
#include <float.h>

// 0x100fcc90 _$E7
// 0x100fcca0 _$E6
// 0x100fccc0 _$E10
// 0x100fccd0 _$E9
// 0x100fccf0 _$E13
// 0x100fcd00 _$E12
// 0x100fcd20 _$E16
// 0x100fcd30 _$E15
// 0x100fcd50 _$E19
// 0x100fcd60 _$E18
// 0x100fcd80 _$E23
// 0x100fcd90 _$E22
VisTData<float> Gfx::BoundingVolume::m_TData;

// 0x100fcdb0 _$E29
// 0x100fcdc0 _$E25
// 0x100fce20 _$E28
// 0x100fce30 _$E26
// 0x100fce50 _$E34
// 0x100fce60 _$E31
// 0x100fcec0 _$E33
// 0x100fced0 _$E32
// 0x100fcef0 _$E39
// 0x100fcf00 _$E36
// 0x100fcf60 _$E38
// 0x100fcf70 _$E37
Gfx::MatrixStack<float, 32> Gfx::BoundingVolume::s_LeftIntersectionMatrixStack;
Gfx::MatrixStack<float, 32> Gfx::BoundingVolume::s_RightIntersectionMatrixStack;
Gfx::MatrixStack<float, 32> Gfx::BoundingVolume::s_IntersectionMatrixStack;
bool Gfx::BoundingVolume::s_SwapIntersectionOperands = false;

// TODO 0x100fcf90 ?PushLeftIntersectionMatrix@BoundingVolume@Gfx@@SAXABV?$TMatrix4@M@@@Z: inlining differs (original inlines multInverseAffine but calls SquareMatrix copy ctor, setValues and the second Push)
void Gfx::BoundingVolume::PushLeftIntersectionMatrix(const TMatrix4<float> &matrix)
{
    if (s_SwapIntersectionOperands) {
        s_RightIntersectionMatrixStack.Push(matrix * s_RightIntersectionMatrixStack.GetTop());
        s_IntersectionMatrixStack.Push(matrix * s_IntersectionMatrixStack.GetTop());
    } else {
        s_LeftIntersectionMatrixStack.Push(matrix * s_LeftIntersectionMatrixStack.GetTop());
        s_IntersectionMatrixStack.Push(s_IntersectionMatrixStack.GetTop().multInverseAffine(matrix));
    }
}

// TODO 0x100fd1c0 ?PushRightIntersectionMatrix@BoundingVolume@Gfx@@SAXABV?$TMatrix4@M@@@Z: same inlining difference as PushLeftIntersectionMatrix (mirror image of it; the original calls the SquareMatrix copy constructor, setValues and the last Push out of line)
void Gfx::BoundingVolume::PushRightIntersectionMatrix(const TMatrix4<float> &matrix)
{
    if (s_SwapIntersectionOperands) {
        s_LeftIntersectionMatrixStack.Push(matrix * s_LeftIntersectionMatrixStack.GetTop());
        s_IntersectionMatrixStack.Push(s_IntersectionMatrixStack.GetTop().multInverseAffine(matrix));
    } else {
        s_RightIntersectionMatrixStack.Push(matrix * s_RightIntersectionMatrixStack.GetTop());
        s_IntersectionMatrixStack.Push(matrix * s_IntersectionMatrixStack.GetTop());
    }
}

// 0x100fd3f0 ?PopLeftIntersectionMatrix@BoundingVolume@Gfx@@SAXXZ
void Gfx::BoundingVolume::PopLeftIntersectionMatrix()
{
    if (s_SwapIntersectionOperands)
        s_RightIntersectionMatrixStack.Pop();
    else
        s_LeftIntersectionMatrixStack.Pop();
    s_IntersectionMatrixStack.Pop();
}

// 0x100fd430 ?PopRightIntersectionMatrix@BoundingVolume@Gfx@@SAXXZ
void Gfx::BoundingVolume::PopRightIntersectionMatrix()
{
    if (s_SwapIntersectionOperands)
        s_LeftIntersectionMatrixStack.Pop();
    else
        s_RightIntersectionMatrixStack.Pop();
    s_IntersectionMatrixStack.Pop();
}

// 0x100fd470 ?SwapIntersectionOperands@BoundingVolume@Gfx@@SAXXZ
void Gfx::BoundingVolume::SwapIntersectionOperands()
{
    s_SwapIntersectionOperands = !s_SwapIntersectionOperands;
}

// 0x100fd480 ?GetLeftIntersectionMatrix@BoundingVolume@Gfx@@SAABV?$TMatrix4@M@@XZ
const TMatrix4<float> &Gfx::BoundingVolume::GetLeftIntersectionMatrix()
{
    if (s_SwapIntersectionOperands)
        return s_RightIntersectionMatrixStack.GetTop();
    return s_LeftIntersectionMatrixStack.GetTop();
}

// 0x100fd4b0 ?GetRightIntersectionMatrix@BoundingVolume@Gfx@@SAABV?$TMatrix4@M@@XZ
const TMatrix4<float> &Gfx::BoundingVolume::GetRightIntersectionMatrix()
{
    if (s_SwapIntersectionOperands)
        return s_LeftIntersectionMatrixStack.GetTop();
    return s_RightIntersectionMatrixStack.GetTop();
}

// 0x100fd4e0 ?GetIntersectionMatrix@BoundingVolume@Gfx@@SAABV?$TMatrix4@M@@XZ
const TMatrix4<float> &Gfx::BoundingVolume::GetIntersectionMatrix()
{
    return s_IntersectionMatrixStack.GetTop();
}

// 0x100fd4f0 ?IntersectionOperandsAreSwapped@BoundingVolume@Gfx@@SA_NXZ
bool Gfx::BoundingVolume::IntersectionOperandsAreSwapped()
{
    return s_SwapIntersectionOperands;
}

// 0x100fd500 ??0BoundingSphere@Gfx@@QAE@XZ
Gfx::BoundingSphere::BoundingSphere() : m_Radius(-1.0f)
{
}

// 0x100fd520 ??0BoundingSphere@Gfx@@QAE@ABV?$Vec3@M@@M@Z
Gfx::BoundingSphere::BoundingSphere(const Vec3<float> &center, float radius)
{
    SetCenter(center);
    m_Radius = radius;
}

// 0x100fd560 ??0BoundingSphere@Gfx@@QAE@ABVBoundingVolume@1@@Z
Gfx::BoundingSphere::BoundingSphere(const Gfx::BoundingVolume &volume)
    : BoundingVolume(volume.GetCenter()), m_Radius(volume.GetRadius())
{
}

// TODO 0x100fd5d0 ??0BoundingSphere@Gfx@@QAE@ABVBoundingBox@1@_N@Z: x87 register use of the running maximum differs
Gfx::BoundingSphere::BoundingSphere(const Gfx::BoundingBox &box, bool largestExtent)
    : BoundingVolume(box.GetCenter())
{
    if (largestExtent) {
        Vec3<float> extent = box.GetMaxima() - GetCenter();
        m_Radius = extent[0];
        m_Radius = Max(m_Radius, extent[1]);
        m_Radius = Max(m_Radius, extent[2]);
    } else {
        Vec3<float> extent = box.GetMaxima() - GetCenter();
        m_Radius = extent.mag();
    }
}

// Ritter's sphere: the two extreme points of the axis with the largest
// spread give the first sphere, then every point outside grows it.
// The original reads the y and z coordinates of a point from p[3] and p[6]
// (both loops) while it copies p[0..2]: kept as it is.
// EQUIV 0x100fd700 ??0BoundingSphere@Gfx@@QAE@ABV?$RVec@M@@HH@Z: reviewed: the centre is summed dia1 + dia2 as in the original; what remains is the order of two independent fmul on different x87 registers for the third centre component (fxch pairs) and one stack load scheduled an instruction earlier; same operations, operands and stores (the z grouping in the source does not change it)
Gfx::BoundingSphere::BoundingSphere(const RVec<float> &values, int stride, int count)
{
    const float *p = values;
    Vec3<float> xMin, xMax, yMin, yMax, zMin, zMax;
    xMin[0] = yMin[1] = zMin[2] = FLT_MAX;
    xMax[0] = yMax[1] = zMax[2] = -FLT_MAX;
    const float *point = p;
    int i;
    for (i = 0; i < count; i++) {
        if (point[0] < xMin[0])
            xMin = *(const Vec3<float> *)point;
        if (point[0] > xMax[0])
            xMax = *(const Vec3<float> *)point;
        if (point[3] < yMin[1])
            yMin = *(const Vec3<float> *)point;
        if (point[3] > yMax[1])
            yMax = *(const Vec3<float> *)point;
        if (point[6] < zMin[2])
            zMin = *(const Vec3<float> *)point;
        if (point[6] > zMax[2])
            zMax = *(const Vec3<float> *)point;
        point += stride;
    }
    float dx = xMax[0] - xMin[0];
    float dy = xMax[1] - xMin[1];
    float dz = xMax[2] - xMin[2];
    float xSpan = dx * dx + dy * dy + dz * dz;
    dx = yMax[0] - yMin[0];
    dy = yMax[1] - yMin[1];
    dz = yMax[2] - yMin[2];
    float ySpan = dx * dx + dy * dy + dz * dz;
    dx = zMax[0] - zMin[0];
    dy = zMax[1] - zMin[1];
    dz = zMax[2] - zMin[2];
    float zSpan = dx * dx + dy * dy + dz * dz;
    Vec3<float> dia1 = xMin;
    Vec3<float> dia2 = xMax;
    float maxSpan = xSpan;
    if (ySpan > maxSpan) {
        maxSpan = ySpan;
        dia1 = yMin;
        dia2 = yMax;
    }
    if (zSpan > maxSpan) {
        dia1 = zMin;
        dia2 = zMax;
    }
    SetCenter(Vec3<float>((dia1[0] + dia2[0]) * 0.5f, (dia1[1] + dia2[1]) * 0.5f, (dia1[2] + dia2[2]) * 0.5f));
    dx = dia2[0] - GetCenter()[0];
    dy = dia2[1] - GetCenter()[1];
    dz = dia2[2] - GetCenter()[2];
    float radius2 = dx * dx + dy * dy + dz * dz;
    m_Radius = (float)sqrt(radius2);
    for (i = 0; i < count; i++) {
        dx = p[i * stride] - GetCenter()[0];
        dy = p[i * stride + 3] - GetCenter()[1];
        dz = p[i * stride + 6] - GetCenter()[2];
        float distance2 = dx * dx + dy * dy + dz * dz;
        if (distance2 > radius2) {
            float distance = (float)sqrt(distance2);
            m_Radius = (m_Radius + distance) * 0.5f;
            radius2 = m_Radius * m_Radius;
            SetCenter(Vec3<float>((m_Radius * GetCenter()[0] + (distance - m_Radius) * p[i * stride]) / distance,
                (m_Radius * GetCenter()[1] + (distance - m_Radius) * p[i * stride + 3]) / distance,
                ((distance - m_Radius) * p[i * stride + 6] + m_Radius * GetCenter()[2]) / distance));
        }
    }
}

// 0x100fdb20 ?Contains@BoundingSphere@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingSphere::Contains(const Vec3<float> &point) const
{
    Vec3<float> d = point - GetCenter();
    return d.mag2() <= m_Radius * m_Radius;
}

#pragma optimize("a", off) // evidence: these three keep the vftable load after the stores through the result references

// 0x100fdbd0 ?Contains@BoundingSphere@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingSphere::Contains(const Gfx::BoundingSphere &sphere) const
{
    Vec3<float> dir = sphere.GetCenter() - GetCenter();
    dir.normalize();
    return Contains(sphere.GetCenter() + dir * sphere.m_Radius);
}

// 0x100fdcf0 ?Intersects@BoundingSphere@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingSphere::Intersects(const Gfx::BoundingSphere &sphere) const
{
    Vec3<float> dir = sphere.GetCenter() - GetCenter();
    dir.normalize();
    Vec3<float> nearest = sphere.GetCenter() - dir * sphere.m_Radius;
    return Contains(nearest);
}

// 0x100fde30 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABV3@AAV3@@Z
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Vec3<float> &point, Vec3<float> &direction) const
{
    direction = point - GetCenter();
    direction.normalize();
    return GetCenter() + direction * m_Radius;
}

#pragma optimize("a", on) // back to no-alias code for the rest of the unit

// TODO 0x100fdf40 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABV12@AAV3@1ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft sums each row starting with elements[3][j] in the original
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::BoundingSphere &sphere, Vec3<float> &spherePoint,
    Vec3<float> &direction, const TMatrix4<float> &matrix) const
{
    Vec3<float> center = matrix.multVec3PointLeft(sphere.GetCenter());
    Vec3<float> point = FindClosestPoint(center, direction);
    spherePoint = center - direction * sphere.GetRadius();
    return point;
}

// TODO 0x100fe0e0 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABVBoundingBox@2@AAV3@1ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeftAffine/multVector3Left sum each row in another order in the original (open problem, see NOTES)
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::BoundingBox &box, Vec3<float> &boxPoint,
    Vec3<float> &direction, const TMatrix4<float> &matrix) const
{
    Gfx::BoundingSphere sphere = TransformInverseAffine(matrix);
    for (int i = 0; i < 3; i++) {
        if (sphere.GetCenter()[i] < box.GetMinima()[i])
            boxPoint[i] = box.GetMinima()[i];
        else if (sphere.GetCenter()[i] > box.GetMaxima()[i])
            boxPoint[i] = box.GetMaxima()[i];
        else
            boxPoint[i] = sphere.GetCenter()[i];
    }
    Vec3<float> point = sphere.FindClosestPoint(boxPoint, direction);
    point = matrix.multVec3PointLeftAffine(point);
    boxPoint = matrix.multVec3PointLeftAffine(boxPoint);
    direction = matrix.multVector3Left(direction);
    return point;
}

// 0x100fe300 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABVLineSegment3f@2@AAV3@1@Z
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::LineSegment3f &segment, Vec3<float> &segmentPoint,
    Vec3<float> &direction) const
{
    segmentPoint = segment.FindClosestPoint(GetCenter());
    return FindClosestPoint(segmentPoint, direction);
}

// 0x100fe370 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABVPlane3f@2@AAV3@1@Z
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::Plane3f &plane, Vec3<float> &planePoint,
    Vec3<float> &direction) const
{
    planePoint = GetCenter() - plane.GetNormal() * plane.ComputeDistance(GetCenter());
    return FindClosestPoint(planePoint, direction);
}

// 0x100fe460 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABVPolygon3f@2@AAV3@1@Z
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::Polygon3f &polygon, Vec3<float> &spherePoint, Vec3<float> &polygonPoint) const
{
    Vec3<float> point = FindClosestPoint(polygon.GetPlane(), spherePoint, polygonPoint);
    if (!polygon.IsAround(spherePoint)) {
        float best = (FindClosestPoint(polygon.GetEdge(0), spherePoint, polygonPoint) - spherePoint).mag2();
        for (int i = 1; i < polygon.GetNumberOfPoints(); i++) {
            Vec3<float> a, b;
            float distance = (FindClosestPoint(polygon.GetEdge(i), a, b) - a).mag2();
            if (distance < best) {
                best = distance;
                spherePoint = a;
                polygonPoint = b;
            }
        }
    }
    return point;
}

// TODO 0x100fe660 ?FindClosestPoint@BoundingSphere@Gfx@@QBE?AV?$Vec3@M@@ABVPolygon3f@2@AAV3@1ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeftAffine sums each row in another order in the original (open problem, see NOTES); multVector3Left is called out of line there
Vec3<float> Gfx::BoundingSphere::FindClosestPoint(const Gfx::Polygon3f &polygon, Vec3<float> &polygonPoint,
    Vec3<float> &direction, const TMatrix4<float> &matrix) const
{
    Gfx::BoundingSphere sphere = TransformInverseAffine(matrix);
    Vec3<float> point = sphere.FindClosestPoint(polygon.GetPlane(), polygonPoint, direction);
    if (!polygon.IsAround(polygonPoint)) {
        float best = (sphere.FindClosestPoint(polygon.GetEdge(0), polygonPoint, direction) - polygonPoint).mag2();
        for (int i = 1; i < polygon.GetNumberOfPoints(); i++) {
            Vec3<float> a, b;
            float distance = (sphere.FindClosestPoint(polygon.GetEdge(i), a, b) - a).mag2();
            if (distance < best) {
                best = distance;
                polygonPoint = a;
                direction = b;
            }
        }
    }
    point = matrix.multVec3PointLeftAffine(point);
    polygonPoint = matrix.multVec3PointLeftAffine(polygonPoint);
    direction = matrix.multVector3Left(direction);
    return point;
}

// 0x100fe9b0 ?IntersectsTransformed@BoundingSphere@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingSphere::IntersectsTransformed(const Gfx::BoundingSphere &sphere) const
{
    if (GetIntersectionMatrixStack().GetSize() == 1)
        return Intersects(sphere);
    if (IntersectionOperandsAreSwapped())
        return Intersects(sphere.TransformInverseAffine(GetIntersectionMatrix()));
    return Intersects(sphere.TransformAffine(GetIntersectionMatrix()));
}

// 0x100fea40 ?IntersectsTransformed@BoundingSphere@Gfx@@QBE_NABV12@ABV?$TMatrix4@M@@@Z
bool Gfx::BoundingSphere::IntersectsTransformed(const Gfx::BoundingSphere &sphere, const TMatrix4<float> &matrix) const
{
    if (GetIntersectionMatrixStack().GetSize() == 1)
        return Intersects(sphere);
    return Intersects(sphere.TransformAffine(matrix));
}

// 0x100feaa0 ?Intersects@BoundingSphere@Gfx@@UBE_NABVRay3f@2@AAM1AAV?$Vec3@M@@@Z
bool Gfx::BoundingSphere::Intersects(const Gfx::Ray3f &ray, float &tNear, float &tFar, Vec3<float> &) const
{
    Vec3<float> direction = ray.GetDirection();
    float scale = 1.0 / sqrt(direction.mag2());
    direction.multScal(scale);
    Vec3<float> toCenter = GetCenter() - ray.GetOrigin();
    float projection = toCenter.dot(direction);
    float discriminant = m_Radius * m_Radius - (toCenter.mag2() - projection * projection);
    if (discriminant > 0.0f) {
        float root = (float)sqrt(discriminant);
        tNear = (projection - root) * scale;
        tFar = (projection + root) * scale;
        return true;
    }
    return false;
}

// 0x100fec00 ?Intersects@BoundingSphere@Gfx@@QBE_NABVPolygon3f@2@@Z
bool Gfx::BoundingSphere::Intersects(const Gfx::Polygon3f &polygon) const
{
    Vec3<float> spherePoint, polygonPoint;
    FindClosestPoint(polygon, spherePoint, polygonPoint);
    if ((spherePoint - GetCenter()).mag2() < m_Radius * m_Radius)
        return true;
    return false;
}

// 0x100fecc0 ?Intersects@BoundingSphere@Gfx@@QBE_NABVPolygon3f@2@ABV?$TMatrix4@M@@@Z
bool Gfx::BoundingSphere::Intersects(const Gfx::Polygon3f &polygon, const TMatrix4<float> &matrix) const
{
    return TransformInverseAffine(matrix).Intersects(polygon);
}

// 0x100fed10 ?ComputeDistanceSquared@BoundingSphere@Gfx@@QBEMABVPolygon3f@2@ABV?$TMatrix4@M@@@Z
float Gfx::BoundingSphere::ComputeDistanceSquared(const Gfx::Polygon3f &polygon, const TMatrix4<float> &matrix) const
{
    Vec3<float> spherePoint, polygonPoint;
    FindClosestPoint(polygon, spherePoint, polygonPoint, matrix);
    return (spherePoint - GetCenter()).mag2();
}

// 0x100fedc0 ?Touches@BoundingSphere@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingSphere::Touches(const Vec3<float> &point) const
{
    Vec3<float> d = point - GetCenter();
    return Abs(d.mag()) == GetEpsilon() + m_Radius;
}

// 0x100fee80 ??HBoundingSphere@Gfx@@QBE?AV01@ABV?$Vec3@M@@@Z
Gfx::BoundingSphere Gfx::BoundingSphere::operator+(const Vec3<float> &point) const
{
    Gfx::BoundingSphere sphere(*this);
    return sphere += point;
}

// 0x100fef20 ??HBoundingSphere@Gfx@@QBE?AV01@ABV01@@Z
Gfx::BoundingSphere Gfx::BoundingSphere::operator+(const Gfx::BoundingSphere &other) const
{
    Gfx::BoundingSphere sphere(*this);
    return sphere += other;
}

// 0x100fefc0 ??YBoundingSphere@Gfx@@QAEAAV01@ABV?$Vec3@M@@@Z
Gfx::BoundingSphere &Gfx::BoundingSphere::operator+=(const Vec3<float> &point)
{
    if (IsValid()) {
        if (!Contains(point)) {
            Vec3<float> d = point - GetCenter();
            m_Radius = (float)fabs(d.mag());
        }
    } else {
        m_Radius = 0.0f;
        SetCenter(point);
    }
    return *this;
}

// The original computes the weight as m_Radius - radius / (2 d) (a
// precedence slip for (m_Radius - radius) / (2 d)); kept as it is.
// 0x100ff0a0 ??YBoundingSphere@Gfx@@QAEAAV01@ABV01@@Z
Gfx::BoundingSphere &Gfx::BoundingSphere::operator+=(const Gfx::BoundingSphere &sphere)
{
    if (IsValid()) {
        if (sphere.IsValid()) {
            if (sphere.Contains(*this)) {
                *this = sphere;
            } else if (!Contains(sphere)) {
                Vec3<float> d = GetCenter() - sphere.GetCenter();
                float distance = d.mag();
                float k = m_Radius - sphere.m_Radius / (2 * distance);
                SetCenter(GetCenter() * (k + 0.5f) + sphere.GetCenter() * (0.5f - k));
                m_Radius = (m_Radius + sphere.m_Radius + distance) * 0.5f;
            }
        }
    } else {
        *this = sphere;
    }
    return *this;
}

// 0x100ff280 ??8BoundingSphere@Gfx@@QAE_NABV01@@Z
bool Gfx::BoundingSphere::operator==(const Gfx::BoundingSphere &sphere)
{
    return IsValid() && sphere.IsValid() && GetCenter() == sphere.GetCenter()
        && m_Radius == sphere.m_Radius;
}

// TODO 0x100ff300 ?TransformAffine@BoundingSphere@Gfx@@QBE?AV12@ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft sums each row starting with elements[3][j] in the original (open problem, see NOTES)
Gfx::BoundingSphere Gfx::BoundingSphere::TransformAffine(const TMatrix4<float> &matrix) const
{
    return Gfx::BoundingSphere(Vec3<float>(matrix.multVec3PointLeft(GetCenter())), m_Radius);
}

// TODO 0x100ff410 ?TransformInverseAffine@BoundingSphere@Gfx@@QBE?AV12@ABV?$TMatrix4@M@@@Z: the original keeps d[1], d[2] in memory and adds the third product with fadd st(1) (same open problem as multVec3PointLeft)
Gfx::BoundingSphere Gfx::BoundingSphere::TransformInverseAffine(const TMatrix4<float> &matrix) const
{
    return Gfx::BoundingSphere(matrix.multVec3PointLeftInverseAffine(GetCenter()), m_Radius);
}

// 0x100ff4d0 ?IsValid@BoundingSphere@Gfx@@UBE_NXZ
bool Gfx::BoundingSphere::IsValid() const
{
    return m_Radius >= 0.0f;
}

// 0x100ff4f0 ?Reset@BoundingSphere@Gfx@@UAEXXZ
void Gfx::BoundingSphere::Reset()
{
    SetCenter(Vec3<float>(0.0f, 0.0f, 0.0f));
    m_Radius = -1.0f;
}

// 0x100ff530 ??0BoundingCylinder@Gfx@@QAE@XZ
Gfx::BoundingCylinder::BoundingCylinder() : m_Radius(-1.0f)
{
}

// 0x100ff550 ?GetRadius@BoundingCylinder@Gfx@@UBEMXZ

// 0x100ff560 ??_GBoundingCylinder@Gfx@@UAEPAXI@Z

// 0x100ff580 ??1BoundingCylinder@Gfx@@UAE@XZ

// 0x100ff590 ??0BoundingCylinder@Gfx@@QAE@ABV?$Vec3@M@@MM@Z
Gfx::BoundingCylinder::BoundingCylinder(const Vec3<float> &center, float radius, float height)
    : BoundingVolume(center), m_Radius(radius), m_Height(height)
{
}

// 0x100ff5d0 ??0BoundingCylinder@Gfx@@QAE@ABV01@@Z
Gfx::BoundingCylinder::BoundingCylinder(const Gfx::BoundingCylinder &cylinder)
    : BoundingVolume(cylinder.GetCenter()), m_Radius(cylinder.m_Radius), m_Height(cylinder.m_Height)
{
}

// 0x100ff610 ??0BoundingCylinder@Gfx@@QAE@ABVBoundingBox@1@_N@Z
Gfx::BoundingCylinder::BoundingCylinder(const Gfx::BoundingBox &box, bool inscribed) : BoundingVolume(box.GetCenter())
{
    if (inscribed)
        m_Radius = (box.GetMaxima() - GetCenter()).mag() * 0.70710677f;
    else
        m_Radius = (box.GetMaxima() - GetCenter()).mag();
}

// 0x100ff750 ?Contains@BoundingCylinder@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingCylinder::Contains(const Gfx::BoundingCylinder &cylinder) const
{
    Vec3<float> direction = cylinder.GetCenter();
    direction -= GetCenter();
    direction.normalize();
    Vec3<float> point = cylinder.GetCenter();
    point += direction * cylinder.m_Radius;
    return Contains(point);
}

// 0x100ff860 ?out@BoundingCylinder@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingCylinder::out(const Gfx::BoundingCylinder &cylinder) const
{
    Vec3<float> direction = cylinder.GetCenter();
    direction -= GetCenter();
    direction.normalize();
    Vec3<float> point = cylinder.GetCenter();
    point += direction * cylinder.m_Radius;
    return !Contains(point);
}

// 0x100ff970 ?Intersects@BoundingCylinder@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingCylinder::Intersects(const Gfx::BoundingCylinder &cylinder) const
{
    return !Contains(cylinder) && !out(cylinder);
}

// 0x100ff9a0 ?Contains@BoundingCylinder@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingCylinder::Contains(const Vec3<float> &point) const
{
    return (point - GetCenter()).mag() < m_Radius + GetEpsilon();
}

// 0x100ffa50 ?Touches@BoundingCylinder@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingCylinder::Touches(const Vec3<float> &point) const
{
    return Abs((point - GetCenter()).mag()) == m_Radius + GetEpsilon();
}

// 0x100ffb10 ?Intersects@BoundingCylinder@Gfx@@UBE_NABVRay3f@2@AAM1AAV?$Vec3@M@@@Z
bool Gfx::BoundingCylinder::Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const
{
    return false;
}

// 0x100ffb20 ??HBoundingCylinder@Gfx@@QBE?AV01@ABV?$Vec3@M@@@Z
Gfx::BoundingCylinder Gfx::BoundingCylinder::operator+(const Vec3<float> &point) const
{
    Gfx::BoundingCylinder cylinder(*this);
    if (!cylinder.Contains(point))
        cylinder.m_Radius = Abs((point - cylinder.GetCenter()).mag());
    return cylinder;
}

// 0x100ffc10 ??HBoundingCylinder@Gfx@@QBE?AV01@ABV01@@Z
Gfx::BoundingCylinder Gfx::BoundingCylinder::operator+(const Gfx::BoundingCylinder &other) const
{
    Gfx::BoundingCylinder cylinder(*this);
    Vec3<float> center = other.GetCenter();
    if (!cylinder.Contains(center))
        cylinder.m_Radius = Abs((center - cylinder.GetCenter()).mag());
    return cylinder;
}

// 0x100ffd20 ??YBoundingCylinder@Gfx@@QAEAAV01@ABV?$Vec3@M@@@Z
Gfx::BoundingCylinder &Gfx::BoundingCylinder::operator+=(const Vec3<float> &point)
{
    if (!Contains(point))
        m_Radius = Abs((point - GetCenter()).mag());
    return *this;
}

// 0x100ffdd0 ??YBoundingCylinder@Gfx@@QAEAAV01@ABV01@@Z
Gfx::BoundingCylinder &Gfx::BoundingCylinder::operator+=(const Gfx::BoundingCylinder &cylinder)
{
    if (!Contains(cylinder))
        m_Radius = Abs((cylinder.GetCenter() - GetCenter()).mag());
    return *this;
}

// 0x100ffe90 ??8BoundingCylinder@Gfx@@QAE_NABV01@@Z
bool Gfx::BoundingCylinder::operator==(const Gfx::BoundingCylinder &cylinder)
{
    return GetCenter() == cylinder.GetCenter() && m_Radius == cylinder.m_Radius && m_Height == cylinder.m_Height;
}

// 0x100fff10 ?IsValid@BoundingCylinder@Gfx@@UBE_NXZ
bool Gfx::BoundingCylinder::IsValid() const
{
    return m_Radius > -1.0f && m_Height > -1.0f;
}

// 0x100fff40 ?Reset@BoundingCylinder@Gfx@@UAEXXZ
void Gfx::BoundingCylinder::Reset()
{
    SetCenter(Vec3<float>(0.0f, 0.0f, 0.0f));
    m_Radius = -1.0f;
    m_Height = -1.0f;
}

// 0x100fff90 ??0BoundingBox@Gfx@@QAE@ABVBoundingSphere@1@@Z
Gfx::BoundingBox::BoundingBox(const Gfx::BoundingSphere &sphere)
{
    Vec3<float> radius(sphere.GetRadius(), sphere.GetRadius(), sphere.GetRadius());
    m_Minima = sphere.GetCenter() - radius;
    m_Maxima = sphere.GetCenter() + radius;
}

// Arvo's method: start from the translation, add the smaller and the
// larger product of every matrix entry with the old extent.
// 0x101000a0 ?TransformAffine@BoundingBox@Gfx@@QBE?AV12@ABV?$TMatrix4@M@@@Z
Gfx::BoundingBox Gfx::BoundingBox::TransformAffine(const TMatrix4<float> &matrix) const
{
    Gfx::BoundingBox box(matrix[3], matrix[3]);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float a = matrix[j][i] * m_Minima[j];
            float b = matrix[j][i] * m_Maxima[j];
            if (a < b) {
                box.m_Minima[i] += a;
                box.m_Maxima[i] += b;
            } else {
                box.m_Minima[i] += b;
                box.m_Maxima[i] += a;
            }
        }
    }
    return box;
}

// The same in the frame of the inverse of an affine matrix.
// TODO 0x101001f0 ?TransformInverseAffine@BoundingBox@Gfx@@QBE?AV12@ABV?$TMatrix4@M@@@Z: the inlined multVector3Right sums left to right keeping the partial sum (fadd st(1)) in the original (open problem, see NOTES), and the original copies the translation row with integer moves; the loops match
Gfx::BoundingBox Gfx::BoundingBox::TransformInverseAffine(const TMatrix4<float> &matrix) const
{
    Vec3<float> translation = matrix.multVector3Right(-Vec3<float>(matrix[3][0], matrix[3][1], matrix[3][2]));
    Gfx::BoundingBox box(translation, translation);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float a = matrix[i][j] * m_Minima[j];
            float b = matrix[i][j] * m_Maxima[j];
            if (a < b) {
                box.m_Minima[i] += a;
                box.m_Maxima[i] += b;
            } else {
                box.m_Minima[i] += b;
                box.m_Maxima[i] += a;
            }
        }
    }
    return box;
}

// Grows (minima, maxima) to hold the projected point p.
#define EXTEND_BOUNDS(minima, maxima, p) \
    { \
        for (int k = 0; k < 3; k++) { \
            if (p[k] < minima[k]) \
                minima[k] = p[k]; \
            if (p[k] > maxima[k]) \
                maxima[k] = p[k]; \
        } \
    }

// Box around the eight corners after a projective transformation.
// TODO 0x101003e0 ?Transform4@BoundingBox@Gfx@@QAEXABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft sums each row starting with elements[3][j] in the original (open problem, see NOTES)
void Gfx::BoundingBox::Transform4(const TMatrix4<float> &matrix)
{
    Vec3<float> minima(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3<float> maxima(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (unsigned int i = 0; i < 8; i++) {
        Vec4<float> p = matrix.multVec3PointLeft(GetCorner(i));
        p.unhomo();
        EXTEND_BOUNDS(minima, maxima, p)
    }
    m_Minima = minima;
    m_Maxima = maxima;
}

// The same with the corners written out.
// TODO 0x10100580 ?Transform4X@BoundingBox@Gfx@@QAEXABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft sums each row starting with elements[3][j] in the original (open problem, see NOTES); the original calls Vec3(x, y, z), multVec3PointLeft and unhomo out of line for the last two corners
void Gfx::BoundingBox::Transform4X(const TMatrix4<float> &matrix)
{
    Vec3<float> minima(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3<float> maxima(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    Vec4<float> p;
    p = matrix.multVec3PointLeft(Vec3<float>(m_Minima[0], m_Minima[1], m_Minima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Maxima[0], m_Minima[1], m_Minima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Minima[0], m_Maxima[1], m_Minima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Maxima[0], m_Maxima[1], m_Minima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Minima[0], m_Minima[1], m_Maxima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Maxima[0], m_Minima[1], m_Maxima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Minima[0], m_Maxima[1], m_Maxima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    p = matrix.multVec3PointLeft(Vec3<float>(m_Maxima[0], m_Maxima[1], m_Maxima[2]));
    p.unhomo();
    EXTEND_BOUNDS(minima, maxima, p)
    m_Minima = minima;
    m_Maxima = maxima;
}

// 0x10100d30 ?GetCorner@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@I@Z
Vec3<float> Gfx::BoundingBox::GetCorner(unsigned int corner) const
{
    switch (corner) {
    case 0:
        return m_Minima;
    case 1:
        return Vec3<float>(m_Maxima[0], m_Minima[1], m_Minima[2]);
    case 2:
        return Vec3<float>(m_Minima[0], m_Maxima[1], m_Minima[2]);
    case 3:
        return Vec3<float>(m_Maxima[0], m_Maxima[1], m_Minima[2]);
    case 4:
        return Vec3<float>(m_Minima[0], m_Minima[1], m_Maxima[2]);
    case 5:
        return Vec3<float>(m_Maxima[0], m_Minima[1], m_Maxima[2]);
    case 6:
        return Vec3<float>(m_Minima[0], m_Maxima[1], m_Maxima[2]);
    case 7:
        return m_Maxima;
    default:
        return GetCenter();
    }
}

// 0x10100ee0 ?GetEdge@BoundingBox@Gfx@@QBE?AVLineSegment3f@2@I@Z
Gfx::LineSegment3f Gfx::BoundingBox::GetEdge(unsigned int edge) const
{
    unsigned int axis = edge % 3;
    switch (edge / 3) {
    case 0:
        return Gfx::LineSegment3f(m_Minima, GetCorner(1 << axis));
    case 1:
        return Gfx::LineSegment3f(GetCorner(3), GetCorner((1 << axis) ^ 3));
    case 2:
        return Gfx::LineSegment3f(GetCorner(5), GetCorner((1 << axis) ^ 5));
    default:
        return Gfx::LineSegment3f(GetCorner(6), GetCorner((1 << axis) ^ 6));
    }
}

// 0x10100fe0 ?GetSurfaceArea@BoundingBox@Gfx@@QBEMXZ
float Gfx::BoundingBox::GetSurfaceArea() const
{
    Vec3<float> d = GetDiagonal();
    return 2.0f * (d[0] * (d[1] + d[2]) + d[1] * d[2]);
}

// Closest point of the box to a point and the outward normal there; a
// point inside snaps to the nearest face. The original takes the normal's
// sign from the face coordinate itself (_copysign(1, face)); kept.
// EQUIV 0x10101030 ?FindClosestPoint@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@ABV3@AAV3@@Z: reviewed: register choice only; the face coordinate and the result copy use eax/ecx/edx swapped, same instructions in the same order
Vec3<float> Gfx::BoundingBox::FindClosestPoint(const Vec3<float> &point, Vec3<float> &normal) const
{
    Vec3<float> closest;
    Vec3<float> face;
    int outside = 0;
    int axis = 0;
    float best;
    for (int i = 0; i < 3; i++) {
        if (point[i] >= m_Maxima[i]) {
            closest[i] = m_Maxima[i];
            normal[i] = 1.0f;
            outside = 1;
        } else if (point[i] <= m_Minima[i]) {
            closest[i] = m_Minima[i];
            normal[i] = -1.0f;
            outside = 1;
        } else {
            float toMaximum = m_Maxima[i] - point[i];
            float toMinimum = point[i] - m_Minima[i];
            closest[i] = point[i];
            normal[i] = 0.0f;
            face[i] = toMinimum < toMaximum ? m_Minima[i] : m_Maxima[i];
            if (i == 0) {
                best = toMinimum < toMaximum ? toMinimum : toMaximum;
            } else {
                float distance = toMinimum < toMaximum ? toMinimum : toMaximum;
                if (distance < best) {
                    axis = i;
                    best = distance;
                }
            }
        }
    }
    if (!outside) {
        closest[axis] = face[axis];
        normal[axis] = (float)_copysign(1.0, face[axis]);
    }
    return closest;
}

// Closest point of the box to a segment: the best of the twelve edges and
// the two end points; the normal is turned away from the centre. When an end
// point wins, the original projects segmentPoint (not the end point) and
// leaves segmentPoint as it is; kept.
// TODO 0x10101160 ?FindClosestPoint@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@ABVLineSegment3f@2@AAV3@1@Z: stack layout only: same instruction sequence (offsets aside), but the original frame is 0x80 bytes (0x6c here) with other slots for the temporaries
Vec3<float> Gfx::BoundingBox::FindClosestPoint(const Gfx::LineSegment3f &segment, Vec3<float> &segmentPoint,
    Vec3<float> &normal) const
{
    Vec3<float> point = GetEdge(0).FindClosestPoint(segment, segmentPoint, normal);
    float best = (point - segmentPoint).mag2();
    for (int i = 1; i < 12; i++) {
        Vec3<float> a, b;
        Vec3<float> edgePoint = GetEdge(i).FindClosestPoint(segment, a, b);
        float distance = (edgePoint - a).mag2();
        if (distance < best) {
            best = distance;
            point = edgePoint;
            segmentPoint = a;
            normal = b;
        }
    }
    {
        Vec3<float> end = segment.GetEndPoint1();
        float distance = ComputeDistanceSquared(end);
        if (distance < best) {
            best = distance;
            point = FindClosestPoint(segmentPoint, normal);
        }
    }
    {
        Vec3<float> end = segment.GetEndPoint2();
        if (ComputeDistanceSquared(end) < best)
            point = FindClosestPoint(segmentPoint, normal);
    }
    if ((point - GetCenter()).dot(normal) < 0.0f)
        normal = -normal;
    return point;
}

// Closest points of the box and a polygon given in the frame of matrix:
// the polygon's edges against the box in box space, then the box's edges
// against the polygon in polygon space.
// TODO 0x101014d0 ?FindClosestPoint@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@ABVPolygon3f@2@AAV3@1ABV?$TMatrix4@M@@@Z: inlining and the multVec3PointLeftAffine row sums differ (the original calls the inverse-affine and Vec3(FixedVec4) helpers, the second multVec3PointLeftAffine, multVector3Left, mag2 and operator- out of line)
Vec3<float> Gfx::BoundingBox::FindClosestPoint(const Gfx::Polygon3f &polygon, Vec3<float> &polygonPoint,
    Vec3<float> &normal, const TMatrix4<float> &matrix) const
{
    Gfx::LineSegment3f segment;
    segment = polygon.GetEdge(0).TransformInverseAffine(matrix);
    Vec3<float> point = FindClosestPoint(segment, polygonPoint, normal);
    float best = (point - polygonPoint).mag2();
    int i;
    for (i = 1; i < polygon.GetNumberOfPoints(); i++) {
        segment = polygon.GetEdge(i).TransformInverseAffine(matrix);
        Vec3<float> a, b;
        Vec3<float> boxPoint = FindClosestPoint(segment, a, b);
        float distance = (boxPoint - a).mag2();
        if (distance < best) {
            best = distance;
            point = boxPoint;
            polygonPoint = a;
            normal = b;
        }
    }
    point = matrix.multVec3PointLeftAffine(point);
    polygonPoint = matrix.multVec3PointLeftAffine(polygonPoint);
    normal = matrix.multVector3Left(normal);
    for (i = 0; i < 12; i++) {
        Gfx::LineSegment3f edge;
        edge = GetEdge(i).Transform(matrix);
        Vec3<float> a, b;
        Vec3<float> onPolygon = polygon.FindClosestPoint(edge, a, b);
        float distance = (onPolygon - a).mag2();
        if (distance < best) {
            best = distance;
            point = a;
            polygonPoint = onPolygon;
            normal = -b;
        }
    }
    return point;
}

// Closest points of this box and another one given in the frame of matrix:
// the other box's edges, transformed, against this box.
// TODO 0x101019c0 ?FindClosestPoint@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@ABV12@AAV3@1ABV?$TMatrix4@M@@@Z: inlining differs (the original inlines the second Vec3(FixedVec4) of the loop's Transform and calls the others out of line)
Vec3<float> Gfx::BoundingBox::FindClosestPoint(const Gfx::BoundingBox &box, Vec3<float> &boxPoint,
    Vec3<float> &normal, const TMatrix4<float> &matrix) const
{
    Gfx::LineSegment3f segment;
    segment = box.GetEdge(0).Transform(matrix);
    Vec3<float> point = FindClosestPoint(segment, boxPoint, normal);
    float best = (point - boxPoint).mag2();
    for (int i = 1; i < 12; i++) {
        Gfx::LineSegment3f edge;
        edge = box.GetEdge(i).Transform(matrix);
        Vec3<float> a, b;
        Vec3<float> onBox = FindClosestPoint(edge, a, b);
        float distance = (onBox - a).mag2();
        if (distance < best) {
            best = distance;
            point = onBox;
            boxPoint = a;
            normal = b;
        }
    }
    return point;
}

// TODO 0x10101ce0 ?FindClosestPoint@BoundingBox@Gfx@@QBE?AV?$Vec3@M@@ABVBoundingSphere@2@AAV3@1ABV?$TMatrix4@M@@@Z: VC6 here loads the centre coordinate once before the first test, the original in each branch
Vec3<float> Gfx::BoundingBox::FindClosestPoint(const Gfx::BoundingSphere &sphere, Vec3<float> &spherePoint, Vec3<float> &normal, const TMatrix4<float> &matrix) const
{
    Gfx::BoundingSphere transformed = sphere.TransformAffine(matrix);
    Vec3<float> point;
    float distance = 0.0f;
    for (int i = 0; i < 3; i++) {
        if (transformed.GetCenter()[i] < m_Minima[i]) {
            float d = transformed.GetCenter()[i] - m_Minima[i];
            point[i] = m_Minima[i];
            distance += d * d;
        } else if (transformed.GetCenter()[i] > m_Maxima[i]) {
            float d = transformed.GetCenter()[i] - m_Maxima[i];
            point[i] = m_Maxima[i];
            distance += d * d;
        } else {
            point[i] = transformed.GetCenter()[i];
        }
    }
    spherePoint = transformed.FindClosestPoint(point, normal);
    return point;
}

// 0x10101de0 ?ComputeDistanceSquared@BoundingBox@Gfx@@QBEMABV?$Vec3@M@@@Z
float Gfx::BoundingBox::ComputeDistanceSquared(const Vec3<float> &point) const
{
    Vec3<float> d;
    for (int i = 0; i < 3; i++) {
        if (point[i] > m_Maxima[i])
            d[i] = point[i] - m_Maxima[i];
        else if (point[i] < m_Minima[i])
            d[i] = m_Minima[i] - point[i];
        else
            d[i] = 0.0f;
    }
    return d.mag2();
}

// 0x10101e60 ?ComputeDistanceSquared@BoundingBox@Gfx@@QBEMABVLineSegment3f@2@@Z
float Gfx::BoundingBox::ComputeDistanceSquared(const Gfx::LineSegment3f &segment) const
{
    float distance = ComputeDistanceSquared(segment.GetEndPoint1());
    float d = ComputeDistanceSquared(segment.GetEndPoint2());
    if (distance >= d)
        distance = d;
    for (int i = 0; i < 12; i++) {
        Vec3<float> boxPoint, segmentPoint;
        d = (segment.FindClosestPoint(GetEdge(i), boxPoint, segmentPoint) - boxPoint).mag2();
        if (distance >= d)
            distance = d;
    }
    return distance;
}

// Squared distance between two boxes: the gaps along the separated axes.
// TODO 0x10101f60 ?ComputeDistanceSquared@BoundingBox@Gfx@@QBEMABV12@@Z: the original walks the two boxes with other base registers and compares a copy of each gap (fld st(0); fcomp) where VC6 here uses fcom; same operations (76% similar)
float Gfx::BoundingBox::ComputeDistanceSquared(const Gfx::BoundingBox &box) const
{
    float gaps[3];
    float *gap = gaps;
    for (int i = 0; i < 3; i++) {
        if (m_Minima[i] - box.m_Maxima[i] > 0.0)
            *gap++ = m_Minima[i] - box.m_Maxima[i];
        else if (box.m_Minima[i] - m_Maxima[i] > 0.0)
            *gap++ = box.m_Minima[i] - m_Maxima[i];
    }
    float count = (float)(gap - gaps);
    if (count == 0.0f)
        return 0.0f;
    if (count == 1.0f)
        return gaps[0] * gaps[0];
    float sum = 0.0f;
    for (float *g = gaps; g < gap; g++)
        sum += *g * *g;
    return sum;
}

// Squared distance to a box given in the frame of matrix: corners both
// ways, then the other box's edges.
// TODO 0x10102040 ?ComputeDistanceSquared@BoundingBox@Gfx@@QBEMABV12@ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeftAffine sums each row in another order in the original (open problem, see NOTES)
float Gfx::BoundingBox::ComputeDistanceSquared(const Gfx::BoundingBox &box, const TMatrix4<float> &matrix) const
{
    float best = ComputeDistanceSquared(matrix.multVec3PointLeftAffine(box.GetCorner(0)));
    int i;
    for (i = 1; i < 8; i++) {
        float d = ComputeDistanceSquared(matrix.multVec3PointLeftAffine(box.GetCorner(i)));
        if (best >= d)
            best = d;
    }
    for (i = 0; i < 8; i++) {
        float d = box.ComputeDistanceSquared(matrix.multVec3PointLeftInverseAffine(GetCorner(i)));
        if (best >= d)
            best = d;
    }
    for (i = 0; i < 12; i++) {
        float d = ComputeDistanceSquared(box.GetEdge(i).Transform(matrix));
        if (best >= d)
            best = d;
    }
    return best;
}

// Squared distance to a polygon given in the frame of matrix.
// TODO 0x101022e0 ?ComputeDistanceSquared@BoundingBox@Gfx@@QBEMABVPolygon3f@2@ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft/multVector3Right row sums differ (open problem, see NOTES) and the original calls Vec3(x, y, z) and Vec3(FixedVec4) out of line for one end point each
float Gfx::BoundingBox::ComputeDistanceSquared(const Gfx::Polygon3f &polygon, const TMatrix4<float> &matrix) const
{
    float best = FLT_MAX;
    int i;
    for (i = 0; i < 12; i++) {
        float d = polygon.ComputeDistanceSquared(GetEdge(i).TransformInverseAffine(matrix));
        if (best >= d)
            best = d;
    }
    for (i = 0; i < polygon.GetNumberOfPoints(); i++) {
        float d = ComputeDistanceSquared(polygon.GetEdge(i).Transform(matrix));
        if (best >= d)
            best = d;
    }
    return best;
}

// 0x10102570 ?Intersects@BoundingBox@Gfx@@QBE_NABV12@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::BoundingBox &box) const
{
    return m_Maxima[0] >= box.m_Minima[0] && m_Minima[0] <= box.m_Maxima[0]
        && m_Maxima[1] >= box.m_Minima[1] && m_Minima[1] <= box.m_Maxima[1]
        && m_Maxima[2] >= box.m_Minima[2] && m_Minima[2] <= box.m_Maxima[2];
}

// 0x101025d0 ?Intersects@BoundingBox@Gfx@@QBE_NABVBoundingSphere@2@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::BoundingSphere &sphere) const
{
    float d = 0.0f;
    for (int i = 0; i < 3; i++) {
        if (sphere.GetCenter()[i] < m_Minima[i])
            d += (sphere.GetCenter()[i] - m_Minima[i]) * (sphere.GetCenter()[i] - m_Minima[i]);
        else if (sphere.GetCenter()[i] > m_Maxima[i])
            d += (sphere.GetCenter()[i] - m_Maxima[i]) * (sphere.GetCenter()[i] - m_Maxima[i]);
    }
    return d <= sphere.GetRadius() * sphere.GetRadius();
}

// Box against box under the current intersection matrix: the transformed
// boxes must overlap, then some edge of one must cross the other.
// TODO 0x10102690 ?IntersectsTransformed@BoundingBox@Gfx@@QBE_NABV12@@Z: inlining of the segment transforms differs (the original calls Vec3(x, y, z), multVector3Right, multVec3PointLeftInverseAffine, multVec3PointLeft and Vec3(FixedVec4) out of line in different places)
bool Gfx::BoundingBox::IntersectsTransformed(const Gfx::BoundingBox &box) const
{
    if (GetIntersectionMatrixStack().GetSize() == 1)
        return Intersects(box);
    int i;
    if (IntersectionOperandsAreSwapped()) {
        if (!Intersects(box.TransformInverseAffine(GetIntersectionMatrix())))
            return false;
        for (i = 0; i < 12; i++) {
            if (Intersects(box.GetEdge(i).TransformInverseAffine(GetIntersectionMatrix())))
                return true;
        }
        for (i = 0; i < 12; i++) {
            if (box.Intersects(GetEdge(i).Transform(GetIntersectionMatrix())))
                return true;
        }
        return false;
    }
    if (!Intersects(box.TransformAffine(GetIntersectionMatrix())))
        return false;
    for (i = 0; i < 12; i++) {
        if (Intersects(box.GetEdge(i).Transform(GetIntersectionMatrix())))
            return true;
    }
    for (i = 0; i < 12; i++) {
        if (box.Intersects(GetEdge(i).TransformInverseAffine(GetIntersectionMatrix())))
            return true;
    }
    return false;
}

// TODO 0x10102a00 ?IntersectsTransformed@BoundingBox@Gfx@@QBE_NABV12@ABV?$TMatrix4@M@@@Z: inlining of the segment transforms differs (as in IntersectsTransformed(box))
bool Gfx::BoundingBox::IntersectsTransformed(const Gfx::BoundingBox &box, const TMatrix4<float> &matrix) const
{
    Gfx::BoundingBox transformed = box.TransformAffine(matrix);
    if (!Intersects(transformed))
        return false;
    int i;
    for (i = 0; i < 12; i++) {
        if (Intersects(box.GetEdge(i).Transform(matrix)))
            return true;
    }
    for (i = 0; i < 12; i++) {
        if (box.Intersects(GetEdge(i).TransformInverseAffine(matrix)))
            return true;
    }
    return false;
}

// 0x10102bd0 ?IntersectsTransformed@BoundingBox@Gfx@@QBE_NABVBoundingSphere@2@@Z
bool Gfx::BoundingBox::IntersectsTransformed(const Gfx::BoundingSphere &sphere) const
{
    if (GetIntersectionMatrixStack().GetSize() == 1)
        return Intersects(sphere);
    if (IntersectionOperandsAreSwapped())
        return Intersects(sphere.TransformInverseAffine(GetIntersectionMatrix()));
    return Intersects(sphere.TransformAffine(GetIntersectionMatrix()));
}

// 0x10102c60 ?IntersectsTransformed@BoundingBox@Gfx@@QBE_NABVBoundingSphere@2@ABV?$TMatrix4@M@@@Z
bool Gfx::BoundingBox::IntersectsTransformed(const Gfx::BoundingSphere &sphere, const TMatrix4<float> &matrix) const
{
    return Intersects(sphere.TransformAffine(matrix));
}

// 0x10102cb0 ?Intersects@BoundingBox@Gfx@@QBE_NABVLineSegment3f@2@AAM1AAV?$Vec3@M@@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::LineSegment3f &segment, float &tNear, float &tFar, Vec3<float> &point) const
{
    return BoundingBox::Intersects(Gfx::Ray3f(segment.GetEndPoint1(), segment.GetEndPoint2() - segment.GetEndPoint1()), tNear, tFar, point)
        && tNear < 1.0;
}

// Slab test (in double precision). The entry face is 2 * axis, plus one
// for the minimum side; the original then indexes the ray and the box with
// the face number instead of the axis (kept).
// 0x10102d60 ?Intersects@BoundingBox@Gfx@@UBE_NABVRay3f@2@AAM1AAV?$Vec3@M@@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::Ray3f &ray, float &tNear, float &tFar, Vec3<float> &normal) const
{
    double tMin[3], tMax[3];
    for (int i = 0; i < 3; i++) {
        if (Abs(ray.GetDirection()[i]) > GetEpsilon()) {
            double inverse = 1.0 / ray.GetDirection()[i];
            double t1 = (m_Minima[i] - ray.GetOrigin()[i]) * inverse;
            double t2 = (m_Maxima[i] - ray.GetOrigin()[i]) * inverse;
            if (t1 < t2) {
                tMin[i] = t1;
                tMax[i] = t2;
            } else {
                tMin[i] = t2;
                tMax[i] = t1;
            }
        } else if (ray.GetOrigin()[i] >= m_Minima[i] && ray.GetOrigin()[i] <= m_Maxima[i]) {
            tMin[i] = -GetMaximum();
            tMax[i] = GetMaximum();
        } else {
            tMin[i] = GetMaximum();
            tMax[i] = -GetMaximum();
        }
    }
    double nearest = tMin[0];
    int face = 0;
    if (tMin[1] > nearest) {
        nearest = tMin[1];
        face = 2;
    }
    if (tMin[2] > nearest) {
        nearest = tMin[2];
        face = 4;
    }
    double farthest = tMax[0] < tMax[1] ? tMax[0] : tMax[1];
    if (tMax[2] < farthest)
        farthest = tMax[2];
    if (nearest <= farthest && farthest > 0.0) {
        if (ray.GetOrigin()[face] < m_Minima[face]
            || (ray.GetOrigin()[face] < m_Maxima[face] && ray.GetDirection()[face] < 0))
            face++;
        tNear = (float)nearest;
        tFar = (float)farthest;
        switch (face) {
        case 0:
            normal = Vec3<float>(1.0f, 0.0f, 0.0f);
            return true;
        case 1:
            normal = Vec3<float>(-1.0f, 0.0f, 0.0f);
            return true;
        case 2:
            normal = Vec3<float>(0.0f, 1.0f, 0.0f);
            return true;
        case 3:
            normal = Vec3<float>(0.0f, -1.0f, 0.0f);
            return true;
        case 4:
            normal = Vec3<float>(0.0f, 0.0f, 1.0f);
            return true;
        case 5:
            normal = Vec3<float>(0.0f, 0.0f, -1.0f);
            return true;
        }
        return true;
    }
    tNear = GetMaximum();
    tFar = -GetMaximum();
    return false;
}

// Slab test of a segment without divisions: the slab parameters are kept
// scaled by |d| and compared crosswise. The final test compares the
// products tMin * |d| and tMax * |d| of the two chosen axes (kept).
// 0x10103090 ?Intersects@BoundingBox@Gfx@@QBE_NABVLineSegment3f@2@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::LineSegment3f &segment) const
{
    Vec3<float> d;
    d[0] = segment.GetEndPoint2()[0] - segment.GetEndPoint1()[0];
    d[1] = segment.GetEndPoint2()[1] - segment.GetEndPoint1()[1];
    d[2] = segment.GetEndPoint2()[2] - segment.GetEndPoint1()[2];
    Vec3<float> length;
    length[0] = (float)fabs(d[0]);
    length[1] = (float)fabs(d[1]);
    length[2] = (float)fabs(d[2]);
    double tMin[3], tMax[3];
    for (int i = 0; i < 3; i++) {
        if (length[i] > GetEpsilon()) {
            double sign = _copysign(1.0, d[i]);
            double t1 = (m_Minima[i] - segment.GetEndPoint1()[i]) * sign;
            double t2 = (m_Maxima[i] - segment.GetEndPoint1()[i]) * sign;
            if (t1 < t2) {
                tMin[i] = t1;
                tMax[i] = t2;
            } else {
                tMin[i] = t2;
                tMax[i] = t1;
            }
        } else if (segment.GetEndPoint1()[i] >= m_Minima[i] && segment.GetEndPoint1()[i] <= m_Maxima[i]) {
            tMin[i] = -GetMaximum();
            tMax[i] = GetMaximum();
        } else {
            tMin[i] = GetMaximum();
            tMax[i] = -GetMaximum();
        }
    }
    int nearest = length[0] * tMin[1] > length[1] * tMin[0] ? 1 : 0;
    if (length[nearest] * tMin[2] > length[2] * tMin[nearest])
        nearest = 2;
    int farthest = length[0] * tMax[1] < length[1] * tMax[0] ? 1 : 0;
    if (length[farthest] * tMax[2] < length[2] * tMax[farthest])
        farthest = 2;
    if (length[nearest] * tMin[nearest] <= length[farthest] * tMax[farthest] && tMax[farthest] > 0.0
        && length[nearest] > tMin[nearest])
        return true;
    return false;
}

// 0x10103290 ?Intersects@BoundingBox@Gfx@@QBE_NABVPolygon3f@2@@Z
bool Gfx::BoundingBox::Intersects(const Gfx::Polygon3f &polygon) const
{
    int n = polygon.GetNumberOfPoints();
    Vec3<float> **points = polygon.GetPoints();
    int i;
    for (i = 0; i < n; i++) {
        if (Contains(*points[i]))
            return true;
    }
    for (i = 0; i < 12; i++) {
        float t;
        if (polygon.Intersects(GetEdge(i), t))
            return true;
    }
    for (i = 0; i < polygon.GetNumberOfPoints(); i++) {
        float tNear, tFar;
        Vec3<float> point;
        if (Intersects(polygon.GetEdge(i), tNear, tFar, point))
            return true;
    }
    return false;
}

// The same for a polygon given in the frame of matrix.
// TODO 0x10103350 ?Intersects@BoundingBox@Gfx@@QBE_NABVPolygon3f@2@ABV?$TMatrix4@M@@@Z: the inlined multVec3PointLeft sums each row in another order in the original (open problem, see NOTES); inlining of the segment transforms differs
bool Gfx::BoundingBox::Intersects(const Gfx::Polygon3f &polygon, const TMatrix4<float> &matrix) const
{
    int n = polygon.GetNumberOfPoints();
    Vec3<float> **points = polygon.GetPoints();
    int i;
    for (i = 0; i < n; i++) {
        if (Contains(Vec3<float>(matrix.multVec3PointLeft(*points[i]))))
            return true;
    }
    for (i = 0; i < 12; i++) {
        float t;
        if (polygon.Intersects(GetEdge(i).TransformInverseAffine(matrix), t))
            return true;
    }
    for (i = 0; i < polygon.GetNumberOfPoints(); i++) {
        float tNear, tFar;
        Vec3<float> point;
        if (Intersects(polygon.GetEdge(i).Transform(matrix), tNear, tFar, point))
            return true;
    }
    return false;
}

// 0x10103620 ?Contains@BoundingBox@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingBox::Contains(const Vec3<float> &point) const
{
    bool contains = true;
    for (int i = 0; i < 3; i++) {
        if (point[i] < m_Minima[i] || point[i] > m_Maxima[i]) {
            contains = false;
            break;
        }
    }
    return contains;
}

// 0x10103660 ?Touches@BoundingBox@Gfx@@UBE_NABV?$Vec3@M@@@Z
bool Gfx::BoundingBox::Touches(const Vec3<float> &point) const
{
    bool touches = false;
    for (int i = 0; i < 3; i++) {
        if (point[i] == m_Minima[i] || point[i] == m_Maxima[i]) {
            touches = true;
            break;
        }
    }
    return touches;
}

// 0x101036a0 ??HBoundingBox@Gfx@@QBE?AV01@ABV?$Vec3@M@@@Z
Gfx::BoundingBox Gfx::BoundingBox::operator+(const Vec3<float> &point) const
{
    Gfx::BoundingBox box(*this);
    box += point;
    return box;
}

// 0x10103770 ??HBoundingBox@Gfx@@QBE?AV01@ABV01@@Z
Gfx::BoundingBox Gfx::BoundingBox::operator+(const Gfx::BoundingBox &other) const
{
    Gfx::BoundingBox box(*this);
    box += other;
    return box;
}

// 0x10103840 ??HBoundingBox@Gfx@@QBE?AV01@ABVBoundingSphere@1@@Z
Gfx::BoundingBox Gfx::BoundingBox::operator+(const Gfx::BoundingSphere &sphere) const
{
    Gfx::BoundingBox box(*this);
    box += sphere;
    return box;
}

// 0x10103910 ??YBoundingBox@Gfx@@QAEAAV01@ABV?$Vec3@M@@@Z
Gfx::BoundingBox &Gfx::BoundingBox::operator+=(const Vec3<float> &point)
{
    for (int i = 0; i < 3; i++) {
        if (point[i] < m_Minima[i])
            m_Minima[i] = point[i];
        if (point[i] > m_Maxima[i])
            m_Maxima[i] = point[i];
    }
    return *this;
}

// TODO 0x10103950 ??YBoundingBox@Gfx@@QAEAAV01@ABV01@@Z: inline budget (the original inlines the copy's Vec3(0, 0, 0), VC6 here calls it)
Gfx::BoundingBox &Gfx::BoundingBox::operator+=(const Gfx::BoundingBox &other)
{
    Gfx::BoundingBox box(other);
    for (int i = 0; i < 3; i++) {
        if (m_Minima[i] > m_Maxima[i]) {
            m_Minima[i] = box.m_Minima[i];
            m_Maxima[i] = box.m_Maxima[i];
        } else {
            if (box.m_Minima[i] < m_Minima[i])
                m_Minima[i] = box.m_Minima[i];
            if (box.m_Maxima[i] > m_Maxima[i])
                m_Maxima[i] = box.m_Maxima[i];
        }
    }
    return *this;
}

// 0x10103a10 ??YBoundingBox@Gfx@@QAEAAV01@ABVBoundingSphere@1@@Z
Gfx::BoundingBox &Gfx::BoundingBox::operator+=(const Gfx::BoundingSphere &sphere)
{
    Vec3<float> radius(sphere.GetRadius(), sphere.GetRadius(), sphere.GetRadius());
    *this += sphere.GetCenter() - radius;
    *this += sphere.GetCenter() + radius;
    return *this;
}

// 0x10103b00 ??8BoundingBox@Gfx@@QAE_NABV01@@Z
bool Gfx::BoundingBox::operator==(const Gfx::BoundingBox &box)
{
    return m_Minima == box.m_Minima && m_Maxima == box.m_Maxima;
}

// 0x10103b80 ?ExtendBy@BoundingBox@Gfx@@QAEXAAV?$AVec@V?$Vec3@M@@@@@Z
void Gfx::BoundingBox::ExtendBy(AVec<Vec3<float> > &points)
{
    int count = points.length();
    const Vec3<float> *point = points;
    for (int i = 0; i < count; i++)
        *this += point[i];
}

// 0x10103bc0 ?ExtendBy@BoundingBox@Gfx@@QAEXABV?$RVec@M@@HH@Z
void Gfx::BoundingBox::ExtendBy(const RVec<float> &values, int stride, int count)
{
    const float *value = values;
    for (int i = 0; i < count; i++) {
        *this += *(const Vec3<float> *)value;
        value += stride;
    }
}

// 0x10103c00 ?Scale@BoundingBox@Gfx@@QAEXABV?$Vec3@M@@@Z
void Gfx::BoundingBox::Scale(const Vec3<float> &scale)
{
    Vec3<float> center = GetCenter();
    m_Minima[0] = (m_Minima[0] - center[0]) * scale[0] + center[0];
    m_Minima[1] = (m_Minima[1] - center[1]) * scale[1] + center[1];
    m_Minima[2] = (m_Minima[2] - center[2]) * scale[2] + center[2];
    m_Maxima[0] = (m_Maxima[0] - center[0]) * scale[0] + center[0];
    m_Maxima[1] = (m_Maxima[1] - center[1]) * scale[1] + center[1];
    m_Maxima[2] = (m_Maxima[2] - center[2]) * scale[2] + center[2];
}

// 0x10103c90 ?IsValid@BoundingBox@Gfx@@UBE_NXZ
bool Gfx::BoundingBox::IsValid() const
{
    return m_Minima[0] <= m_Maxima[0] && m_Minima[1] <= m_Maxima[1] && m_Minima[2] <= m_Maxima[2];
}

// 0x10103cc0 ?Reset@BoundingBox@Gfx@@UAEXXZ
void Gfx::BoundingBox::Reset()
{
    m_Minima = Vec3<float>(GetMaximum(), GetMaximum(), GetMaximum());
    m_Maxima = Vec3<float>(-GetMaximum(), -GetMaximum(), -GetMaximum());
}

// Template and inline instances this unit also emits:

// 0x10103d30 ??_G?$MatrixStack@M$0CA@@Gfx@@UAEPAXI@Z
// 0x10103d50 ??G?$Vec3@M@@QBE?AV0@XZ
// TODO 0x10103db0 ?unhomo@?$FixedVec@M$03@@QAEXXZ: out-of-line copy called by Transform4X in the original; VC6 here inlines every call (body in FixedVec.h)
// TODO 0x10103dd0 ?multVec3PointLeft@?$TMatrix4@M@@QBE?AV?$Vec4@M@@ABV?$Vec3@M@@@Z: out-of-line copy (body in TMatrix4.h); not emitted here (inlined everywhere) and the row sums differ (open problem, see NOTES)
// TODO 0x10103e80 ?multVec3PointLeftInverseAffine@?$TMatrix4@M@@QBE?AV?$Vec3@M@@ABV2@@Z: out-of-line copy (body in TMatrix4.h); not emitted here (inlined everywhere) and the multVector3Right row sums differ
// TODO 0x10103f20 ?Push@?$MatrixStack@M$0CA@@Gfx@@QAEXABV?$TMatrix4@M@@@Z: out-of-line copy called by PushLeft/PushRightIntersectionMatrix in the original (body in Gfx_MatrixStack.h); not emitted here
// 0x10008b50 ??_H@YGXPAXIHP6EX0@Z@Z
// 0x100b4000 ??0?$SquareMatrix@M$03@@QAE@ABV0@@Z
// 0x100e2390 ??1BoundingSphere@Gfx@@UAE@XZ
// 0x100e23a0 ?GetCenter@BoundingVolume@Gfx@@UBEABV?$Vec3@M@@XZ
// 0x100e23b0 ??_GBoundingVolume@Gfx@@UAEPAXI@Z
// 0x100e25c0 ?GetRadius@BoundingSphere@Gfx@@UBEMXZ
// 0x100e25d0 ??_GBoundingSphere@Gfx@@UAEPAXI@Z
// 0x100f3c90 ??1BoundingVolume@Gfx@@UAE@XZ
