// MfMsgDispatcher: RuntimeLib's message dispatcher (RuntimeLib.dll
// 10068190-10069380).
#include "MfMsgDispatcher.h"
#include <windows.h>
#include <process.h>
#include <iostream.h>
#include "MfLiveActor.h"
#include "MfLocoPlanner.h"
#include "MfManipPlanner.h"
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfMsgQueue.h"
#include "MfRootObj.h"
#include "MfSharedMsgQueue.h"
#include "MfSpace.h"
#include "MfTimedMsgQueue.h"

// guards the shared queues
CRITICAL_SECTION gQueueLock;
MfMsgDispatcher *g_msgDispatcher;

// the worker threads: locomotion and manipulation planning (100688e0,
// 10069170)
void LocoPlanningThread(void *queue);
void ManipPlanningThread(void *queue);

// 0x10068190 ??0MfMsgDispatcher@@QAE@XZ
MfMsgDispatcher::MfMsgDispatcher()
    : m0c(0), m24(0), mStopped(0), mProcessing(0), mObjects(53), mIDs(53), mCurrent(0), mRunning(0)
{
    mTimedQueue = new MfTimerQueue;
    mQueue = new MfMsgQueue;
    mSharedQueues = new MfSharedMsgQueue *[4];
    mSharedQueues[0] = new MfSharedMsgQueue(-1, "Shared THREAD_ACTOR Message Queue");
    mSharedQueues[1] = new MfSharedMsgQueue(-1, "Shared THREAD_ACTOR_CONTINUOUS Message Queue");
    mSharedQueues[2] = new MfSharedMsgQueue(25, "Locomotion Planning Work Queue");
    mSharedQueues[3] = new MfSharedMsgQueue(25, "Manipulation Planning Work Queue");
    g_msgDispatcher = this;
    mThreads[0] = _beginthread(LocoPlanningThread, 0, mSharedQueues[2]);
    mThreads[1] = _beginthread(ManipPlanningThread, 0, mSharedQueues[3]);
    InitializeCriticalSection(&gQueueLock);
    InitializeCriticalSection((CRITICAL_SECTION *)mLocks[0]);
    InitializeCriticalSection((CRITICAL_SECTION *)mLocks[1]);
}

// 0x10068390 ?Register@MfMsgDispatcher@@QAEXPAVMfRootObj@@@Z
void MfMsgDispatcher::Register(MfRootObj *obj)
{
    if (!mObjects.Find(obj->mID)) {
        mObjects.Add(obj->mID, obj);
        mIDs.Add((unsigned long)obj, (void *)obj->mID);
        mIDList.Add((void *)obj->mID);
    }
}

// 0x100683e0 ?Unregister@MfMsgDispatcher@@QAEXPAVMfRootObj@@@Z
void MfMsgDispatcher::Unregister(MfRootObj *obj)
{
    MfDblListItem *pli;
    for (void *id = mIDList.GetFirst(pli); id; id = mIDList.GetNext(pli))
        if ((int)id == obj->mID) {
            mIDList.Delete(pli);
            break;
        }
    mObjects.Delete(obj->mID);
    mIDs.Delete((unsigned long)obj);
}

// 0x10068440 ?Find@MfMsgDispatcher@@QAEPAVMfRootObj@@H@Z
MfRootObj *MfMsgDispatcher::Find(int id)
{
    return (MfRootObj *)mObjects.Find(id);
}

// 0x10068460 ?FindID@MfMsgDispatcher@@QAEHPAVMfRootObj@@@Z
int MfMsgDispatcher::FindID(MfRootObj *obj)
{
    return (int)mIDs.Find((unsigned long)obj);
}

// 0x10068480 ?Shutdown@MfMsgDispatcher@@QAEXXZ
void MfMsgDispatcher::Shutdown()
{
    mSharedQueues[2]->Clear();
    mSharedQueues[3]->Clear();
    mSharedQueues[2]->Put(new MfMsg(3, 0, 0, (MfRootObj *)0, (MfRootObj *)0));
    mSharedQueues[3]->Put(new MfMsg(3, 0, 0, (MfRootObj *)0, (MfRootObj *)0));
    Sleep(1000);
    mStopped = 1;
}

