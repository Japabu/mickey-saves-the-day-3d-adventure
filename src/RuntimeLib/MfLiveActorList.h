// MfLiveActorList: a list of live actors; an owning list deletes them.
#ifndef RUNTIMELIB_MFLIVEACTORLIST_H
#define RUNTIMELIB_MFLIVEACTORLIST_H
#include "MfDblList.h"
class MfLiveActor;

class __declspec(dllexport) MfLiveActorList : public MfDblList {
public:
    MfLiveActorList(int owner);
    virtual ~MfLiveActorList();
    virtual void Clear();
    void Prepend(const MfLiveActor *actor) { Insert((void *)actor, LP_BEFORE, pliFirst); }
    void Add(const MfLiveActor *actor) { ((MfDblList *)this)->Append((void *)actor); }
    void Append(const MfLiveActor *actor) { Insert((void *)actor, LP_AFTER, pliLast); }
    int Contains(MfLiveActor *actor);
    MfLiveActor *Remove(MfLiveActor *actor, int keep);

    int mOwner;                                             // 0x10
};

#endif
