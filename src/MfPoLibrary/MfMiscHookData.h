// MfMiscHookData: the data of MF_HOOK_MISC_DATA (0x1b): something about an
// actor changed (MfPoLibrary sends it for ActorNode.colType). Not in the
// SteamboatApp PDB: the names are descriptive. RuntimeLib declares the tail
// as a position and a rotation (src/RuntimeLib/MfHookManager.h).
#ifndef MFPOLIBRARY_MFMISCHOOKDATA_H
#define MFPOLIBRARY_MFMISCHOOKDATA_H
#include "MfHookData.h"
#include "MfString.h"
#include "MfProperty.h"
#include "MfTransform3.h"
class MfAiActor;
class MfAiActorNode;

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

#endif
