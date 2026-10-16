// MfActivity (RuntimeLib.dll 1000d560-1000e210) and its empty interface
// methods (folded by the linker with other empty functions).
// MfStream.h: ReadFromStream sets the stream status directly
#define MF_STREAM_STATUS_ACCESS
#include "MfActivity.h"
#include <string.h>
#include "MfHookManager.h"
#include "MfManagerActivity.h"
#include "MfPoActivity.h"
#include "MfScDataHandle.h"
#include "MfSkeletonTraversal.h"
#include "MfVector3.h"
#include "MfWorld.h"
#include "MfLiveActor.h"
#include "MfLogger.h"
#include "MfMsg.h"
#include "MfMsgDispatcher.h"
#include "MfStream.h"

// the stream helpers of the first activity object (10009690-1000a8b0)
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);
MfStreamError WriteActorReference(MfStream &stream, MfActor *actor);
MfStreamError ReadActorReference(MfStream &stream, MfActor *&actor);

// the data of MSG_ACTIVITY_FINISHED (11): the activity that finished
class MfActivityFinishedData : public MfMsgDataBlock {
public:
    MfActivityFinishedData(MfActivity *activity) { mActivity = activity; }

    MfActivity *mActivity;                                  // 0x08
};

enum { MSG_ACTIVITY_FINISHED = 11 };

const long MfActivity::STREAM_TYPE = 'Acti';

// the implicit copy members of the exported class
static MfActivity &(MfActivity::*s_assign)(const MfActivity &) = &MfActivity::operator=;

// 0x10002920 ??0MfActivity@@QAE@ABV0@@Z (implicit copy constructor)
// 0x10002a10 ??4MfActivity@@QAEAAV0@ABV0@@Z (implicit assignment)
// 0x10002af0 ?CatchMsg@MfActivity@@UAEXAAVMfMsg@@@Z
// 0x100044e0 ?Type@MfActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x100044f0 ?IsType@MfActivity@@WBE@AEHH@Z

MfActivity::MsgHandler MfActivity::aMethods[NUM_MSGS];
int MfActivity::bMsgTableIsSet;

// every activity, as a list and by address (ActivityExists)
// 0x1000d4b0 _$E55
// 0x1000d4c0 _$E51
// 0x1000d4e0 _$E54
// 0x1000d4f0 _$E52
MfDblList MfActivity::activityList;
// 0x1000d510 _$E60
// 0x1000d520 _$E57
// 0x1000d530 _$E59
// 0x1000d540 _$E58
MfIndex MfActivity::activityTable(0x35);

// 0x1000d560 ??0MfActivity@@QAE@XZ
MfActivity::MfActivity()
    : mActor(0), mStartTime(0), mLastTime(0), mBlendTime(0), mTimeout(0), mDuration(0),
      mInitialConfig(0), mFinished(0), m40(1), mParent(0), mPiccoloHandle(0), mCompound(0),
      mDeleteOnFinished(1), mInTransitToFinish(0), mLocoPlugin(0)
{
    InitMsgTable();
    MfPoActivity *po = new MfPoActivity;
    po->SetActivity(this);
    mPiccoloHandle = MfScDataHandle::CreateData(po);
    activityTable.Add((unsigned long)this, this);
    MfDblList *list = &activityList;
    list->Append(this);
}

// 0x1000d680 ??_GMfPoActivity@@UAEPAXI@Z (local copy for the dllimport class MfPoActivity)

