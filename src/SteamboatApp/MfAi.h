#pragma once
// MfAi.h: the Motivate AI interfaces the game talks to (MfActorLib.dll): every object of the
// AI world (MfAi), its actors (MfAiActor) and spaces (MfAiSpace). Declarations from
// SteamboatApp.pdb; all members are pure virtual.
class MfAiActivity;
class MfAiActor;
class MfAiSpace;
class MfAiActivitySpec;
class MfAiActorNode;
class MfAiColNode;
struct MfAiColReport;
class MfAiController;
class MfAiKChain;
class MfAiPath2;
class MfAiSkill;
class MfAiWorld;
struct MfColResultRay;
struct MfCollisionData;
struct MfContactRecord;
struct MfContourFailDownData;
class MfDblList;
struct MfFilterItem;
class MfISoundResource;
struct MfPickData;
class MfProperty;
class MfPropertyTable;
class MfProtoActor;
struct MfProximityData;
class MfRenderScene;
class MfRotation3;
class MfScDataHandle;
class MfString;
class MfStringList;
class MfTransform3;
class MfVector3;

#include "../mfutil/MfAi.h"


// own copy: MfPoLibrary's MfAi*.h open MfPoLibrary/MfAi.h, a second MfAi next to mfutil's,
// and its MfAiActor.h opens <windows.h>, whose macros would rename game members in the units
// that do not include it.
class MfAiActor : public MfAi {
public:
    virtual const MfString &GetClassNameA() = 0;
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
    virtual int SendMessageA(const MfAiActor *, const MfString &) = 0;
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
    virtual MfISoundResource *PlaySoundA(const MfString &, MfAiActorNode *) = 0;
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


// own copy: as above.
class MfAiSpace : public MfAi {
public:
    virtual void GetActorList(MfDblList *, const MfString &) = 0;
    virtual MfAiActor *GetActor(const MfString &) = 0;
    virtual unsigned char CastRay(MfVector3 &, MfVector3 &, MfColResultRay &, unsigned char, unsigned char) = 0;
    virtual unsigned char CheckCollision(const MfAiActor *, const MfDblList *, MfContactRecord &) = 0;
    virtual unsigned char CheckSphereContainsGeometry(MfVector3 &, float) = 0;
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
    virtual void ShowAxis(int, unsigned char) = 0;
    virtual int ShowGrid(int) = 0;
    virtual void ShowGrid(int, unsigned char) = 0;
    virtual void SetCellSpacing(float) = 0;
    virtual MfPropertyTable &Properties() = 0;
};

// The view's event handler interface (MfInterfaceRegistry name "MfAiViewEventHandler").
// own copy: as above.
class MfAiViewEventHandler : public MfAi {
public:
    virtual void SetSpace(MfAiSpace *) = 0;             // vftable+0x0c
    virtual MfAiSpace *GetSpace() = 0;                  // vftable+0x10
};

class MfAiAppWorld;
class MfAiKeyboard;
class MfAiMouse;
class MfMaterial;
class MfRenderer;
class MfSoundManager;

// own copy: as above.
class MfAiWorld : public MfAi {
public:
    virtual int CreateUniqueID() = 0;
    virtual void SetDataPath(const MfString &) = 0;
    virtual void Start(int) = 0;
    virtual void Stop() = 0;
    virtual void Continue() = 0;
    virtual void Reset() = 0;
    virtual int IsRunning() = 0;
    virtual MfAiSpace *CreateSpace() = 0;
    virtual MfAiSpace *LoadSpace(const MfString &, long) = 0;
    virtual int CloseSpace(const MfAiSpace *) = 0;
    virtual int SaveSpace(const MfAiSpace *) = 0;
    virtual int RegisterActorPrototype(MfProtoActor *) = 0;
    virtual MfAiActor *InstantiateActor(MfProtoActor *) = 0;
    virtual MfAiActor *LoadActor(const MfString &, const MfAiSpace *, const MfTransform3 &, int) = 0;
    virtual int TeleportActor(MfAiActor *, const MfString &, int, const MfTransform3 &) = 0;
    virtual int LoadActorGroup(const MfString &) = 0;
    virtual void DeleteActor(const MfAiActor *) = 0;
    virtual int SaveActorGroup(const MfAiActor *, const MfString &) = 0;
    virtual int SaveActor(const MfAiActor *) = 0;
    virtual MfAiActor *GetActor(int) = 0;
    virtual void ProcessMessages() = 0;
    virtual int UpdateAllSpaces(unsigned long) = 0;
    virtual void GetAllSpaces(MfDblList *) = 0;
    virtual void SetRenderable(int) = 0;
    virtual void DisableToolMouseBehavior() = 0;
    virtual void EnableToolMouseBehavior() = 0;
    virtual int GetProperty(const MfString &, MfProperty &) = 0;
    virtual int GetProperty(const MfString &, MfProperty **) = 0;
    virtual void SetProperty(MfProperty &) = 0;
    virtual void SetProperty(MfProperty *) = 0;
    virtual void GetPropertyNames(MfStringList *) = 0;
    virtual MfMaterial *CreateMaterial() = 0;
    virtual int GetInstances(MfProtoActor *, MfDblList &) = 0;
};

// own copy: as above.
class MfAiRuntime : public MfAi {
public:
    virtual int Initialize(const char *, const char *) = 0;
    virtual int Shutdown() = 0;
    virtual MfAiKeyboard *GetKeyboard() = 0;
    virtual MfAiMouse *GetMouse() = 0;
    virtual MfAiWorld *CreateWorld(MfAiAppWorld *) = 0;
    virtual MfAiWorld *GetWorld() = 0;
    virtual int InitializeJoysticks(void *, void *) = 0;
    virtual void PollJoysticks() = 0;
    virtual MfSoundManager *LoadSound(void *, void *, MfString) = 0;
    virtual MfRenderer *LoadRenderer(void *, void *, MfString) = 0;
};

// what the application gives MfAiRuntime::CreateWorld: the world asks it for the clock
// own copy: as above.
class MfAiAppWorld : public MfAi {
public:
    virtual unsigned long GetClock() = 0;
};

// creates the runtime (MfActorLib.dll)
void MfInitializeRuntime(MfAiRuntime **runtime);
