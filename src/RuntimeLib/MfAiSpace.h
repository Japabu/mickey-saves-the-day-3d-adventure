// MfAiSpace: a space (scene) as the application sees it.
// Declaration from the SteamboatApp PDB.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAISPACE_H
#define RUNTIMELIB_MFAISPACE_H

#include "MfAi.h"
class MfAiActor;
class MfAiActorNode;
class MfAiColNode;
class MfAiColReport;
class MfAiWorld;
struct MfColResultRay;
class MfContactRecord;
class MfDblList;
class MfPropertyTable;
class MfRenderScene;
class MfScDataHandle;
class MfString;
class MfTransform3;
class MfVector3;

class MfAiSpace : public MfAi {
public:
    virtual void GetActorList(MfDblList *, const MfString &) = 0;
    virtual MfAiActor *GetActor(const MfString &) = 0;
    virtual bool CastRay(MfVector3 &, MfVector3 &, MfColResultRay &, bool, bool) = 0;
    virtual bool CheckCollision(const MfAiActor *, const MfDblList *, MfContactRecord &) = 0;
    virtual bool CheckSphereContainsGeometry(MfVector3 &, float) = 0;
    virtual MfAiColNode *CreateColModel() = 0;
    virtual MfRenderScene *GetRenderScene() = 0;
    virtual const MfString &GetSpaceName() const = 0;
    virtual void SetSpaceName(const MfString &) = 0;
    virtual MfAiActor *GetLastAddedActor() = 0;
    virtual const MfString &GetLastRemovedActor() = 0;
    virtual MfAiWorld *GetWorld() = 0;
    virtual void StartUpdate() = 0;
    virtual void StopUpdate() = 0;
    virtual MfAiColNode *GetCurrentSceneCollisionModel() = 0;
    virtual void ReportCollisionEvents(const MfAiColReport &) = 0;
    virtual int UpdateEnvironmentModel(int) = 0;
    virtual int &GoesLiveWithWorld() = 0;
    virtual int AddActor(const MfAiActor *, const MfTransform3 &) = 0;
    virtual int RemoveActor(const MfAiActor *) = 0;
    virtual void ReorderActorUpdate(MfAiActor *, MfAiActor *) = 0;
    virtual MfScDataHandle *GetPiccoloHandle() = 0;
    virtual MfAiActorNode *GetRootNode() = 0;
    virtual int ShowAxis(int) = 0;
    virtual void ShowAxis(int, bool) = 0;
    virtual int ShowGrid(int) = 0;
    virtual void ShowGrid(int, bool) = 0;
    virtual void SetCellSpacing(float) = 0;
    virtual MfPropertyTable &Properties() = 0;
};

#endif
