// MfSharedMsgQueue: a message queue the worker threads share with the main
// thread (RTTI name; unit 10069380-10069660): a bounded MfMsgQueue behind a
// critical section, with events for "not full" and "not empty".
#ifndef RUNTIMELIB_MFSHAREDMSGQUEUE_H
#define RUNTIMELIB_MFSHAREDMSGQUEUE_H
#include "MfMsgQueue.h"
#include "MfString.h"
class MfEvent;
class MfMsg;

class MfSharedMsgQueue : public MfMsgQueue {
public:
    MfSharedMsgQueue(int limit, const char *name);
    ~MfSharedMsgQueue();
    // waits while the queue is full
    void Put(MfMsg *msg);
    // the oldest message; 0 if none (waits for one with wait 1)
    MfMsg *Get(int wait);
    // deletes the queued messages
    void Clear();

    char mLock[0x18];                                       // 0x2c CRITICAL_SECTION
    char m44[0x18];                                         // 0x44
    int mMaxCount;                                          // 0x5c -1: unbounded
    long mSharedCount;                                      // 0x60
    MfEvent *mNotFull;                                      // 0x64
    MfEvent *mNotEmpty;                                     // 0x68
    MfString mName;                                         // 0x6c
};

#endif
