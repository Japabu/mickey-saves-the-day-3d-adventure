#pragma once
// MfProtoSoundKeyframe: a sound a skill plays at a time ('PrSK').
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfString.h"

struct MFACTOR_API MfProtoSoundKeyframe : public MfStreamObject {
    MfProtoSoundKeyframe();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int time;               // +0x04
    MfString filename;      // +0x08
    int volume;             // +0x0c
    int mute;               // +0x10
    int ambient;            // +0x14
    int stream;             // +0x18
};
