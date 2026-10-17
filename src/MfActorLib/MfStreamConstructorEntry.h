#pragma once
// MfStreamConstructorEntry: the registry of the stream object constructors.
#include "MfActorLib_api.h"
#include "MfStreamObject.h"

typedef MfStreamObject *(*MfStreamConstructor)();

// Registers the constructor of a stream object class (static instances register
// at startup); MfStream::ReadObject looks the constructors up by stream type.
class MFACTOR_API MfStreamConstructorEntry {
public:
    MfStreamConstructorEntry(long type, MfStreamConstructor create);
    static MfStreamConstructor Find(long type);
};
