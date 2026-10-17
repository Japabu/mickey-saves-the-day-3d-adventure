// MfAttachActivity: moves the actor onto a transform with a manipulation
// skill (RuntimeLib.dll 100196a0-1001ab90).
#include "MfAttachActivity.h"
#include "MfActor.h"
#include "MfManipSkill.h"
#include "MfSkeletonTraversal.h"
#include "MfSkill.h"
#include "MfSkeletonNode.h"
#include "MfSpace.h"
#include "MfStream.h"
#include "MfAiColNode.h"
#include "MfColModel.h"
#include "MfColNode.h"
#include "MfColDistResults.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfLiveActor.h"
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfMsgDispatcher.h"
#include "MfTriangleSet.h"
#include "MfWorld.h"
#include <math.h>

long const MfAttachActivity::STREAM_TYPE = 'AttA';

// 0x100196a0 ??0MfAttachActivity@@QAE@XZ
MfAttachActivity::MfAttachActivity()
{
    me4 = 0;
}

// 0x10019730 ?Type@MfAttachActivity@@UAEHXZ
// 0x10019740 ?IsType@MfAttachActivity@@UAEHH@Z
// 0x10019780 ?PrintName@MfAttachActivity@@UBE?AVMfString@@XZ
// 0x100197b0 ??_GMfAttachActivity@@UAEPAXI@Z

// TODO 0x100197d0 ??0MfAttachActivity@@QAE@PAVMfActor@@KPAVMfSkill@@ABVMfTransform3@@H@Z: calls and
// frame agree; the two local transforms sit in each other's stack slots, the
// skill+4 load is scheduled before the GetNodeName call, and the end calls the
// imported ~MfTransform3 of the locals where the original drops them (81%
// similar; declaring handle first or folding start into one expression is worse)
MfAttachActivity::MfAttachActivity(MfActor *actor, unsigned long startTime, MfSkill *skill,
                                   const MfTransform3 &target, int flags)
    : MfAtomicActivity(actor, startTime, skill)
{
    mTarget = target;
    if (flags & 0x1000)
        me4 = 1;
    else
        me4 = 0;
    if (flags & 0x8000)
        _InitBubbleModel(skill, actor, 1);
    else if (flags & 4)
        _InitBubbleModel(skill, actor, 0);
    else
        mBubbleModel = 0;
    MfTransform3 sample;
    skill->Sample(sample, actor, actor->skeleton()->GetNodeName(), 0);
    MfTransform3 handle;
    skill->GetHandleTransform(handle);
    MfTransform3 parent(((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform());
    MfTransform3 start = ~parent * mTarget * ~handle;
    mStart = start * ~sample;
}

// 0x10019aa0 ??1MfAttachActivity@@UAE@XZ
MfAttachActivity::~MfAttachActivity()
{
}

// 0x10019ac0 ?Update@MfAttachActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfAttachActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    float rate = 0;
    unsigned long sample = time - mStartTime;
    if (mDuration > 0)
        rate = (float)mSkill->GetDuration() / mDuration;
    sample = (unsigned long)(sample * rate);
    if (sample > mLastTime || !mDuration)
        mFinished = 1;
    if (mPlayDirection == 1)
        sample = mLastTime - sample;
    if (mFinished)
        sample = mPlayDirection == 1 ? mFirstTime : mLastTime;
    if (mIgnoreMasks)
        mSkill->sample(sample, mActor->skeletonTraversal(), mIgnoreMasks);
    else
        mSkill->sample(sample, mActor->skeletonTraversal(), 0);
    MfTransform3 xform = mStart * mActor->skeleton()->transform();
    mActor->skeleton()->setTransform(xform);
    mActor->skeleton()->syncChildTransforms(0);
    if (mBubbleModel) {
        MfDblList children;
        mActor->GetChildActors(&children);
        MfDblListItem *pli;
        MfAiActor *child;
        for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
            ((MfActor *)child)->ignoreTriangleCollisionCheck(1);
        mActor->ignoreTriangleCollisionCheck(1);
        _CheckBubble(sample);
        mActor->ignoreTriangleCollisionCheck(0);
        for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
            ((MfActor *)child)->ignoreTriangleCollisionCheck(0);
    }
    blendFromInitialConfig(time, 0);
    updateExtraTracks(mLastSampleTime, sample);
    mLastSampleTime = sample;
    return MfActivity::Update(time);
}

// TODO 0x10019d10 ?_CheckBubble@MfAttachActivity@@MAEXAAK@Z: first body (as MfManipActivity's,
// with me4 the sample goes back in thirds until the bubbles are free); the call sequence
// follows the original, locals and argument details do not (44% similar)
// Moves the bubbles to the sampled configuration and reports the collisions
// of their way through the space's environment to both actors.
void MfAttachActivity::_CheckBubble(unsigned long &sample)
{
    m98 = (int)mActor->mSpace->mSnapshotEnv;
    MfVector3 center;
    MfVector3 step;
    MfVector3 hit;
    MfVector3 normal;
    int eventID = g_world->CreateUniqueID();
    MfColResultRay result;
    MfColResultSphere sphere;
    unsigned long time = sample;
    int backoff = 0;
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
        if (!env->CheckCollision(mBubbleModel->mGlobalCenters[i], step, result, true, reach)) {
            mBubbleModel->mGlobalCenters[i] = center;
            continue;
        }
        hit = result.pos;
        MfAiColNode *other = result.node;
        normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(result.triIndex);
        if (me4) {
            float distance = (hit - mBubbleModel->mGlobalCenters[i]).length();
            unsigned long t = mLastSampleTime + (int)((time - mLastSampleTime) * distance / length);
            sample = t;
            backoff = (t - mLastSampleTime) / 3 + 1;
            for (;;) {
                if (t < mLastSampleTime)
                    t = mLastSampleTime;
                if (mIgnoreMasks)
                    mSkill->sample(t, mActor->skeletonTraversal(), mIgnoreMasks);
                else
                    mSkill->sample(t, mActor->skeletonTraversal(), 0);
                if (!mCompound) {
                    MfTransform3 xform = mStart * mActor->skeleton()->transform();
                    mActor->skeleton()->setTransform(xform);
                    mActor->skeleton()->syncChildTransforms(0);
                }
                center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
                if (t == mLastSampleTime)
                    break;
                step = center - mBubbleModel->mGlobalCenters[i];
                length = step.length();
                if (length < 0.0001f)
                    step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                norm = (float)sqrt(step.lengthSquared());
                step.mX /= norm;
                step.mY /= norm;
                step.mZ /= norm;
                reach = mBubbleModel->mRadii[i] + length;
                if (env->CheckCollision(mBubbleModel->mGlobalCenters[i], step, result, true, reach))
                    break;
                if (!((MfAiColNode *)m98)->CheckCollision(center, mBubbleModel->mRadii[i], sphere))
                    break;
                t -= backoff;
            }
            mBubbleModel->mGlobalCenters[i] = center;
            sample = t;
        }
        MfSkeletonNode *otherNode = other->Owner() ? (MfSkeletonNode *)other->Owner() : 0;
        MfLiveActor *otherActor = (MfLiveActor *)otherNode->mOwner;
        normal = otherNode->globalTransform().rotation() * normal;
        MfVector3 direction;
        MfVector3 point = center;
        direction = center - mBubbleModel->mGlobalCenters[i];
        MfMsg *msg = new MfMsg(35, 0, 0, mActor, mActor);
        msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, mBubbleModel->mNodes[i], otherNode,
                                            mSkill, (float)sample / mLastTime, point, hit, normal, eventID));
        g_msgDispatcher->Post(msg);
        msg = new MfMsg(36, 0, 0, mActor, otherActor);
        msg->SetData(new MfCollisionMsgData(otherActor, (MfLiveActor *)mActor, otherNode, mBubbleModel->mNodes[i],
                                            mSkill, (float)sample / mLastTime, point, hit, normal, eventID));
        g_msgDispatcher->Post(msg);
        mBubbleModel->mGlobalCenters[i] = center;
    }
    if (backoff)
        g_msgDispatcher->Post(new MfMsg(37, 0, 0, mActor, mActor));
    mActor->UseFilter(0, 2, 5, filter);
}

