#pragma once
// MfRotation3 (unit quaternion w + (x, y, z)), imported from mflinalg.dll.
// Declaration from the SteamboatApp PDB; inline members as mflinalg emits them.
#include <math.h>
#include "MfVector3.h"
class MfMath;
class MfXformMatrix3;

class __declspec(dllimport) MfRotation3 {
public:
    MfRotation3(const MfXformMatrix3 &);
    MfRotation3(const MfVector3 &from, const MfVector3 &to) { setValue(from, to); }
    MfRotation3(float angle, const MfVector3 &axis) { setValue(angle, axis); }
    MfRotation3(float w, float x, float y, float z) { _w = w; _x = x; _y = y; _z = z; }
    MfRotation3(const MfRotation3 &src) { _w = src._w; _x = src._x; _y = src._y; _z = src._z; }
    MfRotation3() { _w = 1.0f; _x = 0; _y = 0; _z = 0; }
    ~MfRotation3() {}
    MfRotation3 &operator=(const MfRotation3 &r)
    {
        _w = r._w; _x = r._x; _y = r._y; _z = r._z;
        return *this;
    }
    void Multiply(const MfRotation3 &, const MfRotation3 &);
    MfRotation3 operator~() const;
    MfRotation3 operator-() const;
    MfRotation3 operator*(float) const;
    MfVector3 operator*(const MfVector3 &) const;
    MfRotation3 operator*(const MfRotation3 &) const;
    MfRotation3 operator+(const MfRotation3 &) const;
    int operator==(const MfRotation3 &) const;
    void getValue(float &, MfVector3 &) const;
    void getValue(float &w, float &x, float &y, float &z) const { w = _w; x = _x; y = _y; z = _z; }
    float Angle() const;
    MfVector3 Axis() const;
    void setValue(const MfVector3 &, const MfVector3 &);
    void setValue(float, const MfVector3 &);
    void setValue(float w, float x, float y, float z) { _w = w; _x = x; _y = y; _z = z; }
    void interpolate(const MfRotation3 &, const MfRotation3 &, const MfRotation3 &, const MfRotation3 &, float);
    void interpolate(const MfRotation3 &, const MfRotation3 &, float, int);
    void normalize()
    {
        float inv = 1.0f / (float)::sqrt(_w * _w + _x * _x + _y * _y + _z * _z);
        _w *= inv;
        _x *= inv;
        _y *= inv;
        _z *= inv;
    }
    void fixSign();
    void scalarMult(float);
    void leap(const MfRotation3 &, const MfRotation3 &);
    void bisect(const MfRotation3 &, const MfRotation3 &);
    float dot(const MfRotation3 &) const;
    static MfRotation3 random(float);
    static MfRotation3 identity() { return MfRotation3(); }
    void rotationXform(MfVector3 &, MfVector3 &);
    void Postmultiply(const MfRotation3 &);
private:
    static MfMath _math;
    float _w;
    float _x;
    float _y;
    float _z;
};
