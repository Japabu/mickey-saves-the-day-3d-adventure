// CMBCriticalSection: a Win32 critical section (muffin Runtime/Win). Header-only:
// every binary that uses it compiles its own inline copy.
#ifndef CMBCRITICALSECTION_H
#define CMBCRITICALSECTION_H
#include <windows.h>

class CMBCriticalSection {
public:
    CMBCriticalSection(void) { InitializeCriticalSection(&mSection); }
    ~CMBCriticalSection(void) { DeleteCriticalSection(&mSection); }
    void Enter(void) { EnterCriticalSection(&mSection); }
    void Leave(void) { LeaveCriticalSection(&mSection); }
private:
    CRITICAL_SECTION mSection;
};

#endif
