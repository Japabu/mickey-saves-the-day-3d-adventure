#include "MfGestureActivity.h"
#include "MfStream.h"
#include "MfSkeletonTraversal.h"
#include "MfSkill.h"
#include "MfActorPath.h"
#include "MfActor.h"
#include "MfAiActorNode.h"
#include "MfAiColNode.h"
#include "MfColDistResults.h"
#include "MfColModel.h"
#include "MfColNode.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfLiveActor.h"
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfMsgDispatcher.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfWorld.h"
#include <math.h>

long const MfGestureActivity::STREAM_TYPE = 'GesA';

// Inline members this object emits (the class is exported):
// 0x100066f0 ?Type@MfGestureActivity@@UAEHXZ
// 0x10006700 ?IsType@MfGestureActivity@@UAEHH@Z
// 0x10006740 ?WillFinish@MfGestureActivity@@UAEHXZ
// 0x10006750 ?PrintName@MfGestureActivity@@UBE?AVMfString@@XZ
// 0x1000aa50 ?Type@MfGestureActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000aa60 ?IsType@MfGestureActivity@@WBE@AEHH@Z

// 0x10006780 ??0MfGestureActivity@@QAE@ABV0@@Z
// The copy constructor and the assignment are the memberwise ones (the PDB
// declares them, so they are written out; they come first in the object).
MfGestureActivity::MfGestureActivity(const MfGestureActivity &a)
    : MfAtomicActivity(a), mRepeat(a.mRepeat), mTrace(a.mTrace), mcc(a.mcc), md0(a.md0), md4(a.md4),
      md8(a.md8), mdc(a.mdc)
{
}
// 0x100069a0 ??0MfRootObj@@QAE@ABV0@@Z (implicit)
// 0x100069d0 ??0MfAiActivity@@QAE@ABV0@@Z (implicit)

// 0x10006a00 ??4MfGestureActivity@@QAEAAV0@ABV0@@Z
MfGestureActivity &MfGestureActivity::operator=(const MfGestureActivity &a)
{
    MfAtomicActivity::operator=(a);
    mRepeat = a.mRepeat;
    mTrace = a.mTrace;
    mcc = a.mcc;
    md0 = a.md0;
    md4 = a.md4;
    md8 = a.md8;
    mdc = a.mdc;
    return *this;
}

// 0x10021df0 ??0MfGestureActivity@@QAE@XZ
MfGestureActivity::MfGestureActivity()
    : mRepeat(0)
{
    md0 = 0;
    md4 = 0;
}

// TODO 0x10021e60 ??0MfGestureActivity@@QAE@PAVMfActor@@KPAVMfSkill@@HH@Z: same calls and imports; the original keeps skill in ebx and reloads options from the stack, ours keeps options in ebx; further down the blocks after the bubble-model setup come in another order, not yet checked (76% similar)
MfGestureActivity::MfGestureActivity(MfActor *actor, unsigned long startTime, MfSkill *skill, int cc, int options)
    : MfAtomicActivity(actor, startTime, skill)
{
    mcc = cc;
    if (options & 1)
        mRepeat = -1;
    else
        mRepeat = 1;
    if (options & 2)
        mPlayDirection = 1;
    else
        mPlayDirection = 0;
    if (options & 0x8000)
        _InitBubbleModel(skill, actor, 1);
    else if (options & 4)
        _InitBubbleModel(skill, actor, 0);
    else
        mBubbleModel = 0;
    if (options & 0x1000)
        md4 = 1;
    else
        md4 = 0;
    m98 = 0;

    // the trace: where the skill moves the actor's root
    MfTransform3 home = mActor->skeleton()->transform();
    if (mcc) {
        MfTransform3 hip = home * mActor->skeletonTraversal()->node(1)->transform();
        home.translation() = hip.translation();
    }
    MfTransform3 *saved = mActor->skeletonTraversal()->CopyTransforms();
    MfTransform3 start;
    int fromHip = options & 0x4000;
    if (fromHip) {
        start = mActor->skeletonTraversal()->node(1)->globalTransform();
        md0 = 1;
    } else {
        md0 = 0;
    }
    skill->sample(mPlayDirection == 1 ? skill->lastTime() : skill->firstTime(), mActor->skeletonTraversal(), 0);
    MfTransform3 end;
    if (fromHip) {
        end = mActor->skeletonTraversal()->node(1)->globalTransform();
        end = ~end;
        mTrace = start * end;
    } else {
        if (!skill->sampledPath()->Track(0))
            end = mActor->skeleton()->homeTransform();
        else
            end = mActor->skeleton()->transform();
        if (mcc) {
            MfTransform3 hip = end * mActor->skeletonTraversal()->node(1)->transform();
            end.translation() = hip.translation();
        }
        end = ~end;
        mTrace = home * end;
    }
    mActor->skeletonTraversal()->SetTransforms(saved);
    if (saved)
        delete [] saved;
}

