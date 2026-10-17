// MfPoActor: the Piccolo object of an actor ("Actor"): its properties, the
// activities, plans and skills a script can request, events and timers.
#ifndef MFPOLIBRARY_MFPOACTOR_H
#define MFPOLIBRARY_MFPOACTOR_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfPoBase.h"
class MfAiActor;

class MFPO_API MfPoActor : public MfPoBase {
public:
    MfPoActor(MfAiActor *actor);
    virtual ~MfPoActor();
    virtual void AddRef();
    virtual void Release();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfAiActor *GetActorPtr() { return mActor; }
    void SetActorDeleted() { mActor = (MfAiActor *)-1; }
private:
    MfPoActor(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pelPropExtensions; }
    MfScDataHandle *PMActivateBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAddActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAddFilterItem(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAttachTarget(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMChangeTexture(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMClearActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMClearFilter(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCorrectPlacement(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreate3dPathActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateAttachActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateCameraActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateComplyActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateGestureActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateHoldActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateKChain(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateLocoActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateOrientActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateOverlayActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateRandomActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateSequentialActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDeactivateBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDeleteFilterItem(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDetachTarget(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDisableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDisplayText(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMEnableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMFacePoint(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMFlushAllSkills(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMFlushSkill(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetChildActors(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetCurStateName(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetLastCollisionInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetLastContourFailInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetLastPickInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetLastProximityInfo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetNodeByIndex(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetNodeByName(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetSkill(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadAllSkills(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadSkill(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRemoveActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestGraspPlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestLocoPlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestPlacePlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestPosePlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestSitPlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestTouchPlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRequestUngraspPlan(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSampleAttachSkill(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSendEvent(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSendMsg(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetForwardFacingDirection(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetPlannedActivityOptions(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetProximityDefinition(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetupSkillCache(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSnapTraceToSurface(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStartTimer(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStopTimer(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSyncTraceUnderHip(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMUndoSample(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMUnloadBehavior(MfScDataHandle *, int, MfScDataHandle **);
    static MfScPropertyExtList *pelPropExtensions;
    static MfPoActor *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    static int nExemplarRef;
    MfAiActor *mActor;                                      // 0x14
};

#endif
