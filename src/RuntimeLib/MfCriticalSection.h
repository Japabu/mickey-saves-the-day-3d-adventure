// MfCriticalSection (mfutil.dll): a critical section initialized and deleted
// with its owner. As in src/mfutil (members inline, the class exported).
#ifndef RUNTIMELIB_MFCRITICALSECTION_H
#define RUNTIMELIB_MFCRITICALSECTION_H
#include <windows.h>

class __declspec(dllimport) MfCriticalSection : public _RTL_CRITICAL_SECTION {
public:
    MfCriticalSection() { InitializeCriticalSection(this); }
    ~MfCriticalSection() { DeleteCriticalSection(this); }
};

#endif
