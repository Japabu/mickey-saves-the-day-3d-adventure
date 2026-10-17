#pragma once
// MfAiHookManager: the AI layer's hook manager ("MfAiHookManager" in the
// interface registry). Declaration from the SteamboatApp PDB.
#include "MfAiHook.h"
#include "MfHookData.h"

class MfAiHookManager : public MfAi {
public:
    virtual int AddHook(unsigned long, int (*)(MfHookData *, void *), void *, MfHookPosition) = 0;
    virtual int RemoveHook(unsigned long, int (*)(MfHookData *, void *), void *&) = 0;
    virtual int CallHooks(unsigned long, MfHookData *) = 0;
};
