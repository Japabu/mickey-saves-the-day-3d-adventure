// MfActor: an actor (a skeleton of MfSkeletonNodes built from an MfProtoActor,
// with skills, properties and activities) and the MfAiActor the application
// sees. Layout from the constructor ??0MfActor@@QAE@XZ; vtable order from
// ??_7MfActor@@6BMfRootObj@@@ and ??_7MfActor@@6BMfAiActor@@@.
#ifndef RUNTIMELIB_MFACTOR_H
#define RUNTIMELIB_MFACTOR_H
#include "MfAiActor.h"
#include "MfDblList.h"
#include "MfDictionary.h"
#include "MfLock.h"
#include "MfPropertyTable.h"
#include "MfRootObj.h"
#include "MfString.h"
#include "enums.h"
#include "RuntimeLib_api.h"
class MfAiActivity;
class MfAiActivitySpec;
class MfAiActorNode;
class MfAiController;
class MfAiKChain;
class MfAiPath2;
class MfAiSkill;
class MfAiSpace;
class MfColNode;
struct MfCollisionData;
struct MfContourFailDownData;
struct MfFilterItem;
class MfISoundResource;
class MfMaterial;
struct MfPickData;
class MfProperty;
class MfPropertyTable;
class MfProtoActor;
class MfProtoNode;
struct MfProximityData;
class MfRotation3;
class MfScDataHandle;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfSkill;
class MfSpace;
class MfStringList;
class MfTransform2;
class MfTransform3;
class MfTriangleSet;
class MfVector3;

