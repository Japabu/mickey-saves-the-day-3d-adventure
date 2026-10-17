#pragma once
// MfLeafNode: a leaf of an MfColModel's bounding-sphere tree (one triangle).
#include "MfActorLib_api.h"
#include "MfNode.h"

class MFACTOR_API MfLeafNode : public MfNode {
public:
    MfLeafNode() : MfNode(MF_LEAF_NODE) {}
};
