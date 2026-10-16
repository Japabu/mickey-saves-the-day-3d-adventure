// MfFilterItem: an entry of an actor's collision filter: the kind of filter
// and the actor or node it names (types 3-5 name an actor, the others a node).
#pragma once
class MfAiActor;
class MfAiActorNode;

struct MfFilterItem {
    short type;                                             // 0x00
    union {                                                 // 0x04
        MfAiActorNode *node;
        MfAiActor *actor;
    };
};
