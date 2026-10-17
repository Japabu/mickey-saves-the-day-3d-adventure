#pragma once
// MfNode: a node of an MfColModel's bounding-sphere tree (MfLeafNode per
// triangle, MfSphereNode for the inner spheres).
#include "MfActorLib_api.h"
#include "MfVector3.h"

enum { MF_LEAF_NODE = 0, MF_SPHERE_NODE = 1 };

// size 0x14; bounding-sphere tree node
class MFACTOR_API MfNode {
public:
    MfNode(long type) { m_type = type; m_index = 0; }
    ~MfNode() {}
    MfVector3 m_center;         // 0x00
    unsigned m_type : 2;        // 0x0c bits 0-1 (MF_LEAF_NODE, MF_SPHERE_NODE)
    unsigned m_index : 30;      // 0x0c bits 2-31 (leaf: triangle index)
    float m_radius;             // 0x10
};
