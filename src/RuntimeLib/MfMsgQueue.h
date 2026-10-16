// MfMsgQueue: a growing ring buffer of pointers (messages) with holes for the
// removed entries and an iteration cursor. RuntimeLib's own class (RTTI
// name, unit 10067bd0-10068190); the member names are invented.
#ifndef RUNTIMELIB_MFMSGQUEUE_H
#define RUNTIMELIB_MFMSGQUEUE_H

class MfMsgQueue {
public:
    // removes the timer with the handle (MfTimedMsgQueue)
    virtual void *RemoveTimer(int) { return 0; }
    MfMsgQueue();
    MfMsgQueue(int capacity);
    ~MfMsgQueue();
    void Put(void *item);
    // the oldest item, removed; 0 if empty
    void *Get();
    // removes the item the last GetNext returned
    void *RemoveCurrent();
    // removes an item; the item, 0 if not queued
    void *Remove(void *item);
    // iteration from the cursor; last is set at the last item
    void Rewind()
    {
        mCursor = mTail;
        mVisited = 0;
        mToVisit = mCount;
    }
    void *GetNext(int *last);
protected:
    // closes the holes
    void Compact();
    // triples the capacity
    void Grow();
public:
    int mHead;                                              // 0x04 the next free slot
    int mTail;                                              // 0x08 the oldest item
    int mHoles;                                             // 0x0c removed in place
    int mCount;                                             // 0x10
    int mLimit;                                             // 0x14
    int mCapacity;                                          // 0x18
    int mCursor;                                            // 0x1c
    int mVisited;                                           // 0x20
    int mToVisit;                                           // 0x24
    void **mItems;                                          // 0x28
};

#endif
