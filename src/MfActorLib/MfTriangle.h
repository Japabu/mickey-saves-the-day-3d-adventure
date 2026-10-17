#pragma once
// MfTriangle: the three vertex indices of a triangle of an MfTriangleSet.
#include "MfActorLib_api.h"

// size 0xc
struct MFACTOR_API MfTriangle {
    void Set(int a, int b, int c) { m_v[0] = a; m_v[1] = b; m_v[2] = c; }
    int m_v[3];
};
