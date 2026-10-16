// MfAiAppWorld: what the runtime needs from the application's world.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFAIAPPWORLD_H
#define RUNTIMELIB_MFAIAPPWORLD_H

#include "MfAi.h"
class MfAiAppWorld : public MfAi {
public:
    virtual unsigned long GetClock() = 0;
};

#endif
