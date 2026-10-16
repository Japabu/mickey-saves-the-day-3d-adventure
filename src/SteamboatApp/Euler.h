#pragma once
#include "Quaternion.h"

// A rotation kept as a unit quaternion (EAIStd vecmath; declaration from
// SteamboatApp.pdb).
template <class T>
class Euler : public Quaternion<T> {
public:
    Euler(const T **);
    Euler(const T (*)[4]);
    Euler(const TMatrix4<T> &);
    Euler(const T *, T);
    Euler(const Vec3<T> &, T);
    Euler(const Vec4<T> &);
    Euler(const T *);
    Euler(T, T, T, T);
    Euler(const Quaternion<T> &q) : Quaternion<T>(q) {}
    Euler() : Quaternion<T>(1, 0, 0, 0) {}
    ~Euler() {}
    int operator==(const Euler<T> &);
    int operator==(const Quaternion<T> &);
    static Euler<T> slerp(const Euler<T> &, const Euler<T> &, const T);
private:
    void eulerEnforce();
};

template <class T>
Euler<T> Euler<T>::slerp(const Euler<T> &a, const Euler<T> &b, const T t)
{
    Euler<T> r(Quaternion<T>::slerp(a, b, t));
    r.normalize();
    return r;
}
