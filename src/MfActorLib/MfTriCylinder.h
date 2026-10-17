#pragma once
// MfTriCylinder: a triangle set of a cylinder. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"

class MFACTOR_API MfTriCylinder : public MfTriangleSet {
public:
    MfTriCylinder(const MfMaterial &, float radius, float height, int steps);
};
