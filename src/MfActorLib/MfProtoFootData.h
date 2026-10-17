#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoFootData: a foot of a locomotion skill ('PrFD').
#include "MfStreamObject.h"

struct MFACTOR_API MfProtoFootData : public MfStreamObject {
    MfProtoFootData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int footIndex;          // +0x04
    int hipIndex;           // +0x08
    int soleTriIndex;       // +0x0c
};
