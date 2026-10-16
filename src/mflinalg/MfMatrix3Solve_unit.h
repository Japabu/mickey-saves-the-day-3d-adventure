// Declarations for the MfMatrix3Solve unit of mflinalg.dll (MfMatrix3 solvers,
// eigen decomposition, principal axes). Copied/derived from mflinalg.h.
#ifndef MFMATRIX3SOLVE_H
#define MFMATRIX3SOLVE_H

#include <math.h>
#include <string.h>

#define MFAPI __declspec(dllexport)

class MFAPI MfMath {
public:
    ~MfMath();
    static float det2(float a, float b, float c, float d);
    static float sqrt(float x);
    static int solve2(float a, float b, float c, float d, float e, float f,
                      float &x, float &y);
    static int cubic(float a, float b, float c, float d,
                     float &r1, float &r2, float &r3);
private:
    static short *_sqrttab;
};

// The destructor is inline (the EH funclets call the exported copy, folded
// with ??1MfMath at 0x10001700); the constructors used here are inlined.
class MFAPI MfVector3 {
public:
    MfVector3() {}
    MfVector3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    MfVector3(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; }
    ~MfVector3() {}
    MfVector3 &operator=(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; return *this; }
    void setValue(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    float lengthSquared() const;
    void normalize() { float l = (float)::sqrt(lengthSquared()); x /= l; y /= l; z /= l; }
    float dot(const MfVector3 &o) const;
    MfVector3 cross(const MfVector3 &o) const;
    MfVector3 operator*(float s) const;
    MfVector3 operator-(const MfVector3 &o) const;
    float x, y, z;
};

// Defined after normalize(): the inlined normalize() calls the out-of-line copy
// (0x100017c0), as in the original.
inline float MfVector3::lengthSquared() const { return x * x + y * y + z * z; }

class MFAPI MfXformMatrix3 {
public:
    float *operator[](int i) { return m[i]; }
    float m[3][4];      // 3x3 rotation, column 3 = translation (tx/ty/tz)
};

class MFAPI MfMatrix3 {
public:
    MfMatrix3();
    MfMatrix3(const MfMatrix3 &o);
    ~MfMatrix3();
    MfMatrix3 &operator=(const MfMatrix3 &o);
    float *operator[](int i) { return m[i]; }
    const float *operator[](int i) const { return m[i]; }

    int solve3SiOne(MfVector3 &v);
    int solve3SiTwo(MfVector3 &v1, MfVector3 &v2);
    int eigenValues(float &l1, float &l2, float &l3,
                    MfVector3 &v1, MfVector3 &v2, MfVector3 &v3) const;
    void principalAxis(MfXformMatrix3 &x);
    static MfMatrix3 identity();

    float m[3][3];
};

// 0x100029b0, a static helper of MfMatrix3.cpp in the MfMatrix3 unit's range:
// squared residual |M v|^2 of the matrix given element by element.
float fn_100029b0(const MfVector3 &v, float a, float b, float c, float d,
                  float e, float f, float g, float h, float i);

#endif
