#pragma once
// MfRotation2 (an angle in the plane), imported from mflinalg.dll (see
// mflinalg/MfRotation2.h).
#include "MfVector2.h"

class __declspec(dllimport) MfRotation2 {
public:
    MfRotation2() { _angle = 0; }
    MfRotation2(float a) { _angle = a; }
    MfRotation2(const MfRotation2 &r) { _angle = r._angle; }
    ~MfRotation2() {}
    MfRotation2 &operator=(const MfRotation2 &r) { _angle = r._angle; return *this; }
    MfRotation2 &operator=(float a) { _angle = a; return *this; }
    operator float() const { return _angle; }
    float angle() const { return _angle; }
    void setAngle(float a) { _angle = a; }
    MfVector2 operator*(const MfVector2 &) const;
    void interpolate(const MfRotation2 &, const MfRotation2 &, float);
private:
    float _angle;
};
