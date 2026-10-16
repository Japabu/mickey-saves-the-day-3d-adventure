#pragma once
// MfProtoGeomInfo: what a geometry node holds (MfProtoNode::GeomInfo()): the
// triangle set, a material, a texture scale and four settings (0xe8 bytes;
// see RuntimeLib/MfProtoGeomInfo.h).
#include "MfMaterial.h"
#include "MfVector2.h"
class MfTriangleSet;

struct MfProtoGeomInfo {
    MfTriangleSet *mTriangles;      // 0x00
    MfMaterial mMaterial;           // 0x04
    MfVector2 mTextureScale;        // 0xd0
    int mD8;                        // 0xd8
    int mFacesCamera;               // 0xdc
    int mE0;                        // 0xe0
    int mE4;                        // 0xe4
};
