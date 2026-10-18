// mflinalg.dll 0x10005a80-0x10006660: MfVector3.
#include <math.h>
#include "MfMath.h"
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfTransform3.h"

#define PI 3.1415927f

// 0x10005a80 ??GMfVector3@@QBE?AV0@XZ
MfVector3 MfVector3::operator-() const
{
    return MfVector3(-_x, -_y, -_z);
}

// 0x10005ab0 ??HMfVector3@@QBE?AV0@ABV0@@Z
MfVector3 MfVector3::operator+(const MfVector3 &v) const
{
    return MfVector3(_x + v._x, _y + v._y, _z + v._z);
}

// 0x10005af0 ??GMfVector3@@QBE?AV0@ABV0@@Z
MfVector3 MfVector3::operator-(const MfVector3 &v) const
{
    return MfVector3(_x - v._x, _y - v._y, _z - v._z);
}

// 0x10005b30 ??YMfVector3@@QAEAAV0@ABV0@@Z
MfVector3 &MfVector3::operator+=(const MfVector3 &v)
{
    _x += v._x;
    _y += v._y;
    _z += v._z;
    return *this;
}

// 0x10005b60 ??ZMfVector3@@QAEAAV0@ABV0@@Z
MfVector3 &MfVector3::operator-=(const MfVector3 &v)
{
    _x -= v._x;
    _y -= v._y;
    _z -= v._z;
    return *this;
}

// 0x10005b90 ??DMfVector3@@QBE?AV0@M@Z
MfVector3 MfVector3::operator*(float s) const
{
    return MfVector3(_x * s, _y * s, _z * s);
}

// 0x10005bd0 ??KMfVector3@@QBE?AV0@M@Z
MfVector3 MfVector3::operator/(float s) const
{
    float inv = 1.0f / s;
    return MfVector3(_x * inv, _y * inv, _z * inv);
}

// 0x10005c20 ??XMfVector3@@QAEAAV0@M@Z
MfVector3 &MfVector3::operator*=(float s)
{
    _x *= s;
    _y *= s;
    _z *= s;
    return *this;
}

// 0x10005c50 ??_0MfVector3@@QAEAAV0@M@Z
MfVector3 &MfVector3::operator/=(float s)
{
    float inv = 1.0f / s;
    _x *= inv;
    _y *= inv;
    _z *= inv;
    return *this;
}

// EQUIV 0x10005c80 ??DMfVector3@@QBE?AV0@ABV0@@Z: reviewed: same three products stored to the same fields; only the schedule and the dead scratch slot (arg slot vs frame) differ
MfVector3 MfVector3::operator*(const MfVector3 &v) const
{
    return MfVector3(_x * v._x, _y * v._y, _z * v._z);
}

// EQUIV 0x10005cc0 ??XMfVector3@@QAEAAV0@ABV0@@Z: reviewed: register choice and the operand order of the three fmul (commutative)
MfVector3 &MfVector3::operator*=(const MfVector3 &v)
{
    _x *= v._x;
    _y *= v._y;
    _z *= v._z;
    return *this;
}

// 0x10005cf0 ??KMfVector3@@QBE?AV0@ABV0@@Z
MfVector3 MfVector3::operator/(const MfVector3 &v) const
{
    return MfVector3(_x / v._x, _y / v._y, _z / v._z);
}

// 0x10005d20 ??_0MfVector3@@QAEAAV0@ABV0@@Z
MfVector3 &MfVector3::operator/=(const MfVector3 &v)
{
    _x /= v._x;
    _y /= v._y;
    _z /= v._z;
    return *this;
}

// 0x10005d50 ?dot@MfVector3@@QBEMABV1@@Z
float MfVector3::dot(const MfVector3 &v) const
{
    return _x * v._x + _y * v._y + _z * v._z;
}

// 0x10005d80 ?cross@MfVector3@@QBE?AV1@ABV1@@Z
MfVector3 MfVector3::cross(const MfVector3 &v) const
{
    return MfVector3(_y * v._z - _z * v._y,
                     _z * v._x - _x * v._z,
                     _x * v._y - _y * v._x);
}

// 0x10005de0 ?angle@MfVector3@@QBEMABV1@@Z
// Angle to v, with the table square root.
float MfVector3::angle(const MfVector3 &v) const
{
    float l1 = MfMath::sqrt(lengthSquared());
    float l2 = MfMath::sqrt(v.lengthSquared());
    float c = (_x * v._x + _y * v._y + _z * v._z) / (l1 * l2);
    if (c < -1.0f)
        return PI;
    if (c > 1.0f)
        return 0.0f;
    return (float)acos(c);
}

// EQUIV 0x10005ef0 ?PreciseAngle@MfVector3@@QBEMABV1@@Z: reviewed: operand order of _x * v._x (commutative fmul), otherwise identical
float MfVector3::PreciseAngle(const MfVector3 &v) const
{
    float l1 = (float)sqrt(lengthSquared());
    float l2 = (float)sqrt(v.lengthSquared());
    if (l1 < 1e-7f || l2 < 1e-7f)
        return 0.0f;
    float c = (_x * v._x + _y * v._y + _z * v._z) / (l1 * l2);
    if (c < -1.0f)
        return PI;
    if (c > 1.0f)
        return 0.0f;
    return (float)acos(c);
}

