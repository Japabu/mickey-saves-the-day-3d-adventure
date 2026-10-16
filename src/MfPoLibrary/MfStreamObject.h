// MfStreamObject (MfActorLib.dll): an object that reads and writes itself
// through an MfStream. Vtable: StreamType, ReadFromStream, WriteToStream,
// WriteAsObject.
#pragma once
#include "enums.h"
class MfStream;

class __declspec(dllimport) MfStreamObject {
    friend class MfStream;
public:
    MfStreamObject() {}
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};
