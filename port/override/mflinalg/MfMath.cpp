// PORT: float bit patterns accessed as 32-bit words (unsigned long is 8 bytes on LP64)
// mflinalg.dll 0x10001000-0x100016f0: MfMath. The object includes only
// MfMath.h: the inline members of the other classes come with MfMatrix3.cpp.
#include <math.h>
#include "MfMath.h"

#define PI      3.1415927f
#define TWO_PI  6.2831855f
#define EPS     0.0001

// 0x10001000 ?sin@MfMath@@SAMM@Z (inline, MfMath.h)
// 0x10001020 ??4MfMath@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10001700 ??1MfMath@@QAE@XZ (inline, MfMath.h)

// 0x10001030 _$E11 (calls the constructor and registers the destructor)
// 0x10001040 _$E7
// 0x10001050 _$E10
// 0x10001060 _$E8 (destructor wrapper; tests the guard bit of the static member)
MfMath MfMath::g_math;

float *MfMath::_costable;
float *MfMath::_acostable;
short *MfMath::_sqrttab;
float MfMath::_cosFactor = 8191 / PI;
float MfMath::_acosFactor = 8191 / 2.0f;

// 0x10001080 ??0MfMath@@QAE@XZ
MfMath::MfMath()
{
    int i;
    if (_costable == 0) {
        _costable = new float[8192];
        for (i = 0; i < 8192; i++)
            _costable[i] = (float)::cos(i * (PI / 8191));
    }
    if (_acostable == 0) {
        _acostable = new float[8192];
        for (i = 0; i < 8192; i++)
            _acostable[i] = (float)::acos(-1.0 - i * (-2.0 / 8191));
    }
    if (_sqrttab == 0) {
        // Square roots of 1.0 .. 4.0 by the top mantissa bits (Graphics Gems).
        _sqrttab = new short[1024];
        float f;
        port_u32 *fi = (port_u32 *)&f;    // PORT: float bits are 32-bit (unsigned long is 8 bytes)
        for (unsigned short m = 0; m <= 0x1ff; m++) {
            *fi = (m << MANT_SHIFT) | (127 << 23);
            f = (float)::sqrt(f);
            _sqrttab[m] = (short)((*fi & 0x7fffff) >> MANT_SHIFT);
            *fi = (m << MANT_SHIFT) | (128 << 23);
            f = (float)::sqrt(f);
            _sqrttab[m + 512] = (short)((*fi & 0x7fffff) >> MANT_SHIFT);
        }
    }
}

// 0x100011b0 ?cos@MfMath@@SAMM@Z
float MfMath::cos(float a)
{
    while (a > PI)
        a -= TWO_PI;
    while (a < -PI)
        a += TWO_PI;
    if (a < 0.0f)
        a = -a;
    return _costable[(int)(_cosFactor * a)];
}

// 0x10001220 ?acos@MfMath@@SAMM@Z
float MfMath::acos(float c)
{
    if (c < -1.0f)
        c = -1.0f;
    if (c > 1.0f)
        c = 1.0f;
    return _acostable[(int)((c + 1.0f) * _acosFactor + 0.5)];
}

// 0x10001270 ?real3Root@MfMath@@CAMMM@Z
// Real cube root of the complex number a + bi.
float MfMath::real3Root(float a, float b)
{
    float r = (float)::sqrt(a * a + b * b);
    float theta = (float)asin(b / r);
    if (a < 0.0f && b > 0.0f)
        theta = PI - theta;
    if (a < 0.0f && b < 0.0f)
        theta = PI - theta;
    if (a > 0.0f && b < 0.0f)
        theta += TWO_PI;
    float c = (float)::cos(theta / 3.0f);
    return (float)pow(r, 1.0 / 3.0) * c;
}

// 0x10001360 ?quadratic@MfMath@@SAHMMMAAM0@Z
int MfMath::quadratic(float a, float b, float c, float &r1, float &r2)
{
    float d = b * b - a * c * 4.0f;
    if (fabs(a) >= EPS && d >= 0.0f) {
        d = (float)::sqrt(d);
        float a2 = 2.0f * a;
        r1 = (-b - d) / a2;
        r2 = (d - b) / a2;
        return 1;
    }
    return 0;
}

// TODO 0x100013f0 ?cubic@MfMath@@SAHMMMMAAM00@Z: (85% similar) the original keeps the discriminant q*q + p*p*p unrounded on the x87 stack across the branch (fcom, no store) and keeps the first pow() result on the stack across the second call; here both are spilled to float slots, so the rounding differs
// Roots of a x^3 + b x^2 + c x + d (Cardano). Returns 1 with three roots, 3 when
// only r1 is real, 4 when a is zero.
int MfMath::cubic(float a, float b, float c, float d, float &r1, float &r2, float &r3)
{
    if (fabs(a) < EPS)
        return 4;
    float B = b / a;
    float C = c / a;
    float D = d / a;
    float p = (C * 3.0f - B * B) / 9.0f;
    float q = (D * 27.0f - B * (B * B * -2.0f - C * -9.0f)) / 54.0f;
    float s;
    if (q * q + p * p * p >= 0.0f) {
        s = (float)::sqrt(q * q + p * p * p);
        r1 = (float)pow(s - q, 1.0 / 3.0) + (float)pow(-q - s, 1.0 / 3.0);
    } else {
        s = (float)::sqrt(-(q * q + p * p * p));
        r1 = real3Root(-q, s) + real3Root(-q, s);
    }
    if (quadratic(1.0f, r1, r1 * r1 + 3.0f * p, r2, r3) == 0)
        return 3;
    float shift = B / 3.0f;
    r1 -= shift;
    r2 -= shift;
    r3 -= shift;
    return 1;
}

// 0x10001580 ?det2@MfMath@@SAMMMMM@Z
float MfMath::det2(float a, float b, float c, float d)
{
    return a * d - b * c;
}

// 0x100015a0 ?solve2@MfMath@@SAHMMMMMMAAM0@Z
// Solves a x + b y = e, c x + d y = f.
int MfMath::solve2(float a, float b, float c, float d, float e, float f, float &x, float &y)
{
    float det = det2(a, b, c, d);
    if (det == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        return 0;
    }
    if (fabs(a) < EPS) {
        y = e / b;
        x = (f - d * y) / c;
        return 1;
    }
    x = det2(e, b, f, d) / det;
    y = det2(a, e, c, f) / det;
    return 1;
}

// 0x10001670 ?sqrt@MfMath@@SAMM@Z
// Square root from _sqrttab, indexed by the exponent's low bit and the top
// mantissa bits; the exponent is halved directly.
float MfMath::sqrt(float x)
{
    if (x == 0.0f)
        return 0.0f;
    if (x < 0.0f)
        return 0.0f;
    port_u32 bits = *(port_u32 *)&x;    // PORT: float bits are 32-bit (unsigned long is 8 bytes)
    short exp = (short)((bits >> 23) - 127);
    port_u32 mant = bits & 0x7fffff;    // PORT: 32-bit
    if (exp & 1)
        mant |= 0x800000;
    exp >>= 1;
    *(port_u32 *)&x = ((port_i32)_sqrttab[mant >> MANT_SHIFT] << MANT_SHIFT) | ((exp + 127) << 23);    // PORT: 32-bit store into the float
    return x;
}
