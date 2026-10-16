// MfKeyList (MfActorLib.dll): keyframes ordered by time in a doubly linked
// list ('KeyL'). Declaration as in src/MfActorLib/MfKeyList.h; the destructor
// is inline (RuntimeLib's local deleting destructor calls DeleteKeyframes).
#ifndef RUNTIMELIB_MFKEYLIST_H
#define RUNTIMELIB_MFKEYLIST_H
#include "MfStreamObject.h"
#include "enums.h"
class MfDblList;
class MfKeyframe;
class MfStream;

struct MfKeyListNode {
    MfKeyframe *m_keyframe;                                 // 0x00
    MfKeyListNode *m_prev;                                  // 0x04
    MfKeyListNode *m_next;                                  // 0x08
};

class __declspec(dllimport) MfKeyList : public MfStreamObject {
public:
    MfKeyList();
    MfKeyList(const MfKeyList &list);
    MfKeyList &operator=(const MfKeyList &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    virtual ~MfKeyList() { DeleteKeyframes(); }

    MfKeyListNode *Insert(MfKeyframe *key);
    int Remove(MfKeyframe *key);
    MfKeyframe *Remove(MfKeyListNode *node);
    MfKeyframe *GetFirst(MfKeyListNode *&node);
    MfKeyframe *GetNext(MfKeyListNode *&node);
    MfKeyframe *GetLast(MfKeyListNode *&node);
    MfKeyframe *GetPrev(MfKeyListNode *&node);
    MfKeyframe *PeekPrev(MfKeyListNode *&node);
    MfKeyframe *PeekNext(MfKeyListNode *&node);
    MfKeyframe *KeyframeAtTime(unsigned long time);
    MfKeyframe *LastKeyframeUntil(unsigned long time);
    MfKeyframe *FirstKeyframeAfter(unsigned long time);
    MfKeyframe *LastKeyframeBetween(unsigned long from, unsigned long to);
    bool KeyframesBetween(unsigned long from, unsigned long to, MfDblList &list);
    unsigned long FirstTime() const;
    unsigned long LastTime() const;
    int NumKeyframes() const { return mCount; }
    void DeleteKeyframes();

    static const long STREAM_TYPE;

    MfKeyListNode *mHead;                                   // 0x04
    MfKeyListNode *mTail;                                   // 0x08
    MfKeyListNode *mCurrent;                                // 0x0c cursor of the time lookups
    int mCount;                                             // 0x10
};

#endif
