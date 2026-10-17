#pragma once
// MfStreamSizedObject: a stream object that writes its size in front of itself
// (WriteAsObject: -1, StreamType(), StreamSize(), then WriteToStream).
#include "MfActorLib_api.h"
#include "MfStreamObject.h"

class MFACTOR_API MfStreamSizedObject : public MfStreamObject {
    friend class MfStream;
public:
    MfStreamSizedObject() {}
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
    virtual int StreamSize() const = 0;
};
