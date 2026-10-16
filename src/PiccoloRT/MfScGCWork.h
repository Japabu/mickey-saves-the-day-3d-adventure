#pragma once
#include "MfThread.h"
#include "MfEvent.h"

// The garbage collector thread's work: collects every few seconds until
// told to quit.
class MfScGCWork : public MfThreadWork {
public:
    MfScGCWork() : mQuit(0) {}
    virtual ~MfScGCWork();
    virtual void Work();
    void Quit();
private:
    long mQuit;         // +4
};

// Waits a little and counts a GC timeout.
class MfScGCTimerWork : public MfThreadWork {
public:
    virtual void Work();
    void WakeUp() { mWakeUp.Signal(); }
private:
    MfEvent mWakeUp;    // +4
};
