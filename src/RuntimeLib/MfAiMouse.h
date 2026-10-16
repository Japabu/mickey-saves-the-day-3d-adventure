// MfAiMouse: the mouse as the application sees it (MfRuntime::GetMouse). The
// SteamboatApp PDB does not describe it: the names are descriptive, the slots
// those of MfMouse's vtable (100b7044) and of MfPoLibrary's calls.
#ifndef RUNTIMELIB_MFAIMOUSE_H
#define RUNTIMELIB_MFAIMOUSE_H
#include "MfAi.h"
class MfAiActor;
class MfAiActorNode;

class MfAiMouse : public MfAi {
public:
    // event 0-4 (MfMouse::LBUTTON_DOWN ...), 5: every event
    virtual int SetMouseRoute(int button, MfAiActor *actor) = 0;          // 0x0c
    virtual int ClearMouseRoute(int button, MfAiActor *actor) = 0;        // 0x10
    virtual MfAiActor *GetLastPickedActor() = 0;                          // 0x14
    virtual MfAiActorNode *GetLastPickedFloor() = 0;                      // 0x18
};

#endif