// 0x10022320 ??1MfGestureActivity@@UAE@XZ
MfGestureActivity::~MfGestureActivity()
{
}

// EQUIV 0x10022340 ?Update@MfGestureActivity@@UAE?AW4MfActivityStatus@@K@Z: reviewed: register choice: the original keeps time in ebp, the zero in edi and the repeat count (+0x54) in ebx where ours reloads the count from the object; the cast to the actor base is lea ecx, [eax - 0x14] where ours adds to eax and copies; same calls, arguments, stores and branches
// Samples the skill at the time (repeating it), moves the actor's root
// along the trace and checks the bubble model.
MfActivityStatus MfGestureActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    if (mFinished)
        return (MfActivityStatus)1;
    unsigned long elapsed = time - mStartTime;
    float rate = 0;
    if (mDuration > 0)
        rate = (float)mSkill->GetDuration() / mDuration;
    unsigned long sample = (unsigned long)(elapsed * rate);
    int cycles = sample / (mLastTime + 1);
    sample = sample % (mLastTime + 1);
    if (!mDuration || mRepeat != -1 && cycles >= mRepeat) {
        mFinished = 1;
        sample = mLastTime;
    }
    if (mPlayDirection == 1)
        sample = mLastTime - sample;
    if (mFinished) {
        sample = mPlayDirection == 1 ? mFirstTime : mLastTime;
        time = mLastTime + mStartTime;
    }
    if (mIgnoreMasks)
        mSkill->sample(sample, mActor->skeletonTraversal(), mIgnoreMasks);
    else
        mSkill->sample(sample, mActor->skeletonTraversal(), 0);
    if (!mCompound) {
        MfTransform3 xform;
        if (md0) {
            xform = mActor->skeletonTraversal()->node(1)->globalTransform();
            xform = mTrace * xform;
            xform = xform * ~mActor->skeletonTraversal()->node(1)->transform();
            MfTransform3 parent = ((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform();
            xform = ~parent * xform;
            mActor->skeleton()->setTransform(xform);
        } else {
            if (mSkill->sampledPath()->Track(0) && (!mIgnoreMasks || !mIgnoreMasks[0]))
                xform = mActor->skeleton()->transform();
            else
                xform = mActor->skeleton()->homeTransform();
            if ((!mIgnoreMasks || !mIgnoreMasks[0]) && mSkill->sampledPath()->Track(0)) {
                xform = mTrace * xform;
                mActor->skeleton()->setTransform(xform);
            }
        }
    }
    blendFromInitialConfig(time, 0);
    md8 = time;
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
    updateExtraTracks(mLastSampleTime, sample);
    mLastSampleTime = sample;
    mdc = md8;
    if (mFinished)
        _FixTrace();
    if (mRepeat == -1)
        mFinished = 0;
    return MfActivity::Update(time);
}

// (0x100228e0 ?_FixTrace@MfGestureActivity@@IAEXXZ: an empty function, folded with
// MfLiveActor::LoadActorMethods and claimed there)
void MfGestureActivity::_FixTrace()
{
}

// TODO 0x100228f0 ?_CheckBubble@MfGestureActivity@@MAEXAAK@Z: first body: a ray check along each
// bubble's step, then a sphere check; on a hit (with md4) four bisection steps between the last
// free sample and this one; the call sequence follows the original, locals and the message
// arguments are not settled (47% similar)
// Moves the bubbles to the sampled configuration; a bubble that meets the
// space's environment goes back to the last free time and reports the
// collision to both actors.
void MfGestureActivity::_CheckBubble(unsigned long &sample)
{
    m98 = (int)mActor->mSpace->mSnapshotEnv;
    MfVector3 center;
    MfVector3 step;
    MfVector3 hit;
    int eventID = g_world->CreateUniqueID();
    int collided = 0;
    MfColResultRay ray;
    MfColResultSphere sphere;
    MfDblList filter;
    mActor->UseFilter(1, 2, 5, filter);
    for (int i = 0; i < mBubbleModel->mCount; i++) {
        center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
        unsigned int style = ~mBubbleModel->mNodes[i]->ColNode().GetCheckStyle();
        int pass;
        int found = 0;
        for (pass = 0; pass < 2; pass++) {
            if ((style & 4) || pass == 1 && (style & 2)) {
                mBubbleModel->mGlobalCenters[i] = center;
                continue;
            }
            found = 0;
            if (pass == 0) {
                step = center - mBubbleModel->mGlobalCenters[i];
                float length = step.length();
                if (length < 0.0001f)
                    step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                float norm = (float)sqrt(step.lengthSquared());
                step.mX /= norm;
                step.mY /= norm;
                step.mZ /= norm;
                length += mBubbleModel->mRadii[i];
                if (m98 && ((MfAiColNode *)m98)->CheckCollision(mBubbleModel->mGlobalCenters[i], step, ray, true,
                                                                  length))
                    found = 1;
            } else {
                found = ((MfAiColNode *)m98)->CheckCollision(center, mBubbleModel->mRadii[i], sphere);
            }
            if (found)
                break;
        }
        if (found) {
            m90 = 1;
            MfVector3 normal;
            MfAiColNode *other;
            if (pass == 0) {
                hit = ray.pos;
                other = ray.node;
                normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(ray.triIndex);
            } else {
                hit = sphere.pos;
                other = sphere.node;
                normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(sphere.triIndex);
            }
            MfVector3 last = mBubbleModel->mGlobalCenters[i];
            if (md4) {
                int low = mLastSampleTime;
                int high = sample;
                int lowBlend = mdc;
                int highBlend = md8;
                for (int k = 4; k; k--) {
                    int midBlend = (highBlend + lowBlend) / 2;
                    int diff = high - low;
                    int mid;
                    if (mPlayDirection == 1) {
                        if (diff > 0)
                            diff -= mLastTime;
                        mid = diff / 2 + low;
                        if (mid < 0)
                            mid += mLastTime;
                    } else {
                        if (diff < 0)
                            diff += mLastTime;
                        mid = diff / 2 + low;
                        if (mid > (int)mLastTime)
                            mid -= mLastTime;
                    }
                    if (mIgnoreMasks)
                        mSkill->sample(mid, mActor->skeletonTraversal(), mIgnoreMasks);
                    else
                        mSkill->sample(mid, mActor->skeletonTraversal(), 0);
                    if (!mCompound) {
                        MfTransform3 xform = mTrace * mActor->skeleton()->transform();
                        ((MfAiActorNode *)mActor->skeleton())->SetTransform(xform);
                    }
                    blendFromInitialConfig(midBlend, 0);
                    center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
                    step = center - mBubbleModel->mGlobalCenters[i];
                    float length = step.length();
                    if (length < 0.0001f)
                        step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                    float norm = (float)sqrt(step.lengthSquared());
                    step.mX /= norm;
                    step.mY /= norm;
                    step.mZ /= norm;
                    length += mBubbleModel->mRadii[i];
                    if (((MfAiColNode *)m98)->CheckCollision(mBubbleModel->mGlobalCenters[i], step, ray, true, length) ||
                        ((MfAiColNode *)m98)->CheckCollision(center, mBubbleModel->mRadii[i], sphere)) {
                        high = mid;
                        highBlend = midBlend;
                    } else {
                        low = mid;
                        lowBlend = midBlend;
                    }
                }
                if (mIgnoreMasks)
                    mSkill->sample(low, mActor->skeletonTraversal(), mIgnoreMasks);
                else
                    mSkill->sample(low, mActor->skeletonTraversal(), 0);
                if (!mCompound) {
                    MfTransform3 xform = mTrace * mActor->skeleton()->transform();
                    mActor->skeleton()->setTransform(xform);
                    mActor->skeleton()->syncChildTransforms(0);
                }
                blendFromInitialConfig(lowBlend, 0);
                md8 = lowBlend;
                center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
                sample = low;
            }
            MfSkeletonNode *otherNode = other->Owner() ? (MfSkeletonNode *)other->Owner() : 0;
            MfLiveActor *otherActor = (MfLiveActor *)otherNode->mOwner;
            normal = otherNode->globalTransform().rotation() * normal;
            normal.length();
            MfVector3 direction;
            MfVector3 point = hit;
            direction = center - last;
            collided = 1;
            MfMsg *msg = new MfMsg(35, 0, 0, mActor, mActor);
            msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, mBubbleModel->mNodes[i], otherNode,
                                                mSkill, (float)sample / mLastTime, point, direction, normal,
                                                eventID));
            g_msgDispatcher->Post(msg);
            msg = new MfMsg(36, 0, 0, mActor, otherActor);
            msg->SetData(new MfCollisionMsgData(otherActor, (MfLiveActor *)mActor, otherNode, mBubbleModel->mNodes[i],
                                                mSkill, (float)sample / mLastTime, point, direction, normal,
                                                eventID));
            g_msgDispatcher->Post(msg);
        }
        mBubbleModel->mGlobalCenters[i] = center;
    }
    if (collided)
        g_msgDispatcher->Post(new MfMsg(37, 0, 0, mActor, mActor));
    mActor->UseFilter(0, 2, 5, filter);
}

