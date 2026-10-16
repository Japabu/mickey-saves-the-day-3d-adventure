// MfMsg and the message data blocks (RuntimeLib.dll 10063d10-100648d0).
// Neither is exported; the symbols are the names the classes give them.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfLiveActor.h"
#include "MfScDataHandle.h"
#include "MfSkeletonTraversal.h"

// guards the reference counts of the message data blocks (owned by the
// message dispatcher, which deletes it)
extern CRITICAL_SECTION gMsgDataLock;

// 0x10063d10 ??0MfMsg@@QAE@XZ
MfMsg::MfMsg()
{
    mID = 0;
    mFromID = 0;
    mToID = 0;
    mFrom = 0;
    mTo = 0;
    mData = 0;
    mArg2 = 0;
    mArg1 = 0;
    m18 = 0;
    m24 = 1;
    m28 = 0;
    mToSelf = 0;
    mIsTimer = 0;
    mTimerID = 0;
    m38 = -1;
    m40 = 0;
    mObject = 0;
}

// 0x10063d60 ??_GMfMsg@@UAEPAXI@Z

// 0x10063d80 ??0MfMsg@@QAE@HPAVMfRootObj@@0@Z
MfMsg::MfMsg(int id, MfRootObj *from, MfRootObj *to)
{
    mID = id;
    if (from)
        mFromID = from->mID;
    if (to)
        mToID = to->mID;
    mFrom = from;
    mTo = to;
    mData = 0;
    mArg2 = 0;
    mArg1 = 0;
    m18 = 0;
    m24 = 1;
    m28 = 0;
    mToSelf = 0;
    mIsTimer = 0;
    mTimerID = 0;
    m38 = -1;
    m40 = 0;
    mObject = 0;
}

// 0x10063df0 ??0MfMsg@@QAE@HPAVMfRootObj@@0HH@Z
MfMsg::MfMsg(int id, MfRootObj *from, MfRootObj *to, int fromID, int toID)
{
    mID = id;
    mFromID = fromID;
    mToID = toID;
    mFrom = from;
    mTo = to;
    mData = 0;
    mArg2 = 0;
    mArg1 = 0;
    m18 = 0;
    m24 = 1;
    m28 = 0;
    mToSelf = 0;
    mIsTimer = 0;
    mTimerID = 0;
    m38 = -1;
    m40 = 0;
    mObject = 0;
}

// 0x10063e50 ??0MfMsg@@QAE@HHHPAVMfRootObj@@0@Z
MfMsg::MfMsg(int id, int arg1, int arg2, MfRootObj *from, MfRootObj *to)
{
    mID = id;
    if (from)
        mFromID = from->mID;
    else
        mFromID = 0;
    if (arg2 == 0 && to)
        mToID = to->mID;
    else
        mToID = 0;
    mFrom = from;
    mTo = to;
    mArg2 = arg2;
    mData = 0;
    mArg1 = arg1;
    m18 = 0;
    m24 = 1;
    m28 = 0;
    mToSelf = 0;
    mIsTimer = 0;
    mTimerID = 0;
    m38 = -1;
    m40 = 0;
    mObject = 0;
}

// 0x10063ed0 ??1MfMsg@@UAE@XZ
MfMsg::~MfMsg()
{
    if (mData) {
        EnterCriticalSection(&gMsgDataLock);
        InterlockedDecrement(&mData->mRefCount);
        if (mData->mRefCount == 0)
            delete mData;
        LeaveCriticalSection(&gMsgDataLock);
    }
}

// 0x10063f20 ?SetData@MfMsg@@QAEXPAVMfMsgDataBlock@@@Z
void MfMsg::SetData(MfMsgDataBlock *data)
{
    if (data && !mData) {
        mData = data;
        InterlockedIncrement(&data->mRefCount);
    }
}

// 0x10063f40 ??0MfDeleteActorReqData@@QAE@PAVMfLiveActor@@H@Z
MfDeleteActorReqData::MfDeleteActorReqData(MfLiveActor *actor, int flags)
{
    mActor = actor;
    mFlags = flags;
}

// 0x10063f60 ??_GMfDeleteActorReqData@@UAEPAXI@Z

// 0x10063f80 ??0MfCollisionMsgData@@QAE@PAVMfLiveActor@@0PAVMfSkeletonNode@@1PAVMfSkill@@MVMfVector3@@33H@Z
MfCollisionMsgData::MfCollisionMsgData(MfLiveActor *actor, MfLiveActor *other,
                                       MfSkeletonNode *node, MfSkeletonNode *otherNode,
                                       MfSkill *skill, float playBack, MfVector3 point,
                                       MfVector3 direction, MfVector3 normal, int eventID)
{
    mActor = actor;
    mOther = other;
    mNode = node;
    mOtherNode = otherNode;
    mSkill = skill;
    mPlayBack = playBack;
    mPoint = point;
    mDirection = direction;
    mNormal = normal;
    mEventID = eventID;
}

