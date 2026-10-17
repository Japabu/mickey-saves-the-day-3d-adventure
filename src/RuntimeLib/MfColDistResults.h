// The sphere and distance results of MfAiColNode's queries. Declarations from
// the SteamboatApp PDB. Kept out of MfColResults.h: declaring them there flips
// the operand order of one fimul in MfViewEventHandler::Pick.
#ifndef RUNTIMELIB_MFCOLDISTRESULTS_H
#define RUNTIMELIB_MFCOLDISTRESULTS_H
#include "MfVector3.h"
class MfAiColNode;

// the hit of a sphere (MfAiColNode::CheckCollision)
struct MfColResultSphere {
    MfVector3 pos;                                          // 0x00
    MfAiColNode *node;                                      // 0x0c
    int triIndex;                                           // 0x10
};

// the nearest points of two collision nodes (MfAiColNode::GetMinDistance)
struct MfDistResult {
    MfAiColNode *node1;                                     // 0x00
    MfAiColNode *node2;                                     // 0x04
    int triIndex1;                                          // 0x08
    int triIndex2;                                          // 0x0c
    MfVector3 p1;                                           // 0x10
    MfVector3 p2;                                           // 0x1c
    float distance;                                         // 0x28
};

#endif
