// MfCriticalSection. Declaration from the SteamboatApp PDB; the members are
// inline (the logger expands them), mfutil exports its copies.
#ifndef MFUTIL_MFCRITICALSECTION_H
#define MFUTIL_MFCRITICALSECTION_H

#include "mfutil_api.h"
#include <windows.h>

class MFUTIL_API MfCriticalSection : public _RTL_CRITICAL_SECTION {
public:
    MfCriticalSection() { InitializeCriticalSection(this); }
    ~MfCriticalSection() { DeleteCriticalSection(this); }
    void Enter() { EnterCriticalSection(this); }
    void Leave() { LeaveCriticalSection(this); }
};

#endif
