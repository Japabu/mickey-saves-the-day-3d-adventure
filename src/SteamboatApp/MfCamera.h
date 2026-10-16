#pragma once
// MfCamera.h: the camera value of a Motivate camera node (declaration from SteamboatApp.pdb; the
// game only reads it, through MfAiActorNode::GetCamera).
#include "MfTypedValue.h"
#include "MfString.h"

enum MfCameraProjection {
    MF_CAMERA_PERSPECTIVE = 0,
    MF_CAMERA_ORTHOGRAPHIC = 1
};

class MfCamera : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    MfCamera(const MfCamera &);
    MfCamera();
    virtual ~MfCamera();
    MfCamera &operator=(const MfCamera &);
    MfString name;                                      // +0x000c size 0x4
    MfCameraProjection proj;                            // +0x0010 size 0x4
    float fov;                                          // +0x0014 size 0x4
    float nearClip;                                     // +0x0018 size 0x4
    float farClip;                                      // +0x001c size 0x4
    // MfPropertyList properties;                       // +0x0020 size 0x14 (not used by the game)
};
