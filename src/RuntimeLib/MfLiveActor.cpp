// MfLiveActor (RuntimeLib.dll 10031f20-1003af90; the rest of the unit is
// still in MfLiveActor_1b/_2/_3.cpp).
#include "MfLiveActor.h"
#include "MfAiController.h"
#include "MfColModel.h"
#include "MfDataPath.h"
#include "MfFSMManager.h"
#include "MfFSMSpaceToBhvr.h"
#include "Mf3dPathActivity.h"
#include "MfActorPath.h"
#include "MfFloor.h"
#include "MfHookManager.h"
#include "MfGestureActivity.h"
#include "MfHandle.h"
#include "MfLocoActivity.h"
#include "MfLocoSkill.h"
#include "MfManagerActivity.h"
#include "MfManipActivity.h"
#include "MfManipSkill.h"
#include "MfMsgDispatcher.h"
#include "MfTimedMsgQueue.h"
#include "MfMsgTypeTable.h"
#include "MfProtoActor.h"
#include "MfRenderer.h"
#include "MfMsgData.h"
#include "MfOrientActivity.h"
#include "MfPmhUser.h"
#include "MfPoActor.h"
#include "MfPoEvent.h"
#include "MfOverlayActivity.h"
#include "MfProperty.h"
#include "MfScDataHandle.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfVector3Value.h"
#include "MfWorld.h"
#include "MfMouse.h"
#include "MfPath2.h"
#include "MfRay.h"
#include "MfColResults.h"
#include "MfAiColNode.h"
#include "MfHoldActivity.h"
#include "MfCompliantActivity.h"
#include "MfStateCompiler.h"
#include "MfSequentialActivity.h"
#include "MfRandomActivity.h"
#include "MfRoadMap.h"
#include <fstream.h>

// the data of the event being handled, for the behaviour scripts (a pick,
// a contour failure ...)
extern void *gEventData;

// adds the files of dir with the extension to the list (10001000)
int FindFilesWithExtension(MfStringList *list, const MfString &dir, const MfString &ext);
MfMsg *MakeEventMsg(const MfString &name, MfRootObj *from, MfRootObj *to);
MfMsg *MakeTimerMsg(const MfString &name, int id, MfRootObj *actor);
// reports an error of an actor's request: the message of the code, filled in
// with the arguments that follow (10020780)
void MfReportError(MfString actorName, int code, ...);

// a running timer (mTimers)
struct MfTimerEntry {
    MfTimerEntry() : mHandle(0) {}

    int mHandle;                                            // 0x00 the message in the timed queue
};

MfLiveActor::MsgHandler MfLiveActor::aMethods[NUM_MSGS];
int MfLiveActor::bMsgTableIsSet;

// 0x100322f0 ??_GMfPoActor@@UAEPAXI@Z (local copy, after `new MfPoActor`)
// 0x100322e0 ??1MfCollisionData@@QAE@XZ
// 0x10031f20 ??0MfLiveActor@@QAE@W4MF_THREAD_NAME@@@Z
MfLiveActor::MfLiveActor(MF_THREAD_NAME thread)
    : mConfigRecord(0), mLive(0), mGoingLive(0), ma0(0), mSitting(0), mLastTimeForLoco(0),
      mLocoRequest(0), mGCRequest(0), mReachRequest(0), mMoveRequest(0), mLockRequest(0),
      mCmdState(0), mHFSM(0), m218(0), mTimers(0x35), mPiccoloHandle(0), mControlMode(3),
      mOwner(0), mRootActivity(0), mThread(thread), mLastConfig(0), mLastConfigSize(0),
      mRequestID(1), mLastKey(0), mLastKeyFlags(0), mLastActivity(0), m2c4(0),
      mLastLocoFailure(0), mLastManipFailure(0), mNoLocoIntention(1), mAuthorNodeCount(0),
      mAuthorCheckStyles(0), mAuthorRenderStyles(0), mPlannedLocoOptions(0), mPlannedManipOptions(0), mPlannedLocoParam(0), mPlannedLocoReach(0),
      mController(0), mAnchorNodeStop(0)
{
    InitMsgTable();
    if (!mThread) {
        Register();
        mPiccoloHandle = MfScDataHandle::CreateData(new MfPoActor(this));
    }
    mLastCollision.psnNode = 0;
    mLastCollision.plaOther = 0;
    mLastCollision.psnOtherNode = 0;
    mLastProximity.plaOther = 0;
    mLastPick.pickedNode = 0;
    mLastContourFailDown.failNode = 0;
}

// 0x10032280 ??_EMfLiveActor@@UAEPAXI@Z
// 0x10032310 ??1MfLiveActor@@UAE@XZ
MfLiveActor::~MfLiveActor()
{
    if (!mTempCopy) {
        if (mController) {
            mController->Release();
            mController = 0;
        }
        if (mManipPlan.mTargetConfig)
            delete [] mManipPlan.mTargetConfig;
        MfDblList list;
        if (mSpace)
            ChangeParent(mSpace->mRoot, 0, 0, 0);
        DetachChildren();
        _PmClearTimers();
        if (m218 == 1 && mHFSM)
            delete mHFSM;
        mHFSM = 0;
        if (mPiccoloHandle) {
            MfPoActor *po = dynamic_cast<MfPoActor *>(mPiccoloHandle->GetData());
            if (po)
                po->SetActorDeleted();
            mPiccoloHandle->Release();
        }
        if (!mThread)
            Unregister();
        if (mRootActivity) {
            delete mRootActivity;
            mRootActivity = 0;
        }
        MfActivity::ClearActivityTable(this);
        if (mLastConfig)
            delete [] mLastConfig;
        if (mAuthorCheckStyles)
            delete mAuthorCheckStyles;
        if (mAuthorRenderStyles)
            delete mAuthorRenderStyles;
    }
    if (mConfigRecord) {
        delete mConfigRecord;
        mConfigRecord = 0;
    }
}

// EQUIV 0x10032590 ??1MfGCPath@@QAE@XZ: reviewed: only the last delete [] keeps m3c in esi instead of eax (register choice); same eh vector destructor iterator call, count, size 0x1c and operator delete
MfGCPath::~MfGCPath()
{
    delete [] m00;
    delete [] m04;
    delete [] m08;
    delete [] m0c;
    delete [] m10;
    delete [] m38;
    delete [] m3c;
}

// 0x100326d0 ?GetPiccoloHandle@MfLiveActor@@MAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfLiveActor::GetPiccoloHandle()
{
    if (mPiccoloHandle)
        mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x100326f0 ?ClearOtherActorsActivityThatReferToThisActor@MfLiveActor@@QAEXXZ
void MfLiveActor::ClearOtherActorsActivityThatReferToThisActor()
{
    mSpace->Lock();
    MfDblListItem *pos;
    MfLiveActor *actor = (MfLiveActor *)mSpace->GetLiveActors().GetFirst(pos);
    while (actor) {
        if (actor->GetRootActivity())
            ((MfManagerActivity *)actor->GetRootActivity())->RemoveActivityThatRefersToThisActor(this);
        actor = (MfLiveActor *)mSpace->GetLiveActors().GetNext(pos);
    }
    mSpace->UnLock();
}

// 0x10032780 ?MakeCopy@MfLiveActor@@QAEXPAV1@@Z
void MfLiveActor::MakeCopy(MfLiveActor *src)
{
    mTempCopy = 1;
    AttachToPrototype(src->mPrototype, 0);
    MfTransform3 transform;
    for (int i = 0; i < mAllTraversal->numNodes(); i++) {
        src->Lock();
        transform = src->mAllTraversal->node(i)->transform();
        src->Unlock();
        mAllTraversal->node(i)->setTransform(transform);
        MfAiColNode &from = src->mAllTraversal->node(i)->ColNode();
        MfAiColNode &to = mAllTraversal->node(i)->ColNode();
        to.SetCheckStyle(from.GetCheckStyle());
        if (mAllTraversal->node(i)->IsType(MfHandle::TYPE))
            ((MfHandle *)mAllTraversal->node(i))->Copy(*(MfHandle *)src->mAllTraversal->node(i));
    }
    src->Lock();
    mSitting = src->mSitting;
    mLive = src->mLive;
    (&mChildList)->Copy(src->mChildList);
    mSpace = src->mSpace;
    int index = src->mSkeletonTraversal->nodeIndex(src->mAnchorNodeStop);
    if (index < 0)
        mAnchorNodeStop = 0;
    else
        mAnchorNodeStop = mSkeletonTraversal->node(index);
    src->Unlock();
    mInstanceName = src->mInstanceName;
}

// 0x10032970 ?SetAnchorNodeStop@MfLiveActor@@UAEHPAVMfAiActorNode@@@Z
int MfLiveActor::SetAnchorNodeStop(MfAiActorNode *node)
{
    MfSkeletonNode *skel = (MfSkeletonNode *)node;
    if (skel && mSkeletonTraversal->nodeIndex(skel) == -1)
        return 0;
    Lock();
    mAnchorNodeStop = skel;
    Unlock();
    return 1;
}

// 0x100329d0 ?RemoveSpace@MfLiveActor@@UAEHPAVMfSpace@@@Z
int MfLiveActor::RemoveSpace(MfSpace *space)
{
    if (UpdateFSMWithEvent() == 1) {
        MfMsg msg(45, 0, 0, this, this);
        mHFSM->HandleEvent(msg);
    }
    CleanupActivity(0);
    GoAuthor();
    ma0 = 0;
    return MfActor::RemoveSpace(space);
}

// 0x10032a70 ?SetSpace@MfLiveActor@@UAEHPAVMfSpace@@@Z
int MfLiveActor::SetSpace(MfSpace *space)
{
    int result = MfActor::SetSpace(space);
    if (mPrototype) {
        int setup = 1;
        if (mHFSM) {
            SetupPiccoloHandles();
            setup = 0;
        }
        LoadHFSM(0);
        if (setup == 1)
            SetupPiccoloHandles();
    }
    return result;
}

// 0x10032ac0 ?RefreshChildList@MfLiveActor@@IAEXPAVMfActor@@PAVMfSkeletonNode@@PAVMfManipSkill@@PAVMfHandle@@@Z
void MfLiveActor::RefreshChildList(MfActor *child, MfSkeletonNode *, MfManipSkill *skill, MfHandle *handle)
{
    if (!skill) {
        CleanupChildList((MfLiveActor *)child);
        return;
    }
    if (handle)
        return;
    MfDblListItem *pos;
    MfDblListItem *handPos;
    MfDblList released;
    MfDblList *hands = &skill->mHandList;
    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
         hand = (MfManipHand *)hands->GetNext(handPos)) {
        MfDblList *children = &mChildList;
        for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
             entry = (MfChildEntry *)children->GetNext(pos)) {
            if (entry->mHand == hand->mNode) {
                MfDblList *list = &released;
                list->Append(entry);
            }
        }
    }
    Lock();
    for (MfChildEntry *entry = (MfChildEntry *)released.GetFirst(pos); entry;
         entry = (MfChildEntry *)released.GetNext(pos)) {
        MfDblList *children = &mChildList;
        handPos = children->Find(entry);
        children->Delete(handPos);
        if (entry->mHandle->mOccupied == entry->mNode) {
            entry->mHandle->mOccupied = 0;
            for (MfChildEntry *other = (MfChildEntry *)children->GetFirst(handPos); other;
                 other = (MfChildEntry *)children->GetNext(handPos)) {
                if (other->mHandle == entry->mHandle) {
                    entry->mHandle->mOccupied = other->mNode;
                    break;
                }
            }
        }
        entry->mHandle->ClearFixXformForGrasp();
        delete entry;
    }
    Unlock();
}

// EQUIV 0x10032c90 ?UpdateChildList@MfLiveActor@@IAEXPAVMfActor@@PAVMfSkeletonNode@@PAVMfManipSkill@@PAVMfHandle@@@Z: reviewed: same tests, list entries and calls; the original calls the imported MfTransform3()/MfRotation3::operator= where these headers expand the same copies and zero/identity inits, plus register choice, a 4-byte larger frame and EH state numbering (the original ends the local transform scope before the unlock call)
void MfLiveActor::UpdateChildList(MfActor *child, MfSkeletonNode *node, MfManipSkill *skill, MfHandle *handle)
{
    if (!skill) {
        if (!handle)
            return;
    }
    Lock();
    MfTransform3 inverse;
    inverse = ~child->mSkeleton->mGlobalTransform;
    if (skill) {
        if (handle) {
            MfDblListItem *pos;
            for (MfManipHand *hand = (MfManipHand *)skill->mHandList.GetFirst(pos); hand;
                 hand = (MfManipHand *)skill->mHandList.GetNext(pos)) {
                MfChildEntry *entry = new MfChildEntry;
                entry->mActor = child;
                entry->mHandle = handle;
                entry->mHand = hand->mNode;
                entry->mNode = mSkeletonTraversal->node(hand->mNode);
                entry->mSkill = skill;
                entry->mOffset = inverse * entry->mNode->mGlobalTransform;
                MfDblList *children = &mChildList;
                children->Add(entry);
            }
        }
    } else if (handle) {
        MfChildEntry *entry = new MfChildEntry;
        entry->mActor = child;
        entry->mHandle = handle;
        entry->mNode = node;
        entry->mHand = mSkeletonTraversal->nodeIndex(node);
        entry->mSkill = skill;
        entry->mOffset = inverse * node->mGlobalTransform;
        MfDblList *children = &mChildList;
        children->Add(entry);
    }
    Unlock();
}

// 0x10032f40 ?DetachChildren@MfLiveActor@@QAEXXZ
void MfLiveActor::DetachChildren()
{
    if (mSkeleton) {
        MfSkeletonTraversal *nodes = new MfSkeletonTraversal(mSkeleton, MfSkeletonNode::TYPE, 0);
        for (int i = 0; i < nodes->numNodes(); i++) {
            MfSkeletonNode *node = nodes->node(i);
            MfLiveActor *child = (MfLiveActor *)node->mOwner;
            if (child != this && ((MfSkeletonNode *)node->parent())->mOwner == this && child)
                child->ChangeParent((MfSkeletonNode *)mSkeleton->parent(), 0, 0, 0);
        }
        delete nodes;
    }
    CleanupChildList(0);
}

// 0x10033010 ?CleanupChildList@MfLiveActor@@QAEXPAV1@@Z
void MfLiveActor::CleanupChildList(MfLiveActor *child)
{
    MfDblListItem *pos;
    MfDblListItem *item;
    MfDblList found;
    MfDblList *children = &mChildList;
    MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos);
    while (entry) {
        if (child) {
            if ((MfLiveActor *)entry->mActor == child) {
                MfDblList *list = &found;
                list->Append(entry);
            }
        } else {
            delete entry;
        }
        entry = (MfChildEntry *)children->GetNext(pos);
    }
    Lock();
    if (child) {
        for (MfChildEntry *gone = (MfChildEntry *)found.GetFirst(pos); gone;
             gone = (MfChildEntry *)found.GetNext(pos)) {
            item = children->Find(gone);
            children->Delete(item);
            delete gone;
        }
        found.Clear();
    } else {
        children->Clear();
    }
    Unlock();
}

// 0x10033150 ?ChangeParent@MfLiveActor@@QAEXPAVMfSkeletonNode@@PAVMfManipSkill@@PAVMfHandle@@H@Z
void MfLiveActor::ChangeParent(MfSkeletonNode *parent, MfManipSkill *skill, MfHandle *handle, int)
{
    if (!mSkeleton)
        return;
    MfSkeletonNode *oldParent = (MfSkeletonNode *)mSkeleton->parent();
    MfLiveActor *oldOwner = oldParent ? (MfLiveActor *)oldParent->mOwner : 0;
    MfLiveActor *newOwner = parent ? (MfLiveActor *)parent->mOwner : 0;
    if (mOwner && mOwner == oldOwner && oldOwner != newOwner)
        ((MfLiveActor *)mOwner)->RequestLock(this, 0);
    oldParent = (MfSkeletonNode *)mSkeleton->parent();
    if (oldParent) {
        // free the handles of the old parent that held this actor
        MfSkeletonTraversal *handles = new MfSkeletonTraversal(oldParent, MfHandle::TYPE, 1);
        for (int i = 0; i < handles->numNodes(); i++) {
            MfHandle *h = (MfHandle *)handles->node(i);
            if (h->mOccupied == mSkeleton)
                h->mOccupied = 0;
        }
        delete handles;
    }
    mSkeleton->changeParent(parent, 0);
    if (oldOwner == newOwner) {
        if (oldOwner)
            oldOwner->RefreshChildList(this, parent, skill, handle);
    } else if (oldOwner) {
        oldOwner->CleanupChildList(this);
        // this actor's own handles are free now
        MfSkeletonTraversal *handles = new MfSkeletonTraversal(mSkeleton, MfHandle::TYPE, 1);
        for (int i = 0; i < handles->numNodes(); i++)
            ((MfHandle *)handles->node(i))->mOccupied = 0;
        delete handles;
    }
    if (newOwner)
        newOwner->UpdateChildList(this, parent, skill, handle);
    if (handle)
        handle->mOccupied = parent;
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(55, 0, 0, this, this);
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(55, 0, 0, this, this);
        UpdateAppEventHandler(msg);
    }
    if (MfWorld::enableNetwork) {
        MfMiscHookData data;
        data.pActor = this;
        data.m18 = (int)(MfAiActorNode *)parent;
        data.kind = 2;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
}

// 0x10033460 ?NoIntentionOfLocomoting@MfLiveActor@@QAEHXZ
int MfLiveActor::NoIntentionOfLocomoting()
{
    Lock();
    int none = mNoLocoIntention;
    Unlock();
    return none;
}

// 0x10033490 ?ComputeManipInfo@MfLiveActor@@QAEXAAMAAVMfTransform3@@@Z
void MfLiveActor::ComputeManipInfo(float &size, MfTransform3 &offset)
{
    MfVector3 center;
    MfVector3 extent;
    MfVector3 minBox;
    MfVector3 maxBox;
    mPrototype->GetBoundingBox(minBox, maxBox);
    center = (minBox + maxBox) / 2.0f;
    extent = maxBox - minBox;
    if (extent.x() >= extent.y() && extent.x() >= extent.z())
        size = extent.x();
    else if (extent.y() >= extent.x() && extent.y() >= extent.z())
        size = extent.y();
    else if (extent.z() >= extent.x() && extent.z() >= extent.y())
        size = extent.z();
    offset.translation() = center;
}

