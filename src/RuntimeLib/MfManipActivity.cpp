#include "MfManipActivity.h"
#include "MfHandle.h"
#include "MfActorPath.h"
#include "MfGCPath.h"
#include "MfMsgData.h"
#include "MfManipPlanner.h"
#include "MfManipSkill.h"
#include "MfLiveActor.h"
#include "MfStream.h"
#include "MfAiColNode.h"
#include "MfColModel.h"
#include "MfColNode.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfMsg.h"
#include "MfMsgDispatcher.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfWorld.h"

// the planned paths and references to actors and nodes in a stream (defined
// with the first activities, 10009050-1000a8b0)
MfStreamError WriteGCPath(MfStream &stream, MfActor *actor, MfGCPath *path);
MfStreamError ReadGCPath(MfStream &stream, MfActor *actor, MfGCPath *&path);
MfStreamError WriteMTPath(MfStream &stream, MfActor *actor, MfMTPath *path);
MfStreamError ReadMTPath(MfStream &stream, MfActor *actor, MfMTPath *&path);
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);
MfStreamError WriteActorReference(MfStream &stream, MfActor *actor);
MfStreamError ReadActorReference(MfStream &stream, MfActor *&actor);

long const MfManipActivity::STREAM_TYPE = 'MnpA';

// the implicit copy members of the exported class
static MfManipActivity &(MfManipActivity::*s_assign)(const MfManipActivity &) = &MfManipActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10008320 ?Type@MfManipActivity@@UAEHXZ
// 0x10008330 ?IsType@MfManipActivity@@UAEHH@Z
// 0x10008370 ?getMotionType@MfManipActivity@@QAE?AW4MfMotionType@1@XZ
// 0x10008380 ?getHandle@MfManipActivity@@QAEPAVMfHandle@@XZ
// 0x10008390 ?getTarget@MfManipActivity@@QAEPAVMfActor@@XZ (= MfLocoSkill::FootSampleStep)
// 0x100083a0 ??0MfManipActivity@@QAE@ABV0@@Z
// 0x10008600 ??4MfManipActivity@@QAEAAV0@ABV0@@Z
// 0x1000aa70 ?Type@MfManipActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000aa80 ?IsType@MfManipActivity@@WBE@AEHH@Z

// 0x10044ea0 ??0MfManipActivity@@QAE@XZ
MfManipActivity::MfManipActivity()
    : mPath(0), mGCPath(0), mMTPath(0)
{
}

// 0x10044f10 ??0MfManipActivity@@QAE@PAVMfActor@@KPAVMfManipSkill@@PAUMfGCPath@@PAUMfMTPath@@H0HPAVMfHandle@@PAVMfSkeletonNode@@H@Z
MfManipActivity::MfManipActivity(MfActor *actor, unsigned long time, MfManipSkill *skill, MfGCPath *gcPath,
                                 MfMTPath *mtPath, int duration, MfActor *target, int motionType,
                                 MfHandle *handle, MfSkeletonNode *node, int flags)
    : MfAtomicActivity(actor, time, skill)
{
    MfManipPlanner planner;
    mMTPath = mtPath;
    mGCPath = gcPath;
    if (duration < 0)
        duration = 0;
    mc8 = duration;
    mDuration = duration;
    mf0 = 0;
    mTarget = target;
    if (flags & 0x1000)
        mf0 = 1;
    if (flags & 0x8000)
        _InitBubbleModel(skill, actor, 1);
    else if (flags & 4)
        _InitBubbleModel(skill, actor, 0);
    else
        mBubbleModel = 0;
    if (mTarget)
        mTarget->AddRef();
    mMotionType = (MfMotionType)motionType;
    mac = 0;
    mb4 = node;
    int release = motionType == 2 || motionType == 6;
    if (mGCPath)
        mGrasp = mGCPath->m14;
    if (mMTPath)
        mPath = planner.SampleMove(actor, skill, mMTPath, mc8, release);
    else if (mGCPath)
        mPath = planner.SampleGrasp(actor, skill, mGCPath, mc8);
    mNumSamples = mPath->MaxSamples();
    mHandle = handle;
}

