// MfSpaceHookData: the answer to a space request (loaded, destroyed ...) as
// the runtime hands it to the hooks (declared in the SteamboatApp PDB).
#ifndef RUNTIMELIB_MFSPACEHOOKDATA_H
#define RUNTIMELIB_MFSPACEHOOKDATA_H
#include "MfHookData.h"
#include "MfString.h"
#include "enums.h"
class MfAiSpace;

struct MfSpaceHookData : public MfHookData {
    int rid;                                                // 0x10
    int hid;                                                // 0x14
    MfRequestStatus status;                                 // 0x18
    MfAiSpace *pSpace;                                      // 0x1c
    MfString name;                                          // 0x20
};

#endif
