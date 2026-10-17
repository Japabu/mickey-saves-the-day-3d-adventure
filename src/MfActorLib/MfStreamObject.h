#pragma once
// MfStreamObject: an object that reads and writes itself through an MfStream.
// Vtable: StreamType, ReadFromStream, WriteToStream, WriteAsObject.
#include "MfActorLib_api.h"
#include "enums.h"
class MfStream;

class MFACTOR_API MfStreamObject {
    friend class MfStream;
public:
    MfStreamObject() {}
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};
