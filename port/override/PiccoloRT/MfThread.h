// PORT: the members have the types of the real class in mfutil (pointers where the 32-bit view had int), so sizes and offsets agree on 64-bit.
#pragma once

// A unit of work a MfThread runs (header only: PiccoloRT emits its vtable).
class MfThreadWork {
public:
    virtual ~MfThreadWork() {}
    virtual void Work() = 0;
};

class __declspec(dllimport) MfThread {
public:
    virtual ~MfThread();
protected:
    virtual void ThreadEntry();
public:
    MfThread(int);
    int AssignWork(MfThreadWork *);
    int PostThreadMessageA(unsigned int, unsigned int, int);
    static MfThread * __cdecl GetThread();
    static void __cdecl ReleaseTls();
    unsigned int GetState() const { return mState; }
    unsigned int GetId() const;
    void * GetHandle() const;
    void ModState(int);
    void Resume();
    void SignalShutdown(int);
    void Suspend();
private:
    static void __cdecl ThreadInit(void *);
    void SetState(int);
    static unsigned int iThreadDataIndex;
public:
    // PORT: the member types of mfutil's MfThread (src/mfutil/MfThread.h), so the
    // object is as big as the real one (64 bytes) and MfScThread::mVmEnv lands at +64
    unsigned int field_4;          // dwThreadId
    int field_8;                    // iFlags
    MfThread *field_c;              // pThread
    void *mHandle;                  // +0x10 the thread's handle
    unsigned int mState;
    void *field_18;                 // pevtWork
    void *field_1c;                 // pWork
};