// EQUIV 0x10068560 ??1MfMsgDispatcher@@QAE@XZ: reviewed: register choice only, every instruction and stack slot agrees (register choice (the original keeps this in ebx, the index in esi and the queue in edi))
MfMsgDispatcher::~MfMsgDispatcher()
{
    for (int i = 0; i < 4; i++)
        if (mSharedQueues[i])
            delete mSharedQueues[i];
    delete mSharedQueues;
    if (mTimedQueue)
        delete mTimedQueue;
    if (mQueue)
        delete mQueue;
    DeleteCriticalSection(&gQueueLock);
    DeleteCriticalSection((CRITICAL_SECTION *)mLocks[0]);
    DeleteCriticalSection((CRITICAL_SECTION *)mLocks[1]);
}

// 0x10068640 ?ProcessMessages@MfMsgDispatcher@@QAEHXZ
int MfMsgDispatcher::ProcessMessages()
{
    if (mStopped == 1 || mProcessing == 1)
        return 0;
    mProcessing = 1;
    if (mRunning == 1)
        mTimedQueue->Update();
    mQueue->Rewind();
    do {
        mCurrent = (MfMsg *)mQueue->GetNext(&mLast);
        if (mCurrent) {
            mQueue->RemoveCurrent();
            if (mCurrent->mTo) {
                if (Find(mCurrent->mToID))
                    mCurrent->mTo->CatchMsg(*mCurrent);
                else if (mCurrent->mData)
                    mCurrent->mData->FreeContents();
            } else if (mCurrent->mToSelf == 1) {
                if (Find(mCurrent->mFromID))
                    SendToAll(mCurrent);
                else if (mCurrent->mData)
                    mCurrent->mData->FreeContents();
            }
            if (mCurrent->m24 == 1 && mCurrent)
                delete mCurrent;
        }
    } while (mLast != 1);
    if (mRunning == 1 && mSharedQueues[0]->mSharedCount > 0)
        while ((mCurrent = mSharedQueues[0]->Get(0)) != 0)
            mQueue->Put(mCurrent);
    if (mSharedQueues[1]->mSharedCount > 0)
        while ((mCurrent = mSharedQueues[1]->Get(0)) != 0)
            mQueue->Put(mCurrent);
    mProcessing = 0;
    return 1;
}

// 0x100687b0 ?Start@MfMsgDispatcher@@QAEXXZ
void MfMsgDispatcher::Start()
{
    mRunning = 1;
    mSharedQueues[2]->Clear();
    mSharedQueues[3]->Clear();
    mTimedQueue->Restart();
}

// 0x100687e0 ?Resume@MfMsgDispatcher@@QAEXXZ
void MfMsgDispatcher::Resume()
{
    mRunning = 1;
}

// 0x100687f0 ?Pause@MfMsgDispatcher@@QAEXXZ
void MfMsgDispatcher::Pause()
{
    mRunning = 0;
}

// 0x10068800 ?SendToAll@MfMsgDispatcher@@QAEXPAVMfMsg@@@Z
void MfMsgDispatcher::SendToAll(MfMsg *msg)
{
    MfDblListItem *pli;
    for (void *id = mIDList.GetFirst(pli); id; id = mIDList.GetNext(pli)) {
        MfRootObj *obj = (MfRootObj *)mObjects.Find((unsigned long)id);
        if (obj)
            obj->CatchMsg(*msg);
    }
}

// 0x10068850 ?Post@MfMsgDispatcher@@QAEXPAVMfMsg@@@Z
void MfMsgDispatcher::Post(MfMsg *msg)
{
    if (msg->m24 == 1)
        msg->m28 = 1;
    if (!msg->mArg2) {
        if (!msg->mArg1)
            mQueue->Put(msg);
        else
            mSharedQueues[0]->Put(msg);
    } else
        mSharedQueues[msg->mArg2]->Put(msg);
}

// 0x100688a0 ?Cancel@MfMsgDispatcher@@QAEXPAVMfMsg@@@Z
void MfMsgDispatcher::Cancel(MfMsg *msg)
{
    if (msg != mCurrent)
        mQueue->Remove(msg);
}

// 0x100688c0 ?FlushPending@MfMsgDispatcher@@QAEXXZ
void MfMsgDispatcher::FlushPending()
{
    mSharedQueues[2]->Clear();
    mSharedQueues[3]->Clear();
}

