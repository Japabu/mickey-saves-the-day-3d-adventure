#pragma once
#include "Vec.h"
#include "Vec4.h"
#include "TMatrix4.h"

// Unit quaternion (w, x, y, z) of the scene library's math (EAIStd
// vecmath); declaration from SteamboatApp.pdb.
template <class T>
class Quaternion : public Vec4<T> {
    typedef T (*QuatMat)[4];
    typedef T (*QuatMat3)[3];
public:
    Quaternion(const TMatrix3<T> &);
    Quaternion(const TMatrix4<T> &);
    Quaternion(const T *, T);
    Quaternion(const Vec3<T> &axis, T angle)
    {
        angle *= (T)0.5;
        e[0] = (T)cos(angle);
        Vec3<T> v(axis);
        v.normalize();
        T s = (T)sin(angle);
        e[1] = v.e[0] * s;
        e[2] = v.e[1] * s;
        e[3] = v.e[2] * s;
    }
    Quaternion(const Vec4<T> &);
    Quaternion(const T *);
    Quaternion(T w, T x, T y, T z)
    {
        e[0] = w;
        e[1] = x;
        e[2] = y;
        e[3] = z;
    }
    Quaternion(Vec3<T> &);
    Quaternion(T, T, T);
    Quaternion() {}
    ~Quaternion() {}
    Quaternion<T> operator*(T) const;
    Quaternion<T> operator*(const Quaternion<T> &) const;
    int operator==(const Vec4<T> &);
    int operator==(const Quaternion<T> &);
    void get(TMatrix4<T> &) const;
    void get(TMatrix3<T> &) const;
    void get(T *, T &) const;
    void get(Vec3<T> &, T &) const;
    void get(T *) const;
    void get(T &, T &, T &, T &) const;
    void getEulerAngles(Vec3<T> &) const;
    Quaternion<T> getConjugate() const;
    Quaternion<T> subQuat(const Quaternion<T> &) const;
    Quaternion<T> deriv(T, T, T) const;
    static Quaternion<T> slerp(const Quaternion<T> &, const Quaternion<T> &, T);
    void fromVecAng(const T *, const T);
    void fromEuler(Vec3<T> &);
    void fromEuler(T, T, T);
    void fromMat(const TMatrix3<T> &);
    void fromMat(const TMatrix4<T> &);
    void toVecAng(T *, T &) const;
    void toMat(TMatrix4<T> &) const;
    void toMat3(TMatrix3<T> &mat) const;
};

template <class T>
void Quaternion<T>::toMat3(TMatrix3<T> &mat) const
{
    T ww = e[0] * e[0];
    mat[0][0] = 2 * (e[1] * e[1] + ww - (T)0.5);
    T xy = e[2] * e[1];
    T wz = e[3] * e[0];
    mat[1][0] = 2 * (xy - wz);
    T xz = e[3] * e[1];
    T wy = e[2] * e[0];
    mat[2][0] = 2 * (wy + xz);
    mat[0][1] = 2 * (wz + xy);
    mat[1][1] = 2 * (e[2] * e[2] + ww - (T)0.5);
    T yz = e[3] * e[2];
    T wx = e[0] * e[1];
    mat[2][1] = 2 * (yz - wx);
    mat[0][2] = 2 * (xz - wy);
    mat[1][2] = 2 * (wx + yz);
    mat[2][2] = 2 * (e[3] * e[3] + ww - (T)0.5);
}

// toMat3's statements into the rotation part of a 4 x 4 matrix (the out-of-
// line copy in refFrame.obj): mat[3][j] cleared after each column, then the
// last column set to (0, 0, 0, 1).
template <class T>
void Quaternion<T>::toMat(TMatrix4<T> &mat) const
{
    T ww = e[0] * e[0];
    mat(0, 0) = 2 * (e[1] * e[1] + ww - (T)0.5);
    T xy = e[2] * e[1];
    T wz = e[3] * e[0];
    mat(1, 0) = 2 * (xy - wz);
    T xz = e[3] * e[1];
    T wy = e[2] * e[0];
    mat(2, 0) = 2 * (wy + xz);
    mat(3, 0) = 0;
    mat(0, 1) = 2 * (wz + xy);
    mat(1, 1) = 2 * (e[2] * e[2] + ww - (T)0.5);
    T yz = e[3] * e[2];
    T wx = e[0] * e[1];
    mat(2, 1) = 2 * (yz - wx);
    mat(3, 1) = 0;
    mat(0, 2) = 2 * (xz - wy);
    mat(1, 2) = 2 * (wx + yz);
    mat(2, 2) = 2 * (e[3] * e[3] + ww - (T)0.5);
    mat(3, 2) = 0;
    mat(0, 3) = 0;
    mat(1, 3) = 0;
    mat(2, 3) = 0;
    mat(3, 3) = 1;
}

template <class T>
Quaternion<T> Quaternion<T>::subQuat(const Quaternion<T> &q) const
{
    TMatrix3<T> m1;
    TMatrix3<T> m2;
    toMat3(m1);
    q.toMat3(m2);
    m2.transpose();
    TMatrix3<T> r;
    r.setValues(m1);
    r.multRight(m2);
    TMatrix3<T> m;
    m.setValues(r);
    Quaternion<T> result;
    result.fromMat(m);
    return result;
}

template <class T>
inline Quaternion<T> operator-(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return a.subQuat(b);
}

template <class T>
void Quaternion<T>::toVecAng(T *axis, T &angle) const
{
    T s = (T)sqrt(1.0 - e[0] * e[0]);
    if (s != 0) {
        T inv = 1 / s;
        axis[0] = inv * e[1];
        axis[1] = inv * e[2];
        axis[2] = inv * e[3];
    } else {
        axis[0] = 0;
        axis[1] = 0;
        axis[2] = 0;
    }
    angle = 2 * (T)atan(s / e[0]);
}

template <class T>
void Quaternion<T>::fromMat(const TMatrix3<T> &m)
{
    e[0] = (T)(sqrt(fabs(m(0, 0) + m(1, 1) + m(2, 2) + 1)) * 0.5);
    if (fabs(e[0]) < 0.0001) {
        e[0] = 0;
        e[1] = (T)sqrt(fabs(2 * (m(0, 0) + 1))) * (T)0.5;
        e[2] = (T)sqrt(fabs(2 * (m(1, 1) + 1))) * (T)0.5;
        e[3] = (T)sqrt(fabs(2 * (m(2, 2) + 1))) * (T)0.5;
        if (e[1] >= e[2] && e[1] >= e[3]) {
            T d = e[1] * 4;
            e[2] = (m(0, 1) + m(1, 0)) / d;
            e[3] = (m(0, 2) + m(2, 0)) / d;
        } else if (e[2] >= e[3] && e[2] >= e[1]) {
            T d = e[2] * 4;
            e[1] = (m(0, 1) + m(1, 0)) / d;
            e[3] = (m(1, 2) + m(2, 1)) / d;
        } else {
            T d = e[3] * 4;
            e[1] = (m(0, 2) + m(2, 0)) / d;
            e[2] = (m(1, 2) + m(2, 1)) / d;
        }
    } else {
        T d = e[0] * 4;
        e[1] = (m(1, 2) - m(2, 1)) / d;
        e[2] = (m(2, 0) - m(0, 2)) / d;
        e[3] = (m(0, 1) - m(1, 0)) / d;
    }
}

