// MfThread: a worker thread that runs MfThreadWork items handed to it, or a
// wrapper for the calling thread. Exported names only; layout from the code.
#ifndef MFUTIL_MFTHREAD_H
#define MFUTIL_MFTHREAD_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include <windows.h>
#include "MfThreadWork.h"
class MfEvent;

class MFUTIL_API MfThread {
public:
    // Creation flags
    enum {
        MF_THREAD_CURRENT = 1,      // wrap the calling thread
        MF_THREAD_AUTODELETE = 2    // delete the object when the thread ends
    };
    // State: the low byte is the activity, the high byte modifiers.
    enum {
        MF_THREAD_STARTING = 1,
        MF_THREAD_IDLE = 2,
        MF_THREAD_WORKING = 4,
        MF_THREAD_ENDED = 0x10,
        MF_THREAD_SHUTDOWN = 0x8000
    };

    MfThread(int iFlags = 0);
    virtual ~MfThread();
    int AssignWork(MfThreadWork *);
    int PostThreadMessage(unsigned int, unsigned int, long);
    static MfThread *GetThread();
    static void ReleaseTls();
    unsigned int GetState() const { return uiState; }
    unsigned long GetId() const { return dwThreadId; }
    void *GetHandle() const { return hThread; }
    void ModState(int iMods) { uiState |= iMods & 0xff00; }
    void Resume() { ResumeThread(hThread); }
    void SignalShutdown(int);
    void Suspend() { SuspendThread(hThread); }
protected:
    virtual void ThreadEntry();
private:
    static void ThreadInit(void *);
    void SetState(int iState) { uiState = (uiState & 0xffffff00) | iState; }
    static unsigned long iThreadDataIndex;

    unsigned long dwThreadId;
    int iFlags;
    MfThread *pThread;
    HANDLE hThread;
    unsigned int uiState;
    MfEvent *pevtWork;
    MfThreadWork *pWork;
};

#endif
