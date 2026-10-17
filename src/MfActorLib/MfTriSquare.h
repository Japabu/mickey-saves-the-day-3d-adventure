#pragma once
// MfTriSquare: a triangle set of a square (one of the simple shapes of
// MfProtoNode::CreateColModel). Exported names only.
#include "MfActorLib_api.h"
#include "MfTriangleSet.h"

class MFACTOR_API MfTriSquare : public MfTriangleSet {
public:
    MfTriSquare(const MfMaterial &, float width, float height);
};
