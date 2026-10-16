// MfJointKeyList (MfActorLib.dll): the keyframes of one joint of a skill.
// Declaration as in src/MfActorLib/MfJointKeyList.h; the constructors are inline
// (MfSkill::InsertTracks builds the vtable locally), the destructor is the
// imported one.
#ifndef RUNTIMELIB_MFJOINTKEYLIST_H
#define RUNTIMELIB_MFJOINTKEYLIST_H
#include "MfKeyList.h"
class MfJointKeyframe;

class __declspec(dllimport) MfJointKeyList : public MfKeyList {
public:
    MfJointKeyList() {}
    MfJointKeyList(const MfJointKeyList &list) : MfKeyList(list) {}
    virtual ~MfJointKeyList();
    MfKeyListNode *Insert(MfJointKeyframe *key, int computeBlends);
    int SampleAtTime(unsigned long time, MfJointKeyframe &key, int wrap);
};

#endif
