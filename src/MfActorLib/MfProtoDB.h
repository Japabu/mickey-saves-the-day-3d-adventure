#pragma once
// MfProtoDB: the loaded proto actors, by upper-case name. Exported names only.
#include "MfString.h"
#include "MfHashTable.h"
#include "MfFile.h"
class MfProtoActor;

class __declspec(dllexport) MfProtoDB {
public:
    static MfProtoActor *GetPrototype(MfString fileName, int &bLoaded, int bLoadSkills);
    static int PrototypeExists(MfString name);
    static int AddPrototype(MfProtoActor *proto);
    static int RemovePrototype(MfProtoActor *proto);
    static int RemoveAllPrototypes();
    static MfString UniqueName(MfString name);
protected:
    static MfDictionary protoTable;
    static MfCriticalSection csectDBLock;
};
