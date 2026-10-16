// MfProtoGeomInfo (MfActorLib.dll): what a geometry prototype node holds
// (MfProtoNode::GeomInfo()). Layout from MfProtoNode::SetStyle (an 0xe8-byte
// block: the triangle set, a material, a texture scale and four settings) and
// MfProtoNode::ReadFromStream, which streams the members in this order.
#ifndef RUNTIMELIB_MFPROTOGEOMINFO_H
#define RUNTIMELIB_MFPROTOGEOMINFO_H
#include "MfMaterial.h"
#include "MfVector2.h"
class MfTriangleSet;

struct MfProtoGeomInfo {
    MfTriangleSet *mTriangles;                              // 0x00
    MfMaterial mMaterial;                                   // 0x04
    MfVector2 mTextureScale;                                // 0xd0 (1, 1)
    int mD8;                                                // 0xd8
    int mFacesCamera;                                       // 0xdc 1: the node is kept turned to the camera
    int mE0;                                                // 0xe0 (32)
    int mE4;                                                // 0xe4 (32)
};

#endif
