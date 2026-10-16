#pragma once
#include "FixedVec.h"

// Four-component vector (homogeneous points, colours).
template <class T>
class Vec4 : public FixedVec<T, 4> {
public:
    Vec4(const FixedVec<T, 3> &, T);
    Vec4(const FixedVec<T, 4> &v)
    {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
        e[3] = v.e[3];
    }
    Vec4(const T *);
    Vec4(T e0, T e1, T e2, T e3)
    {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
        e[3] = e3;
    }
    Vec4() {}
    void setValues(T, T, T, T);
    void setValues(const T *);
    void add(T, Vec4<T> &) const;
    void add(const Vec4<T> &, Vec4<T> &) const;
    void multiply(T, Vec4<T> &) const;
    void multiply(const Vec4<T> &, Vec4<T> &) const;
    Vec4<T> operator+(T) const;
    Vec4<T> operator+(const Vec4<T> &) const;
    Vec4<T> operator-() const;
    Vec4<T> operator-(T) const;
    Vec4<T> operator-(const Vec4<T> &) const;
    Vec4<T> operator*(T) const;
    Vec4<T> operator*(const Vec4<T> &) const;
    Vec4<T> operator/(T) const;
    Vec4<T> operator/(const Vec4<T> &) const;
    Vec4<T> &operator+=(T);
    Vec4<T> &operator+=(const Vec4<T> &);
    Vec4<T> &operator-=(T);
    Vec4<T> &operator-=(const Vec4<T> &);
    Vec4<T> &operator*=(T);
    Vec4<T> &operator*=(const Vec4<T> &);
    Vec4<T> &operator/=(T);
    Vec4<T> &operator/=(const Vec4<T> &);
};

// The inlined copy in ShadowAttributeD3D::Push (the three components, then w).
template <class T>
inline Vec4<T>::Vec4(const FixedVec<T, 3> &v, T w)
{
    e[0] = v.e[0];
    e[1] = v.e[1];
    e[2] = v.e[2];
    e[3] = w;
}
