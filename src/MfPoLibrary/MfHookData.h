// The data passed to the application's hooks (structs from the SteamboatApp
// PDB and RuntimeLib); MfPoLibrary sends user hooks (Actor.DisplayText) and
// miscellaneous-data hooks (ActorNode.colType).
#pragma once
#include "MfString.h"
#include "MfProperty.h"
#include "MfTransform3.h"
class MfAiActor;
class MfAiActorNode;

struct MfHookData {
    int reserved[4];                                        // 0x00
};

struct MfUserHookData : public MfHookData {
    MfString event;                                         // 0x10
    MfString data;                                          // 0x14
    void *userData;                                         // 0x18
};

// MF_HOOK_MISC_DATA (0x1b): something about an actor changed (RuntimeLib
// declares the tail as a rotation and a position)
struct MfMiscHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    const MfAiActor *pTarget;                               // 0x14
    MfAiActorNode *pNode;                                   // 0x18
    int kind;                                               // 0x1c 3 collision check style
    MfString str;                                           // 0x20
    unsigned long value;                                    // 0x24
    MfProperty property;                                    // 0x28
    MfTransform3 transform;                                 // 0x34
};
