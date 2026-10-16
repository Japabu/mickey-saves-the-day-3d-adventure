#pragma once
#include "MfThread.h"
class MfScVmEnv;

// A thread that runs scripts: it owns the MfScVmEnv of the thread.
class __declspec(dllexport) MfScThread : public MfThread {
public:
    MfScThread(int suspended = 0);
    virtual ~MfScThread();
private:
    virtual void ThreadEntry();
public:
    MfScVmEnv * GetVmEnv() const;
    static MfScThread * __cdecl GetThread();
    static MfScVmEnv * __cdecl GetThreadVmEnv();
    MfScVmEnv * mVmEnv;
};
