#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfJointKeyList: the MfJointKeyframes of one joint; inserting a keyframe
// recomputes the blends of its neighbours, sampling interpolates between them.
#include "MfKeyList.h"
class MfJointKeyframe;

class MFACTOR_API MfJointKeyList : public MfKeyList {
public:
    MfJointKeyList() {}
    MfJointKeyList(const MfJointKeyList &list) : MfKeyList(list) {}
    virtual ~MfJointKeyList() {}
    MfJointKeyframe *GetFirst(MfKeyListNode *&node) { return (MfJointKeyframe *)MfKeyList::GetFirst(node); }
    MfJointKeyframe *GetNext(MfKeyListNode *&node) { return (MfJointKeyframe *)MfKeyList::GetNext(node); }
    MfKeyListNode *Insert(MfJointKeyframe *key, int computeBlends);
    // Samples the track at time into key; 0 if time lies outside the track
    // (after its end only when wrap is 0).
    int SampleAtTime(unsigned long time, MfJointKeyframe &key, int wrap);
};
