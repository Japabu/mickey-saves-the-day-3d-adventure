#pragma once
#include <windows.h>

// RuntimeLib: thin wrapper over a Win32 critical section (inline members only
// are used here; EnterCriticalSection is called directly on the object).
class __declspec(dllimport) MfCriticalSection {
public:
    MfCriticalSection() { InitializeCriticalSection(&m_cs); }
    ~MfCriticalSection() { DeleteCriticalSection(&m_cs); }
    void Enter() { EnterCriticalSection(&m_cs); }
    void Leave() { LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs; // +0
};
