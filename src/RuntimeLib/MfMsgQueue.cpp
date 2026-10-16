// MfMsgQueue: a growing ring buffer of messages (RuntimeLib.dll
// 10067bd0-100680d0).
#include "MfMsgQueue.h"
#include "MfDblList.h"
#include <string.h>

// 0x10067bd0 ??0MfMsgQueue@@QAE@XZ
MfMsgQueue::MfMsgQueue()
    : mHead(0), mTail(0), mHoles(0), mCount(0), mLimit(200), mCapacity(200), mCursor(0), mVisited(0)
{
    mItems = new void *[200];
    for (int i = 0; i < mCapacity; i++)
        mItems[i] = 0;
}

// 0x10067c30 ??0MfMsgQueue@@QAE@H@Z
MfMsgQueue::MfMsgQueue(int capacity)
    : mHead(0), mTail(0), mHoles(0), mCount(0), mCapacity(capacity), mCursor(0), mVisited(0)
{
    if (capacity < 0)
        mCapacity = 200;
    mToVisit = 0;
    mLimit = mCapacity * 5;
    mItems = new void *[mCapacity];
    for (int i = 0; i < mCapacity; i++)
        mItems[i] = 0;
}

// 0x10067ca0 ??1MfMsgQueue@@QAE@XZ
MfMsgQueue::~MfMsgQueue()
{
    delete mItems;
}

// 0x10067cc0 ?Put@MfMsgQueue@@QAEXPAX@Z
void MfMsgQueue::Put(void *item)
{
    mCount++;
    mItems[mHead] = item;
    mHead++;
    if (mHead == mCapacity)
        mHead = 0;
    if (mHead == mTail) {
        Compact();
        if (mHead == mTail)
            Grow();
    }
}

// EQUIV 0x10067d20 ?Compact@MfMsgQueue@@IAEXXZ: reviewed: same-effect inlining: the original
// calls the local list's Append through the vtable (slot 8); here the inline MfDblList::Append
// expands to its body, Insert(pData, LP_AFTER, pliLast) (as mfutil's Append does, and the local's
// dynamic type is MfDblList); otherwise register choice and order of independent stores only
void MfMsgQueue::Compact()
{
    MfDblList items;
    int i;
    for (i = mTail; i < mCapacity; i++)
        if (mItems[i])
            items.Append(mItems[i]);
    for (i = 0; i < mHead; i++)
        if (mItems[i])
            items.Append(mItems[i]);
    mHoles = 0;
    mHead = mTail + items.GetCount();
    if (mHead >= mCapacity)
        mHead -= mCapacity;
    mCount = items.GetCount();
    for (i = 0; i < mCapacity; i++)
        mItems[i] = 0;
    int j = mTail;
    MfDblListItem *pli;
    for (void *item = items.GetFirst(pli); item; item = items.GetNext(pli)) {
        mItems[j++] = item;
        if (j >= mCapacity)
            j = 0;
    }
    mCursor = mTail;
    mVisited = 0;
    mToVisit = mCount;
}

// 0x10067e50 ?Get@MfMsgQueue@@QAEPAXXZ
void *MfMsgQueue::Get()
{
    if (mTail != mHead && mCount) {
        void *item = mItems[mTail];
        mItems[mTail] = 0;
        if (mTail == mCapacity - 1)
            mTail = 0;
        else
            mTail++;
        mCount--;
        return item;
    }
    return 0;
}

// 0x10067ea0 ?RemoveCurrent@MfMsgQueue@@QAEPAXXZ
void *MfMsgQueue::RemoveCurrent()
{
    void *item = 0;
    int i = mCursor;
    if (!i)
        i = mCapacity;
    i--;
    if (i == mTail) {
        mCount--;
        if (mTail == mCapacity - 1)
            mTail = 0;
        else
            mTail++;
    } else if (i == mHead) {
        mCount--;
        if (mHead == 0)
            mHead = mCapacity - 1;
        else
            mHead--;
    } else
        mHoles++;
    if (i >= 0 && i < mCapacity) {
        item = mItems[i];
        mItems[i] = 0;
    }
    return item;
}

// 0x10067f20 ?Remove@MfMsgQueue@@QAEPAXPAX@Z
void *MfMsgQueue::Remove(void *item)
{
    int i;
    if (mTail > mHead) {
        for (i = mTail; i < mCapacity; i++)
            if (mItems[i] == item) {
                mHoles++;
                mItems[i] = 0;
                return item;
            }
        for (i = 0; i < mHead; i++)
            if (mItems[i] == item) {
                mHoles++;
                mItems[i] = 0;
                return item;
            }
        return 0;
    }
    for (i = mTail; i < mHead; i++)
        if (mItems[i] == item) {
            mHoles++;
            mItems[i] = 0;
            return item;
        }
    return 0;
}

// 0x10067fd0 ?GetNext@MfMsgQueue@@QAEPAXPAH@Z
void *MfMsgQueue::GetNext(int *last)
{
    if (mCursor == mCapacity)
        mCursor = 0;
    mVisited++;
    *last = mCursor == mHead - 1;
    if (mVisited >= mToVisit)
        *last = 1;
    return mItems[mCursor++];
}

// 0x10068020 ?Grow@MfMsgQueue@@IAEXXZ
void MfMsgQueue::Grow()
{
    int count = 0;
    int capacity = mCapacity * 3;
    void **items = new void *[capacity];
    int i;
    for (i = 0; i < capacity; i++)
        items[i] = 0;
    for (i = mTail; i < mCapacity; i++)
        items[count++] = mItems[i];
    for (i = 0; i < mHead; i++)
        items[count++] = mItems[i];
    delete mItems;
    mTail = 0;
    mCursor = 0;
    mVisited = 0;
    mItems = items;
    mCapacity = capacity;
    mHead = count;
    mToVisit = mCount;
}