// 0x10033630 ?LiveUpdate@MfLiveActor@@UAEHJ@Z
int MfLiveActor::LiveUpdate(long clock)
{
    AllocateLastConfig();
    int moved = 0;
    if (mLive) {
        if (mRootActivity) {
            Lock();
            if (mLastConfig)
                mSkeletonTraversal->GetGlobalTransforms(mLastConfig);
            Unlock();
            mRootActivity->Update(clock);
            if (mRootActivity->mSubactivities.GetCount() > 0)
                moved = 1;
        }
        if (mStateChangeFlag)
            moved = 1;
        mStateChangeFlag = 0;
    }
    if (moved)
        mSkeleton->syncChildTransforms(0);
    return moved;
}

// 0x100336d0 ?AllocateLastConfig@MfLiveActor@@QAEXXZ
void MfLiveActor::AllocateLastConfig()
{
    Lock();
    if (mSkeletonTraversal->numNodes() > 0 && mLastConfigSize < mSkeletonTraversal->numNodes()) {
        if (mLastConfig)
            delete [] mLastConfig;
        mLastConfigSize = mSkeletonTraversal->numNodes();
        mLastConfig = mSkeletonTraversal->CopyTransforms();
        mSkeletonTraversal->GetGlobalTransforms(mLastConfig);
    }
    Unlock();
}

// 0x10033750 ?GoLive@MfLiveActor@@UAEXH@Z
void MfLiveActor::GoLive(int)
{
    if (mTempCopy)
        return;
    Lock();
    mLive = 1;
    Unlock();
    ma0 = 1;
    mGoingLive = 1;
    _PmClearTimers();
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = 0;
    mMoveRequest = 0;
    mLockRequest = 0;
    mCmdState = 0;
    if (mHFSM && !mController)
        mHFSM->Enable(1);
    if (mHFSM && mControlMode == 3) {
        MfMsg msg(1, 0, 0);
        mHFSM->CatchMsg(msg);
        mHFSM->UpdateActive();
    }
    if (mControlMode == 3 && (mController || (mHFSM && mHFSM->mActive)))
        SetControlMode(1);
    else
        SetControlMode(mControlMode);
    AllocateLastConfig();
    if (!(mActorAttributes & 0x40)) {
        if (mActorAttributes & 1)
            setActorAttributes(9);
        else
            setActorAttributes(10);
    }
    mGoingLive = 0;
}

// 0x100338b0 ?ContinueLive@MfLiveActor@@UAEXXZ
void MfLiveActor::ContinueLive()
{
    if (mTempCopy)
        return;
    if (!ma0) {
        GoLive(0);
        return;
    }
    Lock();
    mLive = 1;
    Unlock();
    if (mHFSM)
        mHFSM->Enable(1);
    AllocateLastConfig();
    if (mController && mControlMode == 1)
        mController->Resume();
}

// 0x10033930 ?GoAuthor@MfLiveActor@@UAEXXZ
void MfLiveActor::GoAuthor()
{
    if (mTempCopy)
        return;
    Lock();
    mLive = 0;
    Unlock();
    if (mHFSM && !mController)
        mHFSM->Enable(0);
    mAuthorAttributes = mActorAttributes;
    mAuthorNodeCount = mSkeletonTraversal->numNodes();
    if (mAuthorCheckStyles)
        delete [] mAuthorCheckStyles;
    mAuthorCheckStyles = new int[mAuthorNodeCount];
    if (mAuthorRenderStyles)
        delete [] mAuthorRenderStyles;
    mAuthorRenderStyles = new int[mAuthorNodeCount];
    int i;
    for (i = 0; i < mSkeletonTraversal->numNodes(); i++) {
        mAuthorCheckStyles[i] = mSkeletonTraversal->node(i)->mColNode.GetCheckStyle();
        mAuthorRenderStyles[i] = mSkeletonTraversal->node(i)->GetRenderStyle();
    }
    Lock();
    for (i = 0; i < mSkeletonTraversal->numNodes(); i++) {
        MfSkeletonNode *node = mSkeletonTraversal->node(i);
        node->mColNode.SetCheckStyle(node->m1c);
        node->SetRenderStyle(node->m18);
    }
    Unlock();
    if (mController && mControlMode == 1)
        mController->Deactivate();
}

// 0x10033a90 ?IsMoving@MfLiveActor@@QAEHXZ
int MfLiveActor::IsMoving()
{
    if (!mLive)
        return 0;
    if (mRootActivity && mRootActivity->HasMotion())
        return 1;
    float moved = (mSkeleton->mGlobalTransform.translation() - mLastConfig->translation()).length();
    float radius, height;
    mPrototype->GetBoundingCylinder(radius, height);
    if (height * 0.01f < moved)
        return 1;
    return 0;
}

// 0x10033b70 ?ChangeCmdState@MfLiveActor@@QAEXXZ
// Advances the command state machine when a step of a locomotion or
// manipulation command has finished: 1-2 walk, 2 -> 6 closing gesture;
// 4-5 walk to the target, 5 -> 7 gesture, 7/5 -> 8 grasp plan, 8 -> 9 (target
// held elsewhere) or 12, 9 -> 14 lock / move the object / sit, 12, 14 -> move.
void MfLiveActor::ChangeCmdState()
{
    switch (mCmdState) {
    case 0:
        return;
    case 1:
        mCmdState = 2;
        break;
    case 4:
        mCmdState = 5;
        break;
    case 2:
        mCmdState = 0;
        if (mLocoPlan.mSkill) {
            mCmdState = 6;
            MfActivity *gesture = MakeGestureActivity(g_world->mClock, mLocoPlan.mSkill, 0);
            gEventData = &mLocoPlan;
            gesture->SetUniqueID(mLocoPlan.mEventID);
            if (mLocoPlan.m88) {
                mLastEventID = mLocoPlan.mEventID;
                mLastActivity = gesture;
                if (UpdateFSMWithEvent() && !mController) {
                    MfMsg msg(22, 0, 0, this, this);
                    mHFSM->HandleEvent(msg);
                } else if (mControlMode == 1 && mLive) {
                    MfMsg msg(22, 0, 0, this, this);
                    UpdateAppEventHandler(msg);
                }
            } else {
                SetActivity(gesture);
            }
        }
        if (UpdateFSMWithEvent() && !mController) {
            mLastEventID = m2c4;
            MfMsg msg(11, 0, 0, this, this);
            msg.m38 = 1;
            mHFSM->HandleEvent(msg);
        } else if (mControlMode == 1 && mLive) {
            mLastEventID = m2c4;
            MfMsg msg(11, 0, 0, this, this);
            msg.m38 = 1;
            UpdateAppEventHandler(msg);
        }
        break;
    case 5:
        if (!mSpace->GetLiveActors().Contains(mManipPlan.mTarget)) {
            PostPlannerFailure(2);
            break;
        }
        if (!mManipPlan.m4c) {
            // is the target still where the plan expects it?
            MfVector3 moved;
            moved = (mManipPlan.mFrom * mManipPlan.mTargetConfig[0] * mManipPlan.mTargetConfig[1]).translation();
            moved = moved - mManipPlan.mTarget->mSkeletonTraversal->node(1)->mGlobalTransform.translation();
            if (moved.length() > *(float *)&mManipPlan.m24) {
                PostPlannerFailure(2);
                break;
            }
        }
        if (mManipPlan.mSkill) {
            mCmdState = 7;
            MfActivity *gesture = MakeGestureActivity(g_world->mClock, mManipPlan.mSkill, 0);
            gEventData = &mManipPlan;
            gesture->SetUniqueID(mManipPlan.mEventID);
            if (mManipPlan.m74) {
                mLastEventID = mManipPlan.mEventID;
                mLastActivity = gesture;
                if (UpdateFSMWithEvent() && !mController) {
                    MfMsg msg(22, 0, 0, this, this);
                    mHFSM->HandleEvent(msg);
                } else if (mControlMode == 1 && mLive) {
                    MfMsg msg(22, 0, 0, this, this);
                    UpdateAppEventHandler(msg);
                }
            } else {
                SetActivity(gesture);
            }
            break;
        }
        RequestGCPlan(mManipPlan.mTarget, mManipPlan.mManipSkill, mManipPlan.mHandle, mManipPlan.m4c,
                      mManipPlan.mTo);
        break;
    case 7:
        RequestGCPlan(mManipPlan.mTarget, mManipPlan.mManipSkill, mManipPlan.mHandle, mManipPlan.m4c,
                      mManipPlan.mTo);
        break;
    case 8:
        mCmdState = ((MfSkeletonNode *)mManipPlan.mTarget->mSkeleton->parent())->mOwner == this ? 12 : 9;
        break;
    case 9: {
        MfVector3 moved;
        moved = (mManipPlan.mFrom * mManipPlan.mTargetConfig[0] * mManipPlan.mTargetConfig[1]).translation();
        moved = moved - mManipPlan.mTarget->mSkeletonTraversal->node(1)->mGlobalTransform.translation();
        if (moved.length() > *(float *)&mManipPlan.m28) {
            PostPlannerFailure(3);
            break;
        }
        int type = mManipPlan.mManipSkill->mManipType;
        if (type == 1) {
            mCmdState = 14;
            RequestLock(mManipPlan.mTarget, 1);
            break;
        }
        if (mManipPlan.mOtherSkill && type == 0) {
            RequestManipMoveTo(mManipPlan.m74, mManipPlan.mTarget, mManipPlan.mOtherSkill, 1,
                               mManipPlan.mEventID);
            break;
        }
        if (type == 3) {
            // sit down: the actor goes to the node that holds the handle
            MfSkeletonNode *seat = 0;
            MfTreeNode *node = mManipPlan.mHandle;
            do {
                node = node->parent();
                if (node->Type() == MfSkeletonNode::TYPE)
                    seat = (MfSkeletonNode *)node;
            } while (!seat);
            ClearActivity();
            ChangeParent(seat, 0, 0, 0);
            mManipPlan.mHandle->mOccupied = mSkeleton;
        }
        UpdateFSMThatSitManipFinished();
        break;
    }
    case 12:
        mCmdState = 0;
        if (mManipPlan.mOtherSkill) {
            RequestManipMoveTo(mManipPlan.m74, mManipPlan.mTarget, mManipPlan.mOtherSkill, 1,
                               mManipPlan.mEventID);
            break;
        }
        UpdateFSMThatSitManipFinished();
        break;
    case 14:
        mCmdState = 0;
        if (mManipPlan.mManipSkill->mManipType == 1) {
            // the lock is granted: put the target back where it was and take it
            mManipPlan.mTarget->mSkeletonTraversal->SetTransforms(mManipPlan.mTargetConfig);
            MfTransform3 local = mManipPlan.mFrom * mManipPlan.mTargetConfig[0];
            local = ~((MfSkeletonNode *)mManipPlan.mTarget->mSkeleton->parent())->mGlobalTransform * local;
            mManipPlan.mTarget->mSkeleton->setTransform(local);
            mManipPlan.mTarget->ClearActivity();
            mManipPlan.mTarget->ChangeParent(mManipPlan.mNode, mManipPlan.mManipSkill, mManipPlan.mHandle, 1);
            mManipPlan.mHandle->mOccupied = mManipPlan.mNode;
            mManipPlan.mHandle->CacheAwayTargetXforms(mManipPlan.mTarget);
            if (mManipPlan.mOtherSkill) {
                RequestManipMoveTo(mManipPlan.m74, mManipPlan.mTarget, mManipPlan.mOtherSkill, 0,
                                   mManipPlan.mEventID);
                break;
            }
            UpdateFSMThatSitManipFinished();
        }
        break;
    case 10:
        mCmdState = 11;
        break;
    case 11:
        mCmdState = 0;
        UpdateFSMThatSitManipFinished();
        break;
    default:
        mCmdState = 0;
        break;
    }
}

// 0x10034410 ?SetControlMode@MfLiveActor@@UAEHH@Z
int MfLiveActor::SetControlMode(int mode)
{
    mControlMode = mode;
    if (mHFSM && mHFSM->mActive && mode == 1 && mLive && !mController) {
        mHFSM->Resume();
        return 1;
    }
    if (mController && mode == 1 && mLive) {
        mController->Activate();
        return 1;
    }
    if (mController && mode == 0)
        mController->Deactivate();
    return 1;
}

// 0x100344a0 ?IsCurrentActivity@MfLiveActor@@QAEPAVMfActivity@@H@Z
MfActivity *MfLiveActor::IsCurrentActivity(int type)
{
    if (!mRootActivity)
        return 0;
    if (mRootActivity->Type() == type)
        return mRootActivity;
    if (!mRootActivity->IsType(MfManagerActivity::TYPE))
        return 0;
    MfDblListItem *pos;
    MfActivity *activity = (MfActivity *)mRootActivity->mSubactivities.GetFirst(pos);
    while (activity) {
        if (activity->Type() == type)
            return activity;
        if (type == MfLocoActivity::TYPE && activity->Type() == 'PluA' /* MfPluginActivity */ &&
            activity->IsLocoPlugin())
            return activity;
        if (activity->IsType(MfManagerActivity::TYPE)) {
            MfActivity *found = ((MfManagerActivity *)activity)->FindType(type);
            if (found)
                return found;
        }
        activity = (MfActivity *)mRootActivity->mSubactivities.GetNext(pos);
    }
    return 0;
}

// 0x10034580 ?SetActivity@MfLiveActor@@UAEHPBVMfAiActivity@@@Z
int MfLiveActor::SetActivity(const MfAiActivity *aiActivity)
{
    MfActivity *activity = (MfActivity *)aiActivity;
    if (!activity->IsType(MfActivity::TYPE))
        return 0;
    if (!MfActivity::ActivityExists(activity))
        return 0;
    if (aiActivity)
        CleanupActivity(1);
    if (activity)
        activity->PrepareForExecution();
    ApplyActivity(activity);
    return 1;
}

// 0x100345f0 ?ApplyActivity@MfLiveActor@@IAEXPAVMfActivity@@@Z
void MfLiveActor::ApplyActivity(MfActivity *activity)
{
    if (!MfActivity::ActivityExists(activity))
        return;
    if (activity && mRootActivity && mRootActivity->Contains(activity))
        return;
    CleanupActivity(0);
    if (!mRootActivity) {
        MfOverlayActivity *overlay = new MfOverlayActivity(this, g_world->mClock);
        mRootActivity = overlay;
        overlay->mKeepRunning = 1;
    }
    if (MfWorld::enableNetwork) {
        MfActivityHookData data;
        data.pActor = this;
        data.pActivity = activity;
        data.op = 0;
        gHookManager->CallHooks(MF_HOOK_ACTIVITY_DATA, &data);
    }
    if (activity)
        mRootActivity->AddActivity(activity);
    activity->Update(activity->mStartTime);
    if (IsCurrentActivity(MfLocoActivity::TYPE)) {
        Lock();
        mSitting = 0;
        Unlock();
    }
    Lock();
    mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                       !IsCurrentActivity(Mf3dPathActivity::TYPE) ? 1 : 0;
    Unlock();
}

// 0x10034790 ?ClearActivity@MfLiveActor@@UAEXXZ
void MfLiveActor::ClearActivity()
{
    CleanupActivity(1);
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = 0;
    mMoveRequest = 0;
    mLockRequest = 0;
    MfDblList *blocks = &mBlockList;
    blocks->Clear();
    mCmdState = 0;
}

// 0x100347e0 ?CleanupActivity@MfLiveActor@@QAEXH@Z
void MfLiveActor::CleanupActivity(int)
{
    if (mSpace && (IsCurrentActivity(MfLocoActivity::TYPE) || IsCurrentActivity(Mf3dPathActivity::TYPE)))
        mLastTimeForLoco = g_world->mClock;
    MfManipActivity *manip = (MfManipActivity *)IsCurrentActivity(MfManipActivity::TYPE);
    int unlock = manip && (manip->getMotionType() == 0 || manip->getMotionType() == 4) ? 1 : 0;
    if (manip && manip->getMotionType() == 0 && manip->Finished())
        unlock = 0;
    if (unlock) {
        MfLiveActor *target;
        if (manip->getMotionType() == 4)
            target = (MfLiveActor *)manip->getHandle()->mOwner;
        else
            target = (MfLiveActor *)manip->getTarget();
        RequestLock(target, 0);
    }
    if (mSpace && IsCurrentActivity(MfLocoActivity::TYPE))
        mSpace->UpdateMovingActor(this);
    if (mSpace && manip) {
        MfLiveActor *target = (MfLiveActor *)manip->getTarget();
        if (target)
            mSpace->UpdateMovingActor(target);
        mSpace->UpdateMovingActor(this);
    }
    if (mRootActivity)
        mRootActivity->RemoveSubactivities();
    if (mSkeleton)
        mSkeleton->syncChildTransforms(0);
    Lock();
    mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                       !IsCurrentActivity(Mf3dPathActivity::TYPE) ? 1 : 0;
    Unlock();
}

// 0x10034950 ?LoadHFSM@MfLiveActor@@QAEXH@Z
void MfLiveActor::LoadHFSM(int arg)
{
    if (!OldLoadHFSM())
        LoadBehaviors(arg);
}

// TODO 0x10034970 ?OldLoadHFSM@MfLiveActor@@QAEHXZ: (66% similar) control-flow layout: after SetActorHandle the original duplicates the `if (live) GoLive(0)` test of the failure tail where this jumps to the shared tail; otherwise the same calls in the same order (istream::get / MfString::operator<<(char) cached in other registers, virtual base offset kept in a register)
// Loads the old single-file behaviour <prototype>.hfsm if the actor has one;
// 0 if not (LoadHFSM then loads the behaviour files).
int MfLiveActor::OldLoadHFSM()
{
    MfString script;
    MfString file(mPrototype->sName);
    file << ".hfsm";
    MfString path = GetDataPath();
    path << "\\Actors\\" << mPrototype->sName << "\\" << file;
    HANDLE handle = CreateFile((const char *)path, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == INVALID_HANDLE_VALUE)
        return 0;
    CloseHandle(handle);
    int live = mLive;
    if (live)
        GoAuthor();
    if (mHFSM)
        mHFSM->ClearBehaviors();
    ifstream in((char *)path, ios::in);
    MfString dir = GetDataPath();
    dir << "\\Actors\\" << mPrototype->sName << "\\Behaviors";
    if (in.is_open()) {
        while (!in.eof()) {
            char line[256];
            in.getline(line, 255);
            script << line;
            script << '\n';
        }
        MfString name;
        name = mPrototype->sName;
        gStateCompiler->Compile(name, script, dir, path, 0, 1, 0);
    }
    in.close();
    if (script.Length() > 5) {
        mHFSM = gStateCompiler->mManager;
        if (mHFSM) {
            mHFSM->SetHfsmPath(path);
            mHFSM->SetActorHandle(mPiccoloHandle);
        }
        if (mHFSM) {
            m218 = 1;
            mHFSM->SetActorName(InstanceName());
            mHFSM->SetActorClass(Name());
            mHFSM->UpdateActive();
            mHFSM->CompileBehaviors(1, 0);
            mHFSM->UpdateActive();
            mHFSM->Hook25();
            LoadActorMethods();
            if (live)
                GoLive(0);
            return 1;
        }
    }
    if (live)
        GoLive(0);
    return 0;
}

