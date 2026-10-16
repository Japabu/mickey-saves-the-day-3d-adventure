// mflinalg.dll 0x100052c0-0x10005910: MfTransform3.
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfTransform3.h"
#include "MfXformMatrix3.h"

// 0x100052c0 ??0MfTransform3@@QAE@ABVMfXformMatrix3@@@Z
MfTransform3::MfTransform3(const MfXformMatrix3 &m)
{
    _rotation = MfRotation3(m);
    _translation.setValue(m.tx(), m.ty(), m.tz());
}

// 0x10005350 ??SMfTransform3@@QBE?AV0@XZ
MfTransform3 MfTransform3::operator~() const
{
    MfRotation3 inv = ~_rotation;
    return MfTransform3(inv, inv * -_translation);
}

// 0x100053f0 ??DMfTransform3@@QBE?AV0@ABV0@@Z
MfTransform3 MfTransform3::operator*(const MfTransform3 &t) const
{
    return MfTransform3(_rotation * t._rotation, _translation + _rotation * t._translation);
}

// 0x100054a0 ?Multiply@MfTransform3@@QAEXAAVMfVector3@@ABV2@@Z
// r = this transformation applied to the point p.
void MfTransform3::Multiply(MfVector3 &r, const MfVector3 &p)
{
    float a = _rotation._w * _rotation._w
            - (_rotation._x * _rotation._x + _rotation._y * _rotation._y + _rotation._z * _rotation._z);
    float d = 2.0f * (_rotation._x * p.x() + _rotation._y * p.y() + _rotation._z * p.z());
    float w2 = 2.0f * _rotation._w;
    r.x() = w2 * (_rotation._y * p.z() - _rotation._z * p.y()) + d * _rotation._x + a * p.x() + _translation.x();
    r.y() = w2 * (_rotation._z * p.x() - _rotation._x * p.z()) + d * _rotation._y + a * p.y() + _translation.y();
    r.z() = w2 * (_rotation._x * p.y() - _rotation._y * p.x()) + d * _rotation._z + a * p.z() + _translation.z();
}

// 0x100055c0 ?Multiply@MfTransform3@@QAEXABV1@0@Z
// This = a * b (b first, then a).
void MfTransform3::Multiply(const MfTransform3 &a, const MfTransform3 &b)
{
    _rotation._w = a._rotation._w * b._rotation._w - a._rotation._x * b._rotation._x
                 - a._rotation._y * b._rotation._y - a._rotation._z * b._rotation._z;
    _rotation._x = a._rotation._w * b._rotation._x + a._rotation._x * b._rotation._w
                 + a._rotation._y * b._rotation._z - a._rotation._z * b._rotation._y;
    _rotation._y = a._rotation._w * b._rotation._y + a._rotation._y * b._rotation._w
                 + a._rotation._z * b._rotation._x - a._rotation._x * b._rotation._z;
    _rotation._z = a._rotation._w * b._rotation._z + a._rotation._z * b._rotation._w
                 + a._rotation._x * b._rotation._y - a._rotation._y * b._rotation._x;
    const MfVector3 &p = b._translation;
    float s = a._rotation._w * a._rotation._w
            - (a._rotation._x * a._rotation._x + a._rotation._y * a._rotation._y + a._rotation._z * a._rotation._z);
    float d = 2.0f * (a._rotation._x * p.x() + a._rotation._y * p.y() + a._rotation._z * p.z());
    float w2 = 2.0f * a._rotation._w;
    _translation.x() = w2 * (a._rotation._y * p.z() - a._rotation._z * p.y()) + d * a._rotation._x + s * p.x() + a._translation.x();
    _translation.y() = (w2 * (a._rotation._z * p.x() - a._rotation._x * p.z()) + d * a._rotation._y) + s * p.y() + a._translation.y();
    _translation.z() = (w2 * (a._rotation._x * p.y() - a._rotation._y * p.x()) + d * a._rotation._z) + s * p.z() + a._translation.z();
}

// 0x10005790 ??DMfTransform3@@QBE?AVMfVector3@@ABV1@@Z
MfVector3 MfTransform3::operator*(const MfVector3 &v) const
{
    return _translation + _rotation * v;
}

// 0x100057f0 ?interpolate@MfTransform3@@QAEXABV1@0M@Z
void MfTransform3::interpolate(const MfTransform3 &a, const MfTransform3 &b, float t)
{
    _rotation.interpolate(a._rotation, b._rotation, t, 0);
    _translation.interpolate(a._translation, b._translation, t);
}

// 0x10005830 ?interpolate@MfTransform3@@QAEXABV1@000M@Z
void MfTransform3::interpolate(const MfTransform3 &a, const MfTransform3 &b,
                               const MfTransform3 &c, const MfTransform3 &d, float t)
{
    _rotation.interpolate(a._rotation, b._rotation, c._rotation, d._rotation, t);
    _translation.interpolate(a._translation, b._translation, c._translation, d._translation, t);
}

// 0x10005880 ?PremultiplyInverse@MfTransform3@@QAEXABV1@@Z
void MfTransform3::PremultiplyInverse(const MfTransform3 &t)
{
    MfTransform3 inv = ~t;
    MfTransform3 self = *this;
    Multiply(inv, self);
}
