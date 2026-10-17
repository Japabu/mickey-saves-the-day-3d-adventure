// MfActorLib.dll 0x1000b210-0x1000b8a0: MfKeyList.
#include "MfKeyList.h"
#include "MfKeyframe.h"
#include "MfStream.h"
#include "MfDblList.h"

const long MfKeyList::STREAM_TYPE = 'KeyL';

// 0x1000b210 ??0MfKeyList@@QAE@XZ
MfKeyList::MfKeyList() : mHead(0), mTail(0), mCurrent(0), mCount(0)
{
}

// 0x1000b230 ??0MfKeyList@@QAE@ABV0@@Z
MfKeyList::MfKeyList(const MfKeyList &list) : mHead(0), mTail(0), mCurrent(0), mCount(0)
{
    for (MfKeyListNode *node = list.mHead; node; node = node->m_next)
        Insert(node->m_keyframe->CreateCopy());
}

// 0x1000b280 ?Insert@MfKeyList@@QAEPAUMfKeyListNode@@PAVMfKeyframe@@@Z
MfKeyListNode *MfKeyList::Insert(MfKeyframe *key)
{
    unsigned long time = key->m_time;
    MfKeyListNode *node = mHead;
    MfKeyListNode *newNode;
    if (node && time >= node->m_keyframe->m_time) {
        while (node->m_next && time >= node->m_next->m_keyframe->m_time)
            node = node->m_next;
        if (time == node->m_keyframe->m_time) {
            delete node->m_keyframe;
            node->m_keyframe = key;
            return node;
        }
        newNode = new MfKeyListNode;
        newNode->m_keyframe = key;
        newNode->m_prev = node;
        newNode->m_next = node->m_next;
        node->m_next = newNode;
        if (newNode->m_next)
            newNode->m_next->m_prev = newNode;
        else
            mTail = newNode;
        mCount++;
        return newNode;
    }
    newNode = new MfKeyListNode;
    newNode->m_keyframe = key;
    newNode->m_prev = 0;
    newNode->m_next = mHead;
    if (mHead)
        mHead->m_prev = newNode;
    else
        mTail = newNode;
    mHead = newNode;
    mCount++;
    return newNode;
}

// 0x1000b360 ?Remove@MfKeyList@@QAEHPAVMfKeyframe@@@Z
int MfKeyList::Remove(MfKeyframe *key)
{
    MfKeyListNode *node = mHead;
    if (node == 0)
        return 0;
    while (node != 0)
    {
        if (node->m_keyframe == key)
        {
            Remove(node);
            return 1;
        }
        node = node->m_next;
    }
    return 0;
}

// 0x1000b390 ?Remove@MfKeyList@@QAEPAVMfKeyframe@@PAUMfKeyListNode@@@Z
MfKeyframe *MfKeyList::Remove(MfKeyListNode *node)
{
    MfKeyframe *key = node->m_keyframe;
    if (node == mCurrent)
        mCurrent = mCurrent->m_next;
    if (node->m_prev)
        node->m_prev->m_next = node->m_next;
    else
        mHead = node->m_next;
    if (node->m_next)
        node->m_next->m_prev = node->m_prev;
    else
        mTail = node->m_prev;
    delete node;
    mCount--;
    return key;
}

// 0x1000b3f0 ?GetFirst@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::GetFirst(MfKeyListNode *&node)
{
    MfKeyListNode *stored = (mHead != 0) ? mHead->m_next : 0;
    node = stored;
    if (mHead != 0)
        return mHead->m_keyframe;
    return 0;
}

// 0x1000b420 ?GetNext@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::GetNext(MfKeyListNode *&node)
{
    MfKeyframe *key;
    if (node != 0)
        key = node->m_keyframe;
    else
        key = 0;
    if (node != 0)
        node = node->m_next;
    return key;
}

// 0x1000b440 ?GetLast@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::GetLast(MfKeyListNode *&node)
{
    MfKeyListNode *stored = (mTail != 0) ? mTail->m_prev : 0;
    node = stored;
    if (mTail != 0)
        return mTail->m_keyframe;
    return 0;
}

// 0x1000b470 ?GetPrev@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::GetPrev(MfKeyListNode *&node)
{
    MfKeyframe *key;
    if (node != 0)
        key = node->m_keyframe;
    else
        key = 0;
    if (node != 0)
        node = node->m_prev;
    return key;
}

// 0x1000b490 ?PeekPrev@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::PeekPrev(MfKeyListNode *&node)
{
    MfKeyListNode *prev;
    if (node != 0)
    {
        if (node->m_prev == 0)
            return 0;
        prev = node->m_prev;
        prev = prev->m_prev;
        if (prev != 0)
            return prev->m_keyframe;
        return 0;
    }
    if (mTail != 0)
    {
        prev = mTail->m_prev;
        if (prev != 0)
            return prev->m_keyframe;
    }
    return 0;
}

// 0x1000b4d0 ?PeekNext@MfKeyList@@QAEPAVMfKeyframe@@AAPAUMfKeyListNode@@@Z
MfKeyframe * MfKeyList::PeekNext(MfKeyListNode *&node)
{
    if (node != 0)
        return node->m_keyframe;
    return 0;
}

