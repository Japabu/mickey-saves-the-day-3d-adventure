// MfAiRuntime: the runtime: created by MfInitializeRuntime.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFAIRUNTIME_H
#define RUNTIMELIB_MFAIRUNTIME_H

#include "MfAi.h"
class MfAiAppWorld;
class MfAiKeyboard;
class MfAiMouse;
class MfAiWorld;
class MfRenderer;
class MfSoundManager;
class MfString;

class MfAiRuntime : public MfAi {
public:
    virtual int Initialize(const char *, const char *) = 0;
    virtual int Shutdown() = 0;
    virtual MfAiKeyboard *GetKeyboard() = 0;
    virtual MfAiMouse *GetMouse() = 0;
    virtual MfAiWorld *CreateWorld(MfAiAppWorld *) = 0;
    virtual MfAiWorld *GetWorld() = 0;
    virtual int InitializeJoysticks(void *, void *) = 0;
    virtual void PollJoysticks() = 0;
    virtual MfSoundManager *LoadSound(void *, void *, MfString) = 0;
    virtual MfRenderer *LoadRenderer(void *, void *, MfString) = 0;
};

#endif
