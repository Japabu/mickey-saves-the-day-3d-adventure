// mflinalg.dll 0x10006660-0x100072e0: MfXformMatrix3.
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfTransform3.h"
#include "MfXformMatrix3.h"

// 0x10006660 ??0MfXformMatrix3@@QAE@XZ
MfXformMatrix3::MfXformMatrix3()
{
    memset(_m, 0, sizeof(_m));
    _m[2][2] = 1.0f;
    _m[1][1] = 1.0f;
    _m[0][0] = 1.0f;
}

// 0x10006680 ??0MfXformMatrix3@@QAE@ABV0@@Z
// (the linker folded the float array constructor and operator= into this one)
MfXformMatrix3::MfXformMatrix3(const MfXformMatrix3 &m)
{
    memcpy(_m, m._m, sizeof(_m));
}

MfXformMatrix3::MfXformMatrix3(const float *m)
{
    memcpy(_m, m, sizeof(_m));
}

MfXformMatrix3 &MfXformMatrix3::operator=(const MfXformMatrix3 &m)
{
    memcpy(_m, m._m, sizeof(_m));
    return *this;
}

// EQUIV 0x100066a0 ??SMfXformMatrix3@@QBE?AV0@XZ: reviewed: the translation sums parenthesized in the original order (VC5 regrouped them otherwise); the first 0x94 bytes match, the rest is the schedule of the last two rows with identical symbolic values
// Inverse of a rigid transformation (see invert()).
MfXformMatrix3 MfXformMatrix3::operator~() const
{
    MfXformMatrix3 inv;
    inv._m[0][0] = _m[0][0];
    inv._m[0][1] = _m[1][0];
    inv._m[0][2] = _m[2][0];
    inv._m[1][0] = _m[0][1];
    inv._m[1][1] = _m[1][1];
    inv._m[1][2] = _m[2][1];
    inv._m[2][0] = _m[0][2];
    inv._m[2][1] = _m[1][2];
    inv._m[2][2] = _m[2][2];
    inv._m[0][3] = -((inv._m[0][0] * _m[0][3] + inv._m[0][1] * _m[1][3]) + inv._m[0][2] * _m[2][3]);
    inv._m[1][3] = -((inv._m[1][0] * _m[0][3] + inv._m[1][1] * _m[1][3]) + inv._m[1][2] * _m[2][3]);
    inv._m[2][3] = -((inv._m[2][0] * _m[0][3] + inv._m[2][1] * _m[1][3]) + inv._m[2][2] * _m[2][3]);
    return inv;
}

// EQUIV 0x100067b0 ?invert@MfXformMatrix3@@QBEXAAV1@@Z: reviewed: the inv[2][3] sum written in the original grouping (a + c) + b; symbolic x87 evaluation of all twelve stores identical (aliasing order included); integer copies and schedule order differ
// Inverse of a rigid transformation: transposed rotation, rotated negative
// translation.
void MfXformMatrix3::invert(MfXformMatrix3 &inv) const
{
    inv._m[0][0] = _m[0][0];
    inv._m[0][1] = _m[1][0];
    inv._m[0][2] = _m[2][0];
    inv._m[1][0] = _m[0][1];
    inv._m[1][1] = _m[1][1];
    inv._m[1][2] = _m[2][1];
    inv._m[2][0] = _m[0][2];
    inv._m[2][1] = _m[1][2];
    inv._m[2][2] = _m[2][2];
    inv._m[0][3] = -(inv._m[0][0] * _m[0][3] + inv._m[0][1] * _m[1][3] + inv._m[0][2] * _m[2][3]);
    inv._m[1][3] = -(inv._m[1][0] * _m[0][3] + inv._m[1][1] * _m[1][3] + inv._m[1][2] * _m[2][3]);
    inv._m[2][3] = -((inv._m[2][0] * _m[0][3] + inv._m[2][2] * _m[2][3]) + inv._m[2][1] * _m[1][3]);
}

