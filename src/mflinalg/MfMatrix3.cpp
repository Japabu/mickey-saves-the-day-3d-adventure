// mflinalg.dll 0x100016f0-0x100038c0: MfMatrix3. The object includes every
// 3D class header, so the inline members of MfVector3, MfRotation3,
// MfTransform3, MfXformMatrix3 and MfMatrix3 come with it (0x100016f0-0x10001ec0).
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <assert.h>
#include <limits.h>
#include "MfMath.h"
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfTransform3.h"
#include "MfXformMatrix3.h"
#include "MfMatrix3.h"

#define EPS 1e-5f

// Inline members of the class headers:
// 0x100016f0 ??0MfVector3@@QAE@XZ
// 0x10001710 ?x@MfVector3@@QAEAAMXZ
// 0x10001720 ?y@MfVector3@@QAEAAMXZ
// 0x10001730 ?z@MfVector3@@QAEAAMXZ
// 0x10001740 ??0MfVector3@@QAE@MMM@Z
// 0x10001760 ??0MfVector3@@QAE@ABV0@@Z
// 0x10001780 ??AMfVector3@@QAEAAMH@Z
// 0x10001790 ??AMfVector3@@QBEMH@Z
// 0x100017a0 ?setValue@MfVector3@@QAEXMMM@Z
// 0x100017c0 ?lengthSquared@MfVector3@@QBEMXZ
// 0x10001800 ?normalize@MfVector3@@QAEXXZ
// 0x10001870 ?LengthAndNormalize@MfVector3@@QAEMXZ
// 0x100018e0 ?square@MfVector3@@QAE?AV1@XZ
// 0x10001940 ??AMfMatrix3@@QAEPAMH@Z
// 0x10001950 ?transpose@MfMatrix3@@QBEXAAV1@@Z
// 0x10001990 ??0MfRotation3@@QAE@XZ
// 0x100019b0 ??0MfRotation3@@QAE@MMMM@Z
// 0x100019d0 ??0MfRotation3@@QAE@MABVMfVector3@@@Z
// 0x100019f0 ??0MfRotation3@@QAE@ABVMfVector3@@0@Z
// 0x10001a10 ??0MfRotation3@@QAE@ABV0@@Z
// 0x10001a30 ?getValue@MfRotation3@@QBEXAAM000@Z
// 0x10001a60 ?setValue@MfRotation3@@QAEXMMMM@Z
// 0x10001b00 ?fixSign@MfRotation3@@QAEXXZ
// 0x10001bc0 ?identity@MfRotation3@@SA?AV1@XZ
// 0x10001be0 ??0MfTransform3@@QAE@XZ
// 0x10001c00 ??0MfTransform3@@QAE@ABVMfRotation3@@ABVMfVector3@@@Z
// 0x10001c40 ?rotation@MfTransform3@@QAEAAVMfRotation3@@XZ
// 0x10001c50 ??0MfTransform3@@QAE@ABV0@@Z
// 0x10001c90 ??4MfTransform3@@QAEAAV0@ABV0@@Z
// 0x10001cd0 ?setValue@MfTransform3@@QAEXABVMfRotation3@@ABVMfVector3@@@Z
// 0x10001d10 ?identity@MfTransform3@@SA?AV1@XZ
// 0x10001d90 ?ty@MfXformMatrix3@@QAEAAMXZ
// 0x10001da0 ?tz@MfXformMatrix3@@QAEAAMXZ
// 0x10001db0 ?tx@MfXformMatrix3@@QBEMXZ
// 0x10001dc0 ?ty@MfXformMatrix3@@QBEMXZ
// 0x10001dd0 ?tz@MfXformMatrix3@@QBEMXZ
// 0x10001de0 ??0MfXformMatrix3@@QAE@ABVMfTransform3@@@Z
// 0x10001e00 ??0MfXformMatrix3@@QAE@ABVMfRotation3@@ABVMfVector3@@@Z
// 0x10001e20 ?setValue@MfXformMatrix3@@QAEXABVMfTransform3@@@Z
// 0x10001e40 ??AMfXformMatrix3@@QAEPAMH@Z
// 0x10001e50 ?translation@MfXformMatrix3@@QBE?AVMfVector3@@XZ
// 0x10001e80 ?setTranslation@MfXformMatrix3@@QAEXABVMfVector3@@@Z
// 0x10001ea0 ?identity@MfXformMatrix3@@SA?AV1@XZ

// 0x10001ec0 ??0MfMatrix3@@QAE@XZ
MfMatrix3::MfMatrix3()
{
    memset(_m, 0, sizeof(_m));
}

// 0x10001ee0 ??0MfMatrix3@@QAE@ABVMfRotation3@@@Z
MfMatrix3::MfMatrix3(const MfRotation3 &r)
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
}

// 0x10002030 ??0MfMatrix3@@QAE@ABV0@@Z
MfMatrix3::MfMatrix3(const MfMatrix3 &m)
{
    memcpy(_m, m._m, sizeof(_m));
}

