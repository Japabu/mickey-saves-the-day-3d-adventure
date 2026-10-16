// MfProtoHandData (MfActorLib.dll): a hand of a skill prototype, as in
// src/MfActorLib/MfProtoHandData.h.
#ifndef RUNTIMELIB_MFPROTOHANDDATA_H
#define RUNTIMELIB_MFPROTOHANDDATA_H
#include "MfStreamObject.h"

struct __declspec(dllimport) MfProtoHandData : public MfStreamObject {
    MfProtoHandData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    int handIndex;                                         // 0x04
};

#endif