// EQUIV 0x1000d6a0 ??0MfActivity@@QAE@PAVMfActor@@K@Z: reviewed: the new MfPoActivity is spilled to the startTime argument slot instead of the actor slot, and the EH funclet reads it from there; every instruction is otherwise the same
MfActivity::MfActivity(MfActor *actor, unsigned long startTime)
    : mStartTime(startTime), mLastTime(startTime)
{
    mActor = actor;
    mInitialConfig = 0;
    mTimeout = 0;
    mFinished = 0;
    mBlendTime = 0;
    mDuration = 0;
    m40 = 1;
    mCompound = 0;
    mDeleteOnFinished = 1;
    mInTransitToFinish = 0;
    mLocoPlugin = 0;
    MfPoActivity *po = new MfPoActivity;
    po->SetActivity(this);
    mPiccoloHandle = MfScDataHandle::CreateData(po);
    mParent = 0;
    InitMsgTable();
    mUniqueID = MfWorld::CreateUniqueNumber();
    activityTable.Add((unsigned long)this, this);
    MfDblList *list = &activityList;
    list->Append(this);
}

// 0x1000d7d0 ??1MfActivity@@UAE@XZ
MfActivity::~MfActivity()
{
    if (MfWorld::enableNetwork) {
        MfActivityHookData data;
        data.pActor = mActor;
        data.pActivity = this;
        data.activityID = mUniqueID;
        data.m1c = 0;
        data.op = 2;
        gHookManager->CallHooks(MF_HOOK_ACTIVITY_DATA, &data);
    }
    Unregister();
    if (mInitialConfig)
        delete [] mInitialConfig;
    if (mParent) {
        MfDblList *siblings = &mParent->mSubactivities;
        MfDblListItem *item = siblings->Find(this);
        siblings->Delete(item);
        mParent = 0;
    }
    if (mPiccoloHandle) {
        MfPoActivity *po = dynamic_cast<MfPoActivity *>(mPiccoloHandle->GetData());
        if (po)
            po->SetActDeleted();
        mPiccoloHandle->Release();
    }
    activityTable.Delete((unsigned long)this);
    MfDblListItem *item = activityList.Find(this);
    if (item)
        activityList.Delete(item);
}

// 0x1000d940 ?ActivityExists@MfActivity@@SAHPAV1@@Z
int MfActivity::ActivityExists(MfActivity *activity)
{
    if (!activityTable.Find((unsigned long)activity))
        return 0;
    if (activity->IsType(MfManagerActivity::TYPE)) {
        MfDblList &subs = ((MfManagerActivity *)activity)->mSubactivities;
        MfDblListItem *pos;
        MfActivity *sub = (MfActivity *)subs.GetFirst(pos);
        while (sub) {
            if (!ActivityExists(sub))
                return 0;
            sub = (MfActivity *)subs.GetNext(pos);
        }
        return 1;
    }
    return 1;
}

// 0x1000d9b0 ?ClearActivityTable@MfActivity@@SAXPAVMfLiveActor@@@Z
// Deletes the activities of an actor (that still exist).
void MfActivity::ClearActivityTable(MfLiveActor *actor)
{
    MfDblList doomed;
    MfDblList *list = &doomed;
    MfDblListItem *pos;
    MfActivity *activity;
    for (activity = (MfActivity *)activityList.GetFirst(pos); activity;
         activity = (MfActivity *)activityList.GetNext(pos)) {
        if (activity->mActor == actor)
            list->Append(activity);
    }
    for (activity = (MfActivity *)doomed.GetFirst(pos); activity; activity = (MfActivity *)doomed.GetNext(pos)) {
        if (ActivityExists(activity) && activity)
            delete activity;
    }
}

