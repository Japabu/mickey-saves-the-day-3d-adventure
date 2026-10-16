// MfLock (mfutil.dll): a spin lock on an interlocked counter. The constructor
// is inline in the original header.
#ifndef RUNTIMELIB_MFLOCK_H
#define RUNTIMELIB_MFLOCK_H
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class __declspec(dllimport) MfLock {
public:
    MfLock() : mLockCount(0) { InterlockedExchange(&mLockCount, 0); }
    ~MfLock();
    MfLock &operator=(const MfLock &);
    int Lock(long timeout);
    int Test();
    void Unlock();

    long mLockCount;
};

// inline and empty: destructors of objects with an MfLock do not call it
// (~MfColNode, ~MfSpace, the static locks' exit entries), while unwind
// funclets, which cannot expand it, jump to the import
inline MfLock::~MfLock()
{
}

#endif
