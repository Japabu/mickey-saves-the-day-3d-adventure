#pragma once
#include <windows.h>

// mfutil: a Win32 auto-reset event (inline members only are used here).
class __declspec(dllimport) MfEvent {
public:
    MfEvent();
    virtual ~MfEvent();
    int Signal() { return SetEvent(handle); }
    void Wait(unsigned long timeout)
    {
        waiting = 1;
        WaitForSingleObject(handle, timeout);
        waiting = 0;
    }
    int IsWaiting() { return waiting; }
private:
    HANDLE handle;      // +4
    int waiting;        // +8
};
