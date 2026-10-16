#pragma once
// MfProtoAnchorData: an anchor node of a skill ('PrAD').
#include "MfStreamObject.h"

struct __declspec(dllexport) MfProtoAnchorData : public MfStreamObject {
    MfProtoAnchorData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    int anchorIndex;        // +0x04
};
