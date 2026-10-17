#pragma once
// MfVector3. Declaration from the SteamboatApp PDB; the members defined here are
// the ones mflinalg emits with the inline functions of its header.
#include <math.h>
#include "mflinalg_api.h"
class MfTransform3;

class MFLINALG_API MfVector3 {
public:
    MfVector3(const MfVector3 &src) { _x = src._x; _y = src._y; _z = src._z; }
    MfVector3(float x, float y, float z) { _x = x; _y = y; _z = z; }
    MfVector3() { _x = 0; _y = 0; _z = 0; }
    ~MfVector3() {}
    MfVector3 &operator=(const MfVector3 &v) { _x = v._x; _y = v._y; _z = v._z; return *this; }
    MfVector3 operator-(const MfVector3 &) const;
    MfVector3 operator-() const;
    MfVector3 operator+(const MfVector3 &) const;
    MfVector3 &operator+=(const MfVector3 &);
    MfVector3 &operator-=(const MfVector3 &);
    MfVector3 operator*(const MfVector3 &) const;
    MfVector3 operator*(float) const;
    MfVector3 operator/(const MfVector3 &) const;
    MfVector3 operator/(float) const;
    MfVector3 &operator*=(const MfVector3 &);
    MfVector3 &operator*=(float);
    MfVector3 &operator/=(const MfVector3 &);
    MfVector3 &operator/=(float);
    float operator[](int i) const { return (&_x)[i]; }
    float &operator[](int i) { return (&_x)[i]; }
    float dot(const MfVector3 &) const;
    MfVector3 cross(const MfVector3 &) const;
    float angle(const MfVector3 &) const;
    float PreciseAngle(const MfVector3 &) const;
    float &x() { return _x; }
    const float &x() const { return _x; }
    float &y() { return _y; }
    const float &y() const { return _y; }
    float &z() { return _z; }
    const float &z() const { return _z; }
    void setValue(float x, float y, float z) { _x = x; _y = y; _z = z; }
    float length() const;
    float lengthSquared() const;
    MfVector3 unit() const;
    void normalize()
    {
        float l = (float)::sqrt(lengthSquared());
        _x /= l;
        _y /= l;
        _z /= l;
    }
    MfVector3 square() { return MfVector3(_x * _x, _y * _y, _z * _z); }
    void interpolate(const MfVector3 &, const MfVector3 &, const MfVector3 &, const MfVector3 &, float);
    void interpolate(const MfVector3 &, const MfVector3 &, float);
    void tangent(const MfVector3 &, const MfVector3 &, const MfVector3 &, const MfVector3 &, float);
    void Premultiply(const MfTransform3 &);
    void Multiply(const MfTransform3 &, const MfVector3 &);
    void NormalizeCross(const MfVector3 &, const MfVector3 &);
    float LengthAndNormalize()
    {
        float l = (float)::sqrt(lengthSquared());
        _x /= l;
        _y /= l;
        _z /= l;
        return l;
    }
private:
    float _x;
    float _y;
    float _z;
};

// Defined after normalize(): an inlined normalize() calls the out-of-line copy.
inline float MfVector3::lengthSquared() const { return _x * _x + _y * _y + _z * _z; }
