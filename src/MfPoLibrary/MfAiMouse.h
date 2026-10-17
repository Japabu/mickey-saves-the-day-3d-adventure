// MfAiMouse: the mouse as the application sees it (RuntimeLib implements it;
// MfRuntime::GetMouse). The SteamboatApp PDB does not describe it: the names
// are descriptive, the slots are those MfPoMouse calls.
#pragma once
#include "MfAi.h"
class MfAiActor;
class MfAiActorNode;

class MfAiMouse : public MfAi {
public:
    virtual int SetMouseRoute(int mouseCode, MfAiActor *actor) = 0;       // 0x0c
    virtual int ClearMouseRoute(int mouseCode, MfAiActor *actor) = 0;     // 0x10
    virtual MfAiActor *GetLastPickedActor() = 0;                          // 0x14
    virtual MfAiActorNode *GetLastPickedFloor() = 0;                      // 0x18
};