// 0x10034df0 ?SetHfsmPath@MfFSMManager@@QAEXVMfString@@@Z
// 0x10034e40 ?SetActorName@MfFSMManager@@QAEXVMfString@@@Z
// 0x10034e90 ?SetActorClass@MfFSMManager@@QAEXVMfString@@@Z

// 0x10034ee0 ?LoadBehaviors@MfLiveActor@@QAEXH@Z
void MfLiveActor::LoadBehaviors(int all)
{
    if (!mSpace)
        return;
    int live = mLive;
    if (live)
        GoAuthor();
    if (mHFSM && !all)
        mHFSM->ClearBehaviors();
    MfString dir = GetDataPath();
    dir << "\\Actors\\" << mPrototype->sName << "\\Behaviors";
    MfFileHookData data;
    data.fileHandle = 0;
    data.fileLocation = dir;
    if (gHookManager->CallHooks(MF_HOOK_FILE_DESTINATION, &data) > 0)
        dir = data.fileLocation;
    if (!mHFSM)
        mHFSM = new MfFSMManager(dir, mPiccoloHandle, 0, this);
    MfFSMSpaceToBhvr *map = 0;
    MfString unused;
    if (!all) {
        MfString spaceFile(mSpace->mSpaceFileName);
        MfString drive, path, name, ext;
        spaceFile.SplitPath(&drive, &path, &name, &ext);
        spaceFile = name + ext;
        map = Prototype()->FindSpaceBhvrMap(spaceFile);
        if (!map)
            map = Prototype()->FindSpaceBhvrMap("__Default__");
    }
    MfStringList *list;
    int owned = 0;
    if (map)
        list = &map->mActiveBhvrs;
    else {
        owned = 1;
        list = new MfStringList;
        if (all)
            FindFilesWithExtension(list, dir, gBhvrExtension);
    }
    mHFSM->SetActorName(InstanceName());
    mHFSM->SetActorClass(Name());
    MfDblListItem *pos;
    for (MfString *bhvr = (MfString *)list->GetFirst(pos); bhvr; bhvr = (MfString *)list->GetNext(pos)) {
        if (!mHFSM->IsBehaviorLoaded(*bhvr))
            mHFSM->LoadBehavior(*bhvr, 0, 1);
    }
    m218 = 1;
    mHFSM->UpdateActive();
    mHFSM->UpdateActive();
    mHFSM->CompileBehaviors(1, 1);
    LoadActorMethods();
    if (owned == 1 && list)
        delete list;
    if (live)
        GoLive(0);
}

// 0x100352e0 ??_GMfStringList@@UAEPAXI@Z (local copy, after `new MfStringList`)
// 0x10035300 ??1MfFileHookData@@QAE@XZ

// 0x10035310 ?SaveBehaviorsForPlayback@MfLiveActor@@QAEXAAVMfString@@@Z
void MfLiveActor::SaveBehaviorsForPlayback(MfString &file)
{
    if (mHFSM)
        mHFSM->SaveForPlayback(file);
}

// 0x10035330 ?ActorMethodFile@MfLiveActor@@QAE?AVMfString@@XZ
MfString MfLiveActor::ActorMethodFile()
{
    MfString path = MfStringSize(256);
    path << GetDataPath() << "\\Actors\\" << mPrototype->sName << "\\" << mPrototype->sName << "_methods.pc";
    return path;
}

// 0x100228e0 ?LoadActorMethods@MfLiveActor@@QAEXXZ
// The actor's script methods (ActorMethodFile) are not loaded in this
// version; the empty function is folded with other empty ones.
void MfLiveActor::LoadActorMethods()
{
}

// 0x10035410 ?SetupPiccoloHandles@MfLiveActor@@QAEXXZ
void MfLiveActor::SetupPiccoloHandles()
{
    if (mHFSM) {
        MfScDataHandle *space = mSpace->GetPiccoloHandle();
        mHFSM->SetSpaceHandle(space);
        space->Release();
    }
}

// 0x10035450 ?ActivateBehavior@MfLiveActor@@UAEHABVMfString@@@Z
int MfLiveActor::ActivateBehavior(const MfString &name)
{
    return mHFSM->ActivateBehavior(name);
}

// 0x10035480 ?DeactivateBehavior@MfLiveActor@@UAEHABVMfString@@@Z
int MfLiveActor::DeactivateBehavior(const MfString &name)
{
    mHFSM->DeactivateBehavior(name);
    return 1;
}

// 0x100354b0 ?LoadBehavior@MfLiveActor@@UAEHABVMfString@@H@Z
int MfLiveActor::LoadBehavior(const MfString &name, int arg)
{
    if (!mHFSM)
        return 0;
    int result;
    if (!mHFSM->IsBehaviorLoaded(MfString(name)))
        result = mHFSM->LoadBehavior(name, 1, arg);
    return result;
}

// 0x10035570 ?UnloadBehavior@MfLiveActor@@UAEHABVMfString@@@Z
int MfLiveActor::UnloadBehavior(const MfString &name)
{
    mHFSM->UnloadBehavior(name);
    return 1;
}

// 0x100355a0 ?InitMsgTable@MfLiveActor@@MAEXXZ
void MfLiveActor::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        bMsgTableIsSet = 1;
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfLiveActor::DefaultMsgHandler;
        aMethods[10] = &MfLiveActor::HandleMiscEvents;
        aMethods[50] = &MfLiveActor::HandleMiscEvents;
        aMethods[46] = &MfLiveActor::HandleMiscEvents;
        aMethods[51] = &MfLiveActor::HandleMiscEvents;
        aMethods[MSG_STATIC_ACTOR_CHANGE] = &MfLiveActor::HandleStaticActorChange;
        aMethods[15] = &MfLiveActor::HandleProximityEvent;
        aMethods[35] = &MfLiveActor::HandleGestureCollision;
        aMethods[36] = &MfLiveActor::HandleReceivedCollision;
        aMethods[57] = &MfLiveActor::HandleConformFailure;
        aMethods[16] = &MfLiveActor::HandlePlannerFailure;
        aMethods[14] = &MfLiveActor::Handle3rdPartyMsg;
        aMethods[20] = &MfLiveActor::HandleLocoPlan;
        aMethods[28] = &MfLiveActor::HandleManipMoveTo;
        aMethods[26] = &MfLiveActor::HandleManipGC;
        aMethods[24] = &MfLiveActor::HandleCheckReachable;
        aMethods[11] = &MfLiveActor::HandleActivityFinished;
        aMethods[58] = &MfLiveActor::HandleStrMessage;
    }
    mMsgTable = aMethods;
}

