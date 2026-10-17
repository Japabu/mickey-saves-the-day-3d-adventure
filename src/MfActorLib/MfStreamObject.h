#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfStreamObject: an object that reads and writes itself through an MfStream.
// Vtable: StreamType, ReadFromStream, WriteToStream, WriteAsObject.
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

typedef MfStreamObject *(*MfStreamConstructor)();

// Registers the constructor of a stream object class (static instances register
// at startup); MfStream::ReadObject looks the constructors up by stream type.
class MFACTOR_API MfStreamConstructorEntry {
public:
    MfStreamConstructorEntry(long type, MfStreamConstructor create);
    static MfStreamConstructor Find(long type);
};
