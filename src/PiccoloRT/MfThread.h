#pragma once

// A unit of work a MfThread runs (header only: PiccoloRT emits its vtable).
#include "../mfutil/MfThreadWork.h"

class __declspec(dllimport) MfThread {
public:
    virtual ~MfThread();
protected:
    virtual void ThreadEntry();
public:
    MfThread(int);
    int AssignWork(MfThreadWork *);
    int PostThreadMessageA(unsigned int, unsigned int, long);
    static MfThread * __cdecl GetThread();
    static void __cdecl ReleaseTls();
    unsigned int GetState() const { return mState; }
    unsigned long GetId() const;
    void * GetHandle() const;
    void ModState(int);
    void Resume();
    void SignalShutdown(int);
    void Suspend();
private:
    static void __cdecl ThreadInit(void *);
    void SetState(int);
    static unsigned long iThreadDataIndex;
public:
    int field_4;
    int field_8;
    int field_c;
    void *mHandle;                  // +0x10 the thread's handle
    unsigned int mState;
    int field_18;
    int field_1c;
};
