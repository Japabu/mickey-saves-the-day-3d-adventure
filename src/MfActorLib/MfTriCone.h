#pragma once
// MfTriCone: a triangle set of a cone. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"

class MFACTOR_API MfTriCone : public MfTriangleSet {
public:
    MfTriCone(const MfMaterial &, float radius, float height, int steps);
};
