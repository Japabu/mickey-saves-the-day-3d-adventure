// MfCriticalSection. Declaration from the SteamboatApp PDB; the members are
// inline (the logger expands them), mfutil exports its copies.
#ifndef MFUTIL_MFCRITICALSECTION_H
#define MFUTIL_MFCRITICALSECTION_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include <windows.h>

class MFUTIL_API MfCriticalSection : public _RTL_CRITICAL_SECTION {
public:
    MfCriticalSection() { InitializeCriticalSection(this); }
    ~MfCriticalSection() { DeleteCriticalSection(this); }
    void Enter() { EnterCriticalSection(this); }
    void Leave() { LeaveCriticalSection(this); }
};

#endif
