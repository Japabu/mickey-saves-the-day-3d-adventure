// MfSphereNode (MfActorLib.dll): an inner node of the sphere tree. Kept out
// of MfNode.h: declaring it there flips MfLocoSkill::CycleLength's imul operands.
#ifndef RUNTIMELIB_MFSPHERENODE_H
#define RUNTIMELIB_MFSPHERENODE_H
#include "MfNode.h"

class __declspec(dllimport) MfSphereNode : public MfNode {
public:
    MfSphereNode();
    ~MfSphereNode();

    MfNode *mLeft;                                          // 0x14
    MfNode *mRight;                                         // 0x18
};

#endif
