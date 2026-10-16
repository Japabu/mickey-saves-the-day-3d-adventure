// MfProtoDB (MfActorLib.dll): the database of loaded actor prototypes.
#ifndef RUNTIMELIB_MFPROTODB_H
#define RUNTIMELIB_MFPROTODB_H
#include "MfString.h"
class MfProtoActor;

class __declspec(dllimport) MfProtoDB {
public:
    static int AddPrototype(MfProtoActor *proto);
    static MfProtoActor *GetPrototype(MfString name, int &loaded, int);
    static int PrototypeExists(MfString name);
    static int RemoveAllPrototypes();
    static int RemovePrototype(MfProtoActor *proto);
};

#endif
