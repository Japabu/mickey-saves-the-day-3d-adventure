// MfLiveActor: an actor that lives in a space: behaviour (HFSM), activities,
// timers, planning, events and input. Layout from
// ??0MfLiveActor@@QAE@W4MF_THREAD_NAME@@@Z; vtables ??_7MfLiveActor@@6BMfRootObj@@@
// and ??_7MfLiveActor@@6BMfAiActor@@@ (new virtuals at slots 14-21 of the first).
#ifndef RUNTIMELIB_MFLIVEACTOR_H
#define RUNTIMELIB_MFLIVEACTOR_H
#include "MfActor.h"
#include "MfActorData.h"
#include "MfAiActor.h"
#include "MfDblList.h"
#include "MfIndex.h"
#include "MfManagerActivity.h"
#include "MfMsg.h"
#include "MfManipPlanCache.h"
#include "MfLocoPlanCache.h"
#include "MfSkeletonNode.h"
#include "MfString.h"
#include "MfVector3.h"
#include "enums.h"
#include "MfGCPath.h"
#include "RuntimeLib_api.h"
class MfActivity;
class MfAiRenderView;
class MfFSMManager;
class MfFloor;
class MfHandle;
class MfLocoSkill;
class MfManipSkill;
struct MfMTPath;
class MfPath2;
class MfSkill;
class MfTransform2;
class MfScDataHandle;

// what mConfigRecord points to: MfManipPlanner::GetConfigRecord creates it,
// the destructor deletes it
struct MfConfigRecord {
    int m00, m04, m08;                                      // 0x00
    MfTransform3 m0c;                                       // 0x0c
    int m28, m2c;                                           // 0x28
    MfTransform3 m30;                                       // 0x30
    MfGCPath mBuffers;                                      // 0x4c
    int m90;                                                // 0x90 set when the record matched
};

// an actor attached to one of this actor's nodes (an entry of mChildList)
struct MfChildEntry {
    MfChildEntry() { mActor = 0; mHandle = 0; mNode = 0; }

    MfActor *mActor;                                        // 0x00
    MfHandle *mHandle;                                      // 0x04 the handle it is held by
    MfSkeletonNode *mNode;                                  // 0x08 the node that holds it
    int mHand;                                              // 0x0c the hand's node index
    MfManipSkill *mSkill;                                   // 0x10 the skill that holds it
    MfTransform3 mOffset;                                   // 0x14 relative to mNode
};

class RUNTIME_API MfLiveActor : public MfActor {
public:
    typedef void (MfLiveActor::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'LAct' };  // 0x4c416374

    MfLiveActor(MF_THREAD_NAME thread);
    virtual ~MfLiveActor();

    // MfRootObj
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        if (msg.mIsTimer)
            RemoveTimerMessage(msg.mTimerID);
        (this->*handler)(msg);
    }
    virtual MfString GetName();
    virtual void DefaultMsgHandler(MfMsg &msg);
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActor::IsType(type);
    }
protected:
    virtual void InitMsgTable();
