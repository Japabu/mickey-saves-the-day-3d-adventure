// MfActorHookData: the answer to an actor request (loaded, created,
// destroyed ...) as the runtime hands it to the hooks (declared in the
// SteamboatApp PDB).
#ifndef RUNTIMELIB_MFACTORHOOKDATA_H
#define RUNTIMELIB_MFACTORHOOKDATA_H
#include "MfHookData.h"
#include "MfString.h"
#include "enums.h"
class MfAiActor;
class MfAiSpace;

struct MfActorHookData : public MfHookData {
    int rid;                                                // 0x10
    int hid;                                                // 0x14
    MfRequestStatus status;                                 // 0x18
    MfAiActor *pActor;                                      // 0x1c
    MfAiSpace *pSpace;                                      // 0x20
    MfString name;                                          // 0x24
};

#endif
