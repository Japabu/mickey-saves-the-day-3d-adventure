// MfThread (mfutil.dll) and MfScThread (PiccoloRT.dll): the thread a script
// runs on; an MfScThread carries the script environment.
#ifndef RUNTIMELIB_MFSCTHREAD_H
#define RUNTIMELIB_MFSCTHREAD_H
class MfScVmEnv;
class MfThreadWork;

class __declspec(dllimport) MfThread {
public:
    MfThread(int flags);
    virtual ~MfThread();
    int AssignWork(MfThreadWork *work);
    // the object of the calling thread
    static MfThread *GetThread();

private:
    unsigned long mThreadId;                                                    // 0x04
    int mFlags;                                                                 // 0x08
    MfThread *mThread;                                                          // 0x0c
    void *mHandle;                                                              // 0x10
    unsigned int mState;                                                        // 0x14
    void *mWorkEvent;                                                           // 0x18
    MfThreadWork *mWork;                                                        // 0x1c
};

class __declspec(dllimport) MfScThread : public MfThread {
public:
    MfScThread(int flags);
    virtual ~MfScThread();

    MfScVmEnv *mVmEnv;                                                          // 0x20
};

#endif
