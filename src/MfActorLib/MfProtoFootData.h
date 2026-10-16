#pragma once
// MfProtoFootData: a foot of a locomotion skill ('PrFD').
#include "MfStreamObject.h"

struct __declspec(dllexport) MfProtoFootData : public MfStreamObject {
    MfProtoFootData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int footIndex;          // +0x04
    int hipIndex;           // +0x08
    int soleTriIndex;       // +0x0c
};
