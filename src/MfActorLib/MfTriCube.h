#pragma once
// MfTriCube: a triangle set of a box. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"
class MfMaterial;
class MfVector3;

class MFACTOR_API MfTriCube : public MfTriangleSet {
public:
    MfTriCube(const MfMaterial &, const MfVector3 &size);
};