// 0x10035700 ?DefaultMsgHandler@MfLiveActor@@UAEXAAVMfMsg@@@Z (= HandleMiscEvents)
void MfLiveActor::DefaultMsgHandler(MfMsg &msg)
{
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

void MfLiveActor::HandleMiscEvents(MfMsg &msg)
{
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// 0x10035750 ?Handle3rdPartyMsg@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::Handle3rdPartyMsg(MfMsg &msg)
{
    if (mLive != 1)
        return;
    if (UpdateFSMWithEvent() && !mController) {
        mLastEventID = msg.mEventID;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastEventID = msg.mEventID;
        UpdateAppEventHandler(msg);
    }
}

// 0x100357c0 ?HandleStrMessage@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleStrMessage(MfMsg &msg)
{
    MfStrMessageData *data = (MfStrMessageData *)msg.mData;
    if (UpdateFSMWithEvent() && !mController) {
        mLastEventID = data->mID;
        mLastMessage = data->mMessage;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastEventID = data->mID;
        mLastMessage = data->mMessage;
        UpdateAppEventHandler(msg);
    }
}

// 0x10035850 ?CurrentManipIntention@MfLiveActor@@IAEHPAV1@PAVMfManipSkill@@@Z
int MfLiveActor::CurrentManipIntention(MfLiveActor *target, MfManipSkill *skill)
{
    if (((MfSkeletonNode *)target->mSkeleton->parent())->mOwner == this)
        return 6;
    if (skill->mManipType == 1)
        return 0;
    return skill->mManipType == 3 ? 4 : 3;
}

// EQUIV 0x10035890 ?ApplySnapOn@MfLiveActor@@QAEXAAVMfTransform3@@@Z: reviewed: same calls and order; the original assigns the result with the imported MfRotation3::operator= where the rotation copy is expanded here, plus register choice
void MfLiveActor::ApplySnapOn(MfTransform3 &xform)
{
    MfSkeletonNode *parent = (MfSkeletonNode *)mSkeleton->parent();
    if (parent) {
        MfTransform3 parentTransform = parent->globalTransform();
        xform = ~parentTransform * xform;
    }
    mSkeleton->setTransform(xform);
    mSkeleton->syncChildTransforms(0);
    MfVector3 pos = mSkeletonTraversal->node(1)->mGlobalTransform.translation();
    MfFloor *floor = AppropriateFloor(pos, 0, 0, 1);
    if ((!parent || (floor && parent->mOwner == floor->mOwner)) && floor && floor->mOwner != this)
        ChangeParent(floor, 0, 0, 0);
    if (mLastConfig)
        mSkeletonTraversal->GetGlobalTransforms(mLastConfig);
    MfRenderer::Instance()->RenderAllViews();
}

// 0x10035a10 ?AttachToFloor@MfLiveActor@@IAEHXZ
int MfLiveActor::AttachToFloor()
{
    if (!mSpace)
        return 0;
    MfHandle *handle = 0;
    if (mSkeleton->parent()->Type() == MfHandle::TYPE)
        handle = (MfHandle *)mSkeleton->parent();
    MfFloor *floor = AppropriateFloor(0);
    CleanupActivity(0);
    if (floor && floor->mOwner != this) {
        ChangeParent(floor, 0, 0, 0);
        mSkeleton->syncChildTransforms(0);
    }
    Lock();
    if (handle)
        handle->mOccupied = 0;
    Unlock();
    return floor != 0;
}

// 0x10035aa0 ?AppropriateFloor@MfLiveActor@@QAEPAVMfFloor@@PAVMfDblList@@@Z
MfFloor *MfLiveActor::AppropriateFloor(MfDblList *floors)
{
    MfVector3 pos = mSkeletonTraversal->node(1)->mGlobalTransform.translation();
    return AppropriateFloor(pos, floors, 0, 1);
}

// EQUIV 0x10035b20 ?AppropriateFloor@MfLiveActor@@QAEPAVMfFloor@@VMfVector3@@PAVMfDblList@@PAV2@H@Z: reviewed: same logic, calls and constants; the original constructs the local MfTransform3 translation with the imported MfVector3() (zeroes, expanded here), keeps the traversal in edi where this reloads it from its stack slot, register choice
// The floor under pos: a ray cast down from the actor's height above each
// floor; the nearest hit wins, a floor whose prototype says it takes no
// targets wins a tie, and preferred wins whenever it is hit. Floors in
// exclude, and with notOwn the actor's own floors, are skipped.
MfFloor *MfLiveActor::AppropriateFloor(MfVector3 pos, MfDblList *exclude, MfFloor *preferred, int notOwn)
{
    MfSkeletonTraversal *floors = new MfSkeletonTraversal(mSpace->mRoot, MfFloor::TYPE, 0);
    MfTransform3 inverse;
    MfVector3 start;
    MfVector3 down;
    MfVector3 hit;
    float radius, height;
    MfFloor *best = 0;
    float bestDistance = 10000000.0f;
    mPrototype->GetBoundingCylinder(radius, height);
    for (int i = 0; i < floors->numNodes(); i++) {
        MfFloor *floor = (MfFloor *)floors->node(i);
        if (exclude && exclude->Find(floor))
            continue;
        if (floor->mOwner == this && notOwn)
            continue;
        inverse = ~floor->mGlobalTransform;
        start = inverse * pos;
        start.y() += height;
        start = floor->mGlobalTransform * start;
        down = floor->mGlobalTransform.rotation() * MfVector3(0.0f, -1.0f, 0.0f);
        MfColResultRay result;
        if (!floor->mColNode.CheckCollision(start, down, result, true, -1.0f))
            continue;
        if (!((MfSkeletonNode *)floor->parent())->mColNode.CheckCollision(start, down, result, true, -1.0f))
            continue;
        hit = result.pos;
        float distance = (pos - hit).length();
        float difference = distance - bestDistance;
        if (difference < 0.0f)
            difference = -difference;
        if (distance <= bestDistance || difference < 0.00001f) {
            if (difference < 0.00001f && best->mProto->FloorInfo()->m00 == 1)
                continue;
            best = floor;
            bestDistance = distance;
        }
        if (floor == preferred) {
            best = floor;
            break;
        }
    }
    delete floors;
    return best;
}

// 0x10035ee0 ?PostPlannerFailure@MfLiveActor@@IAEXH@Z
void MfLiveActor::PostPlannerFailure(int planner)
{
    MfMsg *msg = new MfMsg(16, 0, 0, this, this);
    msg->SetData(new MfPlannerFailureData(mRequestID, (int)this, planner, 0));
    g_msgDispatcher->Post(msg);
}

// 0x10035fa0 ?HandlePlannerFailure@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandlePlannerFailure(MfMsg &msg)
{
    if (mLocoPlan.mPath) {
        MfPathPoint *point = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
        while (point) {
            delete point;
            point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
        }
        delete mLocoPlan.mPath;
        mLocoPlan.mPath = 0;
    }
    MfPlannerFailureData *data = (MfPlannerFailureData *)msg.mData;
    msg.m38 = data->mPlanner;
    gEventData = data;
    if (!mSpace)
        return;
    if (data->mPlanner == 5 && (data->mReason == 0 || data->mReason == 3)) {
        if (data->mReason == 0)
            mLastLocoFailure = 1;
        else if (data->mReason == 3)
            mLastLocoFailure = 3;
        if (!mLocoPlan.mBlockedEdge && !mLocoPlan.mBlockedTo && !mLocoPlan.mBlockedCell) {
            if (mLocoPlan.mb4)
                mLocoPlan.mb4 = 0;
        } else {
            MfBlockage *blockage = new MfBlockage;
            blockage->mBlocker = (MfActor *)msg.mObject;
            blockage->mActor = this;
            if (blockage->mBlocker)
                blockage->mPosition = blockage->mBlocker->mSkeleton->mGlobalTransform.translation();
            MfRoadEdge *edge = mLocoPlan.mBlockedEdge;
            if (edge) {
                blockage->mEdge = edge;
                edge->mBlocked = 1;
            } else if (mLocoPlan.mBlockedTo) {
                mLocoPlan.mIndex1.Add(mLocoPlan.mBlockedFrom, (void *)1);
                mLocoPlan.mIndex2.Add(mLocoPlan.mBlockedTo, (void *)1);
            } else {
                mLocoPlan.mIndex3.Add(mLocoPlan.mBlockedCell, (void *)1);
            }
            mSpace->mBlockEdge->Add(blockage);
        }
        IsCurrentActivity(MfLocoActivity::TYPE);
        RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                        mLocoPlan.mFloor, mLocoPlan.mbc, mLocoPlan.m00, mLocoPlan.m20,
                        mLocoPlan.mTarget, mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c,
                        mLocoPlan.m30, mLocoPlan.m34, mLocoPlan.mEventID, 0);
        MfMsg failure(21, 0, 0, this, this);
        Handle3rdPartyMsg(failure);
    } else if (data->mPlanner == 5 && (mCmdState == 4 || mCmdState == 5)) {
        ManipFailure(msg);
    } else if (data->mPlanner == 5) {
        LocoFailure(msg);
    } else if (data->mPlanner >= 0 && data->mPlanner <= 4) {
        ManipFailure(msg);
    }
}

// 0x10036240 ?MakeGestureActivity@MfLiveActor@@IAEPAVMfActivity@@KPAVMfSkill@@H@Z
MfActivity *MfLiveActor::MakeGestureActivity(unsigned long time, MfSkill *skill, int a3)
{
    MfActivity *gesture = new MfGestureActivity(this, time, skill, mSitting, a3);
    MfActivity *activity = gesture;
    MfActivity *hold = MakeHoldTargetActivity(0, 1, 0);
    if (hold) {
        MfOverlayActivity *overlay = new MfOverlayActivity(this, time);
        overlay->AddActivity(gesture);
        overlay->AddActivity(hold);
        activity = overlay;
    }
    long duration = skill->lastTime();
    duration -= skill->firstTime();
    activity->setInitialConfig(mSkeletonTraversal, duration < 2000 ? duration / 4 : 500);
    activity->SetUniqueID(activity->GetUniqueID());
    return activity;
}

// 0x10036370 ?HandleGestureCollision@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleGestureCollision(MfMsg &msg)
{
    MfCollisionMsgData *data = (MfCollisionMsgData *)msg.mData;
    gEventData = data;
    if (!mSpace)
        return;
    if (data->mSkill)
        mLastCollision.strSkillName = data->mSkill->Name();
    else
        mLastCollision.strSkillName = NULL_STRING;
    if (mSpace->GetLiveActors().Contains(data->mOther))
        mLastCollision.plaOther = data->mOther;
    else
        mLastCollision.plaOther = 0;
    mLastCollision.collPt = data->mPoint;
    mLastCollision.collDir = data->mDirection;
    mLastCollision.psnOtherNode = data->mNode;
    mLastCollision.psnNode = data->mOtherNode;
    mLastCollision.playBack = data->mPlayBack;
    mLastCollision.surfaceNormal = data->mNormal;
    mLastCollision.surfaceNormal.length();
    mLastEventID = data->mEventID;
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// 0x100364d0 ?HandleReceivedCollision@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleReceivedCollision(MfMsg &msg)
{
    MfCollisionMsgData *data = (MfCollisionMsgData *)msg.mData;
    gEventData = data;
    if (!mSpace)
        return;
    if (data->mSkill && g_world->ActorExists(data->mActor))
        mLastCollision.strSkillName = data->mSkill->Name();
    else
        mLastCollision.strSkillName = NULL_STRING;
    if (mSpace->GetLiveActors().Contains(data->mActor))
        mLastCollision.plaOther = data->mActor;
    else
        mLastCollision.plaOther = 0;
    mLastCollision.collPt = data->mPoint;
    mLastCollision.collDir = data->mDirection;
    mLastCollision.psnOtherNode = data->mOtherNode;
    mLastCollision.psnNode = data->mNode;
    mLastCollision.playBack = data->mPlayBack;
    mLastCollision.surfaceNormal = data->mNormal;
    mLastEventID = data->mEventID;
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// 0x10036650 ?HandleConformFailure@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleConformFailure(MfMsg &msg)
{
    if (msg.m38 == 1) {
        MfContourFailMsgData *data = (MfContourFailMsgData *)msg.mData;
        gEventData = data;
        mLastContourFailDown.failNode = data->mNode;
    }
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// TODO 0x100366c0 ?RequestLocoPlan@MfLiveActor@@IAEHHPAVMfLocoSkill@@PAVMfSkill@@ABVMfTransform2@@PAVMfFloor@@HHHPAV1@MMHMMHH@Z: (55% similar) the original clears a7 and relative only when more stretches follow (`else` of `if (!mLocoPlan.mPath)`), so a walk's last stretch keeps the goal heading; written so here it is 53% similar (VC5 then keeps the zero in edi instead of ebx), so port/edits/RuntimeLib.py makes the port do it; float rounding: the original keeps margin = radius + radius (radius loaded twice) on the x87 stack, here margin is a float spilled to memory (differs on overflow); also the zero in ebp (ebx here), a 4-byte smaller frame, and in the mb0 branch the imported MfRotation3/MfVector3 operator= and here/start copied through the FPU (inline budget)
// Asks the planner for a walk to goal (on floor, or relative to target).
// The request is remembered in mLocoPlan so that it can be repeated; the
// road map gives the floors on the way, and each request covers the stretch
// up to the next turn. Returns the event ID (-1 when no way is found).
int MfLiveActor::RequestLocoPlan(int notify, MfLocoSkill *locoSkill, MfSkill *skill, const MfTransform2 &goal,
                                 MfFloor *floor, int mode, int a7, int relative, MfLiveActor *target,
                                 float a10, float a11, int a12, float a13, float a14, int eventID, int restart)
{
    if (eventID == -1)
        eventID = MfWorld::CreateUniqueNumber();
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = 0;
    mMoveRequest = 0;
    mLockRequest = 0;
    mBlockList.Clear();
    mLocoRequest = ++mRequestID;
    if (relative) {
        // the goal is given relative to the target, on floor
        MfTransform3 local(target->mSkeleton->mGlobalTransform);
        local = ~floor->mGlobalTransform * local;
        MfTransform2 local2;
        float angle;
        MfVector3 axis;
        local2.translation().x() = local.translation().z();
        local2.translation().y() = local.translation().x();
        local.rotation().getValue(angle, axis);
        if (axis.y() < 0.0f)
            angle = -angle;
        local2.rotation() = angle;
        mLocoPlan.mGoal = local2;
    } else {
        mLocoPlan.mGoal = goal;
    }
    mLocoPlan.mFloor = floor;
    mLocoPlan.mLocoSkill = locoSkill;
    mLocoPlan.mSkill = skill;
    mLocoPlan.m00 = a7;
    mLocoPlan.mEventID = eventID;
    mLocoPlan.m88 = notify;
    mLocoPlan.m20 = relative;
    mLocoPlan.m28 = a10;
    mLocoPlan.m2c = a11;
    mLocoPlan.m1c = a12;
    mLocoPlan.mTarget = target;
    mLocoPlan.m30 = a13;
    mLocoPlan.m34 = a14;
    mLocoPlan.mbc = mode;
    if (!IsCurrentActivity(MfLocoActivity::TYPE))
        mCmdState = mode;
    if (restart) {
        if (mLocoPlan.mPath) {
            MfPathPoint *point = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
            while (point) {
                delete point;
                point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
            }
            delete mLocoPlan.mPath;
            mLocoPlan.mPath = 0;
        }
        MfPath2 *path = (MfPath2 *)mLocoPlan.mac;
        if (path)
            delete path;
        mLocoPlan.mb0 = 0;
        mLocoPlan.ma4 = 0;
        mLocoPlan.mac = 0;
        mLocoPlan.mIndex1.Clear();
        mLocoPlan.mIndex2.Clear();
        mLocoPlan.mIndex3.Clear();
        mLocoPlan.mb4 = 1;
        mSpace->ClearPlanningActorBlockage(this);
    }
    MfRoadMapBuilder builder;
    MfVector3 start;
    MfVector3 stop;
    MfVector3 unused;
    MfTransform2 here;
    MfTransform2 there = mLocoPlan.mGoal;
    MfTransform3 frame;
    MfVector3 hip;
    MfFloor *current = 0;
    if (mSkeleton->parent()->IsType(MfFloor::TYPE))
        current = (MfFloor *)mSkeleton->parent();
    if (mLocoPlan.mb0) {
        // go on from where the last stretch ends
        start.x() = mLocoPlan.m8c.translation().y();
        start.y() = 0.0f;
        start.z() = mLocoPlan.m8c.translation().x();
        here = mLocoPlan.m8c;
        frame.rotation() = MfRotation3(mLocoPlan.m8c.rotation(), MfVector3(0.0f, 1.0f, 0.0f));
        frame.translation() = start;
        MfTransform3 &parent = ((MfSkeletonNode *)mSkeleton->parent())->mGlobalTransform;
        start = parent * start;
        frame = parent * frame;
        hip = frame.translation();
        hip.y() = mSkeletonTraversal->node(1)->mTransform.translation().y() + frame.translation().y();
        hip = parent * hip;
    } else {
        start = mSkeleton->mGlobalTransform.translation();
        here = traceTransform2D();
        frame = mSkeleton->mGlobalTransform;
        hip = mSkeletonTraversal->node(1)->mGlobalTransform.translation();
    }
    MfFloor *from;
    MfPathPoint *point;
    if (!mLocoPlan.mPath) {
        from = AppropriateFloor(hip, 0, restart ? 0 : current, 1);
        if (!from && !current)
            return -1;
        if (!from)
            from = current;
        MfSkeletonNode *to = floor;
        if (!to)
            to = from;
        stop.x() = mLocoPlan.mGoal.translation().y();
        stop.z() = mLocoPlan.mGoal.translation().x();
        stop.y() = 0.0f;
        stop = to->mGlobalTransform * stop;
        builder.AddFloor(from, mSpace->mRoadMap);
        mLocoPlan.mPath = builder.FindPath(from, start, to, stop, mSpace->mRoadMap, mLocoPlan.mb4,
                                           &mLocoPlan.mIndex1, &mLocoPlan.mIndex2, &mLocoPlan.mIndex3, &mFilter);
        if (!mLocoPlan.mPath) {
            MfMsg *msg = new MfMsg(16, 0, 0, this, this);
            msg->SetData(new MfPlannerFailureData(mLocoRequest, (int)this, 5, 4));
            g_msgDispatcher->Post(msg);
            return -1;
        }
        point = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
        mLocoPlan.mBlockedFrom = mLocoPlan.mc8 = (int)FindRoadCell(from, start, mSpace->mRoadMap);
        mLocoPlan.mBlockedTo = 0;
        mLocoPlan.mBlockedCell = 0;
        mLocoPlan.mBlockedEdge = 0;
    } else {
        point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
        mLocoPlan.mBlockedTo = 0;
        mLocoPlan.mBlockedFrom = 0;
    }
    // follow the way to the first portal that is not straight ahead
    MfVector3 position;
    MfRoadCell *cell = (MfRoadCell *)mLocoPlan.mc8;
    MfRoadCell *next = cell;
    mLocoPlan.mBlockedEdge = 0;
    while (point) {
        MfRoadPortal *portal = point->mPortal;
        if (!portal) {
            if (mLocoPlan.mPath->GetCount() == 1) {
                mLocoPlan.mBlockedEdge = 0;
            } else {
                there = mLocoPlan.mGoal;
                cell = next;
                from = mLocoPlan.mFloor;
            }
            break;
        }
        if (portal->mFrom == cell)
            next = portal->mTo;
        else
            next = portal->mFrom;
        from = (MfFloor *)cell->mFloor;
        if (from != portal->mFrom->mFloor && from != portal->mTo->mFloor)
            from = current;
        stop = ~from->mGlobalTransform * ((portal->mStart + portal->mEnd) / 2.0f);
        position = ~from->mGlobalTransform * start;
        stop.y() = 0.0f;
        position.y() = 0.0f;
        mLocoPlan.mBlockedEdge = point->mEdge;
        MfDblListItem *first;
        if (point == mLocoPlan.mPath->GetFirst(first)) {
            mLocoPlan.mBlockedTo = (int)point->mPortal;
            mLocoPlan.mBlockedCell = (int)point->mPortal;
        }
        bool passed = (stop - position).length() < 0.0001f;
        if (!passed) {
            there.translation().x() = stop.z();
            there.translation().y() = stop.x();
            break;
        }
        cell = next;
        point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
    }
    mLocoPlan.mc8 = (int)next;
    MfDblListItem *pos = mLocoPlan.mPathPos;
    if (!point || !mLocoPlan.mPath->GetNext(pos)) {
        // the way ends with this stretch
        MfPathPoint *done = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
        while (done) {
            delete done;
            done = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
        }
        delete mLocoPlan.mPath;
        mLocoPlan.mPath = 0;
    }
    frame = ~from->mGlobalTransform * frame;
    MfVector3 axis;
    float angle;
    frame.rotation().getValue(angle, axis);
    if (axis.y() < 0.0f)
        angle = -angle;
    here.translation().y() = frame.translation().x();
    here.translation().x() = frame.translation().z();
    here.rotation() = angle;
    int last = 0;
    if (!mLocoPlan.mPath)
        last = 1;
    a7 = 0;
    relative = 0;
    MfDblList *floors = 0;
    if (cell != next && !next->mFloor->mProto->FloorInfo()->m00 ||
        cell->mFloor->mProto->FloorInfo()->m00 == 1 && next->mFloor->mProto->FloorInfo()->m00 == 1) {
        floors = new MfDblList;
        floors->Add(next->mFloor->parent());
    }
    // the region of the cell, with room for the actor around it
    float radius = mPrototype->cylinderRadius;
    float margin = radius + radius;
    float grow = radius * -4.0f;
    float left = cell->mColumn * cell->mWidth + cell->mOriginX - margin;
    float bottom = cell->mRow * cell->mDepth + cell->mOriginY - margin;
    float width = cell->mWidth - grow;
    float depth = cell->mDepth - grow;
    mLocoPlan.mb8 = 0;
    AddRef();
    MfMsg *msg = new MfMsg(19, 0, 2, this, 0);
    int sit = 0;
    if (mode == 4 && mManipPlan.mManipSkill->mManipType == 3)
        sit = 1;
    float heading = there.rotation() - 3.14159265f;
    msg->SetData(new MfLocoPlanReqData(mRequestID, this, sit, (int)cell->mFloor, (int)next->mFloor,
                                       (const MfVector3 &)here, (const MfVector3 &)there, (int)locoSkill, mode,
                                       a7, relative, (int)target, *(int *)&a10, *(int *)&a11, a12,
                                       *(int *)&a13, *(int *)&a14, *(int *)&left, *(int *)&bottom,
                                       *(int *)&width, *(int *)&depth, last, (int)floors, g_world->mClock,
                                       *(int *)&heading));
    g_msgDispatcher->Post(msg);
    mLocoPlan.mLast = mLocoPlan.mGoal;
    return eventID;
}

// 0x10037610 ?LocoFailure@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::LocoFailure(MfMsg &msg)
{
    MfPlannerFailureData *data = (MfPlannerFailureData *)msg.mData;
    if (mLocoRequest != data->mRequestID)
        return;
    mLocoRequest = 0;
    if (!IsCurrentActivity(MfLocoActivity::TYPE))
        mCmdState = 0;
    mLastEventID = mLocoPlan.mEventID;
    if (data->mReason == 0)
        mLastLocoFailure = 1;
    else if (data->mReason == 1)
        mLastLocoFailure = 2;
    else if (data->mReason == 3)
        mLastLocoFailure = 3;
    else if (data->mReason == 4)
        mLastLocoFailure = 4;
    if (UpdateFSMWithEvent() && !mController)
        mHFSM->HandleEvent(msg);
    else if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
    mSpace->ClearPlanningActorBlockage(this);
}

// the end of a path (a ring keeps only its last RING_SIZE points)
static inline MfTransform2 &LastPoint(MfPath2 *path)
{
    int last = (!path->mOpen ? path->m28 : path->mCount) - 1;
    return path->Point(last);
}

// 0x100376f0 ?HandleLocoPlan@MfLiveActor@@IAEXAAVMfMsg@@@Z
// The planner's path for the last locomotion request: starts walking it (or
// leaves it to the running locomotion), and asks again when the path ends
// away from the goal or on another floor.
void MfLiveActor::HandleLocoPlan(MfMsg &msg)
{
    MfLocoPlanResultData *data = (MfLocoPlanResultData *)msg.mData;
    if (!mSpace) {
        MfPath2 *path = data->mPath;
        if (path)
            delete path;
        return;
    }
    if (mLocoRequest == data->mRequestID && mBlockList.GetCount() <= 0) {
        if (!IsCurrentActivity(MfLocoActivity::TYPE) && !IsCurrentActivity(Mf3dPathActivity::TYPE))
            data->m1c = g_world->mClock;
        mLocoRequest = 0;
        MfLocoSkill *skill = (MfLocoSkill *)data->m10;
        float radius, height;
        if (!mLocoPlan.mb0 && !mLocoPlan.m88)
            CleanupActivity(0);
        MfActivity *loco = IsCurrentActivity(MfLocoActivity::TYPE);
        MfTransform2 end = LastPoint(data->mPath);
        mLocoPlan.mLast = end;
        MfSkeletonNode *parent = (MfSkeletonNode *)mSkeleton->parent();
        if (loco && !loco->Finished() && !mLocoPlan.m88) {
            mLocoPlan.mb0 = 0;
            mLocoPlan.ma8 = data->m14;
            mLocoPlan.mac = (int)data->mPath;
            mLocoPlan.ma4 = (int)loco;
            return;
        }
        ChangeParent((MfSkeletonNode *)data->m14, 0, 0, 0);
        float dist = (end.translation() - mLocoPlan.mGoal.translation()).length();
        int replan = 0;
        if (mLocoPlan.mPath && mLocoPlan.m20 && dist > mLocoPlan.m2c)
            replan = 1;
        else if (data->m14 != (int)mLocoPlan.mFloor)
            replan = 1;
        else if (!mLocoPlan.m20 && dist > 0.01f)
            replan = 1;
        mLocoPlan.m2c = *(float *)&data->m24;
        if (replan) {
            mLocoPlan.mb0 = 1;
            mLocoPlan.m8c = end;
            RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                            mLocoPlan.mFloor, data->m20, mLocoPlan.m00, mLocoPlan.m20, mLocoPlan.mTarget,
                            mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c, mLocoPlan.m30, mLocoPlan.m34,
                            mLocoPlan.mEventID, 0);
        }
        MfActivity *activity;
        if (((MfSkeletonNode *)data->m14)->mProto->FloorInfo()->m00 == 1) {
            MfSkeletonNode *node;
            for (node = (MfSkeletonNode *)mSkeleton->parent(); node && node->GetNodeType() != MF_NODE_GEOMETRY;
                 node = (MfSkeletonNode *)node->parent())
                ;
            activity = MakeLocoActivity(data->mPath, skill, data->m1c, mPlannedLocoOptions | 0x400, mPlannedLocoParam,
                                        mPlannedLocoReach, node, 0);
        } else if (parent->GetNodeType() == MF_NODE_FLOOR && parent->mProto->FloorInfo()->m00 == 1) {
            mPrototype->GetBoundingCylinder(radius, height);
            activity = MakeLocoActivity(data->mPath, mLocoPlan.mLocoSkill, g_world->mClock, mPlannedLocoOptions | 0x200,
                                        mPlannedLocoParam, height * 3.0f, 0, 0);
        } else
            activity = MakeLocoActivity(data->mPath, skill, data->m1c, mPlannedLocoOptions, mPlannedLocoParam, mPlannedLocoReach,
                                        0, 0);
        gEventData = &mLocoPlan;
        activity->SetUniqueID(mLocoPlan.mEventID);
        if (mLocoPlan.m88) {
            mLastEventID = mLocoPlan.mEventID;
            mLastActivity = activity;
            if (UpdateFSMWithEvent() && !mController) {
                MfMsg done(20, 0, 0, this, this);
                mHFSM->HandleEvent(done);
            } else if (mControlMode == 1 && mLive) {
                MfMsg done(20, 0, 0, this, this);
                UpdateAppEventHandler(done);
            }
        } else
            SetActivity(activity);
        mCmdState = data->m20;
        ChangeCmdState();
        return;
    }
    MfPath2 *path = data->mPath;
    if (path)
        delete path;
}

// 0x10037c00 ?MakeLocoActivity@MfLiveActor@@IAEPAVMfActivity@@PAVMfPath2@@PAVMfLocoSkill@@KHMMPAVMfSkeletonNode@@H@Z
MfActivity *MfLiveActor::MakeLocoActivity(MfPath2 *path, MfLocoSkill *skill, unsigned long time, int options,
                                          float a5, float a6, MfSkeletonNode *node, int a8)
{
    // the request options (bits) as MfLocoActivity arguments
    int option2 = 0, option4 = 0, option8000 = 0, option1000 = 0;
    int option10000 = 0;
    int mode = 0;
    int notOption20000 = 1;
    if (options & 0x2)
        option2 = 1;
    if (options & 0x4)
        option4 = 1;
    if (options & 0x8000)
        option8000 = 1;
    if (options & 0x200)
        mode = 1;
    if (options & 0x400)
        mode = 2;
    if (options & 0x1000)
        option1000 = 1;
    if (options & 0x10000)
        option10000 = 1;
    if (options & 0x20000)
        notOption20000 = 0;
    MfActivity *loco = new MfLocoActivity(this, time, skill, path, option2, option4, option8000, mode, a5, a6,
                                          node, option1000, option10000, a8, notOption20000);
    MfActivity *activity = loco;
    MfActivity *hold = MakeHoldTargetActivity(0, 0, 0);
    if (hold) {
        MfOverlayActivity *overlay = new MfOverlayActivity(this, time);
        overlay->AddActivity(loco);
        overlay->AddActivity(hold);
        activity = overlay;
    }
    activity->setInitialConfig(mSkeletonTraversal, 500);
    activity->SetUniqueID(activity->GetUniqueID());
    return activity;
}

// 0x10037da0 ?RequestManipGCPlan@MfLiveActor@@IAEHHPAV1@PAVMfFloor@@PAVMfManipSkill@@2PAVMfLocoSkill@@PAVMfSkill@@PAVMfHandle@@HHABVMfTransform3@@ABVMfVector3@@H@Z
int MfLiveActor::RequestManipGCPlan(int a1, MfLiveActor *target, MfFloor *floor, MfManipSkill *skill,
                                    MfManipSkill *otherSkill, MfLocoSkill *locoSkill, MfSkill *locoBase,
                                    MfHandle *handle, int a9, int hasGoal, const MfTransform3 &goal,
                                    const MfVector3 &offset, int eventID)
{
    if (target) {
        MfGuardHookData guard;
        if (skill && skill->mManipType == 3) {
            guard.pActor = this;
            guard.pLocker = target->GetRootNode();
        } else {
            guard.pActor = target;
            guard.pLocker = mSkeleton;
        }
        if (gHookManager->CallHooks(MF_HOOK_GUARD_COP, &guard) > 0)
            return -1;
    }
    if (!floor && locoSkill)
        return -1;
    if (floor && floor->mProto->FloorInfo()->m00)
        return -1;
    if (eventID == -1)
        eventID = MfWorld::CreateUniqueNumber();
    mManipPlan.mManipSkill = skill;
    mCmdState = 0;
    mManipPlan.mOtherSkill = otherSkill;
    mManipPlan.mLocoSkill = locoSkill;
    mManipPlan.mSkill = locoBase;
    mManipPlan.mHandle = handle;
    mManipPlan.mTarget = target;
    mManipPlan.mFloor = floor;
    mManipPlan.m4c = hasGoal;
    mManipPlan.mEventID = eventID;
    mManipPlan.m74 = a1;
    mManipPlan.mOffset = offset;
    if (mManipPlan.mTargetConfig)
        delete [] mManipPlan.mTargetConfig;
    if (target) {
        mManipPlan.mTargetConfig = target->mSkeletonTraversal->CopyTransforms();
        mManipPlan.mFrom = ((MfSkeletonNode *)target->mSkeleton->parent())->mGlobalTransform;
    } else {
        mManipPlan.mTargetConfig = 0;
    }
    if (hasGoal)
        mManipPlan.mTo = goal;
    RequestCheckReachable(target, floor, skill, handle, hasGoal, goal);
    return eventID;
}

// 0x10038010 ?RequestCheckReachable@MfLiveActor@@IAEXPAV1@PAVMfFloor@@PAVMfManipSkill@@PAVMfHandle@@HABVMfTransform3@@@Z
void MfLiveActor::RequestCheckReachable(MfLiveActor *target, MfFloor *floor, MfManipSkill *skill,
                                        MfHandle *handle, int a5, const MfTransform3 &goal)
{
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = ++mRequestID;
    mMoveRequest = 0;
    mLockRequest = 0;
    AddRef();
    if (target)
        target->AddRef();
    MfMsg *msg = new MfMsg(23, 0, 3, this, 0);
    msg->SetData(new MfCheckReachableReqData(mRequestID, this, target, mManipPlan.m20,
                                             mManipPlan.mTargetConfig, mManipPlan.mFrom, (int)floor,
                                             (int)skill, (int)handle, a5, goal,
                                             (int)mManipPlan.mLocoSkill, mManipPlan.mOffset));
    g_msgDispatcher->Post(msg);
}

// 0x10038140 ?HandleCheckReachable@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleCheckReachable(MfMsg &msg)
{
    MfCheckReachableResultData *data = (MfCheckReachableResultData *)msg.mData;
    if (!mSpace || mReachRequest != data->mRequestID)
        return;
    if (data->mReachable == 0) {
        if (!mSpace->GetLiveActors().Contains(mManipPlan.mTarget)) {
            PostPlannerFailure(1);
            return;
        }
        RequestGCPlan(mManipPlan.mTarget, mManipPlan.mManipSkill, mManipPlan.mHandle, mManipPlan.m4c,
                      mManipPlan.mTo);
    } else if (data->mReachable == 1) {
        if (!mManipPlan.mLocoSkill) {
            PostPlannerFailure(1);
            return;
        }
        if (mSkeleton->parent()->Type() != MfFloor::TYPE && !AttachToFloor()) {
            PostPlannerFailure(1);
            return;
        }
        // the point to walk to is an (x, y, heading) triple
        RequestLocoPlan(mManipPlan.m74, mManipPlan.mLocoSkill, mManipPlan.mSkill,
                        (const MfTransform2 &)data->mPoint, mManipPlan.mFloor, 4, 1, 0, 0, 0.0f, 0.0f,
                        0, 0.0f, 0.0f, mManipPlan.mEventID, 1);
    }
}

// 0x10038260 ?RequestGCPlan@MfLiveActor@@IAEXPAV1@PAVMfManipSkill@@PAVMfHandle@@HABVMfTransform3@@@Z
void MfLiveActor::RequestGCPlan(MfLiveActor *target, MfManipSkill *skill, MfHandle *handle, int a4,
                                const MfTransform3 &goal)
{
    mLocoRequest = 0;
    mGCRequest = ++mRequestID;
    mManipPlan.m6c = mRequestID;
    mReachRequest = 0;
    mMoveRequest = 0;
    mLockRequest = 0;
    mCmdState = 8;
    AddRef();
    if (target)
        target->AddRef();
    MfMsg *msg = new MfMsg(25, 0, 3, this, 0);
    msg->SetData(new MfManipGCReqData(mRequestID, this, target, mManipPlan.m20,
                                      mManipPlan.mTargetConfig, mManipPlan.mFrom, 0, 0, skill, handle,
                                      a4, goal));
    g_msgDispatcher->Post(msg);
}

// 0x10038750 ?RequestManipMoveTo@MfLiveActor@@IAEHHPAV1@PAVMfManipSkill@@HH@Z
int MfLiveActor::RequestManipMoveTo(int a1, MfLiveActor *target, MfManipSkill *skill, int a4,
                                    int eventID)
{
    if (eventID == -1)
        eventID = MfWorld::CreateUniqueNumber();
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = 0;
    mMoveRequest = ++mRequestID;
    mLockRequest = 0;
    mCmdState = 10;
    mManipPlan.mEventID = eventID;
    mManipPlan.m74 = a1;
    AddRef();
    if (target)
        target->AddRef();
    MfMsg *msg = new MfMsg(27, 0, 3, this, 0);
    msg->SetData(new MfManipMoveRequestData(mRequestID, this, target, skill, a4));
    g_msgDispatcher->Post(msg);
    return eventID;
}

// TODO 0x10038390 ?HandleManipGC@MfLiveActor@@IAEXAAVMfMsg@@@Z: (84% similar) control-flow layout (logic and calls otherwise the same): the original puts
// the discarding of a stale path right after the tests (here it goes to the end; early
// return, if/else both ways, a negated condition and nested ifs tried); the three
// duration computations stay separate when the motion is assigned after them
// The planner's grasp path: starts the manipulation with the planned path.
void MfLiveActor::HandleManipGC(MfMsg &msg)
{
    MfManipGCResultData *data = (MfManipGCResultData *)msg.mData;
    if (!mSpace || !mSpace->GetLiveActors().Contains(mManipPlan.mTarget) || mGCRequest != data->mRequestID) {
        // a stale answer
        MfGCPath *path = (MfGCPath *)data->mPlan;
        if (path)
            delete path;
        return;
    }
    {
        MfLiveActor *target = (MfLiveActor *)data->m10;
        MfGCPath *path = (MfGCPath *)data->mPlan;
        MfManipSkill *skill = (MfManipSkill *)data->m14;
        unsigned long now = g_world->mClock;
        MfHandle *handle = (MfHandle *)target->allTraversal()->node(path->m30);
        int motion;
        long duration;
        if (skill->ManipType() == 1) {
            duration = skill->lastTime();
            duration -= skill->firstTime();
            motion = data->m1c ? 6 : 0;
        } else if (skill->ManipType() == 3) {
            Lock();
            SetIsSitting(1);
            Unlock();
            duration = skill->lastTime();
            duration -= skill->firstTime();
            motion = 4;
        } else {
            duration = skill->lastTime();
            duration -= skill->firstTime();
            motion = 3;
        }
        if (motion == 0) {
            MfTransform3 parent = mSkeleton->globalTransform();
            MfTransform3 target = mManipPlan.mFrom * *mManipPlan.mTargetConfig;
            MfTransform3 grasp = ~parent * target;
            handle->mGraspRotation = grasp.rotation();
            handle->SetFixXformForGrasp(path->m34, path->m38);
        }
        MfSkeletonNode *node;
        if (motion == 4)
            node = handle;
        else if (motion == 0) {
            MfDblListItem *pli;
            node = skeletonTraversal()->node(((MfManipHand *)skill->HandList().GetFirst(pli))->mNode);
        } else
            node = 0;
        if (motion == 6)
            handle->SetTargetXforms(target);
        MfActivity *activity = MakeManipActivity(target, path, 0, duration, skill, handle, node, now, motion);
        gEventData = &mManipPlan;
        activity->SetUniqueID(mManipPlan.mEventID);
        if (mManipPlan.m74) {
            mLastEventID = mManipPlan.mEventID;
            mLastActivity = activity;
            if (UpdateFSMWithEvent() && !mController) {
                MfMsg done(26, 0, 0, this, this);
                mHFSM->HandleEvent(done);
            } else if (mControlMode == 1 && mLive) {
                MfMsg done(26, 0, 0, this, this);
                UpdateAppEventHandler(done);
            }
        } else {
            SetActivity(activity);
            ChangeCmdState();
        }
    }
}

// TODO 0x10038890 ?HandleManipMoveTo@MfLiveActor@@IAEXAAVMfMsg@@@Z: (60% similar) control-flow layout: the original places the stale-path discard after the Contains/request tests and merges the two MakeManipActivity calls after seven arguments (here after three, with the early return in the middle); it also calls the local ??_EMfTransform3 for each delete [] where this expands ??_M + delete (same effect)
void MfLiveActor::HandleManipMoveTo(MfMsg &msg)
{
    MfManipMoveResultData *data = (MfManipMoveResultData *)msg.mData;
    if (!mSpace) {
        MfMTPath *path = data->mPath;
        if (path) {
            if (path->mFrom)
                delete [] path->mFrom;
            if (path->mTo)
                delete [] path->mTo;
            delete path;
        }
        return;
    }
    if (!mSpace->GetLiveActors().Contains(data->mTarget)) {
        PostPlannerFailure(4);
    } else if (data->mRequestID == mMoveRequest) {
        MfMTPath *path = data->mPath;
        unsigned long now = g_world->mClock;
        MfLiveActor *target = data->mTarget;
        MfManipSkill *skill = data->mSkill;
        long duration = skill->lastTime();
        duration -= skill->firstTime();
        if (!duration)
            duration = 1000;
        MfActivity *activity;
        if (data->mRelease && ((MfSkeletonNode *)target->mSkeleton->parent())->mOwner == this)
            activity = MakeManipActivity(target, 0, path, duration, skill, 0,
                                         NewParentAfterUngrasp(skill, target), now, 1);
        else
            activity = MakeManipActivity(target, 0, path, duration, skill, 0, 0, now, 2);
        gEventData = &mManipPlan;
        activity->SetUniqueID(mManipPlan.mEventID);
        if (mManipPlan.m74) {
            mLastEventID = mManipPlan.mEventID;
            mLastActivity = activity;
            if (UpdateFSMWithEvent() && !mController) {
                MfMsg done(28, 0, 0, this, this);
                mHFSM->HandleEvent(done);
            } else if (mControlMode == 1 && mLive) {
                MfMsg done(28, 0, 0, this, this);
                UpdateAppEventHandler(done);
            }
        } else {
            SetActivity(activity);
            ChangeCmdState();
        }
        return;
    }
    MfMTPath *path = data->mPath;
    if (path) {
        if (path->mFrom)
            delete [] path->mFrom;
        if (path->mTo)
            delete [] path->mTo;
        delete path;
    }
}

// 0x10038ad0 ?NewParentAfterUngrasp@MfLiveActor@@IAEPAVMfSkeletonNode@@PAVMfManipSkill@@PAV1@@Z
// Where target goes when skill lets go of it: to another hand of this actor
// that still holds it, else to what is under it.
MfSkeletonNode *MfLiveActor::NewParentAfterUngrasp(MfManipSkill *skill, MfLiveActor *target)
{
    MfDblListItem *handPos;
    int held = 0;
    MfSkeletonNode *holder;
    MfDblListItem *pos;
    MfDblList *children = &mChildList;
    for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
         entry = (MfChildEntry *)children->GetNext(pos)) {
        if (entry->mActor == target) {
            MfDblList *hands = &skill->mHandList;
            MfManipHand *hand;
            for (hand = (MfManipHand *)hands->GetFirst(handPos); hand; hand = (MfManipHand *)hands->GetNext(handPos))
                if (entry->mHand == hand->mNode)
                    break;
            if (!hand) {
                held = 1;
                holder = mSkeletonTraversal->node(entry->mHand);
                break;
            }
        }
    }
    if (held) {
        MfDblList *hands = &skill->mHandList;
        MfManipHand *hand;
        for (hand = (MfManipHand *)hands->GetFirst(handPos); hand; hand = (MfManipHand *)hands->GetNext(handPos))
            if (mSkeletonTraversal->node(hand->mNode) == target->mSkeleton->parent())
                break;
        if (hand)
            return holder;
        return (MfSkeletonNode *)target->mSkeleton->parent();
    }
    MfRay ray;
    MfVector3 unused;                   // never used, but the original has it
    float radius, height;
    target->mPrototype->GetBoundingCylinder(radius, height);
    ray.mDirection = ((MfSkeletonNode *)mSkeleton->parent())->mGlobalTransform.rotation() * MfVector3(0.0f, -1.0f, 0.0f);
    ray.mOrigin = target->mSkeleton->mGlobalTransform.translation() - ray.mDirection * height;
    MfColResultRay result;
    if (!ray.Cast(mSpace, mSkeleton, result, 0))
        return mSpace->mRoot;
    return (MfSkeletonNode *)result.node->Owner();
}

// 0x10038d80 ?VerifyMoveObjSkill@MfLiveActor@@IAEHPAVMfManipSkill@@PAV1@0@Z
int MfLiveActor::VerifyMoveObjSkill(MfManipSkill *skill, MfLiveActor *target, MfManipSkill *other)
{
    MfDblListItem *pos;
    MfDblListItem *handPos;
    if (((MfSkeletonNode *)target->mSkeleton->parent())->mOwner == this) {
        MfDblList *children = &mChildList;
        for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
             entry = (MfChildEntry *)children->GetNext(pos)) {
            if ((MfLiveActor *)entry->mActor == target) {
                MfDblList *hands = &skill->mHandList;
                for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
                     hand = (MfManipHand *)hands->GetNext(handPos)) {
                    if (hand->mNode == entry->mHand)
                        return 1;
                }
            }
        }
    }
    if (other) {
        MfDblList *hands = &skill->mHandList;
        for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
             hand = (MfManipHand *)hands->GetNext(handPos)) {
            MfDblList *otherHands = &other->mHandList;
            for (MfManipHand *otherHand = (MfManipHand *)otherHands->GetFirst(pos); otherHand;
                 otherHand = (MfManipHand *)otherHands->GetNext(pos)) {
                if (hand->mNode == otherHand->mNode)
                    return 1;
            }
        }
    }
    return 0;
}

