// The data blocks RuntimeLib's messages carry (MfMsg::SetData): requests to
// the world and the planners, and their results. Class names from the RTTI
// type descriptors; layouts from the constructors at 10063f40-10064870.
// Member names follow the callers (MfWorld::RequestDestroyActor,
// MfSpace::ReportCollisionEvents, MfLiveActor::RequestLocoPlan ...).
#ifndef RUNTIMELIB_MFMSGDATA_H
#define RUNTIMELIB_MFMSGDATA_H
#include "MfMsg.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfHandle;
class MfLiveActor;
class MfManipPlan;
class MfManipSkill;
class MfPath2;
class MfScDataHandle;
class MfSkeletonNode;
class MfSkill;
class MfSpace;

// MfWorld::RequestDestroyActor
class MfDeleteActorReqData : public MfMsgDataBlock {
public:
    MfDeleteActorReqData(MfLiveActor *actor, int flags);

    MfLiveActor *mActor;                                    // 0x08
    int mFlags;                                             // 0x0c
};

// MfWorld::RequestDestroySpace
class MfDeleteSpaceReqData : public MfMsgDataBlock {
public:
    MfDeleteSpaceReqData(MfSpace *space) { mSpace = space; }

    MfSpace *mSpace;                                        // 0x08
};

// MfSpace::ReportCollisionEvents and the activities' _CheckBubble; the
// receivers (MfLiveActor::HandleGestureCollision, HandleReceivedCollision)
// copy it into their MfCollisionData: the skill's name, mPlayBack to playBack
// and mEventID to the last event ID
class MfCollisionMsgData : public MfMsgDataBlock {
public:
    MfCollisionMsgData(MfLiveActor *actor, MfLiveActor *other, MfSkeletonNode *node,
                       MfSkeletonNode *otherNode, MfSkill *skill, float playBack, MfVector3 point,
                       MfVector3 direction, MfVector3 normal, int eventID);

    MfLiveActor *mActor;                                    // 0x08
    MfLiveActor *mOther;                                    // 0x0c
    MfSkeletonNode *mNode;                                  // 0x10 a node of mActor
    MfSkeletonNode *mOtherNode;                             // 0x14 a node of mOther
    MfVector3 mPoint;                                       // 0x18
    MfVector3 mDirection;                                   // 0x24
    MfVector3 mNormal;                                      // 0x30
    MfSkill *mSkill;                                        // 0x3c the gesture that collided
    float mPlayBack;                                        // 0x40
    int mEventID;                                           // 0x44
};

// MfProximityManager
class MfProximityEventData : public MfMsgDataBlock {
public:
    MfProximityEventData(void *actor, void *other, int event);

    void *mActor;                                           // 0x08
    void *mOther;                                           // 0x0c
    int mEvent;                                             // 0x10 MfLiveActor::HandleProximityEvent: path blocked, path clear, approach, retreat
};

// MfLiveActor::RequestLocoPlan
class MfLocoPlanReqData : public MfMsgDataBlock {
public:
    MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target, int locoSkill, int skill,
                      const MfVector3 &goal, const MfVector3 &start, int floor, int goalFloor,
                      int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17,
                      int a18, int a19, int a20, int a21, int a22, int a23, int a24, int a25);

    int mRequestID;                                         // 0x08
    MfLiveActor *mActor;                                    // 0x0c
    MfVector3 mStart;                                       // 0x10
    MfVector3 mGoal;                                        // 0x1c
    int m28, m2c, m30;                                      // 0x28
    int mLocoSkill;                                         // 0x34
    int mSkill;                                             // 0x38
    int m3c, m40, m44, m48, m4c, m50, m54, m58;             // 0x3c
    int m5c, m60, m64, m68, m6c;                            // 0x5c
    int mFloor;                                             // 0x70
    int mGoalFloor;                                         // 0x74
    int mTarget;                                            // 0x78
};

// the dispatcher's answer to an MfLocoPlanReqData
class MfLocoPlanResultData : public MfMsgDataBlock {
public:
    MfLocoPlanResultData(int requestID, int status, int a3, int a4, MfPath2 *path, int a6,
                         const int &a7, int a8);
    virtual ~MfLocoPlanResultData();
    virtual void FreeContents();

    int mRequestID;                                         // 0x08
    int mStatus;                                            // 0x0c
    int m10;                                                // 0x10
    int m14;                                                // 0x14
    MfPath2 *mPath;                                         // 0x18
    int m1c;                                                // 0x1c
    int m20;                                                // 0x20
    int m24;                                                // 0x24
};

// MfLiveActor::PostPlannerFailure and the dispatcher
class MfPlannerFailureData : public MfMsgDataBlock {
public:
    MfPlannerFailureData(int requestID, int detail, int planner, int reason);

    int mRequestID;                                         // 0x08
    int mPlanner;                                           // 0x0c
    int mReason;                                            // 0x10
    int mDetail;                                            // 0x14
};

// MfLiveActor::RequestGCPlan
class MfManipGCReqData : public MfMsgDataBlock {
public:
    MfManipGCReqData(int requestID, MfLiveActor *actor, MfLiveActor *target, int node,
                     MfTransform3 *config, const MfTransform3 &base, int a7, int a8,
                     MfManipSkill *skill, MfHandle *handle, int a11, const MfTransform3 &goal);
    ~MfManipGCReqData();

