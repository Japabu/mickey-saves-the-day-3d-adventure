#pragma once
// MfLock: a spin lock on one word. Declaration from the SteamboatApp PDB.

class __declspec(dllexport) MfLock {
public:
    MfLock();
    ~MfLock();
    int Lock(long);
    void Unlock();
    int Test();
private:
    long lwLockWord;
};
