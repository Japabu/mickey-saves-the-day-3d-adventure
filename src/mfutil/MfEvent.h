// MfEvent: a Win32 auto-reset event. Exported names only; layout from the code.
// The small members are inline (mfutil emits them at the start of the
// MfEvent object and inlines them into MfThread).
#ifndef MFUTIL_MFEVENT_H
#define MFUTIL_MFEVENT_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include <windows.h>

class MFUTIL_API MfEvent {
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