class RUNTIME_API MfActor : public MfRootObj, public MfAiActor {
public:
    enum { TYPE = 'Acto' };
    virtual ~MfActor();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfRootObj::IsType(type);
    }
    virtual int SetSpace(MfSpace *space);
    virtual int RemoveSpace(MfSpace *space);
    virtual void HandleKeyboardEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardDownEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardUpEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardRepeatEvent(unsigned int, unsigned int);
    virtual void CorrectPlacement(MfTransform3 &, const MfVector3 &);

    // MfAi
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    // MfAiActor
    virtual const MfString &GetClassName();
    virtual int SetInstanceName(const MfString &);
    virtual MfString GetInstanceName();
    virtual int &UniqueID() { return mUniqueID; }
    virtual MfAiSpace *GetSpace();
    virtual int GetProperty(const MfString &, MfProperty &);
    virtual int GetProperty(const MfString &, MfProperty **);
    virtual void SetProperty(MfProperty &);
    virtual void SetProperty(MfProperty *);
    virtual void GetPropertyNames(MfStringList *);
    virtual MfPropertyTable &Properties() { return mProperties; }
    virtual void GetCylinder(float &, float &);
    virtual void GetSphere(float &, MfVector3 &);
    virtual int ActivateBehavior(const MfString &);
    virtual int DeactivateBehavior(const MfString &);
    virtual int LoadBehavior(const MfString &, int);
    virtual int UnloadBehavior(const MfString &);
    virtual int GetCurStateName(MfString, MfString &, MfString &) = 0;
    virtual int AttachTarget(const MfAiActor *, const MfString &, const MfString &, int) { return 0; }
    virtual int DetachTarget(const MfAiActor *, int);
    virtual void SetActorAttributes(int);
    virtual int GetActorAttributes() { return mActorAttributes; }
    virtual void ClearActivity();
    virtual int AddActivity(const MfAiActivity *);
    virtual int SetActivity(const MfAiActivity *);
    virtual MfAiActivity *GetRootActivity();
    virtual int RemoveActivity(const MfAiActivity *, int);
    virtual int RemoveActivity(int, int);
    virtual MfAiActorNode *GetRootNode();
    virtual MfAiActorNode *GetPrimaryNode();
    virtual MfAiActorNode *GetNodeByName(const MfString &);
    virtual MfAiActorNode *GetNodeByIndex(int);
    virtual int GetNumNodes();
    virtual void GetChildActors(MfDblList *);
    virtual MfAiActorNode *GetNodeUnderRoot();
    virtual void EnableBehavior(int);
    virtual MfAiSkill *GetSkill(const MfString &);
    virtual int GetNumSkills();
    virtual int LoadSkill(const MfString &, int);
    virtual void FlushSkill(const MfString &);
    virtual void FlushAllSkills();
    virtual void LoadAllSkills();
    virtual void SetupSkillCache(int);
    virtual int SendEvent(const MfAiActor *, const MfString &, unsigned long);
    virtual int SendEvent(const MfAiActor *, MfScDataHandle *, unsigned long);
    virtual int SendMessage(const MfAiActor *, const MfString &);
    virtual int StartTimer(int, const MfString &, int, int);
    virtual int StartTimer(int, MfScDataHandle *, int, int);
    virtual int StopTimer(int);
    virtual MfAiActor *GetLastSender() = 0;
    virtual int GetLastEventID();
    virtual const MfString &GetLastMessage();
    virtual MfAiActivity *GetLastActivity();
    virtual const MfCollisionData *GetLastCollisionInfo();
    virtual const MfProximityData *GetLastProximityInfo();
    virtual const MfPickData *GetLastPickInfo();
    virtual const MfContourFailDownData *GetLastContourFailDownInfo();
    virtual void FacePoint(const MfVector3 &);
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, int);
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestLocoPlan(const MfAiActor *, const MfString &, const MfString &, float, float, float, float, int);
    virtual int RequestGraspPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int);
    virtual int RequestPlacePlan(const MfAiActor *, const MfVector3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestPlacePlan(const MfAiActor *, const MfTransform3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestTouchPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int);
    virtual int RequestSitPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int);
    virtual int RequestPosePlan(const MfAiActor *, const MfString &, int);
    virtual int RequestUngraspPlan(const MfAiActor *, const MfString &, int);
    virtual int SampleAttachSkill(const MfTransform3 &, const MfString &, unsigned long);
    virtual void UndoSample();
    virtual MfAiActivity *CreateHoldActivity(const MfAiActor *, int);
    virtual MfAiActivity *CreateComplyActivity(const MfAiActor *, const MfString &, const MfString &, int, int);
    virtual MfAiActivity *CreateGestureActivity(const MfString &, int, int);
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfAiActor *, const MfString &, int);
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfVector3 &, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int, const MfVector3 *, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, const MfAiActorNode *, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfDblList &, int, const MfVector3 *, int);
    virtual MfAiActivity *CreateOverlayActivity();
    virtual MfAiActivity *CreateRandomActivity(int);
    virtual MfAiActivity *CreateSequentialActivity(int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfVector3 &, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfAiPath2 *, float, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfTransform3 &, const MfTransform3 &, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfVector3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, MfTransform3 *, int, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, MfTransform3 *, int, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfAiActor *, const MfString &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfAiActor *, const MfString &, int, int);
    virtual MfAiActivity *CreateAttachActivity(const MfTransform3 &, const MfString &, int, int);
    virtual MfAiActivity *CreatePluginActivity(const MfAiActivitySpec *, int, int);
    virtual int GetLastLocoFailureInfo();
    virtual int GetLastManipFailureInfo();
    virtual void SetUserData(MfAi *);
    virtual MfAi *GetUserData();
    virtual MfISoundResource *PlaySound(const MfString &, MfAiActorNode *);
    virtual void HandleLButtonDown(MfPickData *);
    virtual void HandleLButtonUp(MfPickData *);
    virtual void HandleRButtonDown(MfPickData *);
    virtual void HandleRButtonUp(MfPickData *);
    virtual void HandleMouseMove(MfPickData *);
    virtual int ChangeTexture(const MfString &, int, const MfString &, const MfString &);
    virtual int GetLastOptionID();
    virtual void SetOptionTarget(const MfAiActor *);
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual void SyncTraceUnderHip();
    virtual void SetPlannedActivityOptions(int, int);
    virtual int SetProximityDefinition(const MfString &, float, float, float, int);
    virtual void SetForwardFacingDirection(const MfRotation3 &);
    virtual int SnapTraceToSurface(const MfVector3 &, int, const MfVector3 &);
    virtual void SetController(MfAiController *);
    virtual MfAiController *GetController();
    virtual MfAiKChain *CreateKChain(const MfString &, int, const MfVector3 &);
    virtual MfProtoActor *GetPrototype() { return mPrototype; }
    virtual void Lock() { mLock.Lock(-1); }
    virtual void Unlock() { mLock.Unlock(); }
    virtual MfDblList *Filter();
    virtual int DeleteFilterItem(MfFilterItem *);
    virtual int AddFilterItem(MfFilterItem *);
    virtual void ClearFilter();
    virtual int SetAnchorNodeStop(MfAiActorNode *);
    virtual MfAiActorNode *GetAnchorNodeStop();
    virtual MfAiActor *OwnerActor();
    virtual MfAiActorNode *GetGeomNodeByIndex(int);
    virtual int GetNumGeomNodes();

    MfActor();
    MfColNode * MakeSubTreeColNode(int);
    MfDblList * skills();
    MfDictionary &NodeDictionary() { return mNodeDictionary; }
    MfProperty & property(MfString);
    MfPropertyTable &properties() { return mProperties; }
    MfProtoActor *Prototype() { return mPrototype; }
    MfSkeletonNode *skeleton() const { return mSkeleton; }
    MfSkeletonTraversal *allTraversal() const { return mAllTraversal; }
    MfSkeletonTraversal *skeletonTraversal() const { return mSkeletonTraversal; }
    MfSkill * GetSkillFromProperty(MfString const &);
    MfSkill * getSkill(MfString);
    MfSpace *Space();
    MfString const & InstanceName();
    MfString const & Name();
    MfTransform2 traceTransform2D();
    MfTriangleSet * getFloor();
    int Load(MfString, int, int);
    int Save(MfSkillFormat);
    int TempCopy() { return mTempCopy; }
    int getProperty(MfString, MfProperty &);
    static void __cdecl AddSkill(MfProtoActor *, MfSkill *);
    static void __cdecl AddSubtree(MfProtoActor *, MfProtoNode *, MfProtoNode *);
    static void __cdecl ChangeMaterial(MfProtoActor *, MfProtoNode *, int, MfMaterial *);
    static void __cdecl Collapse(MfProtoActor *, MfProtoNode *);
    static void __cdecl MoveSubtree(MfProtoActor *, MfProtoNode *, MfProtoNode *);
    static void __cdecl RemoveSubtree(MfProtoActor *, MfProtoNode *);
    static void __cdecl RenameNode(MfProtoActor *, MfProtoNode *, MfString);
    static void __cdecl ReplaceColModel(MfProtoActor *, MfProtoNode *, MfTriangleSet *);
    static void __cdecl ScaleSubtree(MfProtoActor *, MfProtoNode *, MfVector3 const &);
    static void __cdecl SetHomeTransforms(MfProtoActor *, MfTransform3 *);
    static void __cdecl TransformPivot(MfProtoActor *, MfProtoNode *, MfTransform3 const &);
    void AttachToPrototype(MfProtoActor *, int);
    void CreateRenderNodes();
    void DetachFromPrototype();
    void GotoHomeConfig(MfTransform3 const &);
    void GotoHomeConfig(int);
    void SetSkillToProperty(MfString const &, MfString const &);
    void SetSkillToProperty(MfString const &, int, MfSkill *);
    void SetStateChangeFlag(int flag) { mStateChangeFlag = flag; }
    void ShowNodes(int, int);
    void UseFilter(int, int, int, MfDblList &);
    void ignoreTriangleCollisionCheck(int);
    void presampleAllSkills(int);
    void setActorAttributes(int);
    void setProperty(MfProperty &);
    void setSkeleton(MfSkeletonNode *, int);
    static int usePlaybackActors;