public:
    // MfActor
    virtual int SetSpace(MfSpace *space);
    virtual int RemoveSpace(MfSpace *space);
    virtual void HandleKeyboardEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardDownEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardUpEvent(unsigned int, unsigned int);
    virtual void HandleKeyboardRepeatEvent(unsigned int, unsigned int);
    virtual void CorrectPlacement(MfTransform3 &, const MfVector3 &);
    // new virtuals
    virtual int LiveUpdate(long clock);
    virtual void GoLive(int);
    virtual void ContinueLive();
    virtual void GoAuthor();
    virtual void HandleJoystickEvent(int);
    virtual int SetControlMode(int mode);
    virtual void ShowActionMenu(MfAiRenderView *);
    virtual int changeTexture(const MfString &, int, const MfString &, const MfString &);

    // MfAiActor
    virtual int SetInstanceName(const MfString &name);
    virtual int ActivateBehavior(const MfString &);
    virtual int DeactivateBehavior(const MfString &);
    virtual int LoadBehavior(const MfString &, int);
    virtual int UnloadBehavior(const MfString &);
    virtual int GetCurStateName(MfString machine, MfString &state, MfString &sub);
    virtual void ClearActivity();
    virtual int AddActivity(const MfAiActivity *);
    virtual int SetActivity(const MfAiActivity *);
    virtual MfAiActivity *GetRootActivity() { return mRootActivity; }
    virtual int RemoveActivity(const MfAiActivity *, int);
    virtual int RemoveActivity(int, int);
    virtual void EnableBehavior(int);
    virtual int SendEvent(const MfAiActor *, const MfString &, unsigned long);
    virtual int SendEvent(const MfAiActor *, MfScDataHandle *, unsigned long);
    virtual int SendMessage(const MfAiActor *, const MfString &);
    virtual int StartTimer(int, const MfString &, int, int);
    virtual int StartTimer(int, MfScDataHandle *, int, int);
    virtual int StopTimer(int);
    virtual MfAiActor *GetLastSender();
    virtual int GetLastEventID() { return mLastEventID; }
    virtual const MfString &GetLastMessage() { return mLastMessage; }
    virtual MfAiActivity *GetLastActivity();
    virtual const MfCollisionData *GetLastCollisionInfo();
    virtual const MfProximityData *GetLastProximityInfo();
    virtual const MfPickData *GetLastPickInfo() { return &mLastPick; }
    virtual const MfContourFailDownData *GetLastContourFailDownInfo() { return &mLastContourFailDown; }
    virtual int GetLastLocoFailureInfo() { return mLastLocoFailure; }
    virtual int GetLastManipFailureInfo() { return mLastManipFailure; }
    virtual void HandleLButtonDown(MfPickData *);
    virtual void HandleLButtonUp(MfPickData *);
    virtual void HandleRButtonDown(MfPickData *);
    virtual void HandleRButtonUp(MfPickData *);
    virtual void HandleMouseMove(MfPickData *);
    virtual int GetLastOptionID() { return mLastOptionID; }
    virtual void SetOptionTarget(const MfAiActor *target) { plaPendingOptionTarget = (MfLiveActor *)target; }
    virtual void SyncTraceUnderHip();
    virtual void SetPlannedActivityOptions(int, int);
    virtual void SetController(MfAiController *);
    virtual MfAiController *GetController() { return mController; }
    virtual int SetAnchorNodeStop(MfAiActorNode *);
    virtual MfAiActorNode *GetAnchorNodeStop() { return mAnchorNodeStop; }
protected:
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual MfAiActor *OwnerActor() { return mOwner; }
public:

    MfDblList &ChildList() { return mChildList; }
    unsigned long LastTimeForLoco() const { return mLastTimeForLoco; }
    int IsSitting() { return mSitting; }
    void SetIsSitting(int sitting) { mSitting = sitting; }
    int IsLive() { return mLive; }
    int IsGoingLive() { return mGoingLive; }
    void SetCmdState(int state) { mCmdState = state; }
    int IsCmdState(int state) { return mCmdState == state; }
    int GetControlMode() const { return mControlMode; }
    MfActivity *CurrentActivity() { return (MfActivity *)mRootActivity; }
    void SetEventID(int id) { mLastEventID = id; }
    MfFSMManager *GetHFSMManager() { return mHFSM; }
    int GetBlockCount() { return mBlockList.GetCount(); }
    MfTransform3 *LastConfig();

    void ClearOtherActorsActivityThatReferToThisActor();
    void MakeCopy(MfLiveActor *copy);
    void DetachChildren();
    void CleanupChildList(MfLiveActor *);
    void ChangeParent(MfSkeletonNode *, MfManipSkill *, MfHandle *, int);
    int NoIntentionOfLocomoting();
    void ComputeManipInfo(float &, MfTransform3 &);
    void AllocateLastConfig();
    int IsMoving();
    void ChangeCmdState();
    MfActivity *IsCurrentActivity(int type);
    void CleanupActivity(int);
    void LoadHFSM(int);
    int OldLoadHFSM();
    void LoadBehaviors(int);
    void SaveBehaviorsForPlayback(MfString &);
    MfString ActorMethodFile();
    void LoadActorMethods();
    void SetupPiccoloHandles();
    void ApplySnapOn(MfTransform3 &);
    MfFloor *AppropriateFloor(MfDblList *);
    MfFloor *AppropriateFloor(MfVector3, MfDblList *, MfFloor *, int);
    void HandleStaticActorChange(MfMsg &msg);
    void HandleOption(int);
    int sendMsg(const MfAiActor *, const MfString &);
    int sendEvent(const MfLiveActor *, MfMsg *, unsigned long);
    int sendEvent(const MfAiActor *, const MfString &, unsigned long);
    MfString SoundPath();
