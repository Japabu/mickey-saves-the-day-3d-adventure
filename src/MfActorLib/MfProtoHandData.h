#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoHandData: a hand of a skill ('PrHD').
#include "MfStreamObject.h"

struct MFACTOR_API MfProtoHandData : public MfStreamObject {
    MfProtoHandData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int handIndex;          // +0x04
};