// EQUIV 0x10006860 ??DMfXformMatrix3@@QBE?AV0@ABV0@@Z: reviewed: each sum written in the grouping the original evaluates; symbolic x87 evaluation of all twelve elements identical; only the schedule differs
// Product of two affine matrices: m is applied first.
MfXformMatrix3 MfXformMatrix3::operator*(const MfXformMatrix3 &m) const
{
    MfXformMatrix3 r;
    r._m[0][0] = (_m[0][0] * m._m[0][0] + _m[0][2] * m._m[2][0]) + _m[0][1] * m._m[1][0];
    r._m[0][1] = (_m[0][0] * m._m[0][1] + _m[0][2] * m._m[2][1]) + _m[0][1] * m._m[1][1];
    r._m[0][2] = (_m[0][0] * m._m[0][2] + _m[0][1] * m._m[1][2]) + _m[0][2] * m._m[2][2];
    r._m[0][3] = ((_m[0][0] * m._m[0][3] + _m[0][1] * m._m[1][3]) + _m[0][2] * m._m[2][3]) + _m[0][3];
    r._m[1][0] = (_m[1][0] * m._m[0][0] + _m[1][1] * m._m[1][0]) + _m[1][2] * m._m[2][0];
    r._m[1][1] = (_m[1][1] * m._m[1][1] + _m[1][2] * m._m[2][1]) + _m[1][0] * m._m[0][1];
    r._m[1][2] = (_m[1][0] * m._m[0][2] + _m[1][2] * m._m[2][2]) + _m[1][1] * m._m[1][2];
    r._m[1][3] = ((_m[1][1] * m._m[1][3] + _m[1][2] * m._m[2][3]) + _m[1][0] * m._m[0][3]) + _m[1][3];
    r._m[2][0] = (_m[2][0] * m._m[0][0] + _m[2][2] * m._m[2][0]) + _m[2][1] * m._m[1][0];
    r._m[2][1] = (_m[2][1] * m._m[1][1] + _m[2][2] * m._m[2][1]) + _m[2][0] * m._m[0][1];
    r._m[2][2] = (_m[2][1] * m._m[1][2] + _m[2][2] * m._m[2][2]) + _m[2][0] * m._m[0][2];
    r._m[2][3] = ((_m[2][0] * m._m[0][3] + _m[2][2] * m._m[2][3]) + _m[2][1] * m._m[1][3]) + _m[2][3];
    return r;
}

// EQUIV 0x10006a50 ?multiply@MfXformMatrix3@@QBEXABV1@AAV1@@Z: reviewed: each sum written in the grouping the original evaluates; symbolic x87 evaluation identical; remaining differences are the operand order of commutative products and the load order of independent products
// r = this * m (r must not be one of the operands).
void MfXformMatrix3::multiply(const MfXformMatrix3 &m, MfXformMatrix3 &r) const
{
    r._m[0][0] = (_m[0][1] * m._m[1][0] + _m[0][2] * m._m[2][0]) + _m[0][0] * m._m[0][0];
    r._m[0][1] = (_m[0][0] * m._m[0][1] + _m[0][2] * m._m[2][1]) + _m[0][1] * m._m[1][1];
    r._m[0][2] = (_m[0][0] * m._m[0][2] + _m[0][2] * m._m[2][2]) + _m[0][1] * m._m[1][2];
    r._m[0][3] = ((_m[0][0] * m._m[0][3] + _m[0][1] * m._m[1][3]) + _m[0][2] * m._m[2][3]) + _m[0][3];
    r._m[1][0] = (_m[1][0] * m._m[0][0] + _m[1][1] * m._m[1][0]) + _m[1][2] * m._m[2][0];
    r._m[1][1] = (_m[1][1] * m._m[1][1] + _m[1][2] * m._m[2][1]) + _m[1][0] * m._m[0][1];
    r._m[1][2] = (_m[1][0] * m._m[0][2] + _m[1][2] * m._m[2][2]) + _m[1][1] * m._m[1][2];
    r._m[1][3] = ((_m[1][0] * m._m[0][3] + _m[1][2] * m._m[2][3]) + _m[1][1] * m._m[1][3]) + _m[1][3];
    r._m[2][0] = (_m[2][0] * m._m[0][0] + _m[2][2] * m._m[2][0]) + _m[2][1] * m._m[1][0];
    r._m[2][1] = (_m[2][0] * m._m[0][1] + _m[2][1] * m._m[1][1]) + _m[2][2] * m._m[2][1];
    r._m[2][2] = (_m[2][0] * m._m[0][2] + _m[2][1] * m._m[1][2]) + _m[2][2] * m._m[2][2];
    r._m[2][3] = ((_m[2][0] * m._m[0][3] + _m[2][2] * m._m[2][3]) + _m[2][1] * m._m[1][3]) + _m[2][3];
}

// 0x10006bf0 ??DMfXformMatrix3@@QBE?AVMfVector3@@ABV1@@Z
MfVector3 MfXformMatrix3::operator*(const MfVector3 &v) const
{
    MfVector3 r;
    float x = v.x(), y = v.y(), z = v.z();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z + _m[0][3];
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z + _m[1][3];
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z + _m[2][3];
    return r;
}

// 0x10006ca0 ?multiply@MfXformMatrix3@@QBEXABVMfVector3@@AAV2@@Z
void MfXformMatrix3::multiply(const MfVector3 &v, MfVector3 &r) const
{
    float x = v.x(), y = v.y(), z = v.z();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z + _m[0][3];
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z + _m[1][3];
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z + _m[2][3];
}

// EQUIV 0x10006d20 ?rotate@MfXformMatrix3@@QBE?AVMfVector3@@ABV2@@Z: reviewed: symbolic x87 evaluation identical (same grouping and roundings); y/z local slots and the schedule differ
MfVector3 MfXformMatrix3::rotate(const MfVector3 &v) const
{
    MfVector3 r;
    float x = v.x(), z = v.z(), y = v.y();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z;
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z;
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z;
    return r;
}

