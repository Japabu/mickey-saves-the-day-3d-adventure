// mflinalg.dll 0x10004fb0-0x100052c0: MfTransform2.
#include "MfVector2.h"
#include "MfRotation2.h"
#include "MfTransform2.h"

// Inline members of MfTransform2.h:
// 0x10004fb0 ??0MfTransform2@@QAE@ABVMfRotation2@@ABVMfVector2@@@Z
// 0x10004fe0 ?y@MfTransform2@@QAEMXZ
// 0x10004ff0 ??0MfTransform2@@QAE@ABV0@@Z
// 0x10005020 ??4MfTransform2@@QAEAAV0@ABV0@@Z
// EQUIV 0x10005040 ?identity@MfTransform2@@SA?AV1@XZ: reviewed: stack temporaries laid out differently; the original stores the zero vector as constants, same call and same result fields

// 0x100050b0 ??SMfTransform2@@QAE?AV0@XZ
MfTransform2 MfTransform2::operator~()
{
    MfRotation2 inv = ~_rotation;
    return MfTransform2(inv, inv * -_translation);
}

// 0x10005140 ??DMfTransform2@@QAE?AV0@ABV0@@Z
MfTransform2 MfTransform2::operator*(const MfTransform2 &t)
{
    return MfTransform2(_rotation * t._rotation, _translation + _rotation * t._translation);
}

// 0x100051d0 ??DMfTransform2@@QAE?AVMfVector2@@ABV1@@Z
MfVector2 MfTransform2::operator*(const MfVector2 &v)
{
    return _translation + _rotation * v;
}

// 0x10005230 ?interpolate@MfTransform2@@QAEXABV1@0M@Z
void MfTransform2::interpolate(const MfTransform2 &a, const MfTransform2 &b, float t)
{
    _rotation.interpolate(a._rotation, b._rotation, t);
    _translation = a._translation + (b._translation - a._translation) * t;
}
