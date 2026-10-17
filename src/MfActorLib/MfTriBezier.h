#pragma once
// MfTriBezier: a triangle set of a Bezier curve. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"

class MFACTOR_API MfTriBezier : public MfTriangleSet {
public:
    MfTriBezier(const MfMaterial &, MfVector3 *const points, int steps);
    void Update(MfVector3 *const points);
};
