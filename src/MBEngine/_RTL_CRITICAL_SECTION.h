// _RTL_CRITICAL_SECTION.h - Windows' CRITICAL_SECTION (winnt.h).
#ifndef MBENGINE_RTL_CRITICAL_SECTION_H
#define MBENGINE_RTL_CRITICAL_SECTION_H

struct _RTL_CRITICAL_SECTION {
    void *DebugInfo;
    long LockCount;
    long RecursionCount;
    void *OwningThread;
    void *LockSemaphore;
    unsigned long SpinCount;
};

#endif
