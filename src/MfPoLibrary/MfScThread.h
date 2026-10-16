// MfScThread (PiccoloRT.dll): a thread that runs a Piccolo virtual machine.
// MfPoLibrary reaches the running script's environment through it; both
// members it uses are inline.
#pragma once
#include "MfThread.h"
class MfScVmEnv;

class __declspec(dllimport) MfScThread : public MfThread {
public:
    MfScThread(int);
    virtual ~MfScThread();
    static MfScThread *__cdecl GetThread() { return (MfScThread *)MfThread::GetThread(); }
    MfScVmEnv *GetVmEnv() const { return mVmEnv; }
private:
    virtual void ThreadEntry();
    MfScVmEnv *mVmEnv;                                      // 0x20
};
