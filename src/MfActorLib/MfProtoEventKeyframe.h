#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoEventKeyframe: an event a skill posts at a time ('PrEK').
#include "MfStreamObject.h"
#include "MfString.h"

struct MFACTOR_API MfProtoEventKeyframe : public MfStreamObject {
    MfProtoEventKeyframe();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int time;               // +0x04
    MfString eventName;     // +0x08
    int useBroadcast;       // +0x0c
};
