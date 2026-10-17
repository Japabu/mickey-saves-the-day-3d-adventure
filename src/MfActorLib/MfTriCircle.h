#pragma once
// MfTriCircle: a triangle set of a circle. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"
class MfMaterial;

class MFACTOR_API MfTriCircle : public MfTriangleSet {
public:
    MfTriCircle(const MfMaterial &, float radius, int steps);
};
