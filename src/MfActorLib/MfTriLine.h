#pragma once
// MfTriLine: a triangle set of a line between two points. Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"
class MfMaterial;
class MfVector3;

class MFACTOR_API MfTriLine : public MfTriangleSet {
public:
    MfTriLine(const MfMaterial &, const MfVector3 &from, const MfVector3 &to);
    void Update(const MfVector3 &from, const MfVector3 &to);
};
