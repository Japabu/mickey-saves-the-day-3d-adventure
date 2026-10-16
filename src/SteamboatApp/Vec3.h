#pragma once
#include "FixedVec.h"

// Three-component vector (points, directions, colours).
template <class T>
class Vec3 : public FixedVec<T, 3> {
public:
    Vec3(const FixedVec<T, 4> &v)
    {
        if (v.e[3] != 1 && v.e[3] != 0) {
            e[0] = v.e[0] / v.e[3];
            e[1] = v.e[1] / v.e[3];
            e[2] = v.e[2] / v.e[3];
        } else {
            e[0] = v.e[0];
            e[1] = v.e[1];
            e[2] = v.e[2];
        }
    }
    Vec3(const T *v)
    {
        e[0] = v[0];
        e[1] = v[1];
        e[2] = v[2];
    }
    Vec3(T x, T y, T z)
    {
        e[0] = x;
        e[1] = y;
        e[2] = z;
    }
    Vec3() {}
    void setValues(const T *v)
    {
        e[0] = v[0];
        e[1] = v[1];
        e[2] = v[2];
    }
    void setValues(T x, T y, T z)
    {
        e[0] = x;
        e[1] = y;
        e[2] = z;
    }
    void cross(const Vec3<T> &, Vec3<T> &) const;
    Vec3<T> cross(const Vec3<T> &v) const
    {
        Vec3<T> r;
        r.e[0] = e[1] * v.e[2] - e[2] * v.e[1];
        r.e[1] = e[2] * v.e[0] - e[0] * v.e[2];
        r.e[2] = e[0] * v.e[1] - e[1] * v.e[0];
        return r;
    }
    Vec3<T> projectIntoPlane(const Vec3<T> &) const;
    void add(T, Vec3<T> &) const;
    void add(const Vec3<T> &, Vec3<T> &) const;
    void multiply(T, Vec3<T> &) const;
    void multiply(const Vec3<T> &, Vec3<T> &) const;
    Vec3<T> operator+(T) const;
    Vec3<T> operator+(const Vec3<T> &v) const
    {
        return Vec3<T>(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);
    }
    Vec3<T> operator-() const
    {
        Vec3<T> r(*this);
        r.e[0] = -r.e[0];
        r.e[1] = -r.e[1];
        r.e[2] = -r.e[2];
        return r;
    }
    Vec3<T> operator-(T) const;
    Vec3<T> operator-(const Vec3<T> &v) const
    {
        Vec3<T> r(*this);
        r -= v;
        return r;
    }
    Vec3<T> operator*(T s) const
    {
        return Vec3<T>(e[0] * s, e[1] * s, e[2] * s);
    }
    Vec3<T> operator*(const Vec3<T> &) const;
    Vec3<T> operator/(T) const;
    Vec3<T> operator/(const Vec3<T> &) const;
    Vec3<T> &operator+=(T);
    Vec3<T> &operator+=(const Vec3<T> &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }
    Vec3<T> &operator-=(T);
    Vec3<T> &operator-=(const Vec3<T> &v)
    {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }
    Vec3<T> &operator*=(T s)
    {
        e[0] *= s;
        e[1] *= s;
        e[2] *= s;
        return *this;
    }
    Vec3<T> &operator*=(const Vec3<T> &);
    Vec3<T> &operator/=(T s)
    {
        e[0] /= s;
        e[1] /= s;
        e[2] /= s;
        return *this;
    }
    Vec3<T> &operator/=(const Vec3<T> &);
};

