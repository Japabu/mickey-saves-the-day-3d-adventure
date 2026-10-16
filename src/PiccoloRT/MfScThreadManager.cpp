// PiccoloRT.dll 0x100166b0-0x10016a79: script threads and their manager.
#include <windows.h>
#include "MfScThreadManager.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfScDataHandle.h"

MfScThreadManager *gThreadManager;

// 0x100166b0 ??0MfScThread@@QAE@H@Z
MfScThread::MfScThread(int suspended)
    : MfThread(suspended)
{
    mVmEnv = new MfScVmEnv;
}

// 0x10016720 ??1MfScThread@@UAE@XZ
MfScThread::~MfScThread()
{
    delete mVmEnv;
    gThreadManager->Remove(this);
}

// 0x10016790 ??0MfScThreadManager@@QAE@XZ
MfScThreadManager::MfScThreadManager()
{
    mThreads = new MfDblList;
    mLock = new MfCriticalSection;
}

// 0x10016820 ??_GMfScThreadManager@@UAEPAXI@Z

// 0x10016840 ??1MfScThreadManager@@UAE@XZ
MfScThreadManager::~MfScThreadManager()
{
    delete mThreads;
    delete mLock;
}

// 0x10016870 ?Startup@MfScThreadManager@@SAXXZ
void MfScThreadManager::Startup()
{
    gThreadManager = new MfScThreadManager;
}

// 0x100168e0 ?Shutdown@MfScThreadManager@@SAXXZ
void MfScThreadManager::Shutdown()
{
    delete gThreadManager;
    gThreadManager = 0;
}

// 0x10016900 ?Add@MfScThreadManager@@QAEHPAVMfThread@@@Z
int MfScThreadManager::Add(MfThread *thread)
{
    mLock->Enter();
    if (mThreads->Find(thread)) {
        mLock->Leave();
        return 0;
    }
    mThreads->Add(thread);
    mLock->Leave();
    return 1;
}

// 0x10016950 ?Remove@MfScThreadManager@@QAEHPAVMfThread@@@Z
int MfScThreadManager::Remove(MfThread *thread)
{
    mLock->Enter();
    MfDblListItem *pos = mThreads->Find(thread);
    if (pos) {
        mThreads->Delete(pos);
        mLock->Leave();
        return 1;
    }
    mLock->Leave();
    return 0;
}

// 0x100169b0 ?SuspendAll@MfScThreadManager@@QAEXXZ
void MfScThreadManager::SuspendAll()
{
    MfScDataHandle::LockHeap();
    MfThread *self = MfThread::GetThread();
    mLock->Enter();
    MfDblListItem *pos;
    for (MfThread *thread = (MfThread *)mThreads->GetFirst(pos); thread;
         thread = (MfThread *)mThreads->GetNext(pos)) {
        if (thread != self)
            SuspendThread(thread->mHandle);
    }
    mLock->Leave();
    MfScDataHandle::UnlockHeap();
}

// 0x10016a20 ?ResumeAll@MfScThreadManager@@QAEXXZ
void MfScThreadManager::ResumeAll()
{
    MfThread *self = MfThread::GetThread();
    mLock->Enter();
    MfDblListItem *pos;
    for (MfThread *thread = (MfThread *)mThreads->GetFirst(pos); thread;
         thread = (MfThread *)mThreads->GetNext(pos)) {
        if (thread != self)
            ResumeThread(thread->mHandle);
    }
    mLock->Leave();
}
