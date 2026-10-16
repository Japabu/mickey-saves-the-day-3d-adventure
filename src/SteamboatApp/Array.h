#pragma once
#include "Vec.h"

// Vec with indexing (EAIStd); only what the units here use.
template <class T>
class Array : public Vec<T> {
public:
    Array(const Vec<T> &);
    Array(const Array<T> &);
    Array(const T *, int, float);
    Array(int n = 0, float mult = 2.0f) : Vec<T>(n, mult) {}
    Array<T> &operator=(const Vec<T> &);
    Array<T> &operator=(const Array<T> &);
    T &operator[](unsigned int i) { return e[i]; }
    T &operator[](int i) { return e[i]; }
    const T &operator()(unsigned int i) const { return e[i]; }
    const T &operator()(int i) const { return e[i]; }
};
