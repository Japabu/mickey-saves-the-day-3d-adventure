#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfScGcThread.h"
#include "MfScThreadManager.h"

// 0x10008530 ?GetThreadVmEnv@MfScThread@@SAPAVMfScVmEnv@@XZ
MfScVmEnv * __cdecl MfScThread::GetThreadVmEnv()
{
    return ((MfScThread *)MfThread::GetThread())->mVmEnv;
}

// 0x1000b820 ??1MfScGcThread@@UAE@XZ
MfScGcThread::~MfScGcThread()
{
    gThreadManager->Remove(this);
}

// 0x1000b880 ?ThreadEntry@MfScThread@@EAEXXZ
void MfScThread::ThreadEntry()
{
    gThreadManager->Add(this);
    MfThread::ThreadEntry();
}

// 0x1000b880 ?ThreadEntry@MfScGcThread@@EAEXXZ (identical, folded)
void MfScGcThread::ThreadEntry()
{
    gThreadManager->Add(this);
    MfThread::ThreadEntry();
}
