// MfProtoManipData (MfActorLib.dll): the handle of a manipulation skill
// prototype ('Prmd'), as in src/MfActorLib/MfProtoManipData.h.
#ifndef RUNTIMELIB_MFPROTOMANIPDATA_H
#define RUNTIMELIB_MFPROTOMANIPDATA_H
#include "MfStreamObject.h"
#include "MfTransform3.h"
#include "MfVector3.h"

struct __declspec(dllimport) MfProtoManipData : public MfStreamObject {
    MfProtoManipData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    MfVector3 contactForce;                                 // 0x04
    int manipType;                                          // 0x10
    MfTransform3 homeXform;                                 // 0x14
    int handleStyle;                                        // 0x30
    float radius;                                           // 0x34
    float length;                                           // 0x38
};

#endif
