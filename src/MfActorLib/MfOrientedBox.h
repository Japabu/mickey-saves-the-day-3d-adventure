#pragma once
// MfOrientedBox: an oriented bounding box (a transform and half sizes), the box
// of an MfColModel.
#include "MfActorLib_api.h"
#include "MfTransform3.h"
#include "MfVector3.h"

// size 0x28; the copy members are compiler-generated
class MFACTOR_API MfOrientedBox {
public:
    bool CheckIntersection(MfOrientedBox *, MfTransform3 &, MfTransform3 &);
    bool CheckRayIntersection(MfTransform3 &, MfVector3 &, MfVector3 &, float);
    MfTransform3 m_transform;   // 0x00
    MfVector3 m_extent;         // 0x1c half sizes along the box axes
};
