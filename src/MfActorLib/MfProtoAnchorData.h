#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoAnchorData: an anchor node of a skill ('PrAD').
#include "MfStreamObject.h"

struct MFACTOR_API MfProtoAnchorData : public MfStreamObject {
    MfProtoAnchorData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int anchorIndex;        // +0x04
};
