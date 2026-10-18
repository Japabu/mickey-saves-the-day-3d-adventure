// MfTimedMsg, MfTimedMsgQueue and MfTimerQueue: the dispatcher's timers
// (RuntimeLib.dll 10069660-100699d0).
#include "MfTimedMsg.h"
#include <windows.h>
#include "MfMsgDispatcher.h"
#include "MfTimedMsgQueue.h"

static int gNextTimerHandle;

// 0x100680d0 ?RemoveTimer@MfTimedMsgQueue@@UAEPAXH@Z
void *MfTimedMsgQueue::RemoveTimer(int handle)
{
    int i;
    void *item;
    if (mTail > mHead) {
        for (i = mTail; i < mCapacity; i++)
            if (mItems[i] && ((MfTimedMsg *)mItems[i])->mHandle == handle) {
                mHoles++;
                item = mItems[i];
                mItems[i] = 0;
                return item;
            }
        for (i = 0; i < mHead; i++)
            if (mItems[i] && ((MfTimedMsg *)mItems[i])->mHandle == handle) {
                mHoles++;
                item = mItems[i];
                mItems[i] = 0;
                return item;
            }
        return 0;
    }
    for (i = mTail; i < mHead; i++)
        if (mItems[i] && ((MfTimedMsg *)mItems[i])->mHandle == handle) {
            mHoles++;
            item = mItems[i];
            mItems[i] = 0;
            return item;
        }
    return 0;
}

// 0x10069660 ??0MfTimedMsg@@QAE@PAVMfMsg@@HH@Z
MfTimedMsg::MfTimedMsg(MfMsg *msg, int delay, int once)
    : mDelay(delay)
{
    mStart = 0;
    mOwnsMsg = 1;
    mOnce = once;
    mMsg = msg;
    mHandle = gNextTimerHandle++;
    mStart = GetTickCount();
    mDue = mDelay + mStart;
}

// 0x100696f0 ??_GMfTimedMsg@@UAEPAXI@Z

// 0x10069710 ??1MfTimedMsg@@UAE@XZ
MfTimedMsg::~MfTimedMsg()
{
    if (mOwnsMsg == 1 && mMsg)
        delete mMsg;
}

// TODO 0x10069770 ??0MfTimerQueue@@QAE@XZ: (78% similar) the original repeats the store and the return
// after both outcomes of the new
MfTimerQueue::MfTimerQueue()
    : mQueue(new MfTimedMsgQueue)
{
}

// 0x100697e0 ??1MfTimerQueue@@QAE@XZ
MfTimerQueue::~MfTimerQueue()
{
    if (mQueue)
        delete mQueue;
}

// 0x10069800 ??1MfTimedMsgQueue@@QAE@XZ (the implicit destructor)

// 0x10069810 ?Update@MfTimerQueue@@QAEXXZ
void MfTimerQueue::Update()
{
    mNow = GetTickCount();
    mQueue->Rewind();
    do {
        MfTimedMsg *timer = (MfTimedMsg *)mQueue->GetNext(&mLast);
        if (timer && mNow > timer->mDue) {
            timer->mMsg->m24 = 0;
            g_msgDispatcher->Post(timer->mMsg);
            if (timer->mOnce == 1) {
                mQueue->RemoveCurrent();
                timer->mMsg->m24 = 1;
                timer->mOwnsMsg = 0;
                delete timer;
            } else {
                timer->mStart = GetTickCount();
                timer->mDue = timer->mDelay + timer->mStart;
            }
        }
    } while (mLast != 1);
}

// 0x100698b0 ?Post@MfTimerQueue@@QAEHPAVMfMsg@@HH@Z
int MfTimerQueue::Post(MfMsg *msg, int delay, int once)
{
    MfTimedMsg *timer = new MfTimedMsg(msg, delay, once);
    mQueue->Put(timer);
    return timer->mHandle;
}

// 0x10069920 ?Remove@MfTimerQueue@@QAEXH@Z
void MfTimerQueue::Remove(int handle)
{
    MfTimedMsg *timer = (MfTimedMsg *)mQueue->RemoveTimer(handle);
    if (timer) {
        if (g_msgDispatcher->mCurrent == timer->mMsg && g_msgDispatcher->mProcessing == 1) {
            timer->mOwnsMsg = 0;
            timer->mMsg->m24 = 1;
            delete timer;
        } else {
            g_msgDispatcher->Cancel(timer->mMsg);
            delete timer;
        }
    }
}

// 0x10069980 ?Restart@MfTimerQueue@@QAEXXZ
void MfTimerQueue::Restart()
{
    mQueue->Rewind();
    do {
        MfTimedMsg *timer = (MfTimedMsg *)mQueue->GetNext(&mLast);
        if (timer && timer->mOnce == 1) {
            mQueue->RemoveCurrent();
            delete timer;
        }
    } while (mLast != 1);
}
