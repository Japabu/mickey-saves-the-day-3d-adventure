#pragma once
// MfAiColNode.h: a collision node of a Motivate actor (MfActorLib.dll), as the game declares it
// (SteamboatApp.pdb); all members are pure virtual.
#include "MfAi.h"

class MfAiActorNode;
class MfAiColInterface;
class MfAiColReport;
class MfColModel;
struct MfColResultNode;
struct MfColResultRay;
struct MfColResultSphere;
struct MfDistResult;

class MfAiColNode : public MfAi {
public:
    virtual MfTransform3 &Transform() = 0;              // vftable+0x0c
    virtual MfColModel *&ColModel() = 0;                // vftable+0x10
    virtual int GetCheckStyle() = 0;                    // vftable+0x14
    virtual void SetCheckStyle(int) = 0;                // vftable+0x18
    virtual MfAiActorNode *&Owner() = 0;                // vftable+0x1c
    virtual bool CheckCollision(MfAiColNode *, MfColResultNode &) = 0;
    virtual bool CheckCollision(MfVector3 &, MfVector3 &, MfColResultRay &, bool, float) = 0;
    virtual bool CheckCollision(MfVector3 &, float, MfColResultSphere &) = 0;
    virtual bool CheckContainment(MfVector3 &, float) = 0;
    virtual bool GetMinDistance(MfAiColNode *, MfDistResult &, float) = 0;
    virtual bool BeginConstruction() = 0;
    virtual bool AddNode(MfAiColNode *) = 0;
    virtual bool EndConstruction() = 0;
    virtual MfAiColReport *CreateColReport() const = 0;
    virtual MfAiColInterface *CreateColInterface(int) const = 0;
    virtual bool SetUserData(MfAi *) = 0;
    virtual MfAi *GetUserData() = 0;
};
