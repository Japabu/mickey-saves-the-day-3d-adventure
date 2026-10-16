// mflinalg.dll 0x100029b0-0x100038c0: the MfMatrix3 solvers (eigen
// decomposition, principal axes). Part of the MfMatrix3 object in the original;
// compiled on its own here because VC5's x87 operand order depends on the whole
// translation unit and these match in this one.
#include <math.h>
#include <string.h>
#include <float.h>
#include "MfMath.h"
#include "MfVector3.h"
#include "MfRotation3.h"
#include "MfTransform3.h"
#include "MfXformMatrix3.h"
#include "MfMatrix3.h"

#define EPS 0.001

// EQUIV 0x100029b0 ?residualSquared@@YAMABVMfVector3@@MMMMMMMMM@Z: reviewed: same schedule; only the operands of commutative products are loaded in swapped order (symbolic x87 evaluation identical)
// |M v|^2 for the matrix given element by element.
static float residualSquared(const MfVector3 &v, float a, float b, float c, float d,
                             float e, float f, float g, float h, float i)
{
    float r0 = a * v.x() + b * v.y() + c * v.z();
    float r1 = d * v.x() + e * v.y() + f * v.z();
    float r2 = g * v.x() + h * v.y() + i * v.z();
    return r0 * r0 + r1 * r1 + r2 * r2;
}