// 0x10038e90 ?VerifyGraspSkill@MfLiveActor@@IAEHPAVMfManipSkill@@PAV1@@Z
int MfLiveActor::VerifyGraspSkill(MfManipSkill *skill, MfLiveActor *target)
{
    MfDblListItem *pos;
    MfDblListItem *handPos;
    MfDblList *children = &mChildList;
    for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
         entry = (MfChildEntry *)children->GetNext(pos)) {
        if ((MfLiveActor *)entry->mActor == target) {
            MfDblList *hands = &skill->mHandList;
            for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
                 hand = (MfManipHand *)hands->GetNext(handPos)) {
                if (hand->mNode == entry->mHand)
                    return 0;
            }
        }
    }
    return 1;
}

// 0x10038f30 ?VerifyPlaceSkill@MfLiveActor@@IAEHPAVMfManipSkill@@PAV1@HAAPAVMfHandle@@@Z
int MfLiveActor::VerifyPlaceSkill(MfManipSkill *skill, MfLiveActor *target, int allHands,
                                  MfHandle *&handle)
{
    MfDblListItem *pos;
    MfDblListItem *handPos;
    if (((MfSkeletonNode *)target->mSkeleton->parent())->mOwner == this) {
        if (allHands) {
            int matched = 0;
            int numHands = skill->mHandList.GetCount();
            int held = 0;
            MfDblList *children = &mChildList;
            for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
                 entry = (MfChildEntry *)children->GetNext(pos)) {
                if ((MfLiveActor *)entry->mActor == target) {
                    held++;
                    MfDblList *hands = &skill->mHandList;
                    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
                         hand = (MfManipHand *)hands->GetNext(handPos)) {
                        if (hand->mNode == entry->mHand)
                            matched++;
                    }
                }
            }
            if (held == matched && held == numHands)
                return 1;
        } else {
            handle = 0;
            MfDblList *children = &mChildList;
            for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
                 entry = (MfChildEntry *)children->GetNext(pos)) {
                if ((MfLiveActor *)entry->mActor == target) {
                    MfDblList *hands = &skill->mHandList;
                    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(handPos); hand;
                         hand = (MfManipHand *)hands->GetNext(handPos)) {
                        if (hand->mNode == entry->mHand) {
                            if (handle) {
                                if (handle != entry->mHandle)
                                    return 0;
                            } else {
                                handle = entry->mHandle;
                            }
                            if (!handle->mFixXformCount)
                                return 0;
                        }
                    }
                }
            }
            if (handle)
                return 1;
        }
    }
    return 0;
}

// 0x100390c0 ?ManipFailure@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::ManipFailure(MfMsg &msg)
{
    MfPlannerFailureData *data = (MfPlannerFailureData *)msg.mData;
    mLastEventID = mManipPlan.mEventID;
    if (data->mPlanner == 2) {
        if (UpdateFSMWithEvent() && !mController)
            mHFSM->HandleEvent(msg);
        else if (mControlMode == 1 && mLive)
            UpdateAppEventHandler(msg);
    } else if (data->mPlanner == 5) {
        if (data->mRequestID != mLocoRequest)
            return;
        mLocoRequest = 0;
        if (data->mReason == 0)
            mLastLocoFailure = 1;
        else if (data->mReason == 1)
            mLastLocoFailure = 2;
        else if (data->mReason == 3)
            mLastLocoFailure = 3;
        else if (data->mReason == 4)
            mLastLocoFailure = 4;
        mLastManipFailure = 5;
        PostPlannerFailure(2);
        mSpace->ClearPlanningActorBlockage(this);
    } else if (data->mPlanner == 1) {
        if (mReachRequest != data->mRequestID)
            return;
        mReachRequest = 0;
        mLastManipFailure = 6;
        PostPlannerFailure(2);
    } else if (data->mPlanner == 3) {
        if (data->mRequestID != mGCRequest)
            return;
        mGCRequest = 0;
        mLastManipFailure = 6;
        PostPlannerFailure(2);
    } else if (data->mPlanner == 4) {
        if (data->mRequestID != mMoveRequest)
            return;
        mMoveRequest = 0;
        mLastManipFailure = 7;
        PostPlannerFailure(2);
    } else if (data->mPlanner == 0) {
        if (data->mRequestID != mLockRequest)
            return;
        mLockRequest = 0;
        mLastManipFailure = 8;
        PostPlannerFailure(2);
    }
    mCmdState = 0;
}