// 0x1000b4f0 ?KeyframeAtTime@MfKeyList@@QAEPAVMfKeyframe@@K@Z
MfKeyframe *MfKeyList::KeyframeAtTime(unsigned long time)
{
    if (!mCurrent)
        mCurrent = mHead;
    if (!mCurrent)
        return 0;
    if (time < mCurrent->m_keyframe->time())
        mCurrent = mHead;
    while (mCurrent && mCurrent->m_keyframe->m_time < time)
        mCurrent = mCurrent->m_next;
    if (mCurrent && mCurrent->m_keyframe->m_time == time)
        return mCurrent->m_keyframe;
    return 0;
}

// 0x1000b550 ?LastKeyframeUntil@MfKeyList@@QAEPAVMfKeyframe@@K@Z
MfKeyframe *MfKeyList::LastKeyframeUntil(unsigned long time)
{
    if (!mCurrent)
        mCurrent = mHead;
    if (!mCurrent)
        return 0;
    if (time < mCurrent->m_keyframe->time())
        mCurrent = mHead;
    if (time < mCurrent->m_keyframe->m_time)
        return 0;
    while (mCurrent->m_next && mCurrent->m_next->m_keyframe->m_time <= time)
        mCurrent = mCurrent->m_next;
    return mCurrent->m_keyframe;
}

// 0x1000b5c0 ?FirstKeyframeAfter@MfKeyList@@QAEPAVMfKeyframe@@K@Z
MfKeyframe * MfKeyList::FirstKeyframeAfter(unsigned long time)
{
    MfKeyListNode *node = mHead;
    if (node == 0)
        return 0;
    while (node != 0)
    {
        if (node->m_keyframe->m_time >= time)
            return node->m_keyframe;
        node = node->m_next;
    }
    return 0;
}

// 0x1000b5f0 ?LastKeyframeBetween@MfKeyList@@QAEPAVMfKeyframe@@KK@Z
// The last keyframe in [from, to]; with from >= to the interval wraps around.
MfKeyframe *MfKeyList::LastKeyframeBetween(unsigned long from, unsigned long to)
{
    MfKeyframe *key = 0;
    MfKeyListNode *node;
    if (from < to) {
        for (node = mHead; node && node->m_keyframe->m_time <= to; node = node->m_next)
            if (node->m_keyframe->m_time >= from)
                key = node->m_keyframe;
    } else {
        for (node = mHead; node && node->m_keyframe->m_time <= to; node = node->m_next)
            key = node->m_keyframe;
        if (!key)
            for (; node; node = node->m_next)
                if (node->m_keyframe->m_time >= from)
                    key = node->m_keyframe;
    }
    return key;
}

// 0x1000b660 ?KeyframesBetween@MfKeyList@@QAE_NKKAAVMfDblList@@@Z
// Adds the keyframes in [from, to] to list (backwards from the end when from >= to).
bool MfKeyList::KeyframesBetween(unsigned long from, unsigned long to, MfDblList &list)
{
    MfKeyListNode *node;
    if (from < to) {
        for (node = mHead; node && node->m_keyframe->m_time <= to; node = node->m_next)
            if (node->m_keyframe->m_time >= from)
                list.Add(node->m_keyframe);
    } else {
        for (node = mTail; node && node->m_keyframe->m_time >= to; node = node->m_prev)
            if (node->m_keyframe->m_time <= from)
                list.Add(node->m_keyframe);
    }
    return list.GetCount() ? true : false;
}

// 0x1000b6e0 ?FirstTime@MfKeyList@@QBEKXZ
unsigned long MfKeyList::FirstTime() const
{
    MfKeyListNode *node = mHead;
    if (node != 0)
        return node->m_keyframe->m_time;
    return 0x7fffffff;
}

// 0x1000b700 ?LastTime@MfKeyList@@QBEKXZ
unsigned long MfKeyList::LastTime() const
{
    MfKeyListNode *node = mTail;
    if (node != 0)
        return node->m_keyframe->m_time;
    return 0;
}

// 0x1000b710 ?DeleteKeyframes@MfKeyList@@QAEXXZ
void MfKeyList::DeleteKeyframes()
{
    MfKeyListNode *node = mHead;
    while (node) {
        MfKeyListNode *next = node->m_next;
        delete node->m_keyframe;
        delete node;
        node = next;
    }
    mCurrent = 0;
    mTail = 0;
    mHead = 0;
    mCount = 0;
}

// 0x1000b750 ?fn_1000b750@@YAPAVMfStreamObject@@XZ (stream constructor for 'KeyL')
static MfStreamObject *fn_1000b750()
{
    return new MfKeyList;
}

// 0x1000b7b0 _$E4 (jumps to _$E3)
// 0x1000b7c0 _$E3 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_keyListEntry(MfKeyList::STREAM_TYPE, fn_1000b750);

// 0x1000b7e0 ?StreamType@MfKeyList@@UBEJXZ
long MfKeyList::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000b7f0 ?ReadFromStream@MfKeyList@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfKeyList::ReadFromStream(MfStream &s)
{
    int count;
    s >> count;
    for (int i = 0; i < count && s.Status() == 0; i++) {
        MfKeyframe *key = (MfKeyframe *)s.ReadObject(0);
        if (key)
            Insert(key);
    }
    return s.Status();
}

// 0x1000b850 ?WriteToStream@MfKeyList@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfKeyList::WriteToStream(MfStream &s) const
{
    s << mCount;
    for (MfKeyListNode *node = mHead; node && s.Status() == 0; node = node->m_next)
        s.WriteObject(node->m_keyframe);
    return s.Status();
}
