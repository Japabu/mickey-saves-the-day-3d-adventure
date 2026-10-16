#pragma once
// MfXformMatrix3: 3x4 affine matrix, rows of four floats; column 3 is the
// translation (tx, ty, tz). Imported from mflinalg.dll; no PDB declares the layout.
#include "MfTransform3.h"

class __declspec(dllimport) MfXformMatrix3 {
public:
    MfXformMatrix3();
    MfXformMatrix3(const MfXformMatrix3 &);
    MfXformMatrix3(const float *);
    MfXformMatrix3(const MfTransform3 &t) { setValue(t.rotation(), t.translation()); }
    MfXformMatrix3(const MfRotation3 &r, const MfVector3 &t) { setValue(r, t); }
    ~MfXformMatrix3() {}
    MfXformMatrix3 &operator=(const MfXformMatrix3 &);
    float *operator[](int i) { return _m[i]; }
    const float *operator[](int i) const { return _m[i]; }
    float &tx() { return _m[0][3]; }
    float &ty() { return _m[1][3]; }
    float &tz() { return _m[2][3]; }
    float tx() const { return _m[0][3]; }
    float ty() const { return _m[1][3]; }
    float tz() const { return _m[2][3]; }
    void setValue(const MfTransform3 &t) { setValue(t.rotation(), t.translation()); }
    void setValue(const MfRotation3 &, const MfVector3 &);
    void setValue(const MfRotation3 &);
    MfVector3 translation() const { return MfVector3(tx(), ty(), tz()); }
    void setTranslation(const MfVector3 &t) { tx() = t.x(); ty() = t.y(); tz() = t.z(); }
    static MfXformMatrix3 identity() { return MfXformMatrix3(); }
    MfXformMatrix3 operator~() const;
    void invert(MfXformMatrix3 &) const;
    MfXformMatrix3 operator*(const MfXformMatrix3 &) const;
    void multiply(const MfXformMatrix3 &, MfXformMatrix3 &) const;
    MfVector3 operator*(const MfVector3 &) const;
    void multiply(const MfVector3 &, MfVector3 &) const;
    MfVector3 rotate(const MfVector3 &) const;
    void rotate(const MfVector3 &, MfVector3 &) const;
    void getEuler(float &, float &, float &);
private:
    float _m[3][4];
};
