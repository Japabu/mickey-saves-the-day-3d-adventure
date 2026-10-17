// MfPickData: what an actor reports about its last pick (struct from the
// SteamboatApp PDB).
#ifndef MFPOLIBRARY_MFPICKDATA_H
#define MFPOLIBRARY_MFPICKDATA_H
#include "MfVector3.h"
class MfAiActorNode;

struct MfPickData {
    MfAiActorNode *pickedNode;                              // 0x00
    MfVector3 pickPoint;                                    // 0x04
    int triIndex;                                           // 0x10
};

#endif
