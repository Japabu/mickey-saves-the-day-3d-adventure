#pragma once
// MfSphereNode: an inner sphere of an MfColModel's bounding-sphere tree.
#include "MfActorLib_api.h"
#include "MfNode.h"

// size 0x1c
class MFACTOR_API MfSphereNode : public MfNode {
public:
    MfSphereNode() : MfNode(MF_SPHERE_NODE) { m_left = 0; m_right = 0; }
    MfNode *m_left;             // 0x14
    MfNode *m_right;            // 0x18
};
