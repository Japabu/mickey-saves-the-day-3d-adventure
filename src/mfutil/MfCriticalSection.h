#pragma once
// MfCriticalSection. Declaration from the SteamboatApp PDB; the members are
// inline (the logger expands them), mfutil exports its copies.
#include <windows.h>

class __declspec(dllexport) MfCriticalSection : public _RTL_CRITICAL_SECTION {
public:
    MfCriticalSection() { InitializeCriticalSection(this); }
    ~MfCriticalSection() { DeleteCriticalSection(this); }
    void Enter() { EnterCriticalSection(this); }
    void Leave() { LeaveCriticalSection(this); }
};
