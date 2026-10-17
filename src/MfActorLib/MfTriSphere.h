#pragma once
// MfTriSphere: a triangle set of a sphere. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"
class MfMaterial;

class MFACTOR_API MfTriSphere : public MfTriangleSet {
public:
    MfTriSphere(const MfMaterial &, float radius, int steps);
};