// 0x10064020 ??0MfProximityEventData@@QAE@PAX0H@Z
MfProximityEventData::MfProximityEventData(void *actor, void *other, int event)
{
    mActor = actor;
    mOther = other;
    mEvent = event;
}

// 0x10064050 ??0MfLocoPlanReqData@@QAE@HPAVMfLiveActor@@HHHABVMfVector3@@1HHHHHHHHHHHHHHHHHH@Z
MfLocoPlanReqData::MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target,
                                     int locoSkill, int skill, const MfVector3 &goal,
                                     const MfVector3 &start, int floor, int goalFloor,
                                     int a10, int a11, int a12, int a13, int a14, int a15,
                                     int a16, int a17, int a18, int a19, int a20, int a21,
                                     int a22, int a23, int a24, int a25)
{
    mActor = actor;
    // the vectors are copied z first
    mGoal.z() = goal.z();
    mGoal.x() = goal.x();
    mGoal.y() = goal.y();
    mStart.z() = start.z();
    mStart.x() = start.x();
    mStart.y() = start.y();
    mFloor = floor;
    mGoalFloor = goalFloor;
    m28 = a10;
    m2c = a25;
    m3c = a11;
    mLocoSkill = locoSkill;
    mSkill = skill;
    m30 = a12;
    m48 = a13;
    m4c = a14;
    m40 = a15;
    m50 = a16;
    m54 = a17;
    mRequestID = requestID;
    m44 = a22;
    m58 = a23;
    mTarget = target;
    m6c = a24;
    m5c = a18;
    m60 = a19;
    m64 = a20;
    m68 = a21;
}

// 0x10064140 ??0MfLocoPlanResultData@@QAE@HHHHPAVMfPath2@@HABHH@Z
MfLocoPlanResultData::MfLocoPlanResultData(int requestID, int status, int a3, int a4,
                                           MfPath2 *path, int a6, const int &a7, int a8)
{
    mStatus = status;
    m10 = a3;
    m14 = a4;
    mPath = path;
    m1c = a7;
    mRequestID = requestID;
    m20 = a8;
    m24 = a6;
}

// 0x10064190 ??_GMfLocoPlanResultData@@UAEPAXI@Z

// 0x100641b0 ??1MfLocoPlanResultData@@UAE@XZ
MfLocoPlanResultData::~MfLocoPlanResultData()
{
}

// a path the planner made (only its destructor is needed here)
class MfPath2 {
public:
    ~MfPath2();
};

// 0x100641c0 ?FreeContents@MfLocoPlanResultData@@UAEXXZ
void MfLocoPlanResultData::FreeContents()
{
    if (mPath)
        delete mPath;
}

// 0x100641e0 ??0MfPlannerFailureData@@QAE@HHHH@Z
MfPlannerFailureData::MfPlannerFailureData(int requestID, int detail, int planner, int reason)
{
    mDetail = detail;
    mPlanner = planner;
    mReason = reason;
    mRequestID = requestID;
}

// 0x10064210 ??0MfManipGCReqData@@QAE@HPAVMfLiveActor@@0HPAVMfTransform3@@ABV2@HHPAVMfManipSkill@@PAVMfHandle@@H2@Z
MfManipGCReqData::MfManipGCReqData(int requestID, MfLiveActor *actor, MfLiveActor *target,
                                   int node, MfTransform3 *config, const MfTransform3 &base,
                                   int a7, int a8, MfManipSkill *skill, MfHandle *handle,
                                   int a11, const MfTransform3 &goal)
{
    mActor = actor;
    mTarget = target;
    mNode = node;
    if (config) {
        int n = target->skeletonTraversal()->numNodes();
        mConfig = new MfTransform3[n];
        for (int i = 0; i < n; i++)
            mConfig[i] = config[i];
    } else {
        mConfig = 0;
    }
    mBase = base;
    m38 = a7;
    m3c = a8;
    mSkill = skill;
    mHandle = handle;
    m64 = a11;
    mGoal = goal;
    mRequestID = requestID;
}

// 0x100643d0 ??_GMfManipGCReqData@@UAEPAXI@Z

// 0x100643f0 ??1MfManipGCReqData@@UAE@XZ
MfManipGCReqData::~MfManipGCReqData()
{
    if (mConfig)
        delete [] mConfig;
}

