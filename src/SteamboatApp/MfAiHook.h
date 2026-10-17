#pragma once
// MfAiHook.h: the Motivate hook manager (MfActorLib.dll), found by name through
// MfInterfaceRegistry (mfutil.dll); the game installs its hooks with it (declarations from
// SteamboatApp.pdb, hook ids as in MfActorLib's MfAiHook.h).
#include "MfAi.h"
#include "MfString.h"

struct MfHookData;

typedef int (*MfHookFunc)(MfHookData *, void *);

enum MfHookPosition {
    MF_HOOK_FIRST = 0,
    MF_HOOK_LAST = 1
};

// the numbered hook ids the game uses (the others are four-character codes such as 'WLSp')
enum {
    MF_HOOK_USER_EVENT = 18,
    MF_HOOK_FILE_DESTINATION = 32
};

// own copy: MfActorLib's MfAiHook.h is itself a view (it declares a dllimport MfAi and
// MfInterfaceRegistry of its own and opens MfActorLib's MfString.h).
class MfAiHookManager : public MfAi {
public:
    virtual int AddHook(unsigned long, MfHookFunc, void *, MfHookPosition) = 0;
    virtual int RemoveHook(unsigned long, MfHookFunc, void *&) = 0;
    virtual int CallHooks(unsigned long, MfHookData *) = 0;
};

// own copy: mfutil's MfInterfaceRegistry.h declares the class MFUTIL_API (dllimport here;
// the game calls GetInterface through its import thunk) and opens mfutil's MfString.h
class MfInterfaceRegistry {
public:
    static int __cdecl GetInterface(const MfString &, MfAi **);
};
