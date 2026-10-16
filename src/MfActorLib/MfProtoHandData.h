#pragma once
// MfProtoHandData: a hand of a skill ('PrHD').
#include "MfStreamObject.h"

struct __declspec(dllexport) MfProtoHandData : public MfStreamObject {
    MfProtoHandData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int handIndex;          // +0x04
};