// 0x1001a7d0 ?CreateAttachActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateAttachActivity()
{
    return new MfAttachActivity;
}

// 0x1001a840 _$E47
// 0x1001a850 _$E46
static MfStreamConstructorEntry constructorEntry(MfAttachActivity::STREAM_TYPE, CreateAttachActivity);

// 0x1001a870 ?StreamType@MfAttachActivity@@UBEJXZ
long MfAttachActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1001a880 ?WriteToStream@MfAttachActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAttachActivity::WriteToStream(MfStream &stream) const
{
    MfAtomicActivity::WriteToStream(stream);
    stream.WriteBytes(sizeof(MfTransform3), &mTarget);
    stream << me4;
    return stream.Status();
}

// 0x1001a8c0 ?ReadFromStream@MfAttachActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAttachActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfAtomicActivity::ReadFromStream(stream);
    if (error)
        return error;
    stream.ReadBytes(sizeof(MfTransform3), &mTarget);
    stream >> me4;
    if (!stream.Status()) {
        // the start transform as the constructor computes it, from the skill's first frame
        MfTransform3 *saved = mActor->skeletonTraversal()->CopyTransforms();
        MfManipSkill *skill = (MfManipSkill *)mSkill;
        skill->sample(skill->firstTime(), mActor->skeletonTraversal(), 0);
        MfTransform3 root(mActor->skeleton()->transform());
        MfSkeletonNode *handle = (MfSkeletonNode *)skill->GetHandle();
        handle->changeParent((MfSkeletonNode *)mActor->skeleton()->parent(), 0);
        handle->gotoHomeTransform();
        MfTransform3 grip = ~handle->globalTransform() * mActor->skeleton()->globalTransform();
        MfTransform3 parent(((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform());
        MfTransform3 start = ~parent * mTarget * grip;
        mStart = start * ~root;
        mActor->skeletonTraversal()->SetTransforms(saved);
        handle->changeParent(0, 0);
        if (saved)
            delete [] saved;
    }
    return stream.Status();
}

// 0x1001ab30 ?PrepareForExecution@MfAttachActivity@@UAEXXZ
void MfAttachActivity::PrepareForExecution()
{
    MfActor *parent = ((MfSkeletonNode *)mActor->skeleton()->parent())->mOwner;
    if (parent)
        mActor->mSpace->ReorderActorUpdate(mActor, parent);
}

// 0x1001ab70 ?Type@MfAttachActivity@@WBE@AEHXZ
// 0x1001ab80 ?IsType@MfAttachActivity@@WBE@AEHH@Z
