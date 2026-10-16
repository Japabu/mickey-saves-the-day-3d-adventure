#pragma once
// MfVector2, imported from mflinalg.dll (see mflinalg/MfVector2.h).
class __declspec(dllimport) MfVector2 {
public:
    MfVector2() { _x = 0; _y = 0; }
    MfVector2(float x, float y) { _x = x; _y = y; }
    ~MfVector2() {}
    MfVector2 operator-() const;
    MfVector2 operator+(const MfVector2 &) const;
    MfVector2 operator-(const MfVector2 &) const;
    MfVector2 operator*(float) const;
    MfVector2 operator/(float) const;
    float &operator[](int i) { return (&_x)[i]; }
    float operator[](int i) const { return (&_x)[i]; }
    float &x() { return _x; }
    const float &x() const { return _x; }
    float &y() { return _y; }
    const float &y() const { return _y; }
    float dot(const MfVector2 &) const;
    float length() const;
    float lengthSquared() const { return _x * _x + _y * _y; }
    void normalize();
    float angle() const;
private:
    float _x;
    float _y;
};
