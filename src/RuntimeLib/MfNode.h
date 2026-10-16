// MfNode (MfActorLib.dll): a node of a bounding-sphere tree. The type tells
// what the node is (0 leaf, 1 inner sphere, 2 collision node).
#ifndef RUNTIMELIB_MFNODE_H
#define RUNTIMELIB_MFNODE_H
#include "MfVector3.h"

class __declspec(dllimport) MfNode {
public:
    MfNode(long type) { mType = type; mIndex = 0; }

    MfVector3 mCenter;                                      // 0x00
    unsigned mType : 2;                                     // 0x0c
    unsigned mIndex : 30;
    float mRadius;                                          // 0x10
};

#endif
