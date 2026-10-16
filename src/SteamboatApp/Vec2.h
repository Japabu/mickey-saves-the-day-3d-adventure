#pragma once
#include "FixedVec.h"

// Two-component vector (screen points); no PDB declaration beyond the name.
template <class T>
class Vec2 : public FixedVec<T, 2> {
public:
    Vec2(T x, T y)
    {
        e[0] = x;
        e[1] = y;
    }
    Vec2() {}
};
