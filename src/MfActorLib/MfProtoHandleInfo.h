#pragma once
// MfProtoHandleInfo: what a handle node holds (MfProtoNode::HandleInfo()).
struct MfProtoHandleInfo {
    int mStyle;         // 0x00
    float mRadius;      // 0x04
    float mLength;      // 0x08
    int mSymmetric;     // 0x0c
};
