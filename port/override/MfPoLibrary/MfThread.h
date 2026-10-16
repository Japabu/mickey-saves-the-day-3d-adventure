// PORT: the members have the types of the real class in mfutil (pointers where this view had int), so the inline MfScThread::GetVmEnv() reads mVmEnv at the real offset on 64-bit.
// MfThread (mfutil.dll): a worker thread.
#pragma once

class __declspec(dllimport) MfThread {
public:
    MfThread(int);
    virtual ~MfThread();
    static MfThread *__cdecl GetThread();
protected:
    virtual void ThreadEntry();
private:
    // PORT: the member types of mfutil's MfThread (src/mfutil/MfThread.h), so
    // MfScThread::mVmEnv is at the real offset (+64 on 64-bit)
    unsigned int field_4;                                  // dwThreadId
    int field_8;                                            // iFlags
    MfThread *field_c;                                      // pThread
    void *field_10;                                         // hThread
    unsigned int mState;                                    // 0x14
    void *field_18;                                         // pevtWork
    void *field_1c;                                         // pWork
};
