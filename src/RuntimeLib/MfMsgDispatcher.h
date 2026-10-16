// MfMsgDispatcher: RuntimeLib's message dispatcher (not exported; names are
// descriptive). Every registered MfRootObj can receive messages by its ID.
// Unit 10068190-100688e0 with the two worker threads (100688e0, 10069170).
#ifndef RUNTIMELIB_MFMSGDISPATCHER_H
#define RUNTIMELIB_MFMSGDISPATCHER_H
#include "MfDblList.h"
#include "MfIndex.h"
class MfMsg;
class MfMsgQueue;
class MfRootObj;
class MfSharedMsgQueue;
class MfTimerQueue;

class MfMsgDispatcher {
public:
    MfMsgDispatcher();                                      // 10068190
    ~MfMsgDispatcher();
    void Register(MfRootObj *obj);
    void Unregister(MfRootObj *obj);
    // the registered object with the ID, 0 if none
    MfRootObj *Find(int id);
    // the ID of a registered object
    int FindID(MfRootObj *obj);
    // stops the worker threads
    void Shutdown();
    // delivers the queued messages; 0 if it is already running or stopped
    int ProcessMessages();
    // delivery while the world runs (MfWorld::GoLive, ContinueLive) or is
    // authored (GoAuthor)
    void Start();
    void Resume();
    void Pause();
    // the message to every registered object
    void SendToAll(MfMsg *msg);
    void Post(MfMsg *msg);
    // takes a message back out of the queue
    void Cancel(MfMsg *msg);
    // the space is going away (MfSpace::~MfSpace)
    void FlushPending();

    MfTimerQueue *mTimedQueue;                              // 0x00
    int mLast;                                              // 0x04 the queue's last message is out
    MfMsgQueue *mQueue;                                     // 0x08
    int m0c;                                                // 0x0c
    MfSharedMsgQueue **mSharedQueues;                       // 0x10 4: the threads' in and out queues
    int m14, m18;                                           // 0x14
    unsigned long mThreads[2];                              // 0x1c
    int m24;                                                // 0x24
    int mStopped;                                           // 0x28
    int mProcessing;                                        // 0x2c
    MfIndex mObjects;                                       // 0x30 by ID
    MfIndex mIDs;                                           // 0x44 by object
    MfDblList mIDList;                                      // 0x58
    MfMsg *mCurrent;                                        // 0x68 the message being delivered
    int mRunning;                                           // 0x6c
    char mLocks[2][0x18];                                   // 0x70 CRITICAL_SECTIONs
};

extern MfMsgDispatcher *g_msgDispatcher;

#endif
