// Mf3dPathActivity: plays a skill while moving the actor along a 3D path
// (RuntimeLib.dll 1000ab50-1000d4b0).
#include "Mf3dPathActivity.h"
#include "MfActor.h"
#include "MfActorPath.h"
#include "MfLocoSkill.h"
#include "MfPath3.h"
#include "MfSplineNode.h"
#include "MfStream.h"
#include "MfAiActorNode.h"
#include "MfAiColNode.h"
#include "MfColDistResults.h"
#include "MfColModel.h"
#include "MfColNode.h"
#include "MfColResults.h"
#include "MfLiveActor.h"
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfMsgDispatcher.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfWorld.h"
#include <math.h>

// a 3D path in a stream (defined with the first activities, 10009e70/1000a0a0)
MfStreamError WritePath3(MfStream &stream, MfPath3 *path);
MfStreamError ReadPath3(MfStream &stream, MfPath3 *&path, MfActor *actor);

long const Mf3dPathActivity::STREAM_TYPE = '3dpA';

// the implicit copy members of the exported class
static Mf3dPathActivity &(Mf3dPathActivity::*s_assign)(const Mf3dPathActivity &) = &Mf3dPathActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10006ca0 ?HasMotion@Mf3dPathActivity@@UAEHXZ
// 0x10008850 ?Type@Mf3dPathActivity@@UAEHXZ
// 0x10008860 ?IsType@Mf3dPathActivity@@UAEHH@Z
// 0x100088a0 ??0Mf3dPathActivity@@QAE@ABV0@@Z
// 0x10008af0 ??4Mf3dPathActivity@@QAEAAV0@ABV0@@Z
// 0x1000aad0 ?Type@Mf3dPathActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000aae0 ?IsType@Mf3dPathActivity@@WBE@AEHH@Z

// 0x1000ab50 ??0Mf3dPathActivity@@QAE@XZ
Mf3dPathActivity::Mf3dPathActivity()
{
    mPathPos = -1.0f;
    mFirstUpdate = 1;
    mMatchGait = 1;
    mPath = 0;
    mb4 = 0;
    mBackwards = 0;
    mbc = 0;
    mc4 = 0;
    mc8 = 0;
}

// 0x1000abe0 ??0Mf3dPathActivity@@QAE@PAVMfActor@@KPAVMfSkill@@PAVMfPath3@@HH@Z
Mf3dPathActivity::Mf3dPathActivity(MfActor *actor, unsigned long startTime, MfSkill *skill, MfPath3 *path,
                                   int options, int c4)
    : MfAtomicActivity(actor, startTime, skill)
{
    mPath = path;
    // a path along a spline keeps the spline's actor
    if (mPath->mKind == 4 && mPath->mSpline)
        mPath->mSpline->GetOwner()->AddRef();
    mPathPos = -1.0f;
    if (options & 0x1000)
        mb4 = 1;
    else
        mb4 = 0;
    if (options & 1)
        mbc = 1;
    else
        mbc = 0;
    if (options & 2) {
        mBackwards = 1;
        mPlayDirection = 1;
    } else {
        mBackwards = 0;
        mPlayDirection = 0;
    }
    if (options & 0x10000)
        mc0 = 1;
    else
        mc0 = 0;
    if (options & 0x20000)
        mMatchGait = 0;
    else
        mMatchGait = 1;
    mc8 = 0;
    mc4 = c4;
    mFirstUpdate = 1;
    if (options & 0x8000)
        _InitBubbleModel(skill, actor, 1);
    else if (options & 4)
        _InitBubbleModel(skill, actor, 0);
    else
        mBubbleModel = 0;
    if (skill->Type() == MfLocoSkill::TYPE && mMatchGait)
        mLastSampleTime = ((MfLocoSkill *)skill)->BestMatchIndex(actor);
    else
        mLastSampleTime = 0;
}

// 0x1000ad90 ??1Mf3dPathActivity@@UAE@XZ
Mf3dPathActivity::~Mf3dPathActivity()
{
    if (mPath && mPath->mKind == 4 && mPath->mSpline)
        mPath->mSpline->GetOwner()->Release();
    if (mPath)
        delete mPath;
}

