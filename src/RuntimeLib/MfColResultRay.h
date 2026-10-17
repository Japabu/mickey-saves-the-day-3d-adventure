// MfColResultRay: the first hit of a ray (MfAiColNode::CheckCollision,
// MfAiSpace::CastRay). Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFCOLRESULTRAY_H
#define RUNTIMELIB_MFCOLRESULTRAY_H
#include "MfVector3.h"
class MfAiColNode;

struct MfColResultRay {
    MfVector3 pos;                                          // 0x00
    MfAiColNode *node;                                      // 0x0c
    int triIndex;                                           // 0x10
};

#endif
