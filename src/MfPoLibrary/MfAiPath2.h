// MfAiPath2: a path of configurations as the application sees it. The
// SteamboatApp PDB does not describe it: the names are descriptive, the slots
// are those MfPoPath2 calls.
#pragma once
#include "MfAi.h"
class MfAiActorNode;
class MfTransform3;

class MfAiPath2 : public MfAi {
public:
    virtual int GetNumConfigs() = 0;                                      // 0x0c
    virtual MfTransform3 GetTransform(float index) = 0;                   // 0x10
    virtual void SetTransform(int index, const MfTransform3 &xform) = 0;  // 0x14
    virtual MfAiActorNode *GetParentNode() = 0;                           // 0x18
};
