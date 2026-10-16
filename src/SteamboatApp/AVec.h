#pragma once
#include "Vec.h"

// Vec with element-wise arithmetic (EAIStd); only what the units here use.
template <class T>
class AVec : public Vec<T> {
public:
    AVec(const Vec<T> &);
    AVec(const AVec<T> &);
    AVec(const T *, int, float);
    AVec(int n = 0, float mult = 2.0f) : Vec<T>(n, mult) {}
    AVec<T> &operator=(const AVec<T> &v)
    {
        int n = v.len;
        if (alloclen < n)
            reallocMem(n, 0, 0);
        if (n > 0) {
            T *dst = e;
            const T *src = v.e;
            for (int i = 0; i < n; i++)
                dst[i] = src[i];
        }
        len = n;
        return *this;
    }
};

// Real-number vector.
template <class T>
class RVec : public AVec<T> {
public:
    RVec(const RVec<T> &);
    RVec(int n = 0, float mult = 2.0f) : AVec<T>(n, mult) {}
};

// Integer vector with bitwise operators.
template <class T>
class IVec : public AVec<T> {
public:
    IVec(const IVec<T> &);
    IVec(int n = 0, float mult = 2.0f) : AVec<T>(n, mult) {}
    IVec<T> &operator=(const IVec<T> &);
    void zero();
};
