#pragma once
// MfTextureVertex: the texture coordinates of a vertex of an MfTriangleSet.
#include "MfActorLib_api.h"

// size 8
struct MFACTOR_API MfTextureVertex {
    void Set(float u, float v) { m_u = u; m_v = v; }
    float m_u, m_v;
};
