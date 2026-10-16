// mflinalg.dll 0x10005910-0x10005a80: MfVector2.
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "MfMath.h"
#include "MfVector2.h"
#include "MfVector3.h"

// 0x10005910 ??GMfVector2@@QBE?AV0@XZ
MfVector2 MfVector2::operator-() const
{
    return MfVector2(-_x, -_y);
}

// 0x10005930 ??HMfVector2@@QBE?AV0@ABV0@@Z
MfVector2 MfVector2::operator+(const MfVector2 &v) const
{
    return MfVector2(_x + v._x, _y + v._y);
}

// 0x10005960 ??GMfVector2@@QBE?AV0@ABV0@@Z
MfVector2 MfVector2::operator-(const MfVector2 &v) const
{
    return MfVector2(_x - v._x, _y - v._y);
}

// 0x10005990 ??DMfVector2@@QBE?AV0@M@Z
MfVector2 MfVector2::operator*(float s) const
{
    return MfVector2(_x * s, _y * s);
}

// 0x100059c0 ??KMfVector2@@QBE?AV0@M@Z
MfVector2 MfVector2::operator/(float s) const
{
    return MfVector2(_x / s, _y / s);
}

// 0x100059f0 ?dot@MfVector2@@QBEMABV1@@Z
float MfVector2::dot(const MfVector2 &v) const
{
    return _x * v._x + _y * v._y;
}

// 0x10005a10 ?length@MfVector2@@QBEMXZ
float MfVector2::length() const
{
    return (float)sqrt(_x * _x + _y * _y);
}

// 0x10005a40 ?normalize@MfVector2@@QAEXXZ
void MfVector2::normalize()
{
    float inv = 1.0f / length();
    _x *= inv;
    _y *= inv;
}

// 0x10005a70 ?angle@MfVector2@@QBEMXZ
float MfVector2::angle() const
{
    return (float)atan2(_y, _x);
}
