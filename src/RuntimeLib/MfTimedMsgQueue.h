// MfTimerQueue: the dispatcher's timers (delayed and repeated messages); the
// handle identifies a timer. The name is invented (the RTTI name
// MfTimedMsgQueue belongs to the message queue it keeps, MfTimedMsg.h). Kept
// out of MfMsgDispatcher.h (the dispatcher holds only a pointer to it): the
// extra declarations flip functions of the units that include it.
#ifndef RUNTIMELIB_MFTIMEDMSGQUEUE_H
#define RUNTIMELIB_MFTIMEDMSGQUEUE_H
class MfMsg;
class MfTimedMsgQueue;

class MfTimerQueue {
public:
    MfTimerQueue();
    ~MfTimerQueue();
    // queues a message after delay ms, once or repeatedly; the handle
    int Post(MfMsg *msg, int delay, int once);
    void Remove(int handle);
    // queues the messages that are due
    void Update();
    // drops the one-shot timers
    void Restart();

    MfTimedMsgQueue *mQueue;                                // 0x00
    unsigned long mNow;                                     // 0x04
    int mLast;                                              // 0x08
};

#endif