protected:
    MfActor(MfActor const &);
    void AddSubtree(MfProtoNode *, int);
    void Collapse(int);
    void MoveSubtree(int, int);
    void RemoveSubtree(int);
    void ScaleSubtree(int, MfVector3 const &);
    void TransformPivot(int, MfTransform3 const &);

public:

    int mTempCopy;                                          // 0x18
    MfProtoActor *mPrototype;                               // 0x1c
    int mInstanceIndex;                                     // 0x20 position in the prototype's instance list
    int mUniqueID;                                          // 0x24
    int mActorAttributes;                                   // 0x28
    MfSkeletonNode *mSkeleton;                              // 0x2c
    MfSkeletonTraversal *mSkeletonTraversal;                // 0x30
    MfSkeletonTraversal *mAllTraversal;                     // 0x34
    MfDictionary mNodeDictionary;                           // 0x38
    MfPropertyTable mProperties;                            // 0x4c
    MfSpace *mSpace;                                        // 0x64
    long mRefCount;                                         // 0x68
    MfAi *mUserData;                                        // 0x6c
    int mStateChangeFlag;                                   // 0x70
    MfString mInstanceName;                                 // 0x74
    MfString mName;                                         // 0x78
    MfLock mLock;                                           // 0x7c
    MfDblList mFilter;                                      // 0x80
};

#endif
