// MfAiActor: an actor as the application sees it.
// Declaration from the SteamboatApp PDB.
// GetClassName and SendMessage are windows.h macros: the exported names are
// GetClassNameA and SendMessageA. PlaySound keeps its name (no mmsystem.h).
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAIACTOR_H
#define RUNTIMELIB_MFAIACTOR_H

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "MfAi.h"
class MfAiActivity;
class MfAiActivitySpec;
class MfAiActorNode;
class MfAiController;
class MfAiKChain;
class MfAiPath2;
class MfAiSkill;
class MfAiSpace;
struct MfCollisionData;
struct MfContourFailDownData;
class MfDblList;
struct MfFilterItem;
class MfISoundResource;
struct MfPickData;
class MfProperty;
class MfPropertyTable;
class MfProtoActor;
struct MfProximityData;
class MfRotation3;
class MfScDataHandle;
class MfString;
class MfStringList;
class MfTransform3;
class MfVector3;

class MfAiActor : public MfAi {
public:
    virtual const MfString &GetClassName() = 0;
    virtual int SetInstanceName(const MfString &) = 0;
    virtual MfString GetInstanceName() = 0;
    virtual int &UniqueID() = 0;
    virtual MfAiSpace *GetSpace() = 0;
    virtual int GetProperty(const MfString &, MfProperty &) = 0;
    virtual int GetProperty(const MfString &, MfProperty **) = 0;
    virtual void SetProperty(MfProperty &) = 0;
    virtual void SetProperty(MfProperty *) = 0;
    virtual void GetPropertyNames(MfStringList *) = 0;
    virtual MfPropertyTable &Properties() = 0;
    virtual void GetCylinder(float &, float &) = 0;
    virtual void GetSphere(float &, MfVector3 &) = 0;
    virtual int ActivateBehavior(const MfString &) = 0;
    virtual int DeactivateBehavior(const MfString &) = 0;
    virtual int LoadBehavior(const MfString &, int) = 0;
    virtual int UnloadBehavior(const MfString &) = 0;
    virtual int GetCurStateName(MfString, MfString &, MfString &) = 0;
    virtual int AttachTarget(const MfAiActor *, const MfString &, const MfString &, int) = 0;
    virtual int DetachTarget(const MfAiActor *, int) = 0;
    virtual void SetActorAttributes(int) = 0;
    virtual int GetActorAttributes() = 0;
    virtual void ClearActivity() = 0;
    virtual int AddActivity(const MfAiActivity *) = 0;
    virtual int SetActivity(const MfAiActivity *) = 0;
    virtual MfAiActivity *GetRootActivity() = 0;
    virtual int RemoveActivity(const MfAiActivity *, int) = 0;
    virtual int RemoveActivity(int, int) = 0;
    virtual MfAiActorNode *GetRootNode() = 0;
    virtual MfAiActorNode *GetPrimaryNode() = 0;
    virtual MfAiActorNode *GetNodeByName(const MfString &) = 0;
    virtual MfAiActorNode *GetNodeByIndex(int) = 0;
    virtual int GetNumNodes() = 0;
    virtual void GetChildActors(MfDblList *) = 0;
    virtual MfAiActorNode *GetNodeUnderRoot() = 0;
    virtual void EnableBehavior(int) = 0;
    virtual MfAiSkill *GetSkill(const MfString &) = 0;
    virtual int GetNumSkills() = 0;
    virtual int LoadSkill(const MfString &, int) = 0;
    virtual void FlushSkill(const MfString &) = 0;
    virtual void FlushAllSkills() = 0;
    virtual void LoadAllSkills() = 0;
    virtual void SetupSkillCache(int) = 0;
    virtual int SendEvent(const MfAiActor *, const MfString &, unsigned long) = 0;
    virtual int SendEvent(const MfAiActor *, MfScDataHandle *, unsigned long) = 0;
    virtual int SendMessage(const MfAiActor *, const MfString &) = 0;
    virtual int StartTimer(int, const MfString &, int, int) = 0;
    virtual int StartTimer(int, MfScDataHandle *, int, int) = 0;
    virtual int StopTimer(int) = 0;
    virtual MfAiActor *GetLastSender() = 0;
    virtual int GetLastEventID() = 0;
    virtual const MfString &GetLastMessage() = 0;
    virtual MfAiActivity *GetLastActivity() = 0;
    virtual const MfCollisionData *GetLastCollisionInfo() = 0;
    virtual const MfProximityData *GetLastProximityInfo() = 0;
    virtual const MfPickData *GetLastPickInfo() = 0;
    virtual const MfContourFailDownData *GetLastContourFailDownInfo() = 0;
    virtual void FacePoint(const MfVector3 &) = 0;
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, int) = 0;
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int) = 0;
    virtual int RequestLocoPlan(const MfAiActor *, const MfString &, const MfString &, float, float, float, float, int) = 0;
    virtual int RequestGraspPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int) = 0;
    virtual int RequestPlacePlan(const MfAiActor *, const MfVector3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int) = 0;
    virtual int RequestPlacePlan(const MfAiActor *, const MfTransform3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int) = 0;
    virtual int RequestTouchPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int) = 0;
    virtual int RequestSitPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int) = 0;
    virtual int RequestPosePlan(const MfAiActor *, const MfString &, int) = 0;
    virtual int RequestUngraspPlan(const MfAiActor *, const MfString &, int) = 0;
    virtual int SampleAttachSkill(const MfTransform3 &, const MfString &, unsigned long) = 0;
    virtual void UndoSample() = 0;
    virtual MfAiActivity *CreateHoldActivity(const MfAiActor *, int) = 0;
    virtual MfAiActivity *CreateComplyActivity(const MfAiActor *, const MfString &, const MfString &, int, int) = 0;
    virtual MfAiActivity *CreateGestureActivity(const MfString &, int, int) = 0;
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfAiActor *, const MfString &, int) = 0;
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfVector3 &, int) = 0;
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int) = 0;
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int, const MfVector3 *, int) = 0;
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, const MfAiActorNode *, int) = 0;
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfDblList &, int, const MfVector3 *, int) = 0;
    virtual MfAiActivity *CreateOverlayActivity() = 0;
    virtual MfAiActivity *CreateRandomActivity(int) = 0;
    virtual MfAiActivity *CreateSequentialActivity(int) = 0;
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, int, int) = 0;
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, float, int, int) = 0;
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfVector3 &, float, float, int, int) = 0;
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfAiPath2 *, float, float, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, float, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfTransform3 &, const MfTransform3 &, float, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfVector3 &, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, MfTransform3 *, int, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, MfTransform3 *, int, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfAiActor *, const MfString &, int, int) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfAiActor *, const MfString &, int, int) = 0;
    virtual MfAiActivity *CreateAttachActivity(const MfTransform3 &, const MfString &, int, int) = 0;
    virtual MfAiActivity *CreatePluginActivity(const MfAiActivitySpec *, int, int) = 0;
    virtual int GetLastLocoFailureInfo() = 0;
    virtual int GetLastManipFailureInfo() = 0;
    virtual void SetUserData(MfAi *) = 0;
    virtual MfAi *GetUserData() = 0;
    virtual MfISoundResource *PlaySound(const MfString &, MfAiActorNode *) = 0;
    virtual void HandleLButtonDown(MfPickData *) = 0;
    virtual void HandleLButtonUp(MfPickData *) = 0;
    virtual void HandleRButtonDown(MfPickData *) = 0;
    virtual void HandleRButtonUp(MfPickData *) = 0;
    virtual void HandleMouseMove(MfPickData *) = 0;
    virtual int ChangeTexture(const MfString &, int, const MfString &, const MfString &) = 0;
    virtual int GetLastOptionID() = 0;
    virtual void SetOptionTarget(const MfAiActor *) = 0;
    virtual MfScDataHandle *GetPiccoloHandle() = 0;
    virtual void SyncTraceUnderHip() = 0;
    virtual void SetPlannedActivityOptions(int, int) = 0;
    virtual int SetProximityDefinition(const MfString &, float, float, float, int) = 0;
    virtual void SetForwardFacingDirection(const MfRotation3 &) = 0;
    virtual int SnapTraceToSurface(const MfVector3 &, int, const MfVector3 &) = 0;
    virtual void SetController(MfAiController *) = 0;
    virtual MfAiController *GetController() = 0;
    virtual MfAiKChain *CreateKChain(const MfString &, int, const MfVector3 &) = 0;
    virtual MfProtoActor *GetPrototype() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual MfDblList *Filter() = 0;
    virtual int DeleteFilterItem(MfFilterItem *) = 0;
    virtual int AddFilterItem(MfFilterItem *) = 0;
    virtual void ClearFilter() = 0;
    virtual int SetAnchorNodeStop(MfAiActorNode *) = 0;
    virtual MfAiActorNode *GetAnchorNodeStop() = 0;
    virtual MfAiActor *OwnerActor() = 0;
    virtual MfAiActorNode *GetGeomNodeByIndex(int) = 0;
    virtual int GetNumGeomNodes() = 0;
};

#endif