// 0x10039290 ?MakeManipActivity@MfLiveActor@@IAEPAVMfActivity@@PAV1@PAUMfGCPath@@PAUMfMTPath@@HPAVMfManipSkill@@PAVMfHandle@@PAVMfSkeletonNode@@KH@Z
MfActivity *MfLiveActor::MakeManipActivity(MfLiveActor *target, MfGCPath *gcPath, MfMTPath *mtPath,
                                           int duration, MfManipSkill *skill, MfHandle *handle,
                                           MfSkeletonNode *node, unsigned long time, int motionType)
{
    MfOverlayActivity *overlay = new MfOverlayActivity(this, time);
    mManipPlan.mNode = node;
    mManipPlan.mHandle = handle;
    MfManipActivity *manip = new MfManipActivity(this, time, skill, gcPath, mtPath, duration, target,
                                                 motionType, handle, node, mPlannedManipOptions);
    overlay->AddActivity(manip);
    MfDblList targets;
    MfDblList *list = &targets;
    list->Append(target);
    MfActivity *hold;
    if (motionType == 1)
        hold = MakeHoldTargetActivity(&targets, 1, skill);
    else
        hold = MakeHoldTargetActivity(&targets, 1, 0);
    if (hold)
        overlay->AddActivity(hold);
    if (motionType != 4 && motionType != 5 && motionType != 1 && motionType != 2) {
        MfActivity *face = MakeFaceActivity(target, handle);
        if (face)
            overlay->AddActivity(face);
    }
    overlay->setInitialConfig(mSkeletonTraversal, duration < 1000 ? duration / 2 : 500);
    overlay->SetUniqueID(overlay->GetUniqueID());
    return overlay;
}

// 0x10039460 ?RequestLock@MfLiveActor@@IAEXPAV1@H@Z
void MfLiveActor::RequestLock(MfLiveActor *target, int now)
{
    mLocoRequest = 0;
    mGCRequest = 0;
    mReachRequest = 0;
    mMoveRequest = 0;
    mLockRequest = ++mRequestID;
    if (now) {
        int locked = target->ApplyLock(this);
        if (locked)
            ChangeCmdState();
        else
            PostPlannerFailure(locked);
    } else {
        target->ApplyLock(0);
    }
}

// 0x100394e0 ?ApplyLock@MfLiveActor@@IAEHPAV1@@Z
int MfLiveActor::ApplyLock(MfLiveActor *locker)
{
    if (locker != this) {
        MfGuardHookData guard;
        guard.pActor = this;
        guard.pLocker = locker ? locker->mSkeleton : 0;
        if (gHookManager->CallHooks(MF_HOOK_GUARD_COP, &guard) <= 0) {
            if (!locker) {
                mOwner = 0;
                if (UpdateFSMWithEvent() && !mController) {
                    MfMsg unlocked(18, 0, 0, this, this);
                    mHFSM->HandleEvent(unlocked);
                    return 1;
                }
                if (mControlMode == 1 && mLive) {
                    MfMsg unlocked(18, 0, 0, this, this);
                    UpdateAppEventHandler(unlocked);
                    return 1;
                }
                return 1;
            }
            if (!mOwner || mOwner == locker) {
                MfLockHookData lock;
                lock.allow = 1;
                lock.pLocker = locker;
                lock.pActor = this;
                gHookManager->CallHooks('WlAR', &lock);
                if (lock.allow) {
                    mOwner = locker;
                    if (UpdateFSMWithEvent() && !mController) {
                        MfMsg locked(17, 0, 0, this, this);
                        mHFSM->HandleEvent(locked);
                    } else if (mControlMode == 1 && mLive) {
                        MfMsg locked(17, 0, 0, this, this);
                        UpdateAppEventHandler(locked);
                    }
                    if (mRootActivity && mRootActivity->HasMotion())
                        CleanupActivity(1);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// EQUIV 0x10039750 ?MakeFaceActivity@MfLiveActor@@IAEPAVMfActivity@@PAVMfActor@@PAVMfHandle@@@Z: reviewed: one independent reorder (the pushed name address before the property table load); string arguments and calls the same
// Turns the actor's face node (MF_faceNode, looking along MF_eyeVector) to
// the target's face node, the handle or the target.
MfActivity *MfLiveActor::MakeFaceActivity(MfActor *target, MfHandle *handle)
{
    MfString faceNodeName;
    MfString targetNodeName;
    MfVector3 eyeVector;
    MfProperty *prop = mPrototype->Properties().Find("MF_faceNode");
    if (prop) {
        faceNodeName = prop->operator MfString();
        prop = mPrototype->Properties().Find("MF_eyeVector");
        if (prop) {
            eyeVector = *(const MfVector3Value *)prop->Value();
            prop = target->Prototype()->Properties().Find("MF_faceNode");
            MfSkeletonNode *targetNode;
            if (prop) {
                targetNodeName = prop->operator MfString();
                targetNode = target->skeletonTraversal()->Node(targetNodeName);
                if (!targetNode) {
                    targetNode = handle;
                    if (!targetNode)
                        targetNode = target->mSkeleton;
                }
            } else {
                targetNode = handle;
                if (!targetNode)
                    targetNode = target->mSkeleton;
            }
            prop = mPrototype->Properties().Find("MF_neckLength");
            int neckLength = prop ? (int)*prop : 1;
            MfSkeletonNode *faceNode = skeletonTraversal()->Node(faceNodeName);
            if (faceNode) {
                MfOrientActivity *orient = new MfOrientActivity(this, g_world->mClock);
                orient->setup(faceNode, eyeVector, targetNode, neckLength);
                return orient;
            }
        }
    }
    return 0;
}

// EQUIV 0x10039a30 ?MakeHoldTargetActivity@MfLiveActor@@IAEPAVMfActivity@@PAVMfDblList@@HPAVMfManipSkill@@@Z: reviewed: only this and the child swap registers (ebx/esi), consistently
// An activity that keeps hold of the attached actors: a hold activity for
// each actor carried by a hand (except those in targets, none with noHold), a
// compliant activity for the arms whose actor is not attached any more (except
// the hands of skill). 0 when there is nothing to hold.
MfActivity *MfLiveActor::MakeHoldTargetActivity(MfDblList *targets, int noHold, MfManipSkill *skill)
{
    MfOverlayActivity *overlay = new MfOverlayActivity(this, g_world->mClock);
    MfLiveActor *child;
    MfDblListItem *pos;
    MfDblList *children = &mChildList;
    for (MfChildEntry *entry = (MfChildEntry *)children->GetFirst(pos); entry;
         entry = (MfChildEntry *)children->GetNext(pos)) {
        child = (MfLiveActor *)entry->mActor;
        if (child->mSkeleton->parent() == entry->mNode && targets && targets->Find(child))
            continue;
        if (child->mSkeleton->parent() == entry->mNode) {
            if (noHold || entry->mHand < 0)
                continue;
            MfHoldActivity *hold = new MfHoldActivity(this, child, entry->mHand, g_world->mClock);
            overlay->AddActivity(hold);
            continue;
        }
        if (skill) {
            MfDblListItem *handPos;
            MfManipHand *hand;
            for (hand = (MfManipHand *)(&skill->mHandList)->GetFirst(handPos); hand;
                 hand = (MfManipHand *)(&skill->mHandList)->GetNext(handPos))
                if (entry->mNode == mSkeletonTraversal->node(hand->mNode))
                    break;
            if (hand)
                continue;
        }
        MfCompliantActivity *compliant = new MfCompliantActivity(this, g_world->mClock);
        int depth = 1;
        MfTreeNode *node;
        for (node = mSkeletonTraversal->node(entry->mHand)->parent();
             node->numChildren(0) == 1 && node != mSkeletonTraversal->node(1); node = node->parent())
            depth++;
        MfTransform3 target = ~child->mSkeletonTraversal->node(1)->homeTransform() * entry->mOffset;
        compliant->SetupTarget(child->skeletonTraversal()->node(1), target);
        compliant->SetupChain(mSkeletonTraversal->node(entry->mHand), depth);
        overlay->AddActivity(compliant);
    }
    if (!overlay->mSubactivities.GetCount()) {
        delete overlay;
        return 0;
    }
    overlay->SetUniqueID(overlay->GetUniqueID());
    return overlay;
}

// 0x10039da0 ?node@MfSkeletonTraversal@@QAEPAVMfSkeletonNode@@H@Z (inline, emitted for MakeOverlayActivity)

// 0x10039db0 ?HandleProximityEvent@MfLiveActor@@IAEXAAVMfMsg@@@Z
void MfLiveActor::HandleProximityEvent(MfMsg &msg)
{
    MfProximityEventData *data = (MfProximityEventData *)msg.mData;
    gEventData = data;
    if (!mSpace)
        return;
    MfLiveActor *other = (MfLiveActor *)data->mOther;
    mLastProximity.plaOther = other;
    if (!mSpace->GetLiveActors().Contains(other))
        return;
    switch (data->mEvent) {
    case 0:
        RespondToBlockedPath(other);
        break;
    case 1:
        RespondToClearPath(other);
        break;
    case 2:
        RespondToApproach(other);
        break;
    case 3:
        RespondToRetreat(other);
        break;
    }
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// 0x10039e90 ?RespondToBlockedPath@MfLiveActor@@IAEXPAV1@@Z
// Another actor blocks the way: the one with the lower replan priority waits
// for the other (mBlockList), the other one plans a new path.
void MfLiveActor::RespondToBlockedPath(MfLiveActor *other)
{
    MfProperty property;
    int priority = 0;
    if (GetProperty("MF_replanPriority", property))
        priority = property;
    int otherPriority = 0;
    if (other->GetProperty("MF_replanPriority", property))
        otherPriority = property;
    int walking;
    int moving;
    if (IsCurrentActivity(MfLocoActivity::TYPE) && !mLocoPlan.mb8) {
        walking = 1;
        moving = 1;
    } else {
        walking = 0;
        moving = mCmdState == 1 || mCmdState == 4 ? 1 : 0;
    }
    if (priority < 0)
        return;
    if (!moving && !mBlockList.GetCount())
        return;
    if (mCmdState == 5 && other == mManipPlan.mTarget)
        return;
    int replan = 0;
    if (walking) {
        if (mLocoPlan.mPath) {
            MfPathPoint *point = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
            while (point) {
                delete point;
                point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
            }
            delete mLocoPlan.mPath;
            mLocoPlan.mPath = 0;
        }
        mCmdState = 0;
        ClearActivity();
        if (mLocoPlan.mSkill)
            SetActivity(MakeGestureActivity(g_world->mClock, mLocoPlan.mSkill, 0));
        replan = 1;
    }
    int wait = 1;
    int otherMoving;
    if (other->IsCurrentActivity(MfLocoActivity::TYPE) && !other->mLocoPlan.mb8)
        otherMoving = 1;
    else
        otherMoving = other->mCmdState == 1 || other->mCmdState == 4 ? 1 : 0;
    if (otherPriority < 0 || !otherMoving && !other->mBlockList.GetCount())
        wait = 0;
    if (!mControlMode)
        wait = 0;
    if (wait) {
        if (!other->mControlMode) {
            MfDblList *blocks = &mBlockList;
            blocks->Add(other);
            replan = 0;
        } else if (priority == otherPriority) {
            if (this < other) {
                MfDblList *blocks = &mBlockList;
                blocks->Add(other);
                replan = 0;
            }
        } else if (priority < otherPriority) {
            MfDblList *blocks = &mBlockList;
            blocks->Add(other);
            replan = 0;
        }
    }
    if (replan && !mBlockList.GetCount() && !mLocoPlan.mb8)
        RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                        mLocoPlan.mFloor, mLocoPlan.mbc, mLocoPlan.m00, mLocoPlan.m20,
                        mLocoPlan.mTarget, mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c,
                        mLocoPlan.m30, mLocoPlan.m34, mLocoPlan.mEventID, 1);
}

// 0x1003a1d0 ?RespondToClearPath@MfLiveActor@@IAEXPAV1@@Z
void MfLiveActor::RespondToClearPath(MfLiveActor *other)
{
    if (mBlockList.GetCount() <= 0)
        return;
    MfDblList *blocks = &mBlockList;
    MfDblListItem *item = blocks->Find(other);
    if (!item)
        return;
    blocks->Delete(item);
    if (mBlockList.GetCount() == 0 && !mLocoPlan.mb8)
        RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                        mLocoPlan.mFloor, mLocoPlan.mbc, mLocoPlan.m00, mLocoPlan.m20,
                        mLocoPlan.mTarget, mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c,
                        mLocoPlan.m30, mLocoPlan.m34, mLocoPlan.mEventID, 1);
}

// 0x1003a2a0 ?NotifyActivityFinished@@YAXPAVMfLiveActor@@H@Z
// Tells the hooks that an activity of the actor finished (HandleActivityFinished).
void NotifyActivityFinished(MfLiveActor *actor, int activityID)
{
    MfActivityHookData data;
    data.pActor = actor;
    data.pActivity = 0;
    data.activityID = activityID;
    data.m1c = 0;
    data.op = 3;
    gHookManager->CallHooks(MF_HOOK_ACTIVITY_DATA, &data);
}

// the data of an activity-finished message (11): the activity
struct MfActivityDoneData : public MfMsgDataBlock {
    MfActivity *mActivity;                                  // 0x08
};

// TODO 0x1003a2f0 ?HandleActivityFinished@MfLiveActor@@IAEXAAVMfMsg@@@Z: (87% similar) control-flow layout: in the MfManipActivity/cmd-state branch the original duplicates RemoveActivity(0, 0) + ChangeCmdState for a null activity where this merges p ? p + 0x14 : 0 (as both do in the loco branch); also a 4-byte larger frame, other slots and independently ordered EH state stores
// One of the actor's activities finished: removes it, continues a walk with
// the path that was planned meanwhile (or asks for a new one when the walk
// ended away from the goal), advances the command state and tells the
// behaviour.
void MfLiveActor::HandleActivityFinished(MfMsg &msg)
{
    MfActivityDoneData *data = (MfActivityDoneData *)msg.mData;
    if (!MfActivity::ActivityExists(data->mActivity))
        return;
    if (mRootActivity == data->mActivity)
        return;
    if (!data->mActivity->mDeleteOnFinished)
        return;
    data->mActivity->mInTransitToFinish = 0;
    m2c4 = data->mActivity->GetUniqueID();
    mLastEventID = data->mActivity->GetUniqueID();
    if (data->mActivity->IsType(MfManagerActivity::TYPE)) {
        int keep = 0;
        if (data->mActivity->IsType(MfSequentialActivity::TYPE) || data->mActivity->IsType(MfRandomActivity::TYPE))
            keep = 1;
        if (keep)
            RemoveActivity(data->mActivity, 1);
        else if (data->mActivity)
            delete data->mActivity;
        if (keep) {
            if (UpdateFSMWithEvent() && !mController) {
                MfMsg done(11, this, this, mID, mID);
                done.m38 = 0;
                mHFSM->HandleEvent(done);
                NotifyActivityFinished(this, m2c4);
            } else if (mControlMode == 1 && mLive) {
                MfMsg done(11, this, this, mID, mID);
                done.m38 = 0;
                UpdateAppEventHandler(done);
            }
        }
        Lock();
        mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                           !IsCurrentActivity(Mf3dPathActivity::TYPE) ? 1 : 0;
        Unlock();
        return;
    }
    if (data->mActivity->Type() == MfLocoActivity::TYPE && mLocoPlan.mb8) {
        RemoveActivity(data->mActivity, 0);
        if (UpdateFSMWithEvent() && !mController) {
            MfMsg done(11, 0, 0, this, this);
            done.m38 = 1;
            mHFSM->HandleEvent(done);
            NotifyActivityFinished(this, m2c4);
        } else if (mControlMode == 1 && mLive) {
            MfMsg done(11, 0, 0, this, this);
            done.m38 = 1;
            UpdateAppEventHandler(done);
        }
        return;
    }
    if (data->mActivity->Type() == MfLocoActivity::TYPE && !mLocoPlan.mb8) {
        MfVector3 offset;
        MfVector3 goal;
        goal.x() = mLocoPlan.mGoal.translation().y();
        goal.z() = mLocoPlan.mGoal.translation().x();
        offset = mSkeleton->mTransform.translation();
        offset.y() = 0.0f;
        offset = goal - offset;
        float dist = offset.length();
        int request = 1;
        int replan = 0;
        if (mLocoPlan.mPath && mLocoPlan.m20 && dist > mLocoPlan.m2c)
            replan = 1;
        else if (mSkeleton->parent() != mLocoPlan.mFloor)
            replan = 1;
        else if (!mLocoPlan.m20 && dist > 0.01f)
            replan = 1;
        if (replan) {
            RemoveActivity(data->mActivity, 0);
            if ((MfActivity *)mLocoPlan.ma4 == data->mActivity && mLocoPlan.mac) {
                // walk on along the path that was planned while this walk ran
                MfSkeletonNode *parent = (MfSkeletonNode *)mSkeleton->parent();
                ChangeParent((MfSkeletonNode *)mLocoPlan.ma8, 0, 0, 0);
                MfActivity *activity;
                if (((MfSkeletonNode *)mLocoPlan.ma8)->mProto->FloorInfo()->m00 == 1) {
                    MfSkeletonNode *node;
                    for (node = (MfSkeletonNode *)mSkeleton->parent();
                         node && node->GetNodeType() != MF_NODE_GEOMETRY; node = (MfSkeletonNode *)node->parent())
                        ;
                    activity = MakeLocoActivity((MfPath2 *)mLocoPlan.mac, mLocoPlan.mLocoSkill, g_world->mClock,
                                                mPlannedLocoOptions | 0x400, mPlannedLocoParam, mPlannedLocoReach, node, 0);
                } else if (parent->GetNodeType() == MF_NODE_FLOOR && parent->mProto->FloorInfo()->m00 == 1) {
                    float radius;
                    mPrototype->GetBoundingCylinder(radius, dist);
                    activity = MakeLocoActivity((MfPath2 *)mLocoPlan.mac, mLocoPlan.mLocoSkill, g_world->mClock,
                                                mPlannedLocoOptions | 0x200, mPlannedLocoParam, dist * 3.0f, 0, 0);
                } else
                    activity = MakeLocoActivity((MfPath2 *)mLocoPlan.mac, mLocoPlan.mLocoSkill, g_world->mClock,
                                                mPlannedLocoOptions, mPlannedLocoParam, mPlannedLocoReach, 0, 0);
                MfVector3 oldUp = parent->mGlobalTransform.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
                MfVector3 newUp = ((MfSkeletonNode *)mLocoPlan.ma8)->mGlobalTransform.rotation() *
                                  MfVector3(0.0f, 1.0f, 0.0f);
                if (oldUp.dot(newUp) > 0.98f)
                    activity->SetBlendTime(0);
                gEventData = &mLocoPlan;
                activity->SetUniqueID(mLocoPlan.mEventID);
                if (mLocoPlan.m88) {
                    mLastActivity = activity;
                    mLastEventID = mLocoPlan.mEventID;
                    if (UpdateFSMWithEvent() && !mController) {
                        MfMsg done(20, 0, 0, this, this);
                        mHFSM->HandleEvent(done);
                    } else if (mControlMode == 1 && mLive) {
                        MfMsg done(20, 0, 0, this, this);
                        UpdateAppEventHandler(done);
                    }
                } else
                    SetActivity(activity);
                mLocoPlan.mb0 = 0;
                MfTransform2 last = mLocoPlan.mLast;
                if ((last.translation() - mLocoPlan.mGoal.translation()).length() > 0.01f) {
                    mLocoPlan.mb0 = 1;
                    mLocoPlan.m8c = last;
                } else
                    request = 0;
                mLocoPlan.ma4 = 0;
                mLocoPlan.mac = 0;
            } else {
                MfPath2 *path = (MfPath2 *)mLocoPlan.mac;
                if (path)
                    delete path;
                MfTransform2 last = mLocoPlan.mLast;
                if ((last.translation() - mLocoPlan.mGoal.translation()).length() < 0.01f)
                    request = 0;
                else {
                    mLocoPlan.mb0 = 0;
                    mLocoPlan.ma4 = 0;
                    mLocoPlan.mac = 0;
                }
            }
            if (request)
                RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                                mLocoPlan.mFloor, mLocoPlan.mbc, mLocoPlan.m00, mLocoPlan.m20,
                                mLocoPlan.mTarget, mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c,
                                mLocoPlan.m30, mLocoPlan.m34, mLocoPlan.mEventID, 0);
            return;
        }
    }
    if (mSpace) {
        if (data->mActivity->Type() == MfLocoActivity::TYPE) {
            mSpace->UpdateMovingActor(this);
            mSpace->ClearPlanningActorBlockage(this);
        }
        if (mSpace && data->mActivity->Type() == MfManipActivity::TYPE) {
            MfLiveActor *target = (MfLiveActor *)((MfManipActivity *)data->mActivity)->mTarget;
            if (target)
                mSpace->UpdateMovingActor(target);
            mSpace->UpdateMovingActor(this);
        }
    }
    if (data->mActivity->Type() == MfLocoActivity::TYPE) {
        RemoveActivity(data->mActivity, 0);
        mLastTimeForLoco = g_world->mClock;
        ChangeCmdState();
        return;
    }
    if (data->mActivity->Type() == MfManipActivity::TYPE && (mCmdState == 9 || mCmdState == 12 || mCmdState == 11)) {
        RemoveActivity(data->mActivity, 0);
        ChangeCmdState();
        return;
    }
    if (data->mActivity->Type() == MfGestureActivity::TYPE && mCmdState == 7) {
        if (data->mActivity)
            delete data->mActivity;
        ChangeCmdState();
        return;
    }
    if (data->mActivity->Type() == Mf3dPathActivity::TYPE)
        mLastTimeForLoco = g_world->mClock;
    RemoveActivity(data->mActivity, 1);
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg done(11, this, this, mID, mID);
        done.m38 = 0;
        mHFSM->HandleEvent(done);
        NotifyActivityFinished(this, m2c4);
    } else if (mControlMode == 1 && mLive) {
        MfMsg done(11, this, this, mID, mID);
        done.m38 = 0;
        UpdateAppEventHandler(done);
    }
}

// 0x1003ae30 ?HandleStaticActorChange@MfLiveActor@@QAEXAAVMfMsg@@@Z
void MfLiveActor::HandleStaticActorChange(MfMsg &msg)
{
    gEventData = msg.mObject;
    if (!mSpace || !IsCurrentActivity(MfLocoActivity::TYPE))
        return;
    if (mCmdState != 2 && mCmdState != 5)
        return;
    if (mLocoPlan.mPath) {
        MfPathPoint *point = (MfPathPoint *)mLocoPlan.mPath->GetFirst(mLocoPlan.mPathPos);
        while (point) {
            delete point;
            point = (MfPathPoint *)mLocoPlan.mPath->GetNext(mLocoPlan.mPathPos);
        }
        delete mLocoPlan.mPath;
        mLocoPlan.mPath = 0;
    }
    CleanupActivity(1);
    if (!mLocoPlan.mb8)
        RequestLocoPlan(mLocoPlan.m88, mLocoPlan.mLocoSkill, mLocoPlan.mSkill, mLocoPlan.mGoal,
                        mLocoPlan.mFloor, mLocoPlan.mbc, mLocoPlan.m00, mLocoPlan.m20,
                        mLocoPlan.mTarget, mLocoPlan.m28, mLocoPlan.m2c, mLocoPlan.m1c,
                        mLocoPlan.m30, mLocoPlan.m34, mLocoPlan.mEventID, 1);
    if (UpdateFSMWithEvent() && !mController) {
        mHFSM->HandleEvent(msg);
        return;
    }
    if (mControlMode == 1 && mLive)
        UpdateAppEventHandler(msg);
}

// 0x1003b0f0 ?SetPick@MfMouse@@QAEXPAVMfAiActor@@VMfVector3@@@Z

// 0x1003af90 ?HandleRButtonDown@MfLiveActor@@UAEXPAUMfPickData@@@Z
void MfLiveActor::HandleRButtonDown(MfPickData *pick)
{
    mLastPick = *pick;
    gEventData = &mLastPick;
    if (gMouse)
        gMouse->SetPick(this, mLastPick.pickPoint);
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 2;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 2;
        UpdateAppEventHandler(msg);
    }
    if (gMouse)
        gMouse->Notify(2);
}

// 0x1003b110 ?HandleRButtonUp@MfLiveActor@@UAEXPAUMfPickData@@@Z
void MfLiveActor::HandleRButtonUp(MfPickData *)
{
    gEventData = &mLastPick;
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 3;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 3;
        UpdateAppEventHandler(msg);
    }
    if (gMouse)
        gMouse->Notify(3);
}

