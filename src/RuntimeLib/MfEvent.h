// MfEvent (mfutil.dll): a Win32 auto-reset event; declaration as in src/mfutil.
#ifndef RUNTIMELIB_MFEVENT_H
#define RUNTIMELIB_MFEVENT_H
#include <windows.h>

class __declspec(dllimport) MfEvent {
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

#endif
