// MfAiHookManager: application hooks called on runtime events.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFAIHOOKMANAGER_H
#define RUNTIMELIB_MFAIHOOKMANAGER_H

#include "MfAi.h"
#include "enums.h"
struct MfHookData;

class MfAiHookManager : public MfAi {
public:
    virtual int AddHook(unsigned long, int (__cdecl *)(MfHookData *, void *), void *, MfHookPosition) = 0;
    virtual int RemoveHook(unsigned long, int (__cdecl *)(MfHookData *, void *), void *&) = 0;
    virtual int CallHooks(unsigned long, MfHookData *) = 0;
};

#endif