// TODO 0x100688e0 ?LocoPlanningThread@@YAXPAX@Z: (82% similar) calls and arguments agree; the original
// pushes the float words with mov/push, ours loads the *(float *)& casts through fld/fstp
// once the registers run out (VC5 does mov/push for float-typed fields, so the original's
// view of the request probably has float members), and the registers differ from the
// quit test on (69% similar)
// The locomotion planning thread: plans the requests of its queue and posts
// the answers until the quit message comes.
// the request keeps the planner's float arguments as raw words
#define FLOAT_OF(word) (*(float *)&(word))

void LocoPlanningThread(void *arg)
{
    MfSharedMsgQueue *queue = (MfSharedMsgQueue *)arg;
    MfLocoPlanner *planner = new MfLocoPlanner;
    MfMsg *msg;
    for (;;) {
        msg = queue->Get(1);
        if (!msg)
            continue;
        if (msg->mID == 3)
            break;
        cout << "new loco plan message" << endl;
        MfLocoPlanReqData *req = (MfLocoPlanReqData *)msg->mData;
        MfLiveActor *actor = req->mActor;
        MfLocoPlanResult *result;
        if (req->m3c)
            result = planner->PlanToTarget(actor, req->mTarget, FLOAT_OF(req->m5c), FLOAT_OF(req->m60),
                                           FLOAT_OF(req->m64), FLOAT_OF(req->m68),
                                           (MfSkeletonNode *)req->mLocoSkill, req->mSkill, (MfTransform2 *)&req->mGoal,
                                           (MfLocoSkill *)req->mFloor, req->m28, (MfApiActor *)req->m30,
                                           FLOAT_OF(req->m48), (float *)&req->m4c, req->m40, FLOAT_OF(req->m50),
                                           FLOAT_OF(req->m54), req->m44, (MfDblList *)req->m58);
        else
            result = planner->Plan(actor, req->mTarget, FLOAT_OF(req->m5c), FLOAT_OF(req->m60), FLOAT_OF(req->m64),
                                   FLOAT_OF(req->m68), (MfSkeletonNode *)req->mLocoSkill, req->mSkill,
                                   (MfTransform2 *)&req->mGoal, (MfTransform2 *)&req->mStart,
                                   (MfLocoSkill *)req->mFloor, req->m28, FLOAT_OF(req->m2c), 0, req->m44,
                                   (MfDblList *)req->m58, 0);
        actor->Release();
        if (req->m58)
            delete (MfMsgDataBlock *)req->m58;
        MfPath2 *path = result->mPath;
        MfMsg *reply;
        if (path) {
            reply = new MfMsg(20, 2, 0, (MfRootObj *)0, actor);
            reply->SetData(new MfLocoPlanResultData(req->mRequestID, (int)actor, req->mFloor, req->mLocoSkill,
                                                    path, req->m4c, req->m6c, req->mGoalFloor));
        } else {
            reply = new MfMsg(16, 2, 0, (MfRootObj *)0, actor);
            reply->SetData(new MfPlannerFailureData(req->mRequestID, (int)actor, 5, result->mFailure));
        }
        reply->mEventID = result->m08;
        reply->m40 = req->mGoalFloor;
        g_msgDispatcher->Post(reply);
        delete msg;
        if (result)
            delete result;
    }
    if (msg)
        delete msg;
    if (planner)
        delete planner;
    _endthread();
}

// 0x10068bc0 ?CheckReachable@@YAXPAVMfManipPlanner@@PAVMfMsg@@@Z
// The manipulation thread's answer to an MfCheckReachableReqData.
static void CheckReachable(MfManipPlanner *planner, MfMsg *msg)
{
    // where the actor walks to first: floor x, y and heading
    MfTransform2 place;
    MfCheckReachableReqData *req = (MfCheckReachableReqData *)msg->mData;
    MfLiveActor *actor = req->mActor;
    if (!g_msgDispatcher->FindID(actor))
        return;
    InterlockedIncrement(&actor->mBusy);
    InterlockedIncrement(&actor->mSpace->mBusy);
    int reachable = planner->CheckReachable(actor, (MfManipSkill *)req->m3c, req->mTarget, req->m14,
                                            req->mConfig, req->mBase, (MfSkeletonNode *)req->m38,
                                            (MfHandle *)req->m40, (MfVector3 &)place, req->mFloor, req->mGoal,
                                            (MfLocoSkill *)req->mSkill, req->mOffset);
    actor->Release();
    if (req->mTarget)
        req->mTarget->Release();
    if (reachable == 0 || reachable == 1) {
        MfMsg *reply = new MfMsg(24, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfCheckReachableResultData(req->mRequestID, actor, reachable, (MfVector3 &)place));
        g_msgDispatcher->Post(reply);
    } else {
        MfMsg *reply = new MfMsg(16, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfPlannerFailureData(req->mRequestID, (int)actor, 1, reachable));
        g_msgDispatcher->Post(reply);
    }
    InterlockedDecrement(&actor->mSpace->mBusy);
    InterlockedDecrement(&actor->mBusy);
}

