#pragma once
// MfCriticalSection, imported from mfutil.dll (a Win32 critical section).
#include <windows.h>

// mfutil.dll; every member is inline.
class __declspec(dllimport) MfCriticalSection {
public:
    MfCriticalSection() { InitializeCriticalSection(&m_cs); }
    ~MfCriticalSection() { DeleteCriticalSection(&m_cs); }
    void Enter() { EnterCriticalSection(&m_cs); }
    void Leave() { LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs;
};
