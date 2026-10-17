// The results of the collision queries of MfAiColNode. Declarations from the
// SteamboatApp PDB.
#ifndef RUNTIMELIB_MFCOLRESULTS_H
#define RUNTIMELIB_MFCOLRESULTS_H
#include "MfVector3.h"
class MfAiColNode;

struct MfColResultNode {
    MfAiColNode *node1;                                     // 0x00
    MfAiColNode *node2;                                     // 0x04
    int triIndex1;                                          // 0x08
    int triIndex2;                                          // 0x0c
};

// the first hit of a ray (MfAiColNode::CheckCollision, MfAiSpace::CastRay)
struct MfColResultRay {
    MfVector3 pos;                                          // 0x00
    MfAiColNode *node;                                      // 0x0c
    int triIndex;                                           // 0x10
};

#endif