protected:
    void RefreshChildList(MfActor *, MfSkeletonNode *, MfManipSkill *, MfHandle *);
    void UpdateChildList(MfActor *, MfSkeletonNode *, MfManipSkill *, MfHandle *);
    void ApplyActivity(MfActivity *);
    void Handle3rdPartyMsg(MfMsg &);
    void HandleStrMessage(MfMsg &);
    void HandleMiscEvents(MfMsg &);
    int CurrentManipIntention(MfLiveActor *, MfManipSkill *);
    int AttachToFloor();
    void PostPlannerFailure(int);
    void HandlePlannerFailure(MfMsg &);
    MfActivity *MakeGestureActivity(unsigned long, MfSkill *, int);
    void HandleGestureCollision(MfMsg &);
    void HandleReceivedCollision(MfMsg &);
    void HandleConformFailure(MfMsg &);
    int RequestLocoPlan(int, MfLocoSkill *, MfSkill *, const MfTransform2 &, MfFloor *, int, int, int,
                        MfLiveActor *, float, float, int, float, float, int, int);
    void LocoFailure(MfMsg &);
    void HandleLocoPlan(MfMsg &);
    MfActivity *MakeLocoActivity(MfPath2 *, MfLocoSkill *, unsigned long, int, float, float,
                                 MfSkeletonNode *, int);
    int RequestManipGCPlan(int, MfLiveActor *, MfFloor *, MfManipSkill *, MfManipSkill *, MfLocoSkill *,
                           MfSkill *, MfHandle *, int, int, const MfTransform3 &,
                           const MfVector3 &, int);
    void RequestCheckReachable(MfLiveActor *, MfFloor *, MfManipSkill *, MfHandle *, int,
                               const MfTransform3 &);
    void HandleCheckReachable(MfMsg &);
    void RequestGCPlan(MfLiveActor *, MfManipSkill *, MfHandle *, int, const MfTransform3 &);
    void HandleManipGC(MfMsg &);
    int RequestManipMoveTo(int, MfLiveActor *, MfManipSkill *, int, int);
    void HandleManipMoveTo(MfMsg &);
    MfSkeletonNode *NewParentAfterUngrasp(MfManipSkill *, MfLiveActor *);
    int VerifyMoveObjSkill(MfManipSkill *, MfLiveActor *, MfManipSkill *);
    int VerifyGraspSkill(MfManipSkill *, MfLiveActor *);
    int VerifyPlaceSkill(MfManipSkill *, MfLiveActor *, int, MfHandle *&);
    void ManipFailure(MfMsg &);
    MfActivity *MakeManipActivity(MfLiveActor *, MfGCPath *, MfMTPath *, int, MfManipSkill *,
                                  MfHandle *, MfSkeletonNode *, unsigned long, int);
    void RequestLock(MfLiveActor *, int);
    int ApplyLock(MfLiveActor *);
    MfActivity *MakeFaceActivity(MfActor *, MfHandle *);
    MfActivity *MakeHoldTargetActivity(MfDblList *, int, MfManipSkill *);
    void HandleProximityEvent(MfMsg &);
    void RespondToBlockedPath(MfLiveActor *);
    void RespondToClearPath(MfLiveActor *);
    void RespondToApproach(MfLiveActor *);
    void RespondToRetreat(MfLiveActor *);
    void HandleActivityFinished(MfMsg &);
    void _PmClearTimers();
    int UpdateFSMWithEvent();
    void UpdateFSMThatSitManipFinished();
    void PostTimerMessage(int, int, int, MfMsg *);
    void RemoveTimerMessage(int);
    int ActivityToEnum(int);
    int SkillToEnum(int);
    int ManipSkillToEnum(int);
    int CheckActivity(MfActivity *, int, MfString, int);
    int CheckSkill(int, MfSkill *, int, int);
    int CheckSkill(int, const MfString &, MfSkill *&, int, int);
    int CheckManipSkillType(int, MfManipSkill *, int);
    int CheckTargetActor(int, const MfAiActor *);
    void UpdateAppEventHandler(MfMsg &);
