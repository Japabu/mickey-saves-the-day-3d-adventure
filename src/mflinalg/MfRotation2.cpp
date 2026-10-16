// mflinalg.dll 0x100038c0-0x10003a70: MfRotation2. The object is the first to
// include the 2D class headers, so their inline members come with it.
#include <math.h>
#include "MfVector2.h"
#include "MfRotation2.h"
#include "MfTransform2.h"

#define PI      3.1415927f
#define TWO_PI  6.2831855f

// Inline members of the class headers:
// 0x100038c0 ??0MfVector2@@QAE@XZ
// 0x100038d0 ??0MfVector2@@QAE@MM@Z
// 0x100038f0 ??0MfVector2@@QAE@ABV0@@Z (compiler-generated)
// 0x10003910 ?lengthSquared@MfVector2@@QBEMXZ
// 0x10003930 ??0MfRotation2@@QAE@XZ
// 0x10003940 ?setAngle@MfRotation2@@QAEXM@Z
// 0x10003950 ??0MfRotation2@@QAE@ABV0@@Z
// 0x10003960 ??0MfRotation2@@QAE@M@Z
// 0x10003970 ?identity@MfRotation2@@SA?AV1@XZ
// 0x10003990 ??SMfRotation2@@QBE?AV0@XZ
// EQUIV 0x100039b0 ??DMfRotation2@@QBE?AV0@ABV0@@Z: reviewed: fadd operands swapped (commutative), otherwise identical
// 0x10004fd0 ??BMfRotation2@@QBEMXZ

// 0x100039d0 ??DMfRotation2@@QBE?AVMfVector2@@ABV1@@Z
MfVector2 MfRotation2::operator*(const MfVector2 &v) const
{
    float c = (float)cos(_angle);
    float s = (float)sin(_angle);
    return MfVector2(c * v.x() - s * v.y(), s * v.x() + c * v.y());
}

// 0x10003a20 ?interpolate@MfRotation2@@QAEXABV1@0M@Z
// Interpolates along the shorter way round.
void MfRotation2::interpolate(const MfRotation2 &a, const MfRotation2 &b, float t)
{
    float d = b._angle - a._angle;
    if (d > PI)
        d -= TWO_PI;
    else if (d < -PI)
        d += TWO_PI;
    _angle = d * t + a._angle;
}