// TODO 0x10045150 ??1MfManipActivity@@UAE@XZ: the original expands the first delete [] and calls the local ??_EMfTransform3 (10005f20) for the second (as in MfLiveActor::HandleManipMoveTo); here both are expanded (inline budget; 84% similar)
MfManipActivity::~MfManipActivity()
{
    MfMTPath *path = mMTPath;
    if (path) {
        delete [] path->mFrom;
        delete [] path->mTo;
        delete path;
    }
    if (mGCPath)
        delete mGCPath;
    if (mTarget)
        mTarget->Release();
    if (mPath)
        delete mPath;
}

// 0x10045240 ?Update@MfManipActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfManipActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    if (mMotionType == 1 && !mac) {
        if (((MfLiveActor *)mTarget)->mSpace)
            ((MfLiveActor *)mTarget)->ChangeParent(mb4, (MfManipSkill *)mSkill, 0, 0);
        mac = 1;
        mTarget->Release();
        mTarget = 0;
    }
    unsigned long skillTime = time - mStartTime;
    if (mDuration > 0)
        skillTime = mc8 * skillTime / mDuration;
    if (mFinished)
        return (MfActivityStatus)1;
    if (mIgnoreMasks)
        mPath->Sample(skillTime, mActor->skeletonTraversal(), mIgnoreMasks, mSkill->GetInterpolate());
    else
        mPath->Sample(skillTime, mActor->skeletonTraversal(), mSkill->GetInterpolate(), 0, 0);
    if (mTarget && mTarget->mSpace && mTarget->skeleton()->parent())
        ((MfSkeletonNode *)mTarget->skeleton()->parent())->syncChildTransforms(0);
    if (mBubbleModel) {
        MfDblList children;
        mActor->GetChildActors(&children);
        MfDblListItem *pli;
        MfAiActor *child;
        for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
            ((MfActor *)child)->ignoreTriangleCollisionCheck(1);
        mActor->ignoreTriangleCollisionCheck(1);
        _CheckBubble(skillTime);
        mActor->ignoreTriangleCollisionCheck(0);
        for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
            ((MfActor *)child)->ignoreTriangleCollisionCheck(0);
    }
    if (skillTime > mNumSamples)
        mFinished = 1;
    if (mFinished && mMotionType == 6 && mTarget->mSpace) {
        MfTransform3 parent = ((MfSkeletonNode *)mTarget->skeleton()->parent())->globalTransform();
        parent = ~parent;
        MfTransform3 transform = parent * mGrasp;
        mTarget->skeleton()->setTransform(transform);
        mTarget->skeleton()->syncChildTransforms(0);
    }
    MfActivity::mLastTime = time;
    return MfActivity::Update(time);
}

// TODO 0x10045570 ?_CheckBubble@MfManipActivity@@MAEXAAK@Z: first body: the calls and their order
// agree; locals, the message data arguments and the inline expansions are not settled (40% similar)
// Moves the bubbles to the sampled configuration; a bubble whose way meets
// the space's environment reports the collision to both actors (with
// mf0 the sample time goes back to the contact).
void MfManipActivity::_CheckBubble(unsigned long &sample)
{
    m98 = (int)mActor->mSpace->mSnapshotEnv;
    MfVector3 center;
    MfVector3 step;
    MfVector3 hit;
    MfVector3 normal;
    int eventID = g_world->CreateUniqueID();
    MfColResultRay result;
    unsigned long time = sample;
    MfDblList filter;
    mActor->UseFilter(1, 2, 5, filter);
    for (int i = 0; i < mBubbleModel->mCount; i++) {
        center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
        if (~mBubbleModel->mNodes[i]->ColNode().GetCheckStyle() & 4) {
            mBubbleModel->mGlobalCenters[i] = center;
            continue;
        }
        step = center - mBubbleModel->mGlobalCenters[i];
        float length = step.length();
        if (length < 0.0001f)
            step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
        float norm = (float)sqrt(step.lengthSquared());
        step.mX /= norm;
        step.mY /= norm;
        step.mZ /= norm;
        float reach = mBubbleModel->mRadii[i] + length;
        MfAiColNode *env = (MfAiColNode *)m98;
        if (env->CheckCollision(mBubbleModel->mGlobalCenters[i], step, result, true, reach)) {
            hit = result.pos;
            MfAiColNode *other = result.node;
            normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(result.triIndex);
            if (mf0) {
                float distance = (hit - mBubbleModel->mGlobalCenters[i]).length();
                unsigned long start = mLastTime - mStartTime;
                if (mDuration > 0)
                    start = mc8 * start / mDuration;
                sample = start + (int)((time - start) * distance / length);
                mPath->Sample(sample, mActor->skeletonTraversal(), mSkill->GetInterpolate(), 0, 0);
            }
            MfSkeletonNode *otherNode = other->Owner() ? (MfSkeletonNode *)other->Owner() : 0;
            MfLiveActor *otherActor = (MfLiveActor *)otherNode->mOwner;
            normal = otherNode->globalTransform().rotation() * normal;
            normal.length();
            MfVector3 direction;
            MfVector3 point = center;
            direction = center - mBubbleModel->mGlobalCenters[i];
            MfMsg *msg = new MfMsg(35, 0, 0, mActor, mActor);
            msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, mBubbleModel->mNodes[i],
                                                otherNode, mSkill, (float)sample / mLastTime, point, hit,
                                                normal, eventID));
            g_msgDispatcher->Post(msg);
            msg = new MfMsg(36, 0, 0, mActor, otherActor);
            msg->SetData(new MfCollisionMsgData(otherActor, (MfLiveActor *)mActor, otherNode,
                                                mBubbleModel->mNodes[i], mSkill, (float)sample / mLastTime,
                                                point, hit, normal, eventID));
            g_msgDispatcher->Post(msg);
        } else {
            mBubbleModel->mGlobalCenters[i] = center;
        }
    }
    mActor->UseFilter(0, 2, 5, filter);
}

