// MfAiColNode: a collision node as the application sees it.
// Declaration from the SteamboatApp PDB.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef MFPOLIBRARY_MFAICOLNODE_H
#define MFPOLIBRARY_MFAICOLNODE_H

#include "MfAi.h"
#include "enums.h"
class MfAiActorNode;
class MfAiColInterface;
class MfAiColReport;
class MfColModel;
struct MfColResultNode;
struct MfColResultRay;
struct MfColResultSphere;
struct MfDistResult;
class MfTransform3;
class MfVector3;

class MfAiColNode : public MfAi {
public:
    virtual MfTransform3 &Transform() = 0;
    virtual MfColModel *&ColModel() = 0;
    virtual int GetCheckStyle() = 0;
    virtual void SetCheckStyle(int) = 0;
    virtual MfAiActorNode *&Owner() = 0;
    virtual bool CheckCollision(MfAiColNode *, MfColResultNode &) = 0;
    virtual bool CheckCollision(MfVector3 &, MfVector3 &, MfColResultRay &, bool, float) = 0;
    virtual bool CheckCollision(MfVector3 &, float, MfColResultSphere &) = 0;
    virtual bool CheckContainment(MfVector3 &, float) = 0;
    virtual bool GetMinDistance(MfAiColNode *, MfDistResult &, float) = 0;
    virtual bool BeginConstruction() = 0;
    virtual bool AddNode(MfAiColNode *) = 0;
    virtual bool EndConstruction() = 0;
    virtual MfAiColReport *CreateColReport() const = 0;
    virtual MfAiColInterface *CreateColInterface(MfColInterfaceType) const = 0;
    virtual bool SetUserData(MfAi *) = 0;
    virtual MfAi *GetUserData() = 0;
};

#endif
