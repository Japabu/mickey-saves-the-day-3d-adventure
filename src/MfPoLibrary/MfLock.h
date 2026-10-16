// MfLock (mfutil.dll): a spin lock on an interlocked counter. The constructor
// is inline in the original header.
#ifndef MFPOLIBRARY_MFLOCK_H
#define MFPOLIBRARY_MFLOCK_H
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class __declspec(dllimport) MfLock {
public:
    MfLock() : mLockCount(0) { InterlockedExchange(&mLockCount, 0); }
    ~MfLock();
    int Lock(long timeout);
    int Test();
    void Unlock();

    long mLockCount;
};

#endif