// 0x1003b210 ?HandleLButtonDown@MfLiveActor@@UAEXPAUMfPickData@@@Z
void MfLiveActor::HandleLButtonDown(MfPickData *pick)
{
    mLastPick = *pick;
    gEventData = &mLastPick;
    if (gMouse)
        gMouse->SetPick(this, mLastPick.pickPoint);
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 0;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 0;
        UpdateAppEventHandler(msg);
    }
    if (gMouse)
        gMouse->Notify(0);
}

// 0x1003b370 ?HandleLButtonUp@MfLiveActor@@UAEXPAUMfPickData@@@Z
void MfLiveActor::HandleLButtonUp(MfPickData *pick)
{
    mLastPick = *pick;
    gEventData = &mLastPick;
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 1;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 1;
        UpdateAppEventHandler(msg);
    }
    if (gMouse)
        gMouse->Notify(1);
}

// 0x1003b490 ?HandleMouseMove@MfLiveActor@@UAEXPAUMfPickData@@@Z
void MfLiveActor::HandleMouseMove(MfPickData *)
{
    gEventData = &mLastPick;
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 4;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        MfMsg msg(39, 0, 0, this, this);
        msg.m38 = 4;
        UpdateAppEventHandler(msg);
    }
    if (gMouse)
        gMouse->Notify(4);
}

// 0x1003b590 ?HandleKeyboardEvent@MfLiveActor@@UAEXII@Z
void MfLiveActor::HandleKeyboardEvent(unsigned int key, unsigned int flags)
{
    if (UpdateFSMWithEvent() && !mController) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(40, this, this, mID, mID);
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(40, this, this, mID, mID);
        UpdateAppEventHandler(msg);
    }
}

// 0x1003b680 ?HandleKeyboardDownEvent@MfLiveActor@@UAEXII@Z
void MfLiveActor::HandleKeyboardDownEvent(unsigned int key, unsigned int flags)
{
    if (UpdateFSMWithEvent() && !mController) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(42, this, this, mID, mID);
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(42, this, this, mID, mID);
        UpdateAppEventHandler(msg);
    }
}

// 0x1003b770 ?HandleKeyboardUpEvent@MfLiveActor@@UAEXII@Z
void MfLiveActor::HandleKeyboardUpEvent(unsigned int key, unsigned int flags)
{
    if (UpdateFSMWithEvent() && !mController) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(43, this, this, mID, mID);
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(43, this, this, mID, mID);
        UpdateAppEventHandler(msg);
    }
}

// 0x1003b860 ?HandleKeyboardRepeatEvent@MfLiveActor@@UAEXII@Z
void MfLiveActor::HandleKeyboardRepeatEvent(unsigned int key, unsigned int flags)
{
    if (UpdateFSMWithEvent() && !mController) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(44, this, this, mID, mID);
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastKey = key;
        mLastKeyFlags = flags;
        MfMsg msg(44, this, this, mID, mID);
        UpdateAppEventHandler(msg);
    }
}

// 0x1003b950 ?HandleOption@MfLiveActor@@QAEXH@Z
void MfLiveActor::HandleOption(int option)
{
    mLastOptionID = option;
    if (UpdateFSMWithEvent() && !mController) {
        MfMsg msg(41, 0, 0, this, this);
        mHFSM->HandleEvent(msg);
    }
}

// removes the timers of an actor from the timed queue (_PmClearTimers)
class MfClearTimerCB : public MfCallBack {
public:
    MfClearTimerCB(MfLiveActor *actor) : mActor(actor) {}
    virtual int CallBack(void *entry);

    MfLiveActor *mActor;                                    // 0x04
};

// 0x1003b9e0 ?CallBack@MfClearTimerCB@@UAEHPAX@Z
int MfClearTimerCB::CallBack(void *item)
{
    MfTimerEntry *entry = (MfTimerEntry *)item;
    g_msgDispatcher->mTimedQueue->Remove(entry->mHandle);
    delete entry;
    return 1;
}

// 0x1003ba10 ?_PmClearTimers@MfLiveActor@@IAEXXZ
void MfLiveActor::_PmClearTimers()
{
    MfClearTimerCB clear(this);
    mTimers.Clear(&clear);
}

// 0x1003ba60 ?UpdateFSMWithEvent@MfLiveActor@@IAEHXZ
int MfLiveActor::UpdateFSMWithEvent()
{
    return mLive == 1 && mHFSM && mHFSM->mActive == 1 && mControlMode == 1 ? 1 : 0;
}

// 0x1003ba90 ?UpdateFSMThatSitManipFinished@MfLiveActor@@IAEXXZ
void MfLiveActor::UpdateFSMThatSitManipFinished()
{
    if (UpdateFSMWithEvent() && !mController) {
        mLastEventID = m2c4;
        MfMsg msg(11, 0, 0, this, this);
        msg.m38 = 2;
        mHFSM->HandleEvent(msg);
    } else if (mControlMode == 1 && mLive) {
        mLastEventID = m2c4;
        MfMsg msg(11, 0, 0, this, this);
        msg.m38 = 2;
        UpdateAppEventHandler(msg);
    }
}

// EQUIV 0x1003bb80 ?CorrectPlacement@MfLiveActor@@UAEXAAVMfTransform3@@ABVMfVector3@@@Z: reviewed: local = ~local: the original copies the rotation with the imported MfRotation3::operator= and the translation inline, this the rotation inline and the translation with the imported MfVector3::operator=; same copies, same order of calls otherwise
// Moves the placement up or down along dir until the actor's bounding box
// rests on it.
void MfLiveActor::CorrectPlacement(MfTransform3 &xform, const MfVector3 &dir)
{
    MfVector3 boxMin;
    MfVector3 boxMax;
    mPrototype->GetBoundingBox(boxMin, boxMax);
    MfVector3 corners[8];
    corners[0].x() = boxMin.x(); corners[0].y() = boxMin.y(); corners[0].z() = boxMin.z();
    corners[1].x() = boxMin.x(); corners[1].y() = boxMax.y(); corners[1].z() = boxMin.z();
    corners[2].x() = boxMin.x(); corners[2].y() = boxMin.y(); corners[2].z() = boxMax.z();
    corners[3].x() = boxMax.x(); corners[3].y() = boxMin.y(); corners[3].z() = boxMin.z();
    corners[4].x() = boxMax.x(); corners[4].y() = boxMax.y(); corners[4].z() = boxMin.z();
    corners[5].x() = boxMax.x(); corners[5].y() = boxMin.y(); corners[5].z() = boxMax.z();
    corners[6].x() = boxMin.x(); corners[6].y() = boxMax.y(); corners[6].z() = boxMax.z();
    corners[7].x() = boxMax.x(); corners[7].y() = boxMax.y(); corners[7].z() = boxMax.z();
    int i;
    for (i = 0; i < 8; i++)
        corners[i] = xform * corners[i];
    MfTransform3 local;
    local.rotation().setValue(MfVector3(0.0f, 1.0f, 0.0f), dir);
    local.translation() = xform.translation();
    local = ~local;
    float minY = 3.402823466e+38F;
    for (i = 0; i < 8; i++) {
        corners[i] = local * corners[i];
        if (corners[i].y() < minY)
            minY = corners[i].y();
    }
    corners[0] = xform.translation();
    corners[0] = local * corners[0];
    corners[0].y() -= minY;
    xform.translation() = ~local * corners[0];
}

// 0x1003bed0 ?AddActivity@MfLiveActor@@UAEHPBVMfAiActivity@@@Z
int MfLiveActor::AddActivity(const MfAiActivity *aiActivity)
{
    MfActivity *activity = (MfActivity *)aiActivity;
    if (!activity->IsType(MfActivity::TYPE))
        return 0;
    if (!MfActivity::ActivityExists(activity))
        return 0;
    if (mRootActivity) {
        if (aiActivity && mRootActivity->Contains(activity))
            return 0;
        if (MfWorld::enableNetwork) {
            MfActivityHookData data;
            data.pActor = this;
            data.pActivity = activity;
            data.op = 1;
            gHookManager->CallHooks(MF_HOOK_ACTIVITY_DATA, &data);
        }
        if (aiActivity)
            ((MfActivity *)aiActivity)->PrepareForExecution();
        mRootActivity->AddActivity((MfActivity *)aiActivity);
        mRootActivity->Update(((MfActivity *)aiActivity)->mStartTime);
        Lock();
        mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                           !IsCurrentActivity(Mf3dPathActivity::TYPE) ? 1 : 0;
        Unlock();
        return 1;
    }
    if (aiActivity)
        ((MfActivity *)aiActivity)->PrepareForExecution();
    return SetActivity((MfActivity *)aiActivity);
}

