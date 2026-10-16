#pragma once
// MfKeyList: keyframes ordered by time in a doubly linked list ('KeyL'), with a
// cursor that the time lookups move. Reconstructed (no PDB): layout from the code,
// signatures from the export names.
#include "MfStreamObject.h"
class MfDblList;
class MfKeyframe;
class MfStream;

struct MfKeyListNode {
    MfKeyframe *m_keyframe;     // +0
    MfKeyListNode *m_prev;      // +4
    MfKeyListNode *m_next;      // +8
};

// size 0x14; vtable: StreamType, ReadFromStream, WriteToStream, WriteAsObject, ~
class __declspec(dllexport) MfKeyList : public MfStreamObject {
public:
    MfKeyList();
    MfKeyList(const MfKeyList &list);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    virtual ~MfKeyList() { DeleteKeyframes(); }

    // Inserts key by time; a keyframe with the same time is replaced (deleted).
    MfKeyListNode *Insert(MfKeyframe *key);
    int Remove(MfKeyframe *key);
    MfKeyframe *Remove(MfKeyListNode *node);
    // The iteration functions return the keyframe of the current node and
    // leave node at the following one.
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

    MfKeyListNode *mHead;       // +4
    MfKeyListNode *mTail;       // +8
    MfKeyListNode *mCurrent;    // +0xc  cursor of the time lookups
    int mCount;                 // +0x10
};
