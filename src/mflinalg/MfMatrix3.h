#pragma once
// MfMatrix3: 3x3 matrix, rows of three floats. Exported names only; no PDB
// declares the layout.
#include "MfVector3.h"
class MfRotation3;
class MfXformMatrix3;

class MFLINALG_API MfMatrix3 {
public:
    MfMatrix3();
    MfMatrix3(const MfRotation3 &);
    MfMatrix3(const MfMatrix3 &);
    ~MfMatrix3();
    MfMatrix3 &operator=(const MfMatrix3 &);
    float *operator[](int i) { return _m[i]; }
    const float *operator[](int i) const { return _m[i]; }
    void transpose(MfMatrix3 &t) const
    {
        t._m[0][0] = _m[0][0]; t._m[0][1] = _m[1][0]; t._m[0][2] = _m[2][0];
        t._m[1][0] = _m[0][1]; t._m[1][1] = _m[1][1]; t._m[1][2] = _m[2][1];
        t._m[2][0] = _m[0][2]; t._m[2][1] = _m[1][2]; t._m[2][2] = _m[2][2];
    }
    int invert(MfMatrix3 &) const;
    MfMatrix3 operator+(const MfMatrix3 &) const;
    void add(const MfMatrix3 &, MfMatrix3 &) const;
    MfMatrix3 operator-(const MfMatrix3 &) const;
    void subtract(const MfMatrix3 &, MfMatrix3 &) const;
    MfMatrix3 operator-() const;
    void negate(MfMatrix3 &) const;
    MfMatrix3 operator*(float) const;
    void multiply(float, MfMatrix3 &) const;
    MfMatrix3 operator*(const MfMatrix3 &) const;
    void multiply(const MfMatrix3 &, MfMatrix3 &) const;
    MfVector3 operator*(const MfVector3 &) const;
    void multiply(const MfVector3 &, MfVector3 &) const;
    float det() const;
    int solve(const MfVector3 &, MfVector3 &) const;
    int solve3SiOne(MfVector3 &);
    int solve3SiTwo(MfVector3 &, MfVector3 &);
    int eigenValues(float &, float &, float &, MfVector3 &, MfVector3 &, MfVector3 &) const;
    void principalAxis(MfXformMatrix3 &);
    static MfMatrix3 identity();
private:
    float _m[3][3];
};