// 0x1000da90 ?GetPiccoloHandle@MfActivity@@UAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfActivity::GetPiccoloHandle()
{
    mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x1000daa0 ?Reset@MfActivity@@UAEXK@Z
void MfActivity::Reset(unsigned long time)
{
    m40 = 1;
    mFinished = 0;
    mStartTime = time;
    mLastTime = time;
}

// 0x1000dac0 ?Update@MfActivity@@UAE?AW4MfActivityStatus@@K@Z
// Finishes the activity when its timeout has passed; the first update after
// that posts MSG_ACTIVITY_FINISHED to the actor.
MfActivityStatus MfActivity::Update(unsigned long time)
{
    mLastTime = time;
    if (mTimeout && time - mStartTime >= mTimeout)
        mFinished = 1;
    if (mFinished && m40) {
        m40 = 0;
        mInTransitToFinish = 1;
        MfMsg *msg = new MfMsg(MSG_ACTIVITY_FINISHED, 0, 0, this, mActor);
        msg->SetData(new MfActivityFinishedData(this));
        g_msgDispatcher->Post(msg);
    }
    return (MfActivityStatus)(mFinished != 0);
}

// 0x1000dbb0 ??_GMfMsgDataBlock@@UAEPAXI@Z

// 0x1000dbd0 ?setInitialConfig@MfActivity@@UAEXPAVMfSkeletonTraversal@@K@Z
void MfActivity::setInitialConfig(MfSkeletonTraversal *traversal, unsigned long blendTime)
{
    if (mInitialConfig)
        delete [] mInitialConfig;
    mInitialConfig = traversal->CopyTransforms();
    mBlendTime = blendTime;
}

// 0x1000dc20 ?setInitialConfig@MfActivity@@UAEXPAVMfTransform3@@K@Z
void MfActivity::setInitialConfig(MfTransform3 *config, unsigned long blendTime)
{
    if (mInitialConfig)
        delete [] mInitialConfig;
    mInitialConfig = config;
    mBlendTime = blendTime;
}

// 0x1000dc60 ?setTimeout@MfActivity@@UAEXK@Z
void MfActivity::setTimeout(unsigned long timeout)
{
    mTimeout = timeout;
}

// 0x1000dc70 ?blendFromInitialConfig@MfActivity@@MAEXKH@Z
// During the blend time the nodes (without the root when skipRoot is set) go
// from the initial configuration towards their current transforms.
void MfActivity::blendFromInitialConfig(unsigned long time, int skipRoot)
{
    if (mInitialConfig && time - mStartTime < mBlendTime && !mFinished) {
        float t = (float)(time - mStartTime) / (float)mBlendTime;
        MfSkeletonTraversal *nodes = mActor->skeletonTraversal();
        MfTransform3 current;
        MfTransform3 blended;
        for (int i = skipRoot != 0; i < nodes->numNodes(); i++) {
            current = nodes->node(i)->transform();
            blended.interpolate(mInitialConfig[i], current, t);
            nodes->node(i)->setTransform(blended);
        }
    }
}

// 0x1000de20 ?GetInterfaceName@MfActivity@@UAEXPADHAAH1@Z
void MfActivity::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiActivity", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1000de60 ?AddRef@MfActivity@@UAEHXZ
int MfActivity::AddRef()
{
    return 0;
}

int MfActivity::Release()
{
    return 0;
}

// 0x1000de70 ?SetDuration@MfActivity@@UAEXK@Z
void MfActivity::SetDuration(unsigned long duration)
{
    mDuration = duration;
    if (mBlendTime > duration)
        mBlendTime = duration;
    if (IsType(MfManagerActivity::TYPE))
        SetDuration(duration);
}

// 0x1000deb0 ?InitMsgTable@MfActivity@@UAEXXZ
void MfActivity::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        bMsgTableIsSet = 1;
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfActivity::DefaultMsgHandler;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x1000def0 ?DefaultMsgHandler@MfActivity@@UAEXAAVMfMsg@@@Z
void MfActivity::DefaultMsgHandler(MfMsg &)
{
}

// 0x1000df00 ?SetBlendTime@MfActivity@@UAEXK@Z
void MfActivity::SetBlendTime(unsigned long blendTime)
{
    mBlendTime = blendTime;
    if (mActor && blendTime > 0)
        setInitialConfig(mActor->skeletonTraversal(), blendTime);
}

// 0x1000df30 ?CreateActivity@@YAPAVMfStreamObject@@XZ
static MfStreamObject *CreateActivity()
{
    return new MfActivity;
}

// 0x1000dfb0 _$E62
// 0x1000dfa0 _$E63
static MfStreamConstructorEntry constructorEntry(MfActivity::STREAM_TYPE, CreateActivity);

// 0x1000dfd0 ?StreamType@MfActivity@@UBEJXZ
long MfActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000dfe0 ?WriteToStream@MfActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
// The start time is written in world time (with the paused time added back).
MfStreamError MfActivity::WriteToStream(MfStream &stream) const
{
    if (!mActor)
        return (MfStreamError)4;
    if (MfWorld::enableNetwork)
        WriteActorReference(stream, mActor);
    else
        WriteActorReference(stream, 0);
    WriteNodeReference(stream, (MfSkeletonNode *)mActor->skeleton()->parent());
    stream << (int)mTimeout;
    stream << (int)(mStartTime + g_world->mAccumPauseTime) << (int)mBlendTime << (int)mDuration << mUniqueID
           << mCompound;
    stream << mLocoPlugin;
    return stream.Status();
}

// 0x1000e080 ?ReadFromStream@MfActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfActivity::ReadFromStream(MfStream &stream)
{
    ReadActorReference(stream, mActor);
    if (!mActor)
        return stream.status = (MfStreamError)2;
    MfSkeletonNode *parent;
    ReadNodeReference(stream, mActor, parent);
    if (stream.Status() == 0)
        mActor->skeleton()->changeParent(parent, 0);
    stream >> (int &)mTimeout >> (int &)mStartTime;
    mStartTime -= g_world->mAccumPauseTime;
    stream >> (int &)mBlendTime >> (int &)mDuration;
    stream >> mUniqueID >> mCompound;
    stream >> mLocoPlugin;
    if (mBlendTime > 0)
        setInitialConfig(mActor->skeletonTraversal(), mBlendTime);
    mLastTime = mStartTime;
    return stream.Status();
}

// 0x1000e160 ?AddActivity@MfActivity@@UAEHPBVMfAiActivity@@@Z
int MfActivity::AddActivity(const MfAiActivity *)
{
    MfLogger::Log(1, MfString("Activity"), NULL_STRING,
                  "Activity.AddActivity()   This activity cannot take child activities.");
    return 0;
}

// 0x1007dfb0 ?AvoidOcclusion@MfActivity@@UAEXH@Z
void MfActivity::AvoidOcclusion(int)
{
}

// 0x1007dfc0 ?SetBoundary@MfActivity@@UAEXHABVMfVector3@@0@Z
void MfActivity::SetBoundary(int, const MfVector3 &, const MfVector3 &)
{
}

void MfActivity::PrepareForExecution() {}
int MfActivity::RemoveSubactivity(const MfAiActivity *) { return 0; }
int MfActivity::RemoveSubactivity(int) { return 0; }
int MfActivity::RemoveHoldActivity(const MfAiActor *) { return 0; }
void MfActivity::GetSubactivities(MfDblList *) {}
int MfActivity::WillFinish() { return 0; }
int MfActivity::HasMotion() { return 0; }
int *MfActivity::GetIgnoreMasks(int &) { return 0; }
void MfActivity::SetStiffness(int, float) {}
void MfActivity::SetSluggishness(float) {}
void MfActivity::SetGroupFactor(float) {}
void MfActivity::SetFocus(int, float) {}
void MfActivity::SetViewDistance(float) {}
void MfActivity::TargetEntireActor(int) {}
void MfActivity::TrackOrientation(int) {}
void MfActivity::UseNodeOrientation(int) {}
int MfActivity::TestViewDirection(int) { return 0; }
void MfActivity::SelectViewDirection(int) {}
int MfActivity::GetViewDirection() { return 0; }
void MfActivity::SetTargetOffset(const MfVector3 &) {}
void MfActivity::SetViewBias(float) {}
void MfActivity::SetCameraUpVector(int, const MfVector3 &) {}
MfAiPath2 *MfActivity::GetCopy2DPath() { return 0; }
int MfActivity::IsManagerActivity() { return 0; }
MfAiActivitySpec *MfActivity::GetSpec() { return 0; }