// 0x10006000 ?length@MfVector3@@QBEMXZ
float MfVector3::length() const
{
    return (float)sqrt(lengthSquared());
}

// 0x10006040 ?unit@MfVector3@@QBE?AV1@XZ
MfVector3 MfVector3::unit() const
{
    float inv = 1.0f / length();
    return MfVector3(_x * inv, _y * inv, _z * inv);
}

// 0x100060a0 ?interpolate@MfVector3@@QAEXABV1@0M@Z
void MfVector3::interpolate(const MfVector3 &a, const MfVector3 &b, float t)
{
    _x = (b._x - a._x) * t + a._x;
    _y = (b._y - a._y) * t + a._y;
    _z = (b._z - a._z) * t + a._z;
}

// 0x100060e0 ?interpolate@MfVector3@@QAEXABV1@000M@Z
// Cubic Bezier curve through a and d with the control points b and c.
void MfVector3::interpolate(const MfVector3 &a, const MfVector3 &b,
                            const MfVector3 &c, const MfVector3 &d, float t)
{
    float s = 1.0f - t;
    float ca = s * s * s;
    float cb = 3.0f * s * s * t;
    float cc = 3.0f * s * t * t;
    float cd = t * t * t;
    _x = ca * a._x + cb * b._x + cc * c._x + cd * d._x;
    _y = ca * a._y + cb * b._y + cc * c._y + cd * d._y;
    _z = ca * a._z + cb * b._z + cc * c._z + cd * d._z;
}

// TODO 0x100061e0 ?tangent@MfVector3@@QAEXABV1@000M@Z: (75% similar) the original keeps s*s unrounded for ca and spills it (float) for cb, here the other way round, and evaluates cc as (-3t - (-6)s) t; a real rounding difference from the CSE spill choice; statement orders and include sets tried
// Unit tangent of the Bezier curve of interpolate().
void MfVector3::tangent(const MfVector3 &a, const MfVector3 &b,
                        const MfVector3 &c, const MfVector3 &d, float t)
{
    float s = 1.0f - t;
    float ca = -3.0f * (s * s);
    float cb = 3.0f * (s * s) - 6.0f * (s * t);
    float cc = (6.0f * s - 3.0f * t) * t;
    float cd = 3.0f * t * t;
    _x = ca * a._x + cb * b._x + cc * c._x + cd * d._x;
    _y = ca * a._y + cb * b._y + cc * c._y + cd * d._y;
    _z = ca * a._z + cb * b._z + cc * c._z + cd * d._z;
    normalize();
}

// TODO 0x10006350 ?Premultiply@MfVector3@@QAEXABVMfTransform3@@@Z: (36% similar) the original copies only y and z of the quaternion to locals and loads x and w directly; the a and d coefficients are kept unrounded for the x row and spilled for the others (rounding differs); best include set found 87%, not adopted
// Applies t to this point: rotation, then translation.
void MfVector3::Premultiply(const MfTransform3 &t)
{
    float w, x, y, z;
    t.rotation().getValue(w, x, y, z);
    float a = w * w - (x * x + y * y + z * z);
    float d = 2.0f * (x * _x + y * _y + z * _z);
    float w2 = 2.0f * w;
    float nx = w2 * (y * _z - z * _y) + d * x + a * _x;
    float ny = w2 * (z * _x - x * _z) + d * y + a * _y;
    float nz = w2 * (x * _y - y * _x) + d * z + a * _z;
    const MfVector3 &tr = t.translation();
    _x = nx + tr._x;
    _y = ny + tr._y;
    _z = nz + tr._z;
}

// TODO 0x10006480 ?Multiply@MfVector3@@QAEXABVMfTransform3@@ABV1@@Z: (54% similar) the original keeps a and d unrounded on the x87 stack for the x row (float-rounded for y and z), here a is rounded for every row; also loads p after copying y and z (no esi); declaration orders, accessor forms and include sets tried
// This = t applied to the point p.
void MfVector3::Multiply(const MfTransform3 &t, const MfVector3 &p)
{
    float w, x, y, z;
    t.rotation().getValue(w, x, y, z);
    float a = w * w - (x * x + y * y + z * z);
    float d = 2.0f * (x * p._x + y * p._y + z * p._z);
    float w2 = 2.0f * w;
    const MfVector3 &tr = t.translation();
    _x = w2 * (y * p._z - z * p._y) + d * x + a * p._x + tr._x;
    _y = w2 * (z * p._x - x * p._z) + d * y + a * p._y + tr._y;
    _z = w2 * (x * p._y - y * p._x) + d * z + a * p._z + tr._z;
}

// EQUIV 0x100065b0 ?NormalizeCross@MfVector3@@QAEXABV1@0@Z: reviewed: a/b register choice and the reused argument slot; symbolic x87 evaluation identical
void MfVector3::NormalizeCross(const MfVector3 &a, const MfVector3 &b)
{
    _x = a._y * b._z - a._z * b._y;
    _y = a._z * b._x - a._x * b._z;
    _z = a._x * b._y - a._y * b._x;
    float l = lengthSquared();
    l = 1.0f / (float)sqrt(l);
    _x *= l;
    _y *= l;
    _z *= l;
}