    int mRequestID;                                         // 0x08
    MfLiveActor *mActor;                                    // 0x0c
    MfLiveActor *mTarget;                                   // 0x10
    int mNode;                                              // 0x14
    MfTransform3 *mConfig;                                  // 0x18 one transform per node of mTarget
    MfTransform3 mBase;                                     // 0x1c
    int m38, m3c;                                           // 0x38
    MfManipSkill *mSkill;                                   // 0x40
    MfHandle *mHandle;                                      // 0x44
    MfTransform3 mGoal;                                     // 0x48
    int m64;                                                // 0x64
};

class MfManipGCResultData : public MfMsgDataBlock {
public:
    MfManipGCResultData(int requestID, int status, int a3, int a4, MfManipPlan *plan, int a6);
    virtual ~MfManipGCResultData();
    virtual void FreeContents();

    int mRequestID;                                         // 0x08
    int mStatus;                                            // 0x0c
    int m10;                                                // 0x10
    int m14;                                                // 0x14
    MfManipPlan *mPlan;                                     // 0x18
    int m1c;                                                // 0x1c
};

// MfLiveActor::RequestManipMoveTo
class MfManipMoveRequestData : public MfMsgDataBlock {
public:
    MfManipMoveRequestData(int requestID, MfLiveActor *actor, MfLiveActor *target,
                           MfManipSkill *skill, int a5);

    int mRequestID;                                         // 0x08
    MfLiveActor *mActor;                                    // 0x0c
    MfLiveActor *mTarget;                                   // 0x10
    MfManipSkill *mSkill;                                   // 0x14
    int m18;                                                // 0x18
};

// a planned manipulation move (MfLiveActor::MakeManipActivity): the
// configurations along the way, two transform arrays
struct MfMTPath {
    MfTransform3 *mFrom;                                    // 0x00
    MfTransform3 *mTo;                                      // 0x04
};

// the planner's answer to an MfManipMoveRequestData
class MfManipMoveResultData : public MfMsgDataBlock {
public:
    MfManipMoveResultData(int requestID, int a2, int target, int skill, int release, MfMTPath *path);
    virtual ~MfManipMoveResultData();
    virtual void FreeContents();

    int m08;                                                // 0x08
    MfLiveActor *mTarget;                                   // 0x0c
    MfManipSkill *mSkill;                                   // 0x10
    int mRelease;                                           // 0x14 the move ends the grasp
    MfMTPath *mPath;                                        // 0x18
    int mRequestID;                                         // 0x1c
};

// MfLiveActor::RequestCheckReachable
class MfCheckReachableReqData : public MfMsgDataBlock {
public:
    MfCheckReachableReqData(int requestID, MfLiveActor *actor, MfLiveActor *target, int a4,
                            MfTransform3 *config, const MfTransform3 &base, int a7, int a8,
                            int a9, int floor, const MfTransform3 &goal, int skill,
                            const MfVector3 &offset);
    ~MfCheckReachableReqData();

    int mRequestID;                                         // 0x08
    MfLiveActor *mActor;                                    // 0x0c
    MfLiveActor *mTarget;                                   // 0x10
    int m14;                                                // 0x14
    MfTransform3 *mConfig;                                  // 0x18 one transform per node of mTarget
    MfTransform3 mBase;                                     // 0x1c
    int m38, m3c, m40;                                      // 0x38
    MfTransform3 mGoal;                                     // 0x44
    int mFloor;                                             // 0x60
    int mSkill;                                             // 0x64
    MfVector3 mOffset;                                      // 0x68
};

class MfCheckReachableResultData : public MfMsgDataBlock {
public:
    MfCheckReachableResultData(int requestID, MfLiveActor *actor, int reachable,
                               const MfVector3 &point);

    int mRequestID;                                         // 0x08
    MfLiveActor *mActor;                                    // 0x0c
    int mReachable;                                         // 0x10
    MfVector3 mPoint;                                       // 0x14
};

// a node of an actor could not follow the surface down (handled by
// MfLiveActor::HandleConformFailure)
class MfContourFailMsgData : public MfMsgDataBlock {
public:
    MfContourFailMsgData(MfSkeletonNode *node) { mNode = node; }
    MfSkeletonNode *mNode;                                  // 0x08
};

// a string message (MfLiveActor::sendMsg, received by HandleStrMessage)
class MfStrMessageData : public MfMsgDataBlock {
public:
    MfStrMessageData(int id, const MfString &message) { mID = id; mMessage = message; }
    virtual ~MfStrMessageData() { FreeContents(); }

    int mID;                                                // 0x08
    MfString mMessage;                                      // 0x0c
};

// a Piccolo object sent as an event (MfLiveActor::SendEvent, StartTimer)
class MfPiccoloHandleData : public MfMsgDataBlock {
public:
    MfPiccoloHandleData(MfScDataHandle *handle);
    virtual ~MfPiccoloHandleData();

    MfScDataHandle *mHandle;                                // 0x08
};

#endif
