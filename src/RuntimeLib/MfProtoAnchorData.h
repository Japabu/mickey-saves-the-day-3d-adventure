// MfProtoAnchorData (MfActorLib.dll): an anchor node of a skill prototype, as in
// src/MfActorLib/MfProtoAnchorData.h.
#ifndef RUNTIMELIB_MFPROTOANCHORDATA_H
#define RUNTIMELIB_MFPROTOANCHORDATA_H
#include "MfStreamObject.h"

struct __declspec(dllimport) MfProtoAnchorData : public MfStreamObject {
    MfProtoAnchorData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    int anchorIndex;                                         // 0x04
};

#endif