// 0x100235a0 ?IgnoreSubtree@MfGestureActivity@@UAEHABVMfString@@@Z
int MfGestureActivity::IgnoreSubtree(const MfString &name)
{
    int ignored = MfAtomicActivity::IgnoreSubtree(name);
    if (ignored)
        _FixXform();
    return ignored;
}

// 0x100235d0 ?AnimateSubtree@MfGestureActivity@@UAEHABVMfString@@@Z
int MfGestureActivity::AnimateSubtree(const MfString &name)
{
    int animated = MfAtomicActivity::AnimateSubtree(name);
    _FixXform();
    return animated;
}

// 0x100235f0 ?_FixXform@MfGestureActivity@@IAEXXZ
// Recomputes the trace after the animated subtree changed (as the
// constructor does).
void MfGestureActivity::_FixXform()
{
    MfTransform3 *saved = mActor->skeletonTraversal()->CopyTransforms();
    MfTransform3 home = mActor->skeleton()->transform();
    if (mcc) {
        MfTransform3 hip = home * mActor->skeletonTraversal()->node(1)->transform();
        home.translation() = hip.translation();
    }
    MfTransform3 start;
    if (md0)
        start = mActor->skeletonTraversal()->node(1)->globalTransform();
    unsigned long time = mPlayDirection == 1 ? mSkill->lastTime() : mSkill->firstTime();
    if (mIgnoreMasks)
        mSkill->sample(time, mActor->skeletonTraversal(), mIgnoreMasks);
    else
        mSkill->sample(time, mActor->skeletonTraversal(), 0);
    MfTransform3 end;
    if (md0) {
        end = mActor->skeletonTraversal()->node(1)->globalTransform();
        end = ~end;
        mTrace = start * end;
    } else {
        if (!mSkill->sampledPath()->Track(0))
            end = mActor->skeleton()->homeTransform();
        else
            end = mActor->skeleton()->transform();
        if (mcc) {
            MfTransform3 hip = end * mActor->skeletonTraversal()->node(1)->transform();
            end.translation() = hip.translation();
        }
        end = ~end;
        mTrace = home * end;
    }
    mActor->skeletonTraversal()->SetTransforms(saved);
    if (saved)
        delete [] saved;
}

// 0x10023960 ?CreateGestureActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateGestureActivity()
{
    return new MfGestureActivity;
}

// 0x100239d0 _$E46 (original $E: the static initializer of the constructor entry, with 100239e0)
// 0x100239e0 _$E45
static MfStreamConstructorEntry constructorEntry(MfGestureActivity::STREAM_TYPE, CreateGestureActivity);

// 0x10023a00 ?StreamType@MfGestureActivity@@UBEJXZ
long MfGestureActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10023a10 ?WriteToStream@MfGestureActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfGestureActivity::WriteToStream(MfStream &stream) const
{
    MfAtomicActivity::WriteToStream(stream);
    stream << mRepeat;
    stream.WriteBytes(sizeof(MfTransform3), &mTrace);
    stream << mcc << md4 << md0;
    return stream.Status();
}

// 0x10023a70 ?ReadFromStream@MfGestureActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfGestureActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfAtomicActivity::ReadFromStream(stream);
    if (error)
        return error;
    stream >> mRepeat;
    stream.ReadBytes(sizeof(MfTransform3), &mTrace);
    stream >> mcc >> md4 >> md0;
    return stream.Status();
}

// 0x10006be0 ??_EMfGestureActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)