// 0x10064460 ??0MfManipGCResultData@@QAE@HHHHPAVMfManipPlan@@H@Z
MfManipGCResultData::MfManipGCResultData(int requestID, int status, int a3, int a4,
                                         MfManipPlan *plan, int a6)
{
    mStatus = status;
    m10 = a3;
    m14 = a4;
    mPlan = plan;
    mRequestID = requestID;
    m1c = a6;
}

// 0x10064190 ??_GMfManipGCResultData@@UAEPAXI@Z (folded with MfLocoPlanResultData's)

// a manipulation plan (only its destructor is needed here)
class MfManipPlan {
public:
    ~MfManipPlan();
};

// 0x100644a0 ?FreeContents@MfManipGCResultData@@UAEXXZ
void MfManipGCResultData::FreeContents()
{
    if (mPlan)
        delete mPlan;
}

// 0x100644c0 ??0MfManipMoveRequestData@@QAE@HPAVMfLiveActor@@0PAVMfManipSkill@@H@Z
MfManipMoveRequestData::MfManipMoveRequestData(int requestID, MfLiveActor *actor,
                                               MfLiveActor *target, MfManipSkill *skill,
                                               int a5)
{
    mActor = actor;
    mTarget = target;
    mSkill = skill;
    m18 = a5;
    mRequestID = requestID;
}

// 0x10064500 ??0MfManipMoveResultData@@QAE@HHHHHPAUMfMTPath@@@Z
MfManipMoveResultData::MfManipMoveResultData(int requestID, int a2, int target, int skill,
                                             int release, MfMTPath *path)
{
    m08 = a2;
    mTarget = (MfLiveActor *)target;
    mSkill = (MfManipSkill *)skill;
    mRelease = release;
    mPath = path;
    mRequestID = requestID;
}

MfManipGCResultData::~MfManipGCResultData()
{
}

MfManipMoveResultData::~MfManipMoveResultData()
{
}

// 0x10064540 ?FreeContents@MfManipMoveResultData@@UAEXXZ
void MfManipMoveResultData::FreeContents()
{
    MfMTPath *path = mPath;
    if (path) {
        if (path->mFrom)
            delete [] path->mFrom;
        if (path->mTo)
            delete [] path->mTo;
        delete path;
    }
}

// 0x100645a0 ??0MfCheckReachableReqData@@QAE@HPAVMfLiveActor@@0HPAVMfTransform3@@ABV2@HHHH2HABVMfVector3@@@Z
MfCheckReachableReqData::MfCheckReachableReqData(int requestID, MfLiveActor *actor,
                                                 MfLiveActor *target, int a4,
                                                 MfTransform3 *config,
                                                 const MfTransform3 &base, int a7, int a8,
                                                 int a9, int floor, const MfTransform3 &goal,
                                                 int skill, const MfVector3 &offset)
{
    mActor = actor;
    mTarget = target;
    m14 = a4;
    if (config) {
        int n = target->skeletonTraversal()->numNodes();
        mConfig = new MfTransform3[n];
        for (int i = 0; i < n; i++)
            mConfig[i] = config[i];
    } else {
        mConfig = 0;
    }
    mBase = base;
    m38 = a7;
    m3c = a8;
    m40 = a9;
    mGoal = goal;
    mFloor = floor;
    mRequestID = requestID;
    mSkill = skill;
    mOffset = offset;
}

// 0x10064780 ??_GMfCheckReachableReqData@@UAEPAXI@Z

// 0x100647a0 ??1MfCheckReachableReqData@@UAE@XZ
MfCheckReachableReqData::~MfCheckReachableReqData()
{
    if (mConfig)
        delete [] mConfig;
}

// 0x10064810 ??0MfCheckReachableResultData@@QAE@HPAVMfLiveActor@@HABVMfVector3@@@Z
MfCheckReachableResultData::MfCheckReachableResultData(int requestID, MfLiveActor *actor,
                                                       int reachable, const MfVector3 &point)
{
    mActor = actor;
    mReachable = reachable;
    mPoint.z() = point.z();
    mPoint.x() = point.x();
    mPoint.y() = point.y();
    mRequestID = requestID;
}

// 0x10064860 ??1MfMsgDataBlock@@UAE@XZ

// 0x10064870 ??0MfPiccoloHandleData@@QAE@PAVMfScDataHandle@@@Z
MfPiccoloHandleData::MfPiccoloHandleData(MfScDataHandle *handle)
    : mHandle(handle)
{
    handle->AddRef();
}

// 0x10064890 ??_GMfPiccoloHandleData@@UAEPAXI@Z

// 0x100648b0 ??1MfPiccoloHandleData@@UAE@XZ
MfPiccoloHandleData::~MfPiccoloHandleData()
{
    mHandle->Release();
}
