#pragma once
// MfTriRing: a triangle set of a ring. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"

class MFACTOR_API MfTriRing : public MfTriangleSet {
public:
    MfTriRing(const MfMaterial &, float inner, float outer, float width, int steps);
};