// 0x10045d40 ?CreateManipActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateManipActivity()
{
    return new MfManipActivity;
}

// 0x10045db0 _$E50 (original $E: the static initializer of the constructor entry, with 10045dc0)
// 0x10045dc0 _$E49
static MfStreamConstructorEntry constructorEntry(MfManipActivity::STREAM_TYPE, CreateManipActivity);

// 0x10045de0 ?StreamType@MfManipActivity@@UBEJXZ
long MfManipActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10045df0 ?WriteToStream@MfManipActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManipActivity::WriteToStream(MfStream &stream) const
{
    MfAtomicActivity::WriteToStream(stream);
    if (mMTPath) {
        stream << 1;
        WriteMTPath(stream, mActor, mMTPath);
    } else {
        stream << 0;
    }
    if (mGCPath) {
        stream << 1;
        WriteGCPath(stream, mActor, mGCPath);
    } else {
        stream << 0;
    }
    stream << mc8;
    WriteActorReference(stream, mTarget);
    stream << mMotionType;
    if (mMotionType == 2 && !mb4)
        WriteNodeReference(stream, (MfSkeletonNode *)mTarget->skeleton()->parent());
    else
        WriteNodeReference(stream, mb4);
    WriteNodeReference(stream, mHandle);
    stream << mf0;
    return stream.Status();
}

// 0x10045ef0 ?ReadFromStream@MfManipActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManipActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfAtomicActivity::ReadFromStream(stream);
    if (error != MF_STREAM_OK)
        return error;
    MfManipPlanner planner;
    int present;
    stream >> present;
    if (present)
        ReadMTPath(stream, mActor, mMTPath);
    stream >> present;
    if (present)
        ReadGCPath(stream, mActor, mGCPath);
    stream >> mc8;
    ReadActorReference(stream, mTarget);
    if (mTarget)
        mTarget->AddRef();
    stream >> (int &)mMotionType;
    ReadNodeReference(stream, mActor, mb4);
    MfSkeletonNode *handle;
    ReadNodeReference(stream, mActor, handle);
    mHandle = (MfHandle *)handle;
    stream >> mf0;
    if (stream.Status() == MF_STREAM_OK) {
        mac = 0;
        int release = mMotionType == 2 || mMotionType == 6;
        if (mGCPath)
            mGrasp = mGCPath->m14;
        if (mMTPath)
            mPath = planner.SampleMove(mActor, (MfManipSkill *)mSkill, mMTPath, mc8, release);
        else if (mGCPath)
            mPath = planner.SampleGrasp(mActor, (MfManipSkill *)mSkill, mGCPath, mc8);
        mNumSamples = mPath->MaxSamples();
    }
    return stream.Status();
}

// 0x100087f0 ??_EMfManipActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (10006ca0); the port needs its own definition.
int MfManipActivity::HasMotion()
{
    return 1;
}
