#pragma once
// MfColSphere: a collision sphere of a proto node (MfProtoNode::ColSpheres()).
#include "MfVector3.h"

struct MfColSphere {
    MfColSphere() { data = 0; }
    MfVector3 center;   // +0x00
    float radius;       // +0x0c
    int data;           // +0x10
};