// 0x1000ae30 ?Reset@Mf3dPathActivity@@UAEXK@Z
void Mf3dPathActivity::Reset(unsigned long time)
{
    MfActivity::Reset(time);
    if (mSkill->Type() == MfLocoSkill::TYPE && mMatchGait)
        mLastSampleTime = ((MfLocoSkill *)mSkill)->BestMatchIndex(mActor);
    else
        mLastSampleTime = 0;
    mPathPos = -1.0f;
}

// |x| as the original writes it (a compare and fchs, not the fabs intrinsic)
static inline float Abs(float x)
{
    return x < 0.0f ? -x : x;
}

// TODO 0x1000ae90 ?Update@Mf3dPathActivity@@UAE?AW4MfActivityStatus@@K@Z: (81% similar) calls, branches and
// constants follow the original; as in MfLocoActivity::Update the original calls the imported
// MfVector3/MfRotation3 constructors and operator= where this build expands them inline, which
// shifts the frame (60% similar)
// Advances the skill and moves the actor along the path: the distance the
// skill's root moves forward in this step is walked along the path keys (or,
// for a move and for unlooped key and spline paths, the path is followed by
// time), then the skill is sampled on top of the path transform.
MfActivityStatus Mf3dPathActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    if (mFinished)
        return (MfActivityStatus)1;

    unsigned long lastSample = mLastSampleTime;
    MfTransform3 from;
    MfTransform3 to;
    MfTransform3 placement;
    MfTransform3 end;
    MfTransform3 start;
    MfTransform3 point;
    if (mFirstUpdate) {
        mLastTransform = mActor->skeleton()->transform();
        mFirstUpdate = 0;
        if (mBackwards)
            mLastSampleTime = mLastTime;
        mc8 = 0;
    }
    MfJointPath *root = mSkill->sampledPath()->Track(0);
    if (!root && mPath->mKind != MfPath3::MOVE)
        return (MfActivityStatus)1;

    MfSkeletonTraversal *nodes = mActor->skeletonTraversal();
    unsigned long elapsed = time - MfActivity::mLastTime;
    float rate = 0.0f;
    if (mDuration > 0)
        rate = (float)mSkill->GetDuration() / mDuration;
    unsigned long step = (unsigned long)(elapsed * rate);

    float distance;
    unsigned long sample;
    int skillDriven = 1;
    if (mPath->mKind == MfPath3::MOVE || mPath->mKind == MfPath3::SPLINE_KEYS && !mPath->mLooped ||
        mPath->mKind == MfPath3::SPLINE && !mPath->mLooped || mPath->mKind == MfPath3::KEYS && !mPath->mLooped)
        skillDriven = 0;
    if (skillDriven) {
        // the distance the skill's root moves forward in this step
        if (mBackwards) {
            long next = mLastSampleTime - step;
            if (next < 0) {
                mc8++;
                if (mc8 >= mc4 && mc0) {
                    next = 0;
                    from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                    to = root->Sample(next, mSkill->GetInterpolate());
                    distance = Abs(to.z() - from.z());
                } else {
                    from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                    to = root->Sample(0, mSkill->GetInterpolate());
                    distance = Abs(to.z() - from.z());
                    int cycles = (unsigned long)-next / mLastTime;
                    from = root->Sample(mLastTime, mSkill->GetInterpolate());
                    float cycleLength = Abs(to.z() - from.z());
                    mLastSampleTime = mLastTime;
                    distance += cycles * cycleLength;
                    next += (cycles + 1) * mLastTime;
                    from = root->Sample(mLastTime, mSkill->GetInterpolate());
                    to = root->Sample(next, mSkill->GetInterpolate());
                    distance += Abs(to.z() - from.z());
                }
            } else {
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(next, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
            }
            sample = next;
        } else {
            sample = mLastSampleTime + step;
            if (sample > mLastTime) {
                mc8++;
                if (mc8 >= mc4 && mc0) {
                    sample = mLastTime;
                    from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                    to = root->Sample(sample, mSkill->GetInterpolate());
                    distance = Abs(to.z() - from.z());
                } else {
                    from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                    to = root->Sample(mLastTime, mSkill->GetInterpolate());
                    distance = Abs(to.z() - from.z());
                    int cycles = (sample - mLastTime) / mLastTime;
                    from = root->Sample(mFirstTime, mSkill->GetInterpolate());
                    to = root->Sample(mLastTime, mSkill->GetInterpolate());
                    float cycleLength = Abs(to.z() - from.z());
                    distance += cycles * cycleLength;
                    sample -= sample / mLastTime * mLastTime;
                    from = root->Sample(mFirstTime, mSkill->GetInterpolate());
                    to = root->Sample(sample, mSkill->GetInterpolate());
                    distance += Abs(to.z() - from.z());
                }
            } else {
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(sample, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
            }
        }

        // walk that distance along the path keys
        float last = mPathPos;
        start = mPath->Transform(mPathPos);
        float i = (int)mPathPos + 1.0f;
        float walked = 0.0f;
        while (i < mPath->NumKeys()) {
            point = mPath->Transform(i);
            float length = (point.translation() - start.translation()).length();
            if (walked + length > distance) {
                distance = last + (distance - walked) * (i - last) / length;
                placement = mPath->Transform(distance);
                break;
            }
            walked += length;
            start = point;
            last = i;
            i += 1.0f;
        }
        if (i > mPath->NumKeys() - 1) {
            placement = mPath->Transform(mPath->NumKeys() - 1);
            distance = i;
            mFinished = 1;
        }
        if (mc8 >= mc4 && mc0)
            mFinished = 1;
    } else {
        // follow the path by time
        if (mBackwards) {
            long next = mLastSampleTime - step;
            if (next < 0 && mbc)
                next += ((unsigned long)-next / mLastTime + 1) * mLastTime;
            else if (next < 0)
                next = 0;
            sample = next;
        } else {
            sample = mLastSampleTime + step;
            if (sample > mLastTime && mbc)
                sample -= sample / mLastTime * mLastTime;
        }
        if (mPath->mKind == MfPath3::KEYS)
            distance = (float)(time - mStartTime) / mPath->mTotalTime;
        else
            distance = (time - mStartTime) * 0.030303031f;
        if (distance > mPath->NumKeys() - 1) {
            end = mPath->Transform(mPath->NumKeys() - 1);
            placement = end;
            mFinished = 1;
        } else {
            end = mPath->Transform(distance);
            placement = end;
        }
    }

    if (mPath->mKind == MfPath3::MOVE) {
        if (mIgnoreMasks)
            mSkill->sample(sample, nodes, mIgnoreMasks);
        else
            mSkill->sample(sample, nodes, 0);
        MfRotation3 turn = mActor->skeleton()->transform().rotation();
        placement.rotation() = placement.rotation() * turn;
    } else {
        if (mPath->mKind == MfPath3::SPLINE_KEYS || mPath->mKind == MfPath3::SPLINE)
            placement = ~((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform() * placement;
        if (mIgnoreMasks)
            mSkill->sample(sample, nodes, mIgnoreMasks);
        else
            mSkill->sample(sample, nodes, 0);
        MfRotation3 turn = mActor->skeleton()->transform().rotation();
        if (mBackwards && skillDriven && mPath->mKind != MfPath3::ARC)
            turn = MfRotation3(3.14159265f, MfVector3(0.0f, 1.0f, 0.0f)) * turn;
        placement.rotation() = placement.rotation() * turn;
    }
    mActor->skeleton()->setTransform(placement);
    updateExtraTracks(lastSample, sample);
    blendFromInitialConfig(time, 0);
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

    mLastSampleTime = sample;
    mPathPos = distance;
    mLastTransform = mActor->skeleton()->transform();
    MfActivity::mLastTime = time;
    return MfActivity::Update(time);
}

// TODO 0x1000c1c0 ?_CheckBubble@Mf3dPathActivity@@MAEXAAK@Z: (78% similar) first body: a ray check from behind
// each bubble along its step and a sphere check; with mb4 a hit pushes the root out along the
// surface normal, and afterwards three halvings between the pushed and the last root transform;
// the call sequence follows the original, locals and the message arguments are not settled
// (47% similar)
// Moves the bubbles along the path; collisions with the space's environment
// are reported to both actors (and with mb4 the actor is kept clear).
void Mf3dPathActivity::_CheckBubble(unsigned long &sample)
{
    m98 = (int)mActor->mSpace->mSnapshotEnv;
    MfAiColNode *env = (MfAiColNode *)m98;
    MfVector3 center;
    MfVector3 step;
    MfVector3 point;
    MfTransform3 corrected;
    MfTransform3 bubble;
    MfTransform3 pushed;
    MfColResultRay ray;
    MfColResultSphere sphere;
    int collided = 0;
    int eventID = g_world->CreateUniqueID();
    MfDblList filter;
    mActor->UseFilter(1, 2, 5, filter);
    int i;
    for (i = 0; i < mBubbleModel->mCount; i++) {
        center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
        int style = mBubbleModel->mNodes[i]->ColNode().GetCheckStyle();
        for (int pass = 0; pass < 2; pass++) {
            if (pass == 0 && (~style & 4) || pass == 1 && (~style & 2)) {
                mBubbleModel->mGlobalCenters[i] = center;
                continue;
            }
            int found;
            if (pass == 0) {
                step = center - mBubbleModel->mGlobalCenters[i];
                float length = step.length();
                if (length < 1.0e-6f) {
                    step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                    if (mBackwards) {
                        step = -step;
                        length -= mBubbleModel->mRadii[i] * -2.0f;
                    }
                }
                float norm = (float)sqrt(step.lengthSquared());
                step.mX /= norm;
                step.mY /= norm;
                step.mZ /= norm;
                MfVector3 start = mBubbleModel->mGlobalCenters[i] - step * mBubbleModel->mRadii[i];
                found = env->CheckCollision(start, step, ray, false, length);
            } else {
                found = env->CheckCollision(center, mBubbleModel->mRadii[i], sphere);
            }
            if (!found)
                continue;
            MfVector3 normal;
            MfAiColNode *other;
            if (pass == 0) {
                point = ray.pos;
                other = ray.node;
                normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(ray.triIndex);
            } else {
                point = sphere.pos;
                other = sphere.node;
                normal = other->ColModel()->GetTriangleSet()->FindTriangleNormal(sphere.triIndex);
            }
            m90 = 1;
            MfSkeletonNode *otherNode = other->Owner() ? (MfSkeletonNode *)other->Owner() : 0;
            MfLiveActor *otherActor = (MfLiveActor *)otherNode->mOwner;
            normal = otherNode->globalTransform().rotation() * normal;
            normal.length();
            if (pass == 0 && mb4) {
                mActor->skeletonTraversal()->nodeIndex(mBubbleModel->mNodes[i]);
                bubble.rotation() = mBubbleModel->mNodes[i]->globalTransform().rotation();
                bubble.translation() = center;
                pushed.rotation() = mBubbleModel->mNodes[i]->globalTransform().rotation();
                pushed.translation() = mBubbleModel->mNodes[i]->globalTransform().translation();
                pushed.translation() = point + normal * mBubbleModel->mRadii[i];
                corrected = pushed * ~bubble * mActor->skeleton()->globalTransform();
                MfTransform3 parent = ((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform();
                corrected = ~parent * corrected;
                mActor->skeleton()->setTransform(corrected);
                mActor->skeleton()->syncChildTransforms(0);
                collided = 1;
            }
            MfVector3 direction;
            MfVector3 at = point;
            direction = center - mBubbleModel->mGlobalCenters[i];
            MfMsg *msg = new MfMsg(35, 0, 0, mActor, mActor);
            msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, mBubbleModel->mNodes[i], otherNode,
                                                mSkill, (float)sample / mLastTime, at, direction, normal, eventID));
            g_msgDispatcher->Post(msg);
            msg = new MfMsg(36, 0, 0, mActor, otherActor);
            msg->SetData(new MfCollisionMsgData(otherActor, (MfLiveActor *)mActor, otherNode, mBubbleModel->mNodes[i],
                                                mSkill, (float)sample / mLastTime, at, direction, normal, eventID));
            g_msgDispatcher->Post(msg);
        }
        mBubbleModel->mGlobalCenters[i] = center;
    }
    if (collided) {
        if (mb4) {
            MfTransform3 current = mActor->skeleton()->transform();
            MfTransform3 last = mLastTransform;
            ((MfAiActorNode *)mActor->skeleton())->SetTransform(last);
            for (i = 0; i < mBubbleModel->mCount; i++)
                mBubbleModel->mGlobalCenters[i] = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
            for (int k = 3; k; k--) {
                corrected.interpolate(current, last, 0.5f);
                ((MfAiActorNode *)mActor->skeleton())->SetTransform(corrected);
                for (i = 0; i < mBubbleModel->mCount; i++) {
                    int style = ~mBubbleModel->mNodes[i]->ColNode().GetCheckStyle();
                    if ((style & 2) && (style & 4))
                        continue;
                    center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
                    step = center - mBubbleModel->mGlobalCenters[i];
                    float length = step.length();
                    if (length < 1.0e-6f) {
                        step = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                        if (mBackwards) {
                            step = -step;
                            length -= mBubbleModel->mRadii[i] * -2.0f;
                        }
                    }
                    float norm = (float)sqrt(step.mY * step.mY + step.mZ * step.mZ + step.mX * step.mX);
                    step.mX /= norm;
                    step.mY /= norm;
                    step.mZ /= norm;
                    MfVector3 start = mBubbleModel->mGlobalCenters[i] - step * mBubbleModel->mRadii[i];
                    MfColResultSphere test;
                    if (env->CheckCollision(start, step, ray, false, length) ||
                        env->CheckCollision(center, mBubbleModel->mRadii[i], test))
                        break;
                }
                if (i == mBubbleModel->mCount)
                    last = corrected;
                else
                    current = corrected;
            }
            ((MfAiActorNode *)mActor->skeleton())->SetTransform(last);
            for (i = 0; i < mBubbleModel->mCount; i++)
                mBubbleModel->mGlobalCenters[i] = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
        }
        if (collided)
            g_msgDispatcher->Post(new MfMsg(37, 0, 0, mActor, mActor));
    }
    mActor->UseFilter(0, 2, 5, filter);
}

// 0x1000d2b0 ?Create3dPathActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *Create3dPathActivity()
{
    return new Mf3dPathActivity;
}

// 0x1000d320 _$E50 (original $E: the static initializer of the constructor entry, with 1000d330)
// 0x1000d330 _$E49
static MfStreamConstructorEntry constructorEntry(Mf3dPathActivity::STREAM_TYPE, Create3dPathActivity);

// 0x1000d350 ?StreamType@Mf3dPathActivity@@UBEJXZ
long Mf3dPathActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000d360 ?WriteToStream@Mf3dPathActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError Mf3dPathActivity::WriteToStream(MfStream &stream) const
{
    MfAtomicActivity::WriteToStream(stream);
    WritePath3(stream, mPath);
    stream << mb4 << mBackwards << mc0 << mc4 << mMatchGait;
    return stream.Status();
}

// 0x1000d3d0 ?ReadFromStream@Mf3dPathActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError Mf3dPathActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfAtomicActivity::ReadFromStream(stream);
    if (error)
        return error;
    ReadPath3(stream, mPath, mActor);
    if (mPath && mPath->mKind == 4 && mPath->mSpline)
        mPath->mSpline->GetOwner()->AddRef();
    stream >> mb4 >> mBackwards >> mc0 >> mc4 >> mMatchGait;
    if (stream.Status() == MF_STREAM_OK) {
        if (mSkill->Type() == MfLocoSkill::TYPE && mMatchGait)
            mLastSampleTime = ((MfLocoSkill *)mSkill)->BestMatchIndex(mActor);
        else
            mLastSampleTime = 0;
    }
    return stream.Status();
}

// 0x10008cd0 ??_EMf3dPathActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (10006ca0); the port needs its own definition.
int Mf3dPathActivity::WillFinish()
{
    return 1;
}

// Not claimed: in the original this member is folded with an identical function
// (1000c180, MfLocoActivity::setSkill); the port needs its own definition.
void Mf3dPathActivity::setSkill(MfSkill *skill)
{
    MfAtomicActivity::setSkill(skill);
    if (skill->Type() == MfLocoSkill::TYPE)
        mLastSampleTime = ((MfLocoSkill *)skill)->BestMatchIndex(mActor);
}