// 0x1003c080 ?SendMessageA@MfLiveActor@@UAEHPBVMfAiActor@@ABVMfString@@@Z
int MfLiveActor::SendMessage(const MfAiActor *target, const MfString &message)
{
    MfLiveActor *actor = (MfLiveActor *)target;
    if (actor) {
        int found = (&g_world->mActors)->Find(actor) != 0;
        if (!found)
            return -1;
    }
    if (MfWorld::enableNetwork) {
        MfMiscHookData data;
        data.pActor = this;
        data.pTarget = target;
        data.str = message;
        data.kind = 8;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
    return sendMsg(target, message);
}

// 0x1003c1d0 ?sendMsg@MfLiveActor@@QAEHPBVMfAiActor@@ABVMfString@@@Z
int MfLiveActor::sendMsg(const MfAiActor *target, const MfString &message)
{
    int toSelf = 0;
    MfLiveActor *actor = (MfLiveActor *)target;
    if (!actor)
        toSelf = 1;
    if (actor) {
        MfWorld *world = g_world;
        int found = (&world->mActors)->Find(actor) != 0;
        if (!found)
            return -1;
    }
    int id = MfWorld::CreateUniqueNumber();
    MfMsg *msg = new MfMsg(58, 0, 0, this, actor);
    msg->SetData(new MfStrMessageData(id, message));
    msg->mToSelf = toSelf;
    g_msgDispatcher->Post(msg);
    mHFSM->RecordSentMsg(msg, 0);
    return id;
}

// 0x1003c320 ??_GMfStrMessageData@@UAEPAXI@Z
// 0x1003c340 ??1MfStrMessageData@@UAE@XZ

// 0x1003c390 ?MakeEventMsg@@YAPAVMfMsg@@ABVMfString@@PAVMfRootObj@@1@Z
// an event message by its name (MfMsgTypeTable), with a new event ID
MfMsg *MakeEventMsg(const MfString &name, MfRootObj *from, MfRootObj *to)
{
    int id = MfWorld::CreateUniqueNumber();
    MfMsgTypeInfo info;
    info = g_msgTypes->Lookup(name);
    MfMsg *msg = new MfMsg(info.type, 0, 0, from, to);
    msg->mEventID = id;
    msg->m38 = info.sub;
    return msg;
}

// 0x1003c420 ?MakeTimerMsg@@YAPAVMfMsg@@ABVMfString@@HPAVMfRootObj@@@Z
// the message of a timer: the event named, to the actor itself
MfMsg *MakeTimerMsg(const MfString &name, int id, MfRootObj *actor)
{
    MfMsgTypeInfo info;
    info = g_msgTypes->Lookup(name);
    MfMsg *msg = new MfMsg(info.type, actor, actor);
    msg->m38 = info.sub;
    msg->mEventID = id;
    return msg;
}

// 0x1003c4a0 ?SendEvent@MfLiveActor@@UAEHPBVMfAiActor@@ABVMfString@@K@Z
int MfLiveActor::SendEvent(const MfAiActor *target, const MfString &event, unsigned long delay)
{
    if (delay < 10)
        delay = 0;
    int result = sendEvent(target, event, delay);
    MfLiveActor *actor = (MfLiveActor *)target;
    if (actor) {
        int found = (&g_world->mActors)->Find(actor) != 0;
        if (!found)
            return -1;
    }
    if (MfWorld::enableNetwork) {
        MfMiscHookData data;
        data.pActor = this;
        data.pTarget = target;
        data.str = event;
        data.delay = delay;
        data.kind = 7;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
    return result;
}

// 0x1003c600 ?SendEvent@MfLiveActor@@UAEHPBVMfAiActor@@PAVMfScDataHandle@@K@Z
int MfLiveActor::SendEvent(const MfAiActor *target, MfScDataHandle *data, unsigned long delay)
{
    if (delay < 10)
        delay = 0;
    MfPoEvent *event = data ? dynamic_cast<MfPoEvent *>(data->GetData()) : 0;
    if (!event)
        return -2;
    MfLiveActor *actor = (MfLiveActor *)target;
    if (actor) {
        MfWorld *world = g_world;
        int found = (&world->mActors)->Find(actor) != 0;
        if (!found)
            return -1;
    }
    MfMsg *msg = MakeEventMsg(event->Type(), this, actor);
    msg->SetData(new MfPiccoloHandleData(data));
    return sendEvent(actor, msg, delay);
}

// 0x1003c710 ?sendEvent@MfLiveActor@@QAEHPBV1@PAVMfMsg@@K@Z
int MfLiveActor::sendEvent(const MfLiveActor *target, MfMsg *msg, unsigned long delay)
{
    if (delay < 10)
        delay = 0;
    int toSelf = 0;
    if (!target)
        toSelf = 1;
    msg->mToSelf = toSelf;
    if (delay < 10)
        g_msgDispatcher->Post(msg);
    else
        g_msgDispatcher->mTimedQueue->Post(msg, delay, 1);
    mHFSM->RecordSentMsg(msg, delay);
    return msg->mEventID;
}

// 0x1003c770 ?sendEvent@MfLiveActor@@QAEHPBVMfAiActor@@ABVMfString@@K@Z
int MfLiveActor::sendEvent(const MfAiActor *target, const MfString &event, unsigned long delay)
{
    MfLiveActor *actor = (MfLiveActor *)target;
    if (actor) {
        MfWorld *world = g_world;
        int found = (&world->mActors)->Find(actor) != 0;
        if (!found)
            return -1;
    }
    MfMsg *msg = MakeEventMsg(event, this, actor);
    MfPmhUser *handler = new MfPmhUser(this);
    handler->SetSender(this);
    MfScDataHandle *data = MfScDataHandle::CreateData(new MfPoEvent(event, handler));
    msg->SetData(new MfPiccoloHandleData(data));
    data->Release();
    return sendEvent(actor, msg, delay);
}

// 0x1003c8e0 ??_GMfPmhUser@@UAEPAXI@Z (the local copy for `new MfPmhUser`)

// 0x1003c900 ??_GMfPoEvent@@UAEPAXI@Z (the local copy for `new MfPoEvent`)

// 0x1003c920 ?GetLastSender@MfLiveActor@@UAEPAVMfAiActor@@XZ
MfAiActor *MfLiveActor::GetLastSender()
{
    if (mHFSM)
        return mSpace->GetActor(mHFSM->LastSender());
    return 0;
}

// 0x1003c9a0 ?LastSender@MfFSMManager@@QAE?AVMfString@@XZ

// 0x1003c9d0 ?GetLastCollisionInfo@MfLiveActor@@UAEPBUMfCollisionData@@XZ
const MfCollisionData *MfLiveActor::GetLastCollisionInfo()
{
    int valid = (&g_world->mActors)->Find((MfLiveActor *)mLastCollision.plaOther) != 0;
    if (!valid) {
        mLastCollision.plaOther = 0;
        mLastCollision.psnOtherNode = 0;
    }
    return &mLastCollision;
}

// 0x1003ca20 ?GetLastProximityInfo@MfLiveActor@@UAEPBUMfProximityData@@XZ
const MfProximityData *MfLiveActor::GetLastProximityInfo()
{
    int valid = (&g_world->mActors)->Find((MfLiveActor *)mLastProximity.plaOther) != 0;
    if (!valid)
        mLastProximity.plaOther = 0;
    return &mLastProximity;
}

// 0x1003ca60 ?PostTimerMessage@MfLiveActor@@IAEXHHHPAVMfMsg@@@Z
void MfLiveActor::PostTimerMessage(int id, int time, int repeat, MfMsg *msg)
{
    MfTimerEntry *entry = new MfTimerEntry;
    if (repeat) {
        entry->mHandle = g_msgDispatcher->mTimedQueue->Post(msg, time, 0);
        mTimers.Add(id, entry);
    } else {
        entry->mHandle = g_msgDispatcher->mTimedQueue->Post(msg, time, 1);
        msg->mIsTimer = 1;
        msg->mTimerID = id;
        mTimers.Add(id, entry);
    }
}

// 0x1003cb00 ?RemoveTimerMessage@MfLiveActor@@IAEXH@Z
void MfLiveActor::RemoveTimerMessage(int id)
{
    delete (MfTimerEntry *)mTimers.Delete(id);
}

// 0x1003cb20 ?StartTimer@MfLiveActor@@UAEHHPAVMfScDataHandle@@HH@Z
int MfLiveActor::StartTimer(int id, MfScDataHandle *data, int time, int repeat)
{
    MfPoEvent *event = data ? dynamic_cast<MfPoEvent *>(data->GetData()) : 0;
    if (!event)
        return -2;
    MfMsg *msg = MakeTimerMsg(event->Type(), id, this);
    msg->SetData(new MfPiccoloHandleData(data));
    PostTimerMessage(id, time, repeat, msg);
    return id;
}

// 0x1003cbf0 ?StartTimer@MfLiveActor@@UAEHHABVMfString@@HH@Z
int MfLiveActor::StartTimer(int id, const MfString &name, int time, int repeat)
{
    MfMsg *msg = MakeTimerMsg(name, id, this);
    MfScDataHandle *data = MfScDataHandle::CreateData(new MfPoEvent(name));
    msg->SetData(new MfPiccoloHandleData(data));
    data->Release();
    PostTimerMessage(id, time, repeat, msg);
    return id;
}

// 0x1003cce0 ?StopTimer@MfLiveActor@@UAEHH@Z
int MfLiveActor::StopTimer(int id)
{
    MfTimerEntry *entry = (MfTimerEntry *)mTimers.Delete(id);
    if (entry)
        g_msgDispatcher->mTimedQueue->Remove(entry->mHandle);
    delete entry;
    return 1;
}

// 0x1003cd20 ?ActivityToEnum@MfLiveActor@@IAEHH@Z
int MfLiveActor::ActivityToEnum(int)
{
    return 1;
}

// 0x1003cd30 ?SkillToEnum@MfLiveActor@@IAEHH@Z
int MfLiveActor::SkillToEnum(int skill)
{
    switch (skill) {
    case MfLocoSkill::TYPE:
        return 1;
    case MfManipSkill::TYPE:
        return 2;
    case MfSkill::TYPE:
        return 0;
    }
    return 0;
}

// 0x1003cd60 ?ManipSkillToEnum@MfLiveActor@@IAEHH@Z
int MfLiveActor::ManipSkillToEnum(int type)
{
    switch (type) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    case 5:
        return 5;
    }
    return 5;
}

// 0x1003cdc0 ?CheckActivity@MfLiveActor@@IAEHPAVMfActivity@@HVMfString@@H@Z
int MfLiveActor::CheckActivity(MfActivity *activity, int, MfString, int optional)
{
    MfString message;
    if (!activity) {
        if (optional)
            return 1;
        MfReportError(InstanceName(), 0x1b, 2);
        return 0;
    }
    return 1;
}

// 0x1003ce90 ?CheckSkill@MfLiveActor@@IAEHHPAVMfSkill@@HH@Z
int MfLiveActor::CheckSkill(int code, MfSkill *skill, int type, int optional)
{
    MfString message;
    if (!skill) {
        if (optional)
            return 1;
        MfReportError(InstanceName(), 0x1a, code, 4);
        return 0;
    }
    if (!skill->IsType(type)) {
        MfReportError(InstanceName(), 0x39, code, SkillToEnum(skill->Type()), SkillToEnum(type));
        return 0;
    }
    if (skill->IsType(MfLocoSkill::TYPE)) {
        MfActorPath *path = skill->sampledPath();
        if (!path) {
            MfReportError(InstanceName(), 0x36, code);
            return 0;
        }
        if (path->MaxSamples() <= 0) {
            MfReportError(InstanceName(), 0x37, code);
            return 0;
        }
    }
    if (!skill->ValidSkill()) {
        MfReportError(InstanceName(), 0x38, code, skill->Name());
        return 0;
    }
    return 1;
}

// 0x1003d030 ?CheckSkill@MfLiveActor@@IAEHHABVMfString@@AAPAVMfSkill@@HH@Z
int MfLiveActor::CheckSkill(int code, const MfString &name, MfSkill *&skill, int type, int optional)
{
    if (name == NULL_STRING) {
        skill = 0;
        return optional != 0;
    }
    skill = getSkill(name);
    if (!skill)
        return 0;
    return CheckSkill(code, skill, type, 0);
}

// 0x1003d0b0 ?CheckManipSkillType@MfLiveActor@@IAEHHPAVMfManipSkill@@H@Z
int MfLiveActor::CheckManipSkillType(int code, MfManipSkill *skill, int type)
{
    if (skill->mManipType != type) {
        MfReportError(InstanceName(), 0x3a, code, ManipSkillToEnum(skill->ManipType()), ManipSkillToEnum(type));
        return 0;
    }
    return 1;
}

// 0x1003d120 ?CheckTargetActor@MfLiveActor@@IAEHHPBVMfAiActor@@@Z
int MfLiveActor::CheckTargetActor(int code, const MfAiActor *target)
{
    MfString message;
    if (!mSpace->GetLiveActors().Contains((MfLiveActor *)target))
        return 0;
    if (!target) {
        MfReportError(InstanceName(), 0x1a, code, 3);
        return 0;
    }
    if (((const MfLiveActor *)target)->mSpace != mSpace) {
        MfReportError(InstanceName(), 0x3b, code);
        return 0;
    }
    return 1;
}

// EQUIV 0x1003d220 ?UpdateAppEventHandler@MfLiveActor@@IAEXAAVMfMsg@@@Z: reviewed: over the full 944 bytes only this and data swap registers (ebp/edi) consistently; both jump tables and the index table are equal
void MfLiveActor::UpdateAppEventHandler(MfMsg &msg)
{
    int data = mLastEventID;
    int event;
    switch (msg.mID) {
    case 15:
        switch (msg.m38) {
        case 0: event = 0xd; break;
        case 1: event = 0xe; break;
        case 2: event = 0xf; break;
        case 3: event = 0x10; break;
        }
        break;
    case 11:
        switch (msg.m38) {
        case 0: event = 0; break;
        case 1: event = 1; break;
        case 2: event = 2; break;
        }
        break;
    case 40: event = 0x20; break;
    case 42: event = 0x21; break;
    case 43: event = 0x22; break;
    case 44: event = 0x23; break;
    case 12: event = 0x1f; break;
    case 17: event = 0x11; break;
    case 18: event = 0x12; break;
    case 35: event = 0xa; break;
    case 36: event = 0xb; break;
    case 37: event = 0xc; break;
    case 26: event = 4; break;
    case 28: event = 5; break;
    case 20: event = 3; break;
    case 22: event = 6; break;
    case 21: event = 7; break;
    case 16:
        if (msg.m38 == 2)
            event = 9;
        else if (msg.m38 == 5)
            event = 8;
        break;
    case 57:
        if (msg.m38 == 1)
            event = 0x18;
        else if (msg.m38 == 2)
            event = 0x19;
        break;
    case 39:
        if (msg.m38 == 0)
            event = 0x14;
        else if (msg.m38 == 2)
            event = 0x16;
        else if (msg.m38 == 1)
            event = 0x13;
        else if (msg.m38 == 3)
            event = 0x15;
        else if (msg.m38 == 4)
            event = 0x17;
        break;
    case 58: event = 0x1e; break;
    case 55: event = 0x1d; break;
    case 10: event = 0x1a; break;
    case 50: event = 0x1b; break;
    case 51: event = 0x1c; break;
    case 52: event = 0x24; break;
    case 14: {
        MfMsgTypeInfo key;
        key.type = msg.mID;
        key.sub = msg.m38;
        MfString *name = g_msgTypes->Name(&key);
        if (name) {
            data = msg.mEventID;
            event = 0x1e;
            mLastMessage = *name;
            break;
        }
    }
    default:
        data = msg.mEventID;
        event = 0x25;
        break;
    }
    MfAppEventHookData hook;
    hook.event = event;
    hook.data = data;
    hook.pActor = this;
    gHookManager->CallHooks('WAcE', &hook);
    if (mController)
        mController->HandleEvent(&event);
}

// 0x1003d5d0 ?RemoveActivity@MfLiveActor@@UAEHPBVMfAiActivity@@H@Z
int MfLiveActor::RemoveActivity(const MfAiActivity *aiActivity, int flag)
{
    MfActivity *activity = (MfActivity *)aiActivity;
    if (!MfActivity::ActivityExists(activity))
        return 0;
    if (mRootActivity) {
        if (!mRootActivity->RemoveSubactivity(activity, flag) && activity)
            delete activity;
    }
    Lock();
    mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                       !IsCurrentActivity(Mf3dPathActivity::TYPE);
    Unlock();
    return 1;
}

// 0x1003d690 ?RemoveActivity@MfLiveActor@@UAEHHH@Z
int MfLiveActor::RemoveActivity(int id, int flag)
{
    if (mRootActivity)
        mRootActivity->RemoveSubactivity(id, flag);
    Lock();
    mNoLocoIntention = !IsCurrentActivity(MfLocoActivity::TYPE) && !mBlockList.GetCount() && !mLocoRequest &&
                       !IsCurrentActivity(Mf3dPathActivity::TYPE);
    Unlock();
    return 1;
}

// 0x1003d720 ?EnableBehavior@MfLiveActor@@UAEXH@Z
void MfLiveActor::EnableBehavior(int on)
{
    if (mHFSM && on)
        mHFSM->mEnabled = 1;
    if (mHFSM && !on)
        mHFSM->mEnabled = 0;
}

// 0x1003d750 ?GetLastActivity@MfLiveActor@@UAEPAVMfAiActivity@@XZ
MfAiActivity *MfLiveActor::GetLastActivity()
{
    return mLastActivity;
}

// 0x1003d770 ?SyncTraceUnderHip@MfLiveActor@@UAEXXZ
// Moves the trace (the root node) under the hip (node 1) and down to the
// lowest vertex of the actor, keeping the hip where it is.
void MfLiveActor::SyncTraceUnderHip()
{
    MfTransform3 hipGlobal = mSkeletonTraversal->node(1)->mGlobalTransform;
    MfTransform3 parentGlobal = ((MfSkeletonNode *)mSkeleton->parent())->mGlobalTransform;
    MfTransform3 home = mSkeletonTraversal->node(1)->homeTransform();
    MfTransform3 t = hipGlobal * ~home;
    t = ~parentGlobal * t;
    t.rotation().normalize();
    mSkeleton->setTransform(t);
    home.rotation().normalize();
    mSkeletonTraversal->node(1)->setTransform(home);
    mSkeleton->syncChildTransforms(0);

    MfTransform3 toTrace = ~mSkeleton->mGlobalTransform;
    MfVector3 p;
    float minY = 3.4028235e38f;
    for (int i = 0; i < mSkeletonTraversal->numNodes(); i++) {
        MfSkeletonNode *node = mSkeletonTraversal->node(i);
        if (node->colModel()) {
            for (int j = 0; j < node->colModel()->GetTriangleSet()->GetNumVertices(); j++) {
                MfTriangleSet *triangles = node->colModel()->GetTriangleSet();
                p = (toTrace * node->mGlobalTransform) * triangles->Vertex(j);
                if (p.y() < minY)
                    minY = p.y();
            }
        }
    }
    if (minY < 3.4028235e38f) {
        MfVector3 v = (~parentGlobal * mSkeleton->mGlobalTransform) * MfVector3(0.0f, minY, 0.0f);
        t = mSkeleton->mTransform;
        t.translation() = v;
        t.rotation().normalize();
        mSkeleton->setTransform(t);
        MfTransform3 r = ~mSkeleton->mGlobalTransform * hipGlobal;
        r.rotation().normalize();
        mSkeletonTraversal->node(1)->setTransform(r);
        mSkeleton->syncChildTransforms(0);
    }
}

// 0x1003dce0 ?GetCurStateName@MfLiveActor@@UAEHVMfString@@AAV2@1@Z
int MfLiveActor::GetCurStateName(MfString machine, MfString &state, MfString &sub)
{
    if (mHFSM)
        return mHFSM->GetCurStateName(machine, state, sub);
    return 0;
}

// 0x1003dd80 ?SetInstanceName@MfLiveActor@@UAEHABVMfString@@@Z
int MfLiveActor::SetInstanceName(const MfString &name)
{
    int result = MfActor::SetInstanceName(name);
    if (mHFSM)
        mHFSM->SetActorName(InstanceName());
    return result;
}

// 0x1003ddd0 ?SoundPath@MfLiveActor@@QAE?AVMfString@@XZ
MfString MfLiveActor::SoundPath()
{
    MfString path = GetDataPath();
    path << "\\Actors\\" << (mPrototype ? mPrototype->sName : mName) << "\\" << "Sounds\\";
    return path;
}

// 0x1003de80 ?SetController@MfLiveActor@@UAEXPAVMfAiController@@@Z
void MfLiveActor::SetController(MfAiController *controller)
{
    if (mController)
        mController->Release();
    mController = controller;
    if (controller)
        controller->AddRef();
    if (mLive)
        GoLive(0);
}

// 0x1003dec0 ??_EMfStringList@@WBA@AEPAXI@Z (adjustor thunk of the local MfStringList vtable copy)

// Not claimed: in the original this member is folded with an identical function
// (10003390, inline in the class and emitted with MfRuntime); the port needs its own definition.
MfString MfLiveActor::GetName()
{
    return InstanceName();
}

// Not claimed: in the original this member is folded with an identical function
// (10003580, MfActor::AttachTarget); the port needs its own definition.
int MfLiveActor::changeTexture(const MfString &, int, const MfString &, const MfString &)
{
    return 0;
}

// Not claimed: in the original this member is folded with an identical function
// (1007dfb0); the port needs its own definition.
void MfLiveActor::ShowActionMenu(MfAiRenderView *)
{
}

// Not claimed: in the original this member is folded with an identical function
// (1007dfb0); the port needs its own definition.
void MfLiveActor::HandleJoystickEvent(int)
{
}

// Not claimed: in the original this member is folded with an identical function
// (1000def0); the port needs its own definition.
void MfLiveActor::RespondToApproach(MfLiveActor *)
{
}

// Not claimed: in the original this member is folded with an identical function
// (1000def0); the port needs its own definition.
void MfLiveActor::RespondToRetreat(MfLiveActor *)
{
}

MfLiveActor *MfLiveActor::plaPendingOptionTarget;
