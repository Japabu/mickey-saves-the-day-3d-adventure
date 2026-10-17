// MfScThread: a thread that runs scripts.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfThread.h"
class MfScVmEnv;

// A thread that runs scripts: it owns the MfScVmEnv of the thread.
class PICCOLORT_API MfScThread : public MfThread {
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
