// mflinalg.dll 0x10003ac0-0x10004fb0: MfRotation3 (unit quaternion s + (x, y, z)).
// Compiled with /O2 /GX (functions with MfRotation3/MfVector3 locals have EH frames).
//
// Note on matching: VC5 orders the operands of commutative x87 operations by an
// internal symbol counter, so which of two equivalent operand orders it emits depends
// on how many symbols (declarations, locals, casts) precede a function in the
// translation unit. The forward declarations in MfRotation3.h and a few named locals
// below (half, l2, dist, the (float) casts in random) are chosen so that every
// function reproduces the original order; they do not change the generated code
// otherwise.
#include "MfRotation3_unit.h"

#define EPSILON 1e-6f
#define ONE_EPS 0.999999f

// 0x10003ac0 ??0MfRotation3@@QAE@ABVMfXformMatrix3@@@Z
MfRotation3::MfRotation3(const MfXformMatrix3 &m)
{
    float t = m[0][0] + m[1][1] + m[2][2] + 1.0f;
    if (t > EPSILON) {
        s = (float)sqrt(t) * 0.5f;
        float d = s * 4.0f;
        x = (m[2][1] - m[1][2]) / d;
        y = (m[0][2] - m[2][0]) / d;
        z = (m[1][0] - m[0][1]) / d;
        return;
    }
    s = 0.0f;
    t = (m[1][1] + m[2][2]) * -0.5f;
    if (t > EPSILON) {
        x = (float)sqrt(t);
        float d = x + x;
        y = m[0][1] / d;
        z = m[0][2] / d;
        return;
    }
    x = 0.0f;
    t = (1.0f - m[2][2]) * 0.5f;
    if (t > EPSILON) {
        y = (float)sqrt(t);
        float d = y + y;
        z = m[1][2] / d;
        return;
    }
    y = 0.0f;
    z = 1.0f;
}

// 0x10003bb0 ??SMfRotation3@@QBE?AV0@XZ
MfRotation3 MfRotation3::operator~() const
{
    return MfRotation3(s, -x, -y, -z);
}

// 0x10003be0 ??GMfRotation3@@QBE?AV0@XZ
MfRotation3 MfRotation3::operator-() const
{
    return MfRotation3(-s, -x, -y, -z);
}

// 0x10003c10 ??DMfRotation3@@QBE?AV0@ABV0@@Z
MfRotation3 MfRotation3::operator*(const MfRotation3 &q) const
{
    return MfRotation3(s * q.s - x * q.x - y * q.y - z * q.z,
                       s * q.x + x * q.s + y * q.z - z * q.y,
                       s * q.y + y * q.s + z * q.x - x * q.z,
                       s * q.z + z * q.s + x * q.y - y * q.x);
}

// 0x10003cd0 ?Multiply@MfRotation3@@QAEXABV1@0@Z
void MfRotation3::Multiply(const MfRotation3 &a, const MfRotation3 &b)
{
    s = a.s * b.s - a.x * b.x - a.y * b.y - a.z * b.z;
    x = a.s * b.x + a.x * b.s + a.y * b.z - a.z * b.y;
    y = a.s * b.y + a.y * b.s + a.z * b.x - a.x * b.z;
    z = a.s * b.z + a.z * b.s + a.x * b.y - a.y * b.x;
}

// 0x10003d90 ??DMfRotation3@@QBE?AVMfVector3@@ABV1@@Z
MfVector3 MfRotation3::operator*(const MfVector3 &p) const
{
    float a = s * s - (x * x + y * y + z * z);
    float d = 2.0f * (x * p.x + y * p.y + z * p.z);
    float s2 = 2.0f * s;
    return MfVector3(s2 * (y * p.z - z * p.y) + d * x + a * p.x,
                     s2 * (z * p.x - x * p.z) + d * y + a * p.y,
                     s2 * (x * p.y - y * p.x) + d * z + a * p.z);
}

// 0x10003ec0 ??DMfRotation3@@QBE?AV0@M@Z
MfRotation3 MfRotation3::operator*(float f) const
{
    return MfRotation3(f * s, x * f, y * f, z * f);
}

// 0x10003f10 ??HMfRotation3@@QBE?AV0@ABV0@@Z
MfRotation3 MfRotation3::operator+(const MfRotation3 &q) const
{
    return MfRotation3(s + q.s, x + q.x, y + q.y, z + q.z);
}

// 0x10003f60 ??8MfRotation3@@QBEHABV0@@Z
int MfRotation3::operator==(const MfRotation3 &q) const
{
    float d = s * q.s + x * q.x + y * q.y + z * q.z;
    return d > ONE_EPS || d < -ONE_EPS;
}

