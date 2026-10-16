#include <windows.h>
#include <process.h>
#include "MfEvent.h"
#include "MfThread.h"

// Inline and compiler-generated members of MfThread.h:
// 0x10006070 ?Suspend@MfThread@@QAEXXZ
// 0x10006080 ?Resume@MfThread@@QAEXXZ
// 0x10006090 ?ModState@MfThread@@QAEXH@Z
// 0x100060b0 ?SetState@MfThread@@AAEXH@Z
// 0x100060d0 ??0MfThread@@QAE@ABV0@@Z
// 0x10006110 ??4MfThread@@QAEAAV0@ABV0@@Z
// 0x10006150 ??_FMfThread@@QAEXXZ
// 0x10006160 ??_EMfThread@@UAEPAXI@Z

unsigned long MfThread::iThreadDataIndex = 0xffffffff;

// 0x100101e0 ??0MfThread@@QAE@H@Z
MfThread::MfThread(int iFlags)
    : dwThreadId(0), iFlags(iFlags), pThread(this), hThread(0),
      uiState(MF_THREAD_STARTING), pevtWork(0), pWork(0)
{
    if (iThreadDataIndex == 0xffffffff)
        iThreadDataIndex = TlsAlloc();
    if (this->iFlags & MF_THREAD_CURRENT) {
        dwThreadId = GetCurrentThreadId();
        DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                        &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS);
        TlsSetValue(iThreadDataIndex, this);
        return;
    }
    hThread = (HANDLE)_beginthread(ThreadInit, 0, this);
}

// 0x10010290 ??1MfThread@@UAE@XZ
MfThread::~MfThread()
{
    if (iFlags & MF_THREAD_CURRENT)
        CloseHandle(hThread);
    if (pevtWork) {
        delete pevtWork;
        pevtWork = 0;
    }
    TlsSetValue(iThreadDataIndex, 0);
}

// 0x100102d0 ?ReleaseTls@MfThread@@SAXXZ
void MfThread::ReleaseTls()
{
    if (iThreadDataIndex != 0xffffffff) {
        TlsFree(iThreadDataIndex);
        iThreadDataIndex = 0xffffffff;
    }
}

// 0x100102f0 ?SignalShutdown@MfThread@@QAEXH@Z
void MfThread::SignalShutdown(int bWait)
{
    HANDLE h = hThread;
    ModState(MF_THREAD_SHUTDOWN);
    if (uiState & MF_THREAD_IDLE)
        AssignWork(0);
    if (bWait)
        WaitForSingleObject(h, INFINITE);
}

// 0x10010320 ?GetThread@MfThread@@SAPAV1@XZ
MfThread *MfThread::GetThread()
{
    if (iThreadDataIndex == 0xffffffff)
        return 0;
    return (MfThread *)TlsGetValue(iThreadDataIndex);
}

// 0x10010340 ?PostThreadMessageA@MfThread@@QAEHIIJ@Z
int MfThread::PostThreadMessage(unsigned int uiMsg, unsigned int wParam, long lParam)
{
    return ::PostThreadMessage(dwThreadId, uiMsg, wParam, lParam);
}

// 0x10010360 ?AssignWork@MfThread@@QAEHPAVMfThreadWork@@@Z
// Waits up to 10 s for a starting thread.
int MfThread::AssignWork(MfThreadWork *pw)
{
    if (uiState & MF_THREAD_WORKING)
        return 0;
    int iTries = 200;
    while ((uiState & MF_THREAD_STARTING) && iTries-- > 0)
        Sleep(50);
    pWork = pw;
    return pevtWork->Signal();
}

// 0x100103b0 ?ThreadEntry@MfThread@@MAEXXZ
void MfThread::ThreadEntry()
{
    while (~uiState & MF_THREAD_SHUTDOWN) {
        SetState(MF_THREAD_IDLE);
        pevtWork->Wait(INFINITE);
        if (pWork) {
            SetState(MF_THREAD_WORKING);
            pWork->DoWork();
            pWork = 0;
        }
    }
}

// 0x10010410 ?ThreadInit@MfThread@@CAXPAX@Z
void MfThread::ThreadInit(void *pv)
{
    MfThread *pt = (MfThread *)pv;
    pt->dwThreadId = GetCurrentThreadId();
    pt->pevtWork = new MfEvent;
    TlsSetValue(iThreadDataIndex, pt);
    pt->ThreadEntry();
    if (pt->iFlags & MF_THREAD_AUTODELETE)
        delete pt;
    else
        pt->uiState = MF_THREAD_ENDED;
    _endthread();
}