// (identical to the copy constructor; the linker folded them)
MfMatrix3 &MfMatrix3::operator=(const MfMatrix3 &m)
{
    memcpy(_m, m._m, sizeof(_m));
    return *this;
}

// 0x10002050 ??1MfMatrix3@@QAE@XZ
MfMatrix3::~MfMatrix3()
{
}

// 0x10002060 ?invert@MfMatrix3@@QBEHAAV1@@Z
int MfMatrix3::invert(MfMatrix3 &inv) const
{
    float d = det();
    if (fabs(d) < EPS)
        return 0;
    inv._m[0][0] = (_m[2][2] * _m[1][1] - _m[1][2] * _m[2][1]) / d;
    inv._m[0][1] = -((_m[0][1] * _m[2][2] - _m[0][2] * _m[2][1]) / d);
    inv._m[0][2] = (_m[0][1] * _m[1][2] - _m[0][2] * _m[1][1]) / d;
    inv._m[1][0] = -((_m[1][0] * _m[2][2] - _m[1][2] * _m[2][0]) / d);
    inv._m[1][1] = (_m[0][0] * _m[2][2] - _m[0][2] * _m[2][0]) / d;
    inv._m[1][2] = -((_m[0][0] * _m[1][2] - _m[0][2] * _m[1][0]) / d);
    inv._m[2][0] = (_m[1][0] * _m[2][1] - _m[1][1] * _m[2][0]) / d;
    inv._m[2][1] = -((_m[0][0] * _m[2][1] - _m[0][1] * _m[2][0]) / d);
    inv._m[2][2] = (_m[0][0] * _m[1][1] - _m[0][1] * _m[1][0]) / d;
    return 1;
}

// 0x10002150 ??HMfMatrix3@@QBE?AV0@ABV0@@Z
MfMatrix3 MfMatrix3::operator+(const MfMatrix3 &m) const
{
    MfMatrix3 r;
    const float *a = &_m[0][0];
    const float *b = &m._m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] + b[i];
    return r;
}

// 0x100021e0 ?add@MfMatrix3@@QBEXABV1@AAV1@@Z
void MfMatrix3::add(const MfMatrix3 &m, MfMatrix3 &r) const
{
    const float *b = &m._m[0][0];
    const float *a = &_m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] + b[i];
}

// 0x10002210 ??GMfMatrix3@@QBE?AV0@ABV0@@Z
MfMatrix3 MfMatrix3::operator-(const MfMatrix3 &m) const
{
    MfMatrix3 r;
    const float *a = &_m[0][0];
    const float *b = &m._m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] - b[i];
    return r;
}

// 0x100022a0 ?subtract@MfMatrix3@@QBEXABV1@AAV1@@Z
void MfMatrix3::subtract(const MfMatrix3 &m, MfMatrix3 &r) const
{
    const float *a = &_m[0][0];
    const float *b = &m._m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] - b[i];
}

// 0x100022d0 ??GMfMatrix3@@QBE?AV0@XZ
MfMatrix3 MfMatrix3::operator-() const
{
    MfMatrix3 r;
    const float *a = &_m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = -a[i];
    return r;
}

// 0x10002360 ?negate@MfMatrix3@@QBEXAAV1@@Z
void MfMatrix3::negate(MfMatrix3 &r) const
{
    const float *a = &_m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = -a[i];
}

// 0x10002380 ??DMfMatrix3@@QBE?AV0@M@Z
MfMatrix3 MfMatrix3::operator*(float s) const
{
    MfMatrix3 r;
    const float *a = &_m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] * s;
    return r;
}

// 0x10002410 ?multiply@MfMatrix3@@QBEXMAAV1@@Z
void MfMatrix3::multiply(float s, MfMatrix3 &r) const
{
    const float *a = &_m[0][0];
    float *c = &r._m[0][0];
    for (int i = 0; i < 9; i++)
        c[i] = a[i] * s;
}

// EQUIV 0x10002430 ??DMfMatrix3@@QBE?AV0@ABV0@@Z: reviewed: each sum written in the grouping the original evaluates (the default canonicalization regrouped four of the nine elements); symbolic x87 evaluation of all nine elements identical; only the x87 schedule, the extra edi save and the frame layout differ
MfMatrix3 MfMatrix3::operator*(const MfMatrix3 &m) const
{
    MfMatrix3 r;
    r._m[0][0] = (_m[0][0] * m._m[0][0] + _m[0][2] * m._m[2][0]) + _m[0][1] * m._m[1][0];
    r._m[0][1] = (_m[0][0] * m._m[0][1] + _m[0][2] * m._m[2][1]) + _m[0][1] * m._m[1][1];
    r._m[0][2] = (_m[0][0] * m._m[0][2] + _m[0][1] * m._m[1][2]) + _m[0][2] * m._m[2][2];
    r._m[1][0] = (_m[1][0] * m._m[0][0] + _m[1][2] * m._m[2][0]) + _m[1][1] * m._m[1][0];
    r._m[1][1] = (_m[1][0] * m._m[0][1] + _m[1][2] * m._m[2][1]) + _m[1][1] * m._m[1][1];
    r._m[1][2] = (_m[1][0] * m._m[0][2] + _m[1][2] * m._m[2][2]) + _m[1][1] * m._m[1][2];
    r._m[2][0] = (_m[2][0] * m._m[0][0] + _m[2][1] * m._m[1][0]) + _m[2][2] * m._m[2][0];
    r._m[2][1] = (_m[2][0] * m._m[0][1] + _m[2][1] * m._m[1][1]) + _m[2][2] * m._m[2][1];
    r._m[2][2] = (_m[2][0] * m._m[0][2] + _m[2][1] * m._m[1][2]) + _m[2][2] * m._m[2][2];
    return r;
}