// 0x10002a40 ?solve3SiOne@MfMatrix3@@QAEHAAVMfVector3@@@Z
int MfMatrix3::solve3SiOne(MfVector3 &v)
{
    float a = _m[0][0], b = _m[0][1], c = _m[0][2];
    float d = _m[1][0], e = _m[1][1], f = _m[1][2];
    float g = _m[2][0], h = _m[2][1], i = _m[2][2];
    MfVector3 r(0.0f, 0.0f, 0.0f);
    float minErr = FLT_MAX;
    MfVector3 t(0.0f, 0.0f, 0.0f);
    float y, z, err;

    if (MfMath::solve2(b, c, e, f, -a, -d, y, z) == 1) {
        t.setValue(1.0f, y, z);
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    if (MfMath::solve2(b, c, h, i, -a, -g, y, z) == 1) {
        t.setValue(1.0f, y, z);
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    if (MfMath::solve2(e, f, h, i, -d, -g, y, z) == 1) {
        t.setValue(1.0f, y, z);
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    if (c != 0.0f) {
        t.setValue(0.0f, 1.0f, -(b / c));
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    if (f != 0.0f) {
        t.setValue(0.0f, 1.0f, -(e / f));
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    if (i != 0.0f) {
        t.setValue(0.0f, 1.0f, -(h / i));
        err = residualSquared(t, a, b, c, d, e, f, g, h, i);
        if (err < minErr) {
            minErr = err;
            v = t;
        }
    }
    t.setValue(0.0f, 0.0f, 1.0f);
    err = residualSquared(t, a, b, c, d, e, f, g, h, i);
    if (err < minErr) {
        minErr = err;
        v = t;
    }
    return 1;
}

// 0x10002ef0 ?solve3SiTwo@MfMatrix3@@QAEHAAVMfVector3@@0@Z
int MfMatrix3::solve3SiTwo(MfVector3 &v1, MfVector3 &v2)
{
    float a = _m[0][0];
    float b = _m[0][1];
    float c = _m[0][2];

    if (fabs(a) < EPS && fabs(b) < EPS && fabs(c) < EPS) {
        v1.setValue(1.0f, 0.0f, 0.0f);
        v2.setValue(0.0f, 1.0f, 0.0f);
        return 1;
    }
    if (fabs(a) < EPS && fabs(b) < EPS) {
        v1.setValue(1.0f, 0.0f, 0.0f);
        v2.setValue(0.0f, 1.0f, 0.0f);
        return 1;
    }
    if (fabs(a) < EPS && fabs(c) < EPS) {
        v1.setValue(1.0f, 0.0f, 0.0f);
        v2.setValue(0.0f, 0.0f, 1.0f);
        return 1;
    }
    if (fabs(b) < EPS && fabs(c) < EPS) {
        v1.setValue(0.0f, 1.0f, 0.0f);
        v2.setValue(0.0f, 0.0f, 1.0f);
        return 1;
    }
    if (a != 0.0f) {
        v1.setValue(-(b / a), 1.0f, 0.0f);
        v2.setValue(-(c / a), 0.0f, 1.0f);
        return 1;
    }
    if (b != 0.0f) {
        v1.setValue(1.0f, -(a / b), 0.0f);
        v2.setValue(0.0f, -(c / b), 1.0f);
        return 1;
    }
    v1.setValue(0.0f, 0.0f, 0.0f);
    v2.setValue(0.0f, 0.0f, 0.0f);
    return 5;
}

// 0x100030f0 ?eigenValues@MfMatrix3@@QBEHAAM00AAVMfVector3@@11@Z
int MfMatrix3::eigenValues(float &l1, float &l2, float &l3,
                           MfVector3 &v1, MfVector3 &v2, MfVector3 &v3) const
{
    MfMatrix3 A, B, C;
    A = *this;
    B = *this;
    C = *this;

    float a, b, c, d, e, f, g, h, i;
    a = _m[0][0];
    b = _m[0][1];
    c = _m[0][2];
    d = _m[1][0];
    e = _m[1][1];
    f = _m[1][2];
    g = _m[2][0];
    h = _m[2][1];
    i = _m[2][2];

    float c0 = a * e * i + b * f * g + c * d * h - a * f * h - b * d * i - c * e * g;
    float c1 = c * g - (a * e + a * i + e * i) + b * d + f * h;
    float c2 = a + e + i;
    int n = MfMath::cubic(-1.0f, c2, c1, c0, l1, l2, l3);
    if (n != 1)
        return n;
    int r;

    A[0][0] -= l1; A[1][1] -= l1; A[2][2] -= l1;
    B[0][0] -= l2; B[1][1] -= l2; B[2][2] -= l2;
    C[0][0] -= l3; C[1][1] -= l3; C[2][2] -= l3;

    // Two identical blocks (tail-merged by the compiler); with one block
    // the register-constant choice changes (ebx = 1 instead of 0).
    if (fabs(l1 - l2) < EPS && fabs(l2 - l3) < EPS) {
        v1.setValue(1.0f, 0.0f, 0.0f);
        v2.setValue(0.0f, 1.0f, 0.0f);
        v3.setValue(0.0f, 0.0f, 1.0f);
        return 1;
    }
    if (fabs(_m[0][1]) < EPS && fabs(_m[0][2]) < EPS && fabs(_m[1][0]) < EPS &&
        fabs(_m[1][2]) < EPS && fabs(_m[2][0]) < EPS && fabs(_m[2][1]) < EPS) {
        v1.setValue(1.0f, 0.0f, 0.0f);
        v2.setValue(0.0f, 1.0f, 0.0f);
        v3.setValue(0.0f, 0.0f, 1.0f);
        return 1;
    }
    if (fabs(l1 - l2) < EPS) {
        r = A.solve3SiTwo(v1, v2);
        if (r != 1)
            return r;
        r = C.solve3SiOne(v3);
        if (r != 1)
            return r;
    }
    if (fabs(l1 - l3) < EPS) {
        r = A.solve3SiTwo(v1, v3);
        if (r != 1)
            return r;
        r = B.solve3SiOne(v2);
        if (r != 1)
            return r;
    }
    if (fabs(l2 - l3) < EPS) {
        r = B.solve3SiTwo(v2, v3);
        if (r != 1)
            return r;
        r = A.solve3SiOne(v1);
        if (r != 1)
            return r;
    }
    r = A.solve3SiOne(v1);
    if (r != 1)
        return r;
    r = B.solve3SiOne(v2);
    if (r != 1)
        return r;
    r = C.solve3SiOne(v3);
    if (r != 1)
        return r;
    return 1;
}

// 0x10003610 ?principalAxis@MfMatrix3@@QAEXAAVMfXformMatrix3@@@Z
void MfMatrix3::principalAxis(MfXformMatrix3 &x)
{
    float l1, l2, l3;
    MfVector3 v1(0.0f, 0.0f, 0.0f);
    MfVector3 v2(0.0f, 0.0f, 0.0f);
    MfVector3 v3(0.0f, 0.0f, 0.0f);
    MfVector3 t(0.0f, 0.0f, 0.0f);

    if (eigenValues(l1, l2, l3, v1, v2, v3) == 1) {
        v1.normalize();
        v2.normalize();
        float d = v1.dot(v2);
        v2 = v2 - v1 * d;
        v2.normalize();
        v3 = v1.cross(v2);
        x[0][0] = v1.x(); x[0][1] = v1.y(); x[0][2] = v1.z(); x[0][3] = 0.0f;
        x[1][0] = v2.x(); x[1][1] = v2.y(); x[1][2] = v2.z(); x[1][3] = 0.0f;
        x[2][0] = v3.x(); x[2][1] = v3.y(); x[2][2] = v3.z(); x[2][3] = 0.0f;
    } else {
        x[0][0] = 1.0f; x[0][1] = 0.0f; x[0][2] = 0.0f; x[0][3] = 0.0f;
        x[1][0] = 0.0f; x[1][1] = 1.0f; x[1][2] = 0.0f; x[1][3] = 0.0f;
        x[2][0] = 0.0f; x[2][1] = 0.0f; x[2][2] = 1.0f; x[2][3] = 0.0f;
    }
}

// 0x10003820 ?identity@MfMatrix3@@SA?AV1@XZ
MfMatrix3 MfMatrix3::identity()
{
    MfMatrix3 r;
    memset(&r, 0, sizeof(r));
    r[2][2] = 1.0f;
    r[1][1] = 1.0f;
    r[0][0] = 1.0f;
    return r;
}
