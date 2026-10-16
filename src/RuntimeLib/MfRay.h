// MfRay: a ray (origin and direction); the PDB knows the name only
// (MfFloor::CreateFloor takes one). Cast is defined with MfProximityManager.
#ifndef RUNTIMELIB_MFRAY_H
#define RUNTIMELIB_MFRAY_H
#include "MfVector3.h"
class MfSkeletonNode;
class MfSpace;
struct MfColResultRay;

class MfRay {
public:
    // the first hit of the ray in the space's environment (without node, or,
    // with partial set, only near it)
    int Cast(MfSpace *space, MfSkeletonNode *node, MfColResultRay &result, int partial);

    MfVector3 mOrigin;                                      // 0x00
    MfVector3 mDirection;                                   // 0x0c
};

#endif