// 0x10068de0 ?PlanGrasp@@YAXPAVMfManipPlanner@@PAVMfMsg@@@Z
// The manipulation thread's answer to an MfManipGCReqData.
static void PlanGrasp(MfManipPlanner *planner, MfMsg *msg)
{
    MfManipGCReqData *req = (MfManipGCReqData *)msg->mData;
    MfLiveActor *actor = req->mActor;
    if (!g_msgDispatcher->FindID(actor))
        return;
    InterlockedIncrement(&actor->mBusy);
    InterlockedIncrement(&actor->mSpace->mBusy);
    MfGCPath *path = planner->PlanGrasp(actor, req->mSkill, req->mTarget, req->mNode, req->mConfig,
                                        req->mBase, req->mHandle, req->m64, req->mGoal);
    actor->Release();
    if (req->mTarget)
        req->mTarget->Release();
    if (path) {
        MfMsg *reply = new MfMsg(26, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfManipGCResultData(req->mRequestID, (int)actor, (int)req->mTarget, (int)req->mSkill,
                                               (MfManipPlan *)path, req->m64));
        g_msgDispatcher->Post(reply);
    } else {
        MfMsg *reply = new MfMsg(16, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfPlannerFailureData(req->mRequestID, (int)actor, 3, 0));
        g_msgDispatcher->Post(reply);
    }
    InterlockedDecrement(&actor->mSpace->mBusy);
    InterlockedDecrement(&actor->mBusy);
}

// 0x10068fb0 ?PlanMove@@YAXPAVMfManipPlanner@@PAVMfMsg@@@Z
// The manipulation thread's answer to an MfManipMoveRequestData.
static void PlanMove(MfManipPlanner *planner, MfMsg *msg)
{
    MfManipMoveRequestData *req = (MfManipMoveRequestData *)msg->mData;
    MfLiveActor *actor = req->mActor;
    if (!g_msgDispatcher->FindID(actor))
        return;
    InterlockedIncrement(&actor->mBusy);
    InterlockedIncrement(&actor->mSpace->mBusy);
    MfMTPath *path = planner->PlanMove(actor, req->mSkill, req->mTarget, req->m18);
    actor->Release();
    if (req->mTarget)
        req->mTarget->Release();
    if (path) {
        MfMsg *reply = new MfMsg(28, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfManipMoveResultData(req->mRequestID, (int)actor, (int)req->mTarget,
                                                 (int)req->mSkill, req->m18, path));
        g_msgDispatcher->Post(reply);
    } else {
        MfMsg *reply = new MfMsg(16, 3, 0, (MfRootObj *)0, actor);
        reply->SetData(new MfPlannerFailureData(req->mRequestID, (int)actor, 4, 0));
        g_msgDispatcher->Post(reply);
    }
    InterlockedDecrement(&actor->mSpace->mBusy);
    InterlockedDecrement(&actor->mBusy);
}

// EQUIV 0x10069170 ?ManipPlanningThread@@YAXPAX@Z: reviewed: the queue and the cached
// ostream::operator<< swap ebx/ebp, and the vtable loads of the three deletes rotate eax/edx;
// nothing else differs
// The manipulation planning thread: answers the requests of its queue until
// the quit message comes.
void ManipPlanningThread(void *queue)
{
    MfManipPlanner *planner = new MfManipPlanner;
    MfMsg *msg;
    for (;;) {
        msg = ((MfSharedMsgQueue *)queue)->Get(1);
        if (!msg)
            continue;
        if (msg->mID == 3)
            break;
        switch (msg->mID) {
        case 23:
            CheckReachable(planner, msg);
            delete msg;
            break;
        case 25:
            PlanGrasp(planner, msg);
            delete msg;
            break;
        case 27:
            PlanMove(planner, msg);
            delete msg;
            break;
        default:
            cout << "Undefined manipulation request received" << endl;
            delete msg;
            break;
        }
    }
    if (msg)
        delete msg;
    if (planner)
        delete planner;
    _endthread();
}