// EQUIV 0x100025e0 ?multiply@MfMatrix3@@QBEXABV1@AAV1@@Z: reviewed: each sum written in the grouping the original evaluates; symbolic x87 evaluation of the nine stores identical (aliasing order included); only the schedule and the operand order of commutative products differ
void MfMatrix3::multiply(const MfMatrix3 &m, MfMatrix3 &r) const
{
    r._m[0][0] = (_m[0][0] * m._m[0][0] + _m[0][2] * m._m[2][0]) + _m[0][1] * m._m[1][0];
    r._m[0][1] = (_m[0][0] * m._m[0][1] + _m[0][2] * m._m[2][1]) + _m[0][1] * m._m[1][1];
    r._m[0][2] = (_m[0][0] * m._m[0][2] + _m[0][2] * m._m[2][2]) + _m[0][1] * m._m[1][2];
    r._m[1][0] = (_m[1][0] * m._m[0][0] + _m[1][2] * m._m[2][0]) + _m[1][1] * m._m[1][0];
    r._m[1][1] = (_m[1][0] * m._m[0][1] + _m[1][1] * m._m[1][1]) + _m[1][2] * m._m[2][1];
    r._m[1][2] = (_m[1][0] * m._m[0][2] + _m[1][2] * m._m[2][2]) + _m[1][1] * m._m[1][2];
    r._m[2][0] = (_m[2][0] * m._m[0][0] + _m[2][1] * m._m[1][0]) + _m[2][2] * m._m[2][0];
    r._m[2][1] = (_m[2][1] * m._m[1][1] + _m[2][2] * m._m[2][1]) + _m[2][0] * m._m[0][1];
    r._m[2][2] = (_m[2][0] * m._m[0][2] + _m[2][1] * m._m[1][2]) + _m[2][2] * m._m[2][2];
}

// 0x10002710 ??DMfMatrix3@@QBE?AVMfVector3@@ABV1@@Z
MfVector3 MfMatrix3::operator*(const MfVector3 &v) const
{
    MfVector3 r;
    float x = v.x(), y = v.y(), z = v.z();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z;
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z;
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z;
    return r;
}

// 0x100027b0 ?multiply@MfMatrix3@@QBEXABVMfVector3@@AAV2@@Z
void MfMatrix3::multiply(const MfVector3 &v, MfVector3 &r) const
{
    float x = v.x(), y = v.y(), z = v.z();
    r.x() = _m[0][0] * x + _m[0][1] * y + _m[0][2] * z;
    r.y() = _m[1][0] * x + _m[1][1] * y + _m[1][2] * z;
    r.z() = _m[2][0] * x + _m[2][1] * y + _m[2][2] * z;
}

// EQUIV 0x10002820 ?det@MfMatrix3@@QBEMXZ: reviewed: the first product is parenthesized as the original evaluates it ((a00 a11) a22; unparenthesized, VC5 regrouped it); symbolic x87 evaluation identical; only the schedule differs
float MfMatrix3::det() const
{
    return (_m[0][0] * _m[1][1]) * _m[2][2] + _m[0][1] * _m[1][2] * _m[2][0]
         + _m[0][2] * _m[1][0] * _m[2][1] - _m[0][0] * _m[1][2] * _m[2][1]
         - _m[0][1] * _m[1][0] * _m[2][2] - _m[0][2] * _m[1][1] * _m[2][0];
}

// 0x10002880 ?solve@MfMatrix3@@QBEHABVMfVector3@@AAV2@@Z
// Cramer's rule.
int MfMatrix3::solve(const MfVector3 &b, MfVector3 &x) const
{
    float d = det();
    MfMatrix3 m;
    if (fabs(d) < EPS)
        return 0;
    m = *this;
    m._m[0][0] = b.x(); m._m[1][0] = b.y(); m._m[2][0] = b.z();
    x.x() = m.det() / d;
    m = *this;
    m._m[0][1] = b.x(); m._m[1][1] = b.y(); m._m[2][1] = b.z();
    x.y() = m.det() / d;
    m = *this;
    m._m[0][2] = b.x(); m._m[1][2] = b.y(); m._m[2][2] = b.z();
    x.z() = m.det() / d;
    return 1;
}

