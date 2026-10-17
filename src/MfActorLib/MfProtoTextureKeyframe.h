#pragma once
// MfProtoTextureKeyframe: a skill's material change at a time ('PrTK').
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfMaterial.h"

struct MFACTOR_API MfProtoTextureKeyframe : public MfStreamObject {
    MfProtoTextureKeyframe();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int time;               // +0x04
    int nodeIndex;          // +0x08
    int matIndex;           // +0x0c
    MfMaterial material;    // +0x10
};