public:

    static MfLiveActor *plaPendingOptionTarget;
    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;

    MsgHandler *mMsgTable;                                  // 0x90
    MfConfigRecord *mConfigRecord;                          // 0x94
    int mLive;                                              // 0x98
    int mGoingLive;                                         // 0x9c
    int ma0;                                                // 0xa0
    int mSitting;                                           // 0xa4
    unsigned long mLastTimeForLoco;                         // 0xa8
    int mLocoRequest;                                       // 0xac the pending planner requests
    int mGCRequest;                                         // 0xb0
    int mReachRequest;                                      // 0xb4
    int mMoveRequest;                                       // 0xb8
    int mLockRequest;                                       // 0xbc
    int mCmdState;                                          // 0xc0
    MfManipPlanCache mManipPlan;                            // 0xc4
    MfLocoPlanCache mLocoPlan;                              // 0x148
    MfFSMManager *mHFSM;                                    // 0x214
    int m218;                                               // 0x218
    MfIndex mTimers;                                        // 0x21c
    MfCollisionData mLastCollision;                         // 0x230
    MfProximityData mLastProximity;                         // 0x268
    MfPickData mLastPick;                                   // 0x26c
    MfContourFailDownData mLastContourFailDown;             // 0x280
    MfScDataHandle *mPiccoloHandle;                         // 0x284
    MfDblList mChildList;                                   // 0x288
    int mControlMode;                                       // 0x298
    MfActor *mOwner;                                        // 0x29c
    MfManagerActivity *mRootActivity;                       // 0x2a0
    MF_THREAD_NAME mThread;                                 // 0x2a4
    MfTransform3 *mLastConfig;                              // 0x2a8 global transforms before the update
    int mLastConfigSize;                                    // 0x2ac
    int mRequestID;                                         // 0x2b0 the last planner request
    unsigned int mLastKey;                                  // 0x2b4
    unsigned int mLastKeyFlags;                             // 0x2b8
    int mLastEventID;                                       // 0x2bc
    MfActivity *mLastActivity;                              // 0x2c0
    int m2c4;                                               // 0x2c4
    int mLastOptionID;                                      // 0x2c8
    MfString mLastMessage;                                  // 0x2cc
    int mLastLocoFailure;                                   // 0x2d0
    int mLastManipFailure;                                  // 0x2d4
    int mNoLocoIntention;                                   // 0x2d8
    int mAuthorAttributes;                                  // 0x2dc saved by GoAuthor
    int mAuthorNodeCount;                                   // 0x2e0
    int *mAuthorCheckStyles;                                // 0x2e4
    int *mAuthorRenderStyles;                               // 0x2e8
    MfDblList mBlockList;                                   // 0x2ec
    int mPlannedLocoOptions;                                // 0x2fc SetPlannedActivityOptions(1, ...)
    int mPlannedManipOptions;                               // 0x300 SetPlannedActivityOptions(2, ...)
    float mPlannedLocoParam;                                // 0x304 0x200: three times the bounding height
    float mPlannedLocoReach;                                // 0x308 the same
    MfAiController *mController;                            // 0x30c
    MfSkeletonNode *mAnchorNodeStop;                        // 0x310
};

#endif
