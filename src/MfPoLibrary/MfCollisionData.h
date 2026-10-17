// MfCollisionData: what an actor reports about its last collision (struct
// from the SteamboatApp PDB).
#ifndef MFPOLIBRARY_MFCOLLISIONDATA_H
#define MFPOLIBRARY_MFCOLLISIONDATA_H
#include "MfString.h"
#include "MfVector3.h"
class MfAiActor;
class MfAiActorNode;

struct MfCollisionData {
    MfString strSkillName;                                  // 0x00
    MfAiActorNode *psnNode;                                 // 0x04
    MfVector3 collPt;                                       // 0x08
    MfVector3 collDir;                                      // 0x14
    MfVector3 surfaceNormal;                                // 0x20
    MfAiActor *plaOther;                                    // 0x2c
    MfAiActorNode *psnOtherNode;                            // 0x30
    float playBack;                                         // 0x34
};

#endif
