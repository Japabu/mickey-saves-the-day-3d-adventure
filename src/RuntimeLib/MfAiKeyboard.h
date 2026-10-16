// MfAiKeyboard: the keyboard as the application sees it (MfRuntime::GetKeyboard).
// The SteamboatApp PDB does not describe it: the names are descriptive, the
// slots those of MfKeyboard's vtable (100b6f8c) and of MfPoLibrary's calls.
#ifndef RUNTIMELIB_MFAIKEYBOARD_H
#define RUNTIMELIB_MFAIKEYBOARD_H
#include "MfAi.h"
class MfAiActor;

class MfAiKeyboard : public MfAi {
public:
    // a key event (WM_CHAR) to the actor routed for the key
    virtual MfAiActor *HandleKey(unsigned int key, unsigned int repeat, unsigned int flags) = 0;   // 0x0c
    virtual MfAiActor *SetKeyRoute(int key, MfAiActor *actor) = 0;                                  // 0x10
    virtual MfAiActor *GetKeyRoute(int key) = 0;                                                    // 0x14
    virtual MfAiActor *SetKeyChangeRoute(int key, MfAiActor *actor) = 0;                            // 0x18
    // a key going down or up (down: 1, up: 0) to the actor routed for changes
    virtual MfAiActor *HandleKeyChange(unsigned int key, unsigned int flags, int down) = 0;         // 0x1c
    virtual MfAiActor *GetKeyChangeRoute(int key) = 0;                                              // 0x20
    virtual int GetRepeatCount() = 0;                                                               // 0x24
    virtual void SetKeyPressPollRate(int pollRate) = 0;                                             // 0x28
    virtual int GetLastKeycode() = 0;                                                               // 0x2c
    virtual int GetLastFlags() = 0;                                                                 // 0x30
    virtual void EnableAuthorToolKeys(int enable) = 0;                                              // 0x34
    virtual int ShiftKeyDown() = 0;                                                                 // 0x38
    virtual int LShiftKeyDown() = 0;                                                                // 0x3c
    virtual int RShiftKeyDown() = 0;                                                                // 0x40
    virtual int CtrlKeyDown() = 0;                                                                  // 0x44
    virtual int LCtrlKeyDown() = 0;                                                                 // 0x48
    virtual int RCtrlKeyDown() = 0;                                                                 // 0x4c
};

#endif
