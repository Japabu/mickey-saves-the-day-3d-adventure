// A collision sphere of a prototype node (the items of MfProtoNode::ColSpheres).
#ifndef RUNTIMELIB_MFCOLSPHERE_H
#define RUNTIMELIB_MFCOLSPHERE_H
#include "MfVector3.h"

struct MfColSphere {
    MfVector3 mCenter;                                      // 0x00 in the node's frame
    float mRadius;                                          // 0x0c
};

#endif
