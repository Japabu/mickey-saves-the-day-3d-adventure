#pragma once
#include "MfThread.h"

// The garbage collector's thread: registered with the thread manager like
// the script threads, but without a script environment.
class MfScGcThread : public MfThread {
public:
    MfScGcThread(int suspended) : MfThread(suspended) {}
    virtual ~MfScGcThread();
private:
    virtual void ThreadEntry();
};
