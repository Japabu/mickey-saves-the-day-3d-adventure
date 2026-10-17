// MfLock: a spin lock on one word. Declaration from the SteamboatApp PDB.
#ifndef MFUTIL_MFLOCK_H
#define MFUTIL_MFLOCK_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif

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
