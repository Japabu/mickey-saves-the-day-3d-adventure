#pragma once
#include <math.h>
#include <string.h>

class ostream;

// Larger of two values.
template <class T>
inline T Max(T a, T b)
{
    return (b < a) ? a : b;
}

// Absolute value without the library call.
template <class T>
inline T Abs(T x)
{
    return (x < 0) ? -x : x;
}

// Fixed-size vector of the scene library's math (EAIStd vecmath): N
// components stored in place.
template <class T, int N>
class FixedVec {
public:
    T e[N];
    FixedVec(const T *);
    FixedVec() {}
    operator T *() { return e; }
    operator const T *() const { return e; }
    T &operator[](unsigned int i) { return e[i]; }
    T &operator[](int i) { return e[i]; }
    const T &operator()(unsigned int i) const { return e[i]; }
    const T &operator()(int i) const { return e[i]; }
    void setValue(int, T &);
    void setValues(const T *);
    ostream &write(ostream &) const;
    int operator==(const FixedVec<T, N> &v) const
    {
        int equal = 1;
        for (int i = 0; i < N && equal; i++)
            if (e[i] != v.e[i])
                equal = 0;
        return equal;
    }
    int equals(const FixedVec<T, N> &v, T tolerance) const
    {
        int equal = 1;
        for (int i = 0; i < N && equal; i++) {
            T a = e[i];
            T b = v.e[i];
            if (b > a) {
                if (!(b - a < tolerance))
                    equal = 0;
            } else if (!(a - b < tolerance)) {
                equal = 0;
            }
        }
        return equal;
    }
    T dot(const FixedVec<T, N> &v) const
    {
        T sum = 0;
        for (int i = 0; i < N; i++)
            sum += e[i] * v.e[i];
        return sum;
    }
    T mag() const { return (T)sqrt(mag2()); }
    T mag2() const
    {
        T sum = 0;
        for (int i = 0; i < N; i++)
            sum += e[i] * e[i];
        return sum;
    }
    void inverse();
    void zero() { memset(e, 0, sizeof(e)); }
    void normalize() { multScal(1 / (T)sqrt(mag2())); }
    bool IsNAN() const;
    bool IsFinite() const;
    FixedVec<T, N> &addVec(const FixedVec<T, N> &v)
    {
        for (int i = 0; i < N; i++)
            e[i] += v.e[i];
        return *this;
    }
    FixedVec<T, N> &subVec(const FixedVec<T, N> &v)
    {
        for (int i = 0; i < N; i++)
            e[i] -= v.e[i];
        return *this;
    }
    FixedVec<T, N> &multVec(const FixedVec<T, N> &);
    FixedVec<T, N> &divVec(const FixedVec<T, N> &);
    FixedVec<T, N> &addScal(T);
    FixedVec<T, N> &subScal(T);
    FixedVec<T, N> &multScal(T s)
    {
        for (int i = 0; i < N; i++)
            e[i] *= s;
        return *this;
    }
    FixedVec<T, N> &divScal(T);
    FixedVec<T, N> &negate();
    void unhomo() { multScal(1 / e[N - 1]); }
protected:
    void checkBounds(int &) const;
};

// Three-component versions without the loop.
inline FixedVec<float, 3> &FixedVec<float, 3>::multScal(float s)
{
    e[0] *= s;
    e[1] *= s;
    e[2] *= s;
    return *this;
}
