#pragma once
// MfColResults.h: the results of Motivate's collision queries (MfActorLib.dll), as the game
// declares them (SteamboatApp.pdb).
#include "MfVector3.h"

class MfAiColNode;

// own copy: RuntimeLib's MfColResults.h opens RuntimeLib/MfVector3.h, a second MfVector3 next
// to this module's (MfVector3.h).
struct MfColResultNode {
    MfAiColNode *node1;                                 // +0x0000
    MfAiColNode *node2;                                 // +0x0004
    int triIndex1;                                      // +0x0008
    int triIndex2;                                      // +0x000c
};

// the first hit of a ray (MfAiColNode::CheckCollision, MfAiSpace::CastRay)
// own copy: as above.
struct MfColResultRay {
    MfVector3 pos;                                      // +0x0000
    MfAiColNode *node;                                  // +0x000c
    int triIndex;                                       // +0x0010
};
