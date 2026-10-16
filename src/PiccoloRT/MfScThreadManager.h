#pragma once
#include "MfDblList.h"
#include "MfCriticalSection.h"
class MfThread;

// The script threads that run: they add themselves when they start and
// remove themselves when they end; the garbage collector suspends the others
// while it runs.
class MfScThreadManager {
public:
    MfScThreadManager();
    virtual ~MfScThreadManager();
    int Add(MfThread *thread);
    int Remove(MfThread *thread);
    // suspends every thread but the calling one (and locks the handle heaps)
    void SuspendAll();
    void ResumeAll();

    static void Startup();
    static void Shutdown();

private:
    MfDblList *mThreads;            // +0x04
    MfCriticalSection *mLock;       // +0x08
};

// the manager while the runtime runs (0x1003525c)
extern MfScThreadManager *gThreadManager;
