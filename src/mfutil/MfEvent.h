#pragma once
// MfEvent: a Win32 auto-reset event. Exported names only; layout from the code.
// The small members are inline (mfutil emits them at the start of the
// MfEvent object and inlines them into MfThread).
#include <windows.h>

class __declspec(dllexport) MfEvent {
public:
    MfEvent();
    virtual ~MfEvent();
    int Signal() { return SetEvent(handle); }
    int Reset() { return ResetEvent(handle); }
    int Pulse() { return PulseEvent(handle); }
    void Wait(unsigned long timeout)
    {
        waiting = 1;
        WaitForSingleObject(handle, timeout);
        waiting = 0;
    }
    int IsWaiting() { return waiting; }
private:
    HANDLE handle;
    int waiting;
};
