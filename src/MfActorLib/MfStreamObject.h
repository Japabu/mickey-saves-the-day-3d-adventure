#pragma once
// MfStreamObject: an object that reads and writes itself through an MfStream.
// Vtable: StreamType, ReadFromStream, WriteToStream, WriteAsObject.
#include "enums.h"
class MfStream;

class __declspec(dllexport) MfStreamObject {
    friend class MfStream;
public:
    MfStreamObject() {}
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

typedef MfStreamObject *(*MfStreamConstructor)();

// Registers the constructor of a stream object class (static instances register
// at startup); MfStream::ReadObject looks the constructors up by stream type.
class __declspec(dllexport) MfStreamConstructorEntry {
public:
    MfStreamConstructorEntry(long type, MfStreamConstructor create);
    static MfStreamConstructor Find(long type);
};