// 0x10006dc0 ?rotate@MfXformMatrix3@@QBEXABVMfVector3@@AAV2@@Z
void MfXformMatrix3::rotate(const MfVector3 &v, MfVector3 &r) const
{
    float x = v.x(), y = v.y(), z = v.z();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z;
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z;
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z;
}

// TODO 0x10006e30 ?setValue@MfXformMatrix3@@QAEXABVMfRotation3@@ABVMfVector3@@@Z: (76% similar) the original spills the doubled products 2wy/2wz and 2xz differently: m[0][2] and m[2][0] use the unrounded x*z and a float-rounded w*y in the original, the other way round here (CSE spill choice, a real rounding difference); every declaration order of w, x, y, z tried (best 69%)
// Rotation matrix of the unit quaternion r, then the translation t.
void MfXformMatrix3::setValue(const MfRotation3 &r, const MfVector3 &t)
{
    float x, y, z, w;
    r.getValue(w, x, y, z);
    _m[0][0] = 1.0f - 2.0f * (y * y + z * z);
    _m[0][1] = 2.0f * (x * y) - 2.0f * (w * z);
    _m[0][2] = 2.0f * (x * z) + 2.0f * (w * y);
    _m[1][0] = 2.0f * (x * y) + 2.0f * (w * z);
    _m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    _m[1][2] = 2.0f * (y * z) - 2.0f * (w * x);
    _m[2][0] = 2.0f * (x * z) - 2.0f * (w * y);
    _m[2][1] = 2.0f * (y * z) + 2.0f * (w * x);
    _m[2][2] = 1.0f - 2.0f * (x * x + y * y);
    setTranslation(t);
}

// 0x10006f90 ?setValue@MfXformMatrix3@@QAEXABVMfRotation3@@@Z
// Pure rotation: the translation is cleared.
void MfXformMatrix3::setValue(const MfRotation3 &r)
{
    float w, x, y, z;
    r.getValue(w, x, y, z);
    _m[0][0] = 1.0f - 2.0f * (y * y + z * z);
    _m[0][1] = 2.0f * (x * y) - 2.0f * (w * z);
    _m[0][2] = 2.0f * (x * z) + 2.0f * (w * y);
    _m[1][0] = 2.0f * (x * y) + 2.0f * (w * z);
    _m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    _m[1][2] = 2.0f * (y * z) - 2.0f * (w * x);
    _m[2][0] = 2.0f * (x * z) - 2.0f * (w * y);
    _m[2][1] = 2.0f * (y * z) + 2.0f * (w * x);
    _m[2][2] = 1.0f - 2.0f * (x * x + y * y);
    _m[2][3] = 0.0f;
    _m[1][3] = 0.0f;
    _m[0][3] = 0.0f;
}

#define PI      3.1415927f
#define TWO_PI  6.2831855f

// 0x100070e0 ?getEuler@MfXformMatrix3@@QAEXAAM00@Z
// Euler angles (rotation about x, then y, then z) of the rotation part, in
// [0, 2 pi). At the gimbal-lock and the half-turn cases one angle is chosen 0.
void MfXformMatrix3::getEuler(float &rx, float &ry, float &rz)
{
    float c = (float)sqrt(_m[0][0] * _m[0][0] + _m[1][0] * _m[1][0]);
    float y = (float)atan2(-_m[2][0], c);
    c = (float)cos(y);
    ry = y;
    if (fabs(c) > 0.001) {
        rz = (float)atan2(_m[1][0] / c, _m[0][0] / c);
        rx = (float)atan2(_m[2][1] / c, _m[2][2] / c);
    } else {
        rz = 0.0f;
        rx = (float)atan2(_m[0][1], _m[1][1]);
    }
    if (rx < 0.0f)
        rx += TWO_PI;
    if (ry < 0.0f)
        ry += TWO_PI;
    if (rz < 0.0f)
        rz += TWO_PI;
    if (fabs(rx - PI) < 0.001 && fabs(ry - PI) < 0.001) {
        ry = 0.0f;
        rx = 0.0f;
        rz = (float)atan2(_m[1][0], _m[0][0]);
        if (rz < 0.0f)
            rz += TWO_PI;
    }
    if (fabs(rx - PI) < 0.001 && fabs(rz - PI) < 0.001) {
        rz = 0.0f;
        rx = 0.0f;
        ry = (float)atan2(_m[0][2], _m[0][0]);
        if (ry < 0.0f)
            ry += TWO_PI;
    }
    if (fabs(ry - PI) < 0.001 && fabs(rz - PI) < 0.001) {
        rz = 0.0f;
        ry = 0.0f;
        rx = (float)atan2(_m[2][1], _m[2][2]);
        if (rx < 0.0f)
            rx += TWO_PI;
    }
}
