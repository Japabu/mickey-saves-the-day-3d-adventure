// MfProtoFootData (MfActorLib.dll): a foot of a locomotion skill prototype
// ('PrFD'), as in src/MfActorLib/MfProtoFootData.h.
#ifndef RUNTIMELIB_MFPROTOFOOTDATA_H
#define RUNTIMELIB_MFPROTOFOOTDATA_H
#include "MfStreamObject.h"

struct __declspec(dllimport) MfProtoFootData : public MfStreamObject {
    MfProtoFootData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    int footIndex;                                          // 0x04
    int hipIndex;                                           // 0x08
    int soleTriIndex;                                       // 0x0c
};

#endif
