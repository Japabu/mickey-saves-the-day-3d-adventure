#pragma once
// MfProtoManipData: the handle of a manipulation skill ('Prmd').
#include "MfStreamObject.h"
#include "MfVector3.h"
#include "MfTransform3.h"

struct __declspec(dllexport) MfProtoManipData : public MfStreamObject {
    MfProtoManipData();
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    MfVector3 contactForce;     // +0x04
    int manipType;              // +0x10
    MfTransform3 homeXform;     // +0x14
    int handleStyle;            // +0x30
    float radius;               // +0x34
    float length;               // +0x38
};