// 0x10003fc0 ?getValue@MfRotation3@@QBEXAAMAAVMfVector3@@@Z
void MfRotation3::getValue(float &angle, MfVector3 &axis) const
{
    if (s < ONE_EPS && s > -ONE_EPS) {
        float half = (float)acos(s);
        angle = half * 2.0f;
        float l = (float)sqrt(x * x + y * y + z * z);
        if (l < EPSILON) {
            axis.setValue(1.0f, 0.0f, 0.0f);
            return;
        }
        float inv = 1.0f / l;
        axis.x = inv * x;
        axis.y = inv * y;
        axis.z = inv * z;
        return;
    }
    angle = 0.0f;
    axis.setValue(1.0f, 0.0f, 0.0f);
}

// 0x100040a0 ?Angle@MfRotation3@@QBEMXZ
float MfRotation3::Angle() const
{
    if (s < ONE_EPS && s > -ONE_EPS) {
        float half = (float)acos(s);
        return half * 2.0f;
    }
    return 0.0f;
}

// 0x100040d0 ?Axis@MfRotation3@@QBE?AVMfVector3@@XZ
MfVector3 MfRotation3::Axis() const
{
    if (s < ONE_EPS && s > -ONE_EPS) {
        float l2 = x * x + y * y + z * z;
        float l = (float)sqrt(l2);
        if (l < EPSILON)
            return MfVector3(1.0f, 0.0f, 0.0f);
        float inv = 1.0f / l;
        return MfVector3(inv * x, inv * y, inv * z);
    }
    return MfVector3(1.0f, 0.0f, 0.0f);
}

// 0x10004190 ?setValue@MfRotation3@@QAEXMABVMfVector3@@@Z
void MfRotation3::setValue(float angle, const MfVector3 &axis)
{
    float h = angle * 0.5;
    float sn = (float)sin(h);
    s = (float)cos(h);
    x = sn * axis.x;
    y = sn * axis.y;
    z = sn * axis.z;
}

// 0x100041d0 ?setValue@MfRotation3@@QAEXABVMfVector3@@0@Z
void MfRotation3::setValue(const MfVector3 &from, const MfVector3 &to)
{
    float d = from.dot(to);
    if (d > ONE_EPS) {
        s = 1.0f;
        x = y = z = 0.0f;
        return;
    }
    if (d < -ONE_EPS) {
        y = 1.0f;
        s = x = z = 0.0f;
        return;
    }
    MfVector3 c = from.cross(to);
    if (c.lengthSquared() < EPSILON) {
        s = 1.0f;
        x = y = z = 0.0f;
        return;
    }
    float angle = (float)acos(d);
    setValue(angle, c.unit());
}

// 0x10004310 ?interpolate@MfRotation3@@QAEXABV1@0MH@Z
void MfRotation3::interpolate(const MfRotation3 &q1, const MfRotation3 &q2, float t, int spin)
{
    double omega, phi, cosom, isinom, scale1, scale2;
    MfRotation3 q3 = q2;
    cosom = q3.s * q1.s + q3.x * q1.x + q3.y * q1.y + q3.z * q1.z;
    if (cosom < 0.0) {
        q3 = -q3;
        cosom = -cosom;
    }
    if (1.0 - cosom > 1e-11f) {
        omega = acos(cosom);
        isinom = 1.0 / sin(omega);
        phi = omega + spin * 3.1415927f;
        scale1 = sin(omega - t * phi) * isinom;
        scale2 = sin(t * phi) * isinom;
    } else {
        scale1 = 1.0 - t;
        scale2 = t;
    }
    s = scale1 * q1.s + scale2 * q3.s;
    x = scale1 * q1.x + scale2 * q3.x;
    y = scale1 * q1.y + scale2 * q3.y;
    z = scale1 * q1.z + scale2 * q3.z;
    normalize();
}

// 0x10004540 ?interpolate@MfRotation3@@QAEXABV1@000M@Z
void MfRotation3::interpolate(const MfRotation3 &q0, const MfRotation3 &q1, const MfRotation3 &q2,
                              const MfRotation3 &q3, float t)
{
    MfRotation3 a, b, c, d, e;
    a.interpolate(q0, q1, t, 0);
    b.interpolate(q1, q2, t, 0);
    c.interpolate(q2, q3, t, 0);
    d.interpolate(a, b, t, 0);
    e.interpolate(b, c, t, 0);
    interpolate(d, e, t, 0);
}

// 0x100046b0 ?leap@MfRotation3@@QAEXABV1@0@Z
void MfRotation3::leap(const MfRotation3 &a, const MfRotation3 &b)
{
    float d = 2.0f * (a.s * b.s + a.x * b.x + a.y * b.y + a.z * b.z);
    s = d * b.s - a.s;
    x = d * b.x - a.x;
    y = d * b.y - a.y;
    z = d * b.z - a.z;
    normalize();
}

