// MfRotation3 (mflinalg.dll): a unit quaternion (s, x, y, z). The constructors,
// assignment and destructor are inline in the original header.
#ifndef RUNTIMELIB_MFROTATION3_H
#define RUNTIMELIB_MFROTATION3_H
#include <math.h>
class MfMath;
class MfVector3;
class MfXformMatrix3;

class __declspec(dllimport) MfRotation3 {
public:
    MfRotation3 &operator=(const MfRotation3 &r) { s = r.s; x = r.x; y = r.y; z = r.z; return *this; }
    MfRotation3 operator*(MfRotation3 const &) const;
    MfRotation3 operator*(float) const;
    MfRotation3 operator+(MfRotation3 const &) const;
    MfRotation3 operator-() const;
    MfRotation3 operator~() const;
    MfRotation3() { s = 1.0f; x = 0.0f; y = 0.0f; z = 0.0f; }
    MfRotation3(const MfRotation3 &r) { s = r.s; x = r.x; y = r.y; z = r.z; }
    MfRotation3(const MfVector3 &from, const MfVector3 &to) { setValue(from, to); }
    MfRotation3(MfXformMatrix3 const &);
    MfRotation3(float angle, const MfVector3 &axis) { setValue(angle, axis); }
    MfRotation3(float, float, float, float);
    MfVector3 Axis() const;
    MfVector3 operator*(MfVector3 const &) const;
    float Angle() const;
    float dot(MfRotation3 const &) const;
    int operator==(MfRotation3 const &) const;
    static MfRotation3 __cdecl identity();
    static MfRotation3 __cdecl random(float);
    void Multiply(MfRotation3 const &, MfRotation3 const &);
    void Postmultiply(MfRotation3 const &);
    void bisect(MfRotation3 const &, MfRotation3 const &);
    void fixSign();
    void getValue(float &, MfVector3 &) const;
    void getValue(float &, float &, float &, float &) const;
    void interpolate(MfRotation3 const &, MfRotation3 const &, MfRotation3 const &, MfRotation3 const &, float);
    void interpolate(MfRotation3 const &, MfRotation3 const &, float, int);
    void leap(MfRotation3 const &, MfRotation3 const &);
    void normalize()
    {
        float inv = 1.0f / (float)::sqrt(x * x + y * y + z * z + s * s);
        s *= inv;
        x *= inv;
        y *= inv;
        z *= inv;
    }
    void rotationXform(MfVector3 &, MfVector3 &);
    void scalarMult(float);
    void setValue(MfVector3 const &, MfVector3 const &);
    void setValue(float, MfVector3 const &);
    void setValue(float, float, float, float);
    ~MfRotation3() {}
private:
    static MfMath _math;
    float s;
    float x;
    float y;
    float z;
};

#endif
