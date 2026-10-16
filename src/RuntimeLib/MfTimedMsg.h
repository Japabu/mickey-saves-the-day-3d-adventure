// MfTimedMsg and MfTimedMsgQueue: a message that waits for its time, and the
// queue of them (RTTI names; unit 10069660-100699d0, with MfTimerQueue).
#ifndef RUNTIMELIB_MFTIMEDMSG_H
#define RUNTIMELIB_MFTIMEDMSG_H
#include "MfMsg.h"
#include "MfMsgQueue.h"

class MfTimedMsg : public MfMsg {
public:
    MfTimedMsg(MfMsg *msg, int delay, int once);
    virtual ~MfTimedMsg();

    int mOwnsMsg;                                           // 0x48
    MfMsg *mMsg;                                            // 0x4c
    unsigned long mStart;                                   // 0x50
    unsigned long mDue;                                     // 0x54
    int mDelay;                                             // 0x58
    int mOnce;                                              // 0x5c
    int mHandle;                                            // 0x60
};

class MfTimedMsgQueue : public MfMsgQueue {
public:
    MfTimedMsgQueue() {}
    virtual void *RemoveTimer(int handle);
};

#endif