// 0x10004790 ?bisect@MfRotation3@@QAEXABV1@0@Z
void MfRotation3::bisect(const MfRotation3 &a, const MfRotation3 &b)
{
    MfRotation3 q = b;
    float d = q.s * a.s + q.x * a.x + q.y * a.y + q.z * a.z;
    if (d < 0.0f) {
        q = -q;
        d = -d;
    }
    if (d > ONE_EPS) {
        *this = a;
        return;
    }
    s = q.s + a.s;
    x = q.x + a.x;
    y = q.y + a.y;
    z = q.z + a.z;
    normalize();
}

// 0x10004930 ?dot@MfRotation3@@QBEMABV1@@Z
float MfRotation3::dot(const MfRotation3 &q) const
{
    return s * q.s + x * q.x + y * q.y + z * q.z;
}

// 0x10004960 ?random@MfRotation3@@SA?AV1@M@Z
MfRotation3 MfRotation3::random(float range)
{
    MfRotation3 r;
    r.s = 1.0f;
    r.x = (float)(rand() % 1001 - 500) * range * 0.002f;
    r.y = (float)(rand() % 1001 - 500) * range * 0.002f;
    r.z = (float)(rand() % 1001 - 500) * range * 0.002f;
    r.normalize();
    return r;
}

// 0x10004aa0 ?rotationXform@MfRotation3@@QAEXAAVMfVector3@@0@Z
void MfRotation3::rotationXform(MfVector3 &from, MfVector3 &to)
{
    MfVector3 axis(0.0f, 0.0f, 0.0f);
    from.normalize();
    to.normalize();
    float d = from.dot(to);
    if (fabs(d - 1.0) < 1e-6) {
        if (fabs(from.x) < 1e-6)
            axis.setValue(1.0f, 0.0f, 0.0f);
        else if (fabs(from.y) < 1e-6)
            axis.setValue(0.0f, 1.0f, 0.0f);
        else if (fabs(from.z) < 1e-6)
            axis.setValue(0.0f, 0.0f, 1.0f);
        else {
            float r = -from.y / from.z;
            axis.setValue(0.0f, 1.0f, r);
            axis.normalize();
        }
        setValue(0.0f, axis);
        return;
    }
    if (fabs(d - -1.0) < 1e-5) {
        if (fabs(from.x) < 1e-6)
            axis.setValue(1.0f, 0.0f, 0.0f);
        else if (fabs(from.y) < 1e-6)
            axis.setValue(0.0f, 1.0f, 0.0f);
        else if (fabs(from.z) < 1e-6)
            axis.setValue(0.0f, 0.0f, 1.0f);
        else {
            float r = -from.y / from.z;
            float l = (float)sqrt(r * r + 1.0f);
            axis.setValue(0.0f / l, 1.0f / l, r / l);
        }
        setValue(3.1415927f, axis);
        return;
    }
    axis = from.cross(to);
    axis.normalize();
    float angle = (float)acos(d);
    MfRotation3 q(angle, axis);
    MfVector3 r(0.0f, 0.0f, 0.0f), diff(0.0f, 0.0f, 0.0f);
    r = q * from;
    diff = to - r;
    float dist = diff.length();
    if (dist > 0.0001)
        axis = -axis;
    setValue(angle, axis);
}

// 0x10004ef0 ?Postmultiply@MfRotation3@@QAEXABV1@@Z
void MfRotation3::Postmultiply(const MfRotation3 &q)
{
    float ns = s * q.s - x * q.x - y * q.y - z * q.z;
    float nx = s * q.x + x * q.s + y * q.z - z * q.y;
    float ny = s * q.y + y * q.s + z * q.x - x * q.z;
    float nz = s * q.z + z * q.s + x * q.y - y * q.x;
    s = ns;
    x = nx;
    y = ny;
    z = nz;
}


// Inline members of MfRotation3_unit.h that this object emits (dllexport) and
// that match here (the MfMatrix3 object, which emits them first in the
// original, orders their operands differently):
// 0x10001a80 ?normalize@MfRotation3@@QAEXXZ
// 0x10001b80 ?scalarMult@MfRotation3@@QAEXM@Z

// The table object of the class (its constructor runs at load time). Defined
// last: VC5 orders commutative operands by a symbol counter, and defining it
// before the functions changes the operand order of several of them.
// 0x10003a70 _$E12 (compiler-generated: calls the constructor and registers the destructor)
// 0x10003a80 _$E8 (compiler-generated: constructs _math)
// 0x10003a90 _$E11 (compiler-generated: atexit registration)
// 0x10003aa0 _$E9 (compiler-generated: destructor wrapper; tests the guard bit)
inline MfMath::~MfMath() {}
MfMath MfRotation3::_math;
