// MfFilterItem: an entry of an actor's collision filter (MfActor::mFilter):
// the node or actor (types 3-5) the actor does not collide with. As in
// MfPoLibrary/MfFilterItem.h.
#ifndef RUNTIMELIB_MFFILTERITEM_H
#define RUNTIMELIB_MFFILTERITEM_H
class MfAiActor;
class MfAiActorNode;

struct MfFilterItem {
    short type;                                             // 0x00
    union {                                                 // 0x04
        MfAiActorNode *node;
        MfAiActor *actor;
    };
};

// MF_HOOK_FILTER_EDIT: an actor's filter changes (kind 0 add, 1 clear,
// 2 delete)
struct MfFilterHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    MfFilterItem *item;                                     // 0x14
    int kind;                                               // 0x18
};

#endif
