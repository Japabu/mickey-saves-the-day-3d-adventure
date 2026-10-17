// MfUserHookData: the data of a user hook (MfPoLibrary sends one for
// Actor.DisplayText). Struct from the SteamboatApp PDB and RuntimeLib.
#ifndef MFPOLIBRARY_MFUSERHOOKDATA_H
#define MFPOLIBRARY_MFUSERHOOKDATA_H
#include "MfHookData.h"
#include "MfString.h"

struct MfUserHookData : public MfHookData {
    MfString event;                                         // 0x10
    MfString data;                                          // 0x14
    void *userData;                                         // 0x18
};

#endif
