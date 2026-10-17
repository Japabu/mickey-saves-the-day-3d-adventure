// MfAiKeyboard: the keyboard as the application sees it (RuntimeLib implements
// it; MfRuntime::GetKeyboard). The SteamboatApp PDB does not describe it: the
// names are descriptive, the slots are those MfPoKeyboard calls.
#pragma once
#include "MfAi.h"
class MfAiActor;

class MfAiKeyboard : public MfAi {
public:
    virtual void Slot0c() = 0;                                            // 0x0c
    virtual MfAiActor *SetKeyRoute(int keyCode, MfAiActor *actor) = 0;    // 0x10
    virtual void Slot14() = 0;                                            // 0x14
    virtual MfAiActor *SetKeyChangeRoute(int keyCode, MfAiActor *actor) = 0;  // 0x18
    virtual void Slot1c() = 0;                                            // 0x1c
    virtual void Slot20() = 0;                                            // 0x20
    virtual int GetRepeatCount() = 0;                                     // 0x24
    virtual void SetKeyPressPollRate(int pollRate) = 0;                   // 0x28
    virtual int GetLastKeycode() = 0;                                     // 0x2c
    virtual void Slot30() = 0;                                            // 0x30
    virtual void EnableAuthorToolKeys(int enable) = 0;                    // 0x34
    virtual int ShiftKeyDown() = 0;                                       // 0x38
    virtual int LShiftKeyDown() = 0;                                      // 0x3c
    virtual int RShiftKeyDown() = 0;                                      // 0x40
    virtual int CtrlKeyDown() = 0;                                        // 0x44
    virtual int LCtrlKeyDown() = 0;                                       // 0x48
    virtual int RCtrlKeyDown() = 0;                                       // 0x4c
};
