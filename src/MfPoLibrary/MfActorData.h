// What an actor reports about its last collision, proximity event, pick and
// contour failure (structs from the SteamboatApp PDB).
#ifndef MFPOLIBRARY_MFACTORDATA_H
#define MFPOLIBRARY_MFACTORDATA_H
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

struct MfProximityData {
    MfAiActor *plaOther;                                    // 0x00
};

struct MfPickData {
    MfAiActorNode *pickedNode;                              // 0x00
    MfVector3 pickPoint;                                    // 0x04
    int triIndex;                                           // 0x10
};

struct MfContourFailDownData {
    MfAiActorNode *failNode;                                // 0x00
};

#endif
