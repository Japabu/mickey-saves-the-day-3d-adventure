// MfLock: a spin lock on one word. Declaration from the SteamboatApp PDB.
#ifndef MFUTIL_MFLOCK_H
#define MFUTIL_MFLOCK_H

#include "mfutil_api.h"

class MFUTIL_API MfLock {
public:
    MfLock();
    ~MfLock();
    int Lock(long);
    void Unlock();
    int Test();
private:
    long lwLockWord;
};

#endif
