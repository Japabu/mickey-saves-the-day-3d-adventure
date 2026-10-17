// MfRotation3 (unit quaternion) unit of mflinalg.dll: reconstructed declarations.
// Layouts copied from mflinalg.h and from the code of this range.
// own copy: same layouts as the canonical MfMath.h, MfVector3.h, MfXformMatrix3.h
// and MfRotation3.h, but MfRotation3.cpp's operand orders depend on VC5's symbol
// counter, which these exact declarations set; compiled against the canonical
// headers (members renamed) 4 to 10 of its 28 functions differ for every include
// order and padding tried (sweep of 2026-10-17), including the inline normalize
// and scalarMult that this object emits.
#ifndef MFROTATION3_H
#define MFROTATION3_H

#include <math.h>
// rand is called through the import table (msvcrt.dll).
extern "C" __declspec(dllimport) int __cdecl rand(void);

#include "mflinalg_api.h"

// The forward declarations also set VC5's symbol counter (see MfRotation3.cpp).
class MfVector2;
class MfRotation2;
class MfMatrix3;
class MfTransform3;
class MfXformMatrix3;

class MFLINALG_API MfMath {
public:
    MfMath();
    ~MfMath();
    static float sqrt(float x);
    static float sin(float a);
    static float cos(float a);
    static float acos(float c);
    static float det2(float a, float b, float c, float d);
    static int quadratic(float a, float b, float c, float &r1, float &r2);
    static int cubic(float a, float b, float c, float d, float &r1, float &r2, float &r3);
    static int solve2(float a, float b, float c, float d, float e, float f, float &x, float &y);
private:
    static float real3Root(float a, float b);
    static short *_sqrttab;
    static float *_costable;
    static float _cosFactor;
    static float *_acostable;
    static float _acosFactor;
    static MfMath g_math;
};

class MFLINALG_API MfVector3 {
public:
    MfVector3() {}
    MfVector3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    MfVector3(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; }
    ~MfVector3() {}
    MfVector3 &operator=(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; return *this; }
    float &operator[](int i) { return (&x)[i]; }
    float operator[](int i) const { return (&x)[i]; }
    void setValue(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    float lengthSquared() const { return x * x + y * y + z * z; }
    void normalize()
    {
        float l = (float)::sqrt(lengthSquared());
        x /= l;
        y /= l;
        z /= l;
    }
    float LengthAndNormalize();
    MfVector3 square();

    MfVector3 operator+(const MfVector3 &o) const;
    MfVector3 operator-(const MfVector3 &o) const;
    MfVector3 operator-() const;
    MfVector3 operator*(const MfVector3 &o) const;
    MfVector3 operator*(float f) const;
    MfVector3 operator/(const MfVector3 &o) const;
    MfVector3 operator/(float f) const;
    MfVector3 &operator+=(const MfVector3 &o);
    MfVector3 &operator-=(const MfVector3 &o);
    MfVector3 &operator*=(const MfVector3 &o);
    MfVector3 &operator*=(float f);
    MfVector3 &operator/=(const MfVector3 &o);
    MfVector3 &operator/=(float f);

    float length() const;
    float dot(const MfVector3 &o) const;
    MfVector3 cross(const MfVector3 &o) const;
    void NormalizeCross(const MfVector3 &a, const MfVector3 &b);
    MfVector3 unit() const;
    float angle(const MfVector3 &o) const;
    float PreciseAngle(const MfVector3 &o) const;
    void interpolate(const MfVector3 &a, const MfVector3 &b, float t);
    void interpolate(const MfVector3 &a, const MfVector3 &b, const MfVector3 &c,
                     const MfVector3 &d, float t);
    void tangent(const MfVector3 &a, const MfVector3 &b, const MfVector3 &c,
                 const MfVector3 &d, float t);
    void Multiply(const MfTransform3 &m, const MfVector3 &v);
    void Premultiply(const MfTransform3 &m);

    float x, y, z;
};

// Rows of four floats (operator[] = this + 16 * i).
class MFLINALG_API MfXformMatrix3 {
public:
    float *operator[](int i) { return m[i]; }
    const float *operator[](int i) const { return m[i]; }
    float m[4][4];
};

class MFLINALG_API MfRotation3 {
public:
    MfRotation3() { s = 1.0f; x = 0.0f; y = 0.0f; z = 0.0f; }
    MfRotation3(float s_, float x_, float y_, float z_) { s = s_; x = x_; y = y_; z = z_; }
    MfRotation3(float angle, const MfVector3 &axis) { setValue(angle, axis); }
    MfRotation3(const MfVector3 &from, const MfVector3 &to) { setValue(from, to); }
    MfRotation3(const MfRotation3 &o) { s = o.s; x = o.x; y = o.y; z = o.z; }
    MfRotation3(const MfXformMatrix3 &m);
    ~MfRotation3() {}
    MfRotation3 &operator=(const MfRotation3 &o)
    {
        s = o.s; x = o.x; y = o.y; z = o.z;
        return *this;
    }

    MfRotation3 operator~() const;
    MfRotation3 operator-() const;
    MfRotation3 operator*(const MfRotation3 &q) const;
    void Multiply(const MfRotation3 &a, const MfRotation3 &b);
    MfVector3 operator*(const MfVector3 &p) const;
    MfRotation3 operator*(float f) const;
    MfRotation3 operator+(const MfRotation3 &q) const;
    int operator==(const MfRotation3 &q) const;

    void getValue(float &s_, float &x_, float &y_, float &z_) const
    {
        s_ = s; x_ = x; y_ = y; z_ = z;
    }
    void getValue(float &angle, MfVector3 &axis) const;
    float Angle() const;
    MfVector3 Axis() const;
    void setValue(float s_, float x_, float y_, float z_)
    {
        s = s_; x = x_; y = y_; z = z_;
    }
    void setValue(float angle, const MfVector3 &axis);
    void setValue(const MfVector3 &from, const MfVector3 &to);

    void normalize()
    {
        float inv = 1.0f / (float)::sqrt(s * s + x * x + y * y + z * z);
        s *= inv;
        x *= inv;
        y *= inv;
        z *= inv;
    }
    void fixSign();
    void scalarMult(float f) { s *= f; x *= f; y *= f; z *= f; }
    static MfRotation3 identity() { return MfRotation3(); }

    void interpolate(const MfRotation3 &q1, const MfRotation3 &q2, float t, int spin);
    void interpolate(const MfRotation3 &q0, const MfRotation3 &q1, const MfRotation3 &q2,
                     const MfRotation3 &q3, float t);
    void leap(const MfRotation3 &a, const MfRotation3 &b);
    void bisect(const MfRotation3 &a, const MfRotation3 &b);
    float dot(const MfRotation3 &q) const;
    static MfRotation3 random(float range);
    void rotationXform(MfVector3 &from, MfVector3 &to);
    void Postmultiply(const MfRotation3 &q);

    float s;        // scalar part (cos of half the angle)
    float x, y, z;  // vector part
private:
    static MfMath _math;
};

#endif
