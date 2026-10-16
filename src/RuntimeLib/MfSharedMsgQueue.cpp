// MfSharedMsgQueue: a message queue shared between threads (RuntimeLib.dll
// 10069380-10069660).
#include "MfSharedMsgQueue.h"
#include "MfEvent.h"
#include "MfMsg.h"

// 0x10069380 ??0MfSharedMsgQueue@@QAE@HPBD@Z
MfSharedMsgQueue::MfSharedMsgQueue(int limit, const char *name)
    : MfMsgQueue(limit), mMaxCount(limit), mSharedCount(0), mName(name)
{
    mNotFull = new MfEvent;
    mNotEmpty = new MfEvent;
    mNotEmpty->Reset();
    mNotFull->Signal();
    InitializeCriticalSection((CRITICAL_SECTION *)mLock);
}

// 0x10069470 ??_GMfEvent@@UAEPAXI@Z

// 0x10069490 ??1MfSharedMsgQueue@@QAE@XZ
MfSharedMsgQueue::~MfSharedMsgQueue()
{
    if (mNotFull)
        delete mNotFull;
    if (mNotEmpty)
        delete mNotEmpty;
    DeleteCriticalSection((CRITICAL_SECTION *)mLock);
}

// 0x10069510 ?Put@MfSharedMsgQueue@@QAEXPAVMfMsg@@@Z
void MfSharedMsgQueue::Put(MfMsg *msg)
{
    EnterCriticalSection((CRITICAL_SECTION *)mLock);
    if (mSharedCount >= mMaxCount && mMaxCount != -1) {
        mNotFull->Reset();
        LeaveCriticalSection((CRITICAL_SECTION *)mLock);
        mNotFull->Wait(INFINITE);
        EnterCriticalSection((CRITICAL_SECTION *)mLock);
    }
    MfMsgQueue::Put(msg);
    InterlockedIncrement(&mSharedCount);
    mNotEmpty->Signal();
    LeaveCriticalSection((CRITICAL_SECTION *)mLock);
}

// 0x100695a0 ?Get@MfSharedMsgQueue@@QAEPAVMfMsg@@H@Z
MfMsg *MfSharedMsgQueue::Get(int wait)
{
    EnterCriticalSection((CRITICAL_SECTION *)mLock);
    if (!mSharedCount && wait == 1) {
        mNotEmpty->Reset();
        LeaveCriticalSection((CRITICAL_SECTION *)mLock);
        mNotEmpty->Wait(INFINITE);
        EnterCriticalSection((CRITICAL_SECTION *)mLock);
    }
    MfMsg *msg = (MfMsg *)MfMsgQueue::Get();
    if (msg) {
        InterlockedDecrement(&mSharedCount);
        mNotFull->Signal();
    }
    LeaveCriticalSection((CRITICAL_SECTION *)mLock);
    return msg;
}

// 0x10069630 ?Clear@MfSharedMsgQueue@@QAEXXZ
void MfSharedMsgQueue::Clear()
{
    MfMsg *msg;
    while ((msg = Get(0)) != 0)
        delete msg;
}
