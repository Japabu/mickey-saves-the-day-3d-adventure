#include "MfLocoActivity.h"
#include <float.h>
#include <math.h>
#include "MfActor.h"
#include "MfActorPath.h"
#include "MfLocoSkill.h"
#include "MfPath2.h"
#include "MfSkeletonNode.h"
#include "MfSkeletonTraversal.h"
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
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfWorld.h"

// a path in a stream (defined with the first activities, 10009690/10009b30)
MfStreamError WritePath(MfStream &stream, MfPath2 *path);
MfStreamError ReadPath(MfStream &stream, MfPath2 *&path);
// a reference to a node of an actor in a stream (1000a6e0/1000a760)
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);

long const MfLocoActivity::STREAM_TYPE = 'LocA';

// the implicit copy members of the exported class
static MfLocoActivity &(MfLocoActivity::*s_assign)(const MfLocoActivity &) = &MfLocoActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10006c40 ?Type@MfLocoActivity@@UAEHXZ
// 0x10006c50 ?IsType@MfLocoActivity@@UAEHH@Z
// 0x10006c90 ?getPath@MfLocoActivity@@QBEPAVMfPath2@@XZ
// 0x10006ca0 ?HasMotion@MfLocoActivity@@UAEHXZ
// 0x10006cb0 ?PrintName@MfLocoActivity@@UBE?AVMfString@@XZ
// 0x10006ce0 ??0MfLocoActivity@@QAE@ABV0@@Z
// 0x10006f70 ??4MfLocoActivity@@QAEAAV0@ABV0@@Z
// 0x1000ab10 ?Type@MfLocoActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000ab20 ?IsType@MfLocoActivity@@WBE@AEHH@Z

// 0x1000c180 ?setSkill@MfLocoActivity@@UAEXPAVMfSkill@@@Z (folded with Mf3dPathActivity::setSkill)
void MfLocoActivity::setSkill(MfSkill *skill)
{
    MfAtomicActivity::setSkill(skill);
    if (skill->Type() == MfLocoSkill::TYPE)
        mLastSampleTime = ((MfLocoSkill *)skill)->BestMatchIndex(mActor);
}

// 0x1003e030 ??0MfLocoActivity@@QAE@XZ
MfLocoActivity::MfLocoActivity()
    : mMode(0), mCheckCollision(0), mWithChildren(0), mBackwards(0), mbc(0), mLimitCycles(0), mCycles(0),
      mPath(0), mPathPos(-1.0f), md0(0), md4(0), mNode(0), mFirstUpdate(1), mMatchGait(1), mCycleCount(0)
{
}

// EQUIV 0x1003e0f0 ??0MfLocoActivity@@QAE@PAVMfActor@@KPAVMfSkill@@PAVMfPath2@@HHHHMMPAVMfSkeletonNode@@HHHH@Z: reviewed: the stores of the arguments into the fields load them in another order through other registers; every field gets the same argument
MfLocoActivity::MfLocoActivity(MfActor *actor, unsigned long startTime, MfSkill *skill, MfPath2 *path,
                               int backwards, int checkCollision, int withChildren, int mode, float a5,
                               float a6, MfSkeletonNode *node, int bc, int limitCycles, int cycles,
                               int matchGait)
    : MfAtomicActivity(actor, startTime, skill)
{
    md0 = a5;
    md4 = a6;
    mCheckCollision = checkCollision;
    mPath = path;
    mMatchGait = matchGait;
    mWithChildren = withChildren;
    mMode = mode;
    mLimitCycles = limitCycles;
    mPathPos = -1.0f;
    mBackwards = backwards;
    mbc = bc;
    mFirstUpdate = 1;
    mCycles = cycles;
    mCycleCount = 0;
    if (bc)
        mCheckCollision = 1;
    if (backwards)
        mPlayDirection = 1;
    else
        mPlayDirection = 0;
    if (a5 < 0.0f)
        md0 = -a5;
    if (a6 < 0.0f)
        md4 = -a6;
    mNode = node;
    float radius, height;
    actor->GetCylinder(radius, height);
    if (mMode == 2) {
        md4 = md0 = radius * 3.0f;
    } else if (md0 <= 0.0001f) {
        md0 = radius * 0.5f;
    }
    if (withChildren)
        _InitBubbleModel(skill, actor, 1);
    else if (mCheckCollision == 0 && mMode == 0)
        mBubbleModel = 0;
    else
        _InitBubbleModel(skill, actor, 0);
    if (skill->Type() == MfLocoSkill::TYPE && mMatchGait)
        mLastSampleTime = ((MfLocoSkill *)skill)->BestMatchIndex(actor);
    else if (backwards)
        mLastSampleTime = mLastTime;
    else
        mLastSampleTime = 0;
    mPath->mParent = (MfSkeletonNode *)mActor->skeleton()->parent();
}

// 0x1003e350 ??1MfLocoActivity@@UAE@XZ
MfLocoActivity::~MfLocoActivity()
{
    if (mPath)
        delete mPath;
}

// 0x1003e3d0 ?Reset@MfLocoActivity@@UAEXK@Z
void MfLocoActivity::Reset(unsigned long time)
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

// TODO 0x1003e430 ?Update@MfLocoActivity@@UAE?AW4MfActivityStatus@@K@Z: control flow and calls agree; the original expands the mflinalg inlines (MfVector3/MfRotation3 constructors, operator=) only in the path part and calls the imports in the sampling part, which shifts the frame; the inline MfPath2 point count and key lookup test their flags the other way round (63% similar)
MfActivityStatus MfLocoActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    if (mFinished)
        return (MfActivityStatus)1;

    unsigned long lastSample = mLastSampleTime;
    MfTransform3 from;
    MfTransform3 to;
    if (mFirstUpdate) {
        mLastTransform = mActor->skeleton()->transform();
        mCycleCount = 0;
    }
    MfJointPath *root = mSkill->sampledPath()->Track(0);
    if (!root)
        return (MfActivityStatus)1;

    MfSkeletonTraversal *nodes = mActor->skeletonTraversal();
    unsigned long elapsed = time - MfActivity::mLastTime;
    float rate = 0.0f;
    if (mDuration > 0)
        rate = (float)mSkill->GetDuration() / mDuration;
    unsigned long step = (unsigned long)(elapsed * rate);

    // the distance the skill's root moves forward in this step
    float distance;
    unsigned long sample;
    if (mBackwards) {
        long next = mLastSampleTime - step;
        if (next <= 0) {
            mCycleCount++;
            if (mCycleCount >= mCycles && mLimitCycles) {
                next = 0;
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(next, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
            } else {
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(0, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
                unsigned long cycles = (unsigned long)-next / mLastTime;
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
        if (sample >= mLastTime) {
            mCycleCount++;
            if (mCycleCount >= mCycles && mLimitCycles) {
                sample = mLastTime;
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(sample, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
            } else {
                from = root->Sample(mLastSampleTime, mSkill->GetInterpolate());
                to = root->Sample(mLastTime, mSkill->GetInterpolate());
                distance = Abs(to.z() - from.z());
                unsigned long cycles = (sample - mLastTime) / mLastTime;
                from = root->Sample(mFirstTime, mSkill->GetInterpolate());
                float cycleLength = Abs(to.z() - from.z());
                distance += cycles * cycleLength;
                mLastSampleTime = 0;
                sample -= (sample / mLastTime) * mLastTime;
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

    // walk that distance along the path
    MfTransform2 start;
    MfTransform2 point;
    MfTransform2 end;
    float pos = mPathPos;
    start = mPath->PointAt(pos);
    int i = (int)mPathPos + 1;
    float walked = 0.0f;
    while (i < mPath->NumPoints()) {
        point = mPath->Point(i);
        float length = (point.translation() - start.translation()).length();
        if (walked + length > distance) {
            pos += (distance - walked) * (i - pos) / length;
            end = mPath->PointAt(pos);
            break;
        }
        walked += length;
        start = point;
        pos = (float)i;
        i++;
    }
    if (i >= mPath->NumPoints()) {
        end = mPath->Point(mPath->NumPoints() - 1);
        pos = (float)i;
        mFinished = 1;
    }
    if (mCycleCount >= mCycles && mLimitCycles)
        mFinished = 1;

    MfTransform3 placement;
    placement.x() = end.y();
    placement.z() = end.x();
    if (mBubbleModel)
        placement.y() = mActor->skeleton()->transform().translation().mY;
    else
        placement.y() = 0.0f;
    if (mBackwards)
        placement.rotation().setValue(end.rotation().angle() - -3.14159265f, MfVector3(0.0f, 1.0f, 0.0f));
    else
        placement.rotation().setValue(end.rotation().angle(), MfVector3(0.0f, 1.0f, 0.0f));

    // the skill's own turn about the vertical axis goes to the hip
    float angle;
    MfVector3 axis(0.0f, 0.0f, 0.0f);
    to.rotation().getValue(angle, axis);
    if (axis.mY < 0.0f)
        angle = -angle;
    MfRotation3 turn;
    turn.setValue(angle, MfVector3(0.0f, 1.0f, 0.0f));

    nodes->node(0)->setTransform(placement);
    if (mIgnoreMasks) {
        int saved = *mIgnoreMasks;
        *mIgnoreMasks = 1;
        mSkill->sample(sample, nodes, mIgnoreMasks);
        *mIgnoreMasks = saved;
    } else {
        mSkill->sample(sample, nodes, 1);
    }
    updateExtraTracks(lastSample, sample);

    MfTransform3 hip;
    hip = nodes->node(1)->transform();
    hip.rotation() = turn * hip.rotation();
    nodes->node(1)->setTransform(hip);
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
    mPathPos = pos;
    mLastTransform = mActor->skeleton()->transform();
    if (mFirstUpdate)
        mFirstUpdate = 0;
    return MfActivity::Update(time);
}

// 0x1003f3c0 ?_InitBubbleModel@MfLocoActivity@@MAEXPAVMfSkill@@PAVMfActor@@H@Z
void MfLocoActivity::_InitBubbleModel(MfSkill *skill, MfActor *actor, int withChildren)
{
    MfAtomicActivity::_InitBubbleModel(skill, actor, withChildren);
    if (mBubbleModel) {
        MfDblListItem *pos;
        MfDblList &legs = ((MfLocoSkill *)skill)->legList();
        for (MfLocoLeg *leg = (MfLocoLeg *)legs.GetFirst(pos); leg; leg = (MfLocoLeg *)legs.GetNext(pos)) {
            MfSkeletonNode *foot = actor->skeletonTraversal()->node(leg->mNode);
            for (int i = 0; i < mBubbleModel->mCount; i++) {
                if (foot == mBubbleModel->mNodes[i])
                    mBubbleModel->mHits[i] = 1;
            }
        }
    }
}

// TODO 0x1003f450 ?_CheckBubble@MfLocoActivity@@MAEXAAK@Z: the call sequence and branches
// follow the original (conform to the floor, the two bubble passes, the slide with mbc and the
// three upright halvings); the frame has the original size but the locals sit in other
// slots, and edi/ebx swap as the zero and import registers (27% similar)
// Conforms the feet to the floor, then moves the bubbles along the step; collisions
// with the space's environment are reported to both actors (and with mbc the actor
// slides along the surface and is kept upright and clear).
void MfLocoActivity::_CheckBubble(unsigned long &sample)
{
    m98 = (int)mActor->mSpace->mSnapshotEnv;
    if (!m98)
        return;
    MfVector3 center;
    int collided = 0;
    MfDblList filter;
    mActor->UseFilter(1, 2, 5, filter);
    int result;
    if (mMode)
        result = _ConformToFloor(sample);
    if (result == 2) {
        MfMsg *msg = new MfMsg(0x39, 0, 0, mActor, mActor);
        msg->m38 = 2;
        g_msgDispatcher->Post(msg);
    }
    if (!mCheckCollision && !mWithChildren) {
        mActor->UseFilter(0, 2, 5, filter);
        return;
    }
    MfVector3 step;
    MfVector3 unused;                                       // set up, never read
    MfVector3 point;
    MfVector3 start;
    MfVector3 slide;
    float count = 0.0f;
    MfTransform3 corrected;
    MfColResultRay ray;
    MfColResultSphere sphere;
    MfVector3 forward = mActor->skeleton()->transform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    forward.mY = 0.0f;
    float norm = (float)sqrt(forward.mY * forward.mY + forward.mZ * forward.mZ + forward.mX * forward.mX);
    forward.mX /= norm;
    forward.mY /= norm;
    forward.mZ /= norm;
    forward = ((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform().rotation() * forward;
    if (mBackwards)
        forward = -forward;
    int eventID = g_world->CreateUniqueID();
    m90 = 0;
    int i;
    for (i = 0; i < mBubbleModel->mCount; i++) {
        if (mMode && mBubbleModel->mHits[i])
            continue;
        center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
        int style = mBubbleModel->mNodes[i]->ColNode().GetCheckStyle();
        int slid = 0;
        int pass;
        float length;
        for (pass = 0; pass < 2; pass++) {
            if (pass == 0 && (~style & 4) || pass == 1 && (~style & 2)) {
                mBubbleModel->mGlobalCenters[i] = center;
                continue;
            }
            int found;
            if (pass == 0) {
                step = center - mBubbleModel->mGlobalCenters[i];
                length = step.length();
                if (length < 1.0e-6f || mFirstUpdate)
                    step = forward;
                length -= mBubbleModel->mRadii[i] * -2.0f;
                float norm = (float)sqrt(step.mY * step.mY + step.mZ * step.mZ + step.mX * step.mX);
                step.mX /= norm;
                step.mY /= norm;
                step.mZ /= norm;
                start = mBubbleModel->mGlobalCenters[i] - step * mBubbleModel->mRadii[i];
                found = ((MfAiColNode *)m98)->CheckCollision(start, step, ray, false, length);
            } else {
                found = ((MfAiColNode *)m98)->CheckCollision(center, mBubbleModel->mRadii[i], sphere);
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
            if (pass == 0 && mbc) {
                float d = forward.dot(step * (length - (point - start).length()));
                if (d >= 0.0f) {
                    slide += forward * d;
                    count += 1.0f;
                }
                slid = 1;
            }
            MfSkeletonNode *otherNode = (MfSkeletonNode *)other->Owner();
            MfLiveActor *otherActor = (MfLiveActor *)otherNode->mOwner;
            MfVector3 direction;
            MfVector3 at = point;
            direction = center - mBubbleModel->mGlobalCenters[i];
            normal = otherNode->globalTransform().rotation() * normal;
            normal.length();
            collided = 1;
            if (pass == 0 || !slid) {
                MfMsg *msg = new MfMsg(35, 0, 0, mActor, mActor);
                msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, otherNode, mBubbleModel->mNodes[i],
                                                    mSkill, (float)sample / mLastTime, at, direction, normal, eventID));
                g_msgDispatcher->Post(msg);
            }
            MfMsg *msg = new MfMsg(36, 0, 0, mActor, otherActor);
            msg->SetData(new MfCollisionMsgData((MfLiveActor *)mActor, otherActor, otherNode, mBubbleModel->mNodes[i],
                                                mSkill, (float)sample / mLastTime, at, direction, normal, eventID));
            g_msgDispatcher->Post(msg);
            break;
        }
        mBubbleModel->mGlobalCenters[i] = center;
    }
    if (collided) {
        if (mbc) {
            MfTransform3 current = mActor->skeleton()->transform();
            MfTransform3 last = mLastTransform;
            MfVector3 up(0.0f, 1.0f, 0.0f);
            MfRotation3 upright;
            MfVector3 axis = last.rotation() * up;
            upright.setValue(axis, up);
            last.rotation() = upright * last.rotation();
            mLastTransform = last;
            ((MfAiActorNode *)mActor->skeleton())->SetTransform(last);
            for (i = 0; i < mBubbleModel->mCount; i++)
                mBubbleModel->mGlobalCenters[i] = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
            for (int k = 3; k; k--) {
                corrected.interpolate(current, last, 0.5f);
                axis = corrected.rotation() * up;
                upright.setValue(axis, up);
                corrected.rotation() = upright * corrected.rotation();
                ((MfAiActorNode *)mActor->skeleton())->SetTransform(corrected);
                for (i = 0; i < mBubbleModel->mCount; i++) {
                    int style = ~mBubbleModel->mNodes[i]->ColNode().GetCheckStyle();
                    if ((style & 2) && (style & 4))
                        continue;
                    center = mBubbleModel->mNodes[i]->globalTransform() * mBubbleModel->mCenters[i];
                    step = center - mBubbleModel->mGlobalCenters[i];
                    float length = step.length();
                    if (length < 1.0e-6f || mFirstUpdate)
                        step = forward;
                    length -= mBubbleModel->mRadii[i] * -2.0f;
                    float norm = (float)sqrt(step.mY * step.mY + step.mZ * step.mZ + step.mX * step.mX);
                    step.mX /= norm;
                    step.mY /= norm;
                    step.mZ /= norm;
                    start = mBubbleModel->mGlobalCenters[i] - step * mBubbleModel->mRadii[i];
                    MfColResultSphere test;
                    if (((MfAiColNode *)m98)->CheckCollision(start, step, ray, false, length) ||
                        ((MfAiColNode *)m98)->CheckCollision(center, mBubbleModel->mRadii[i], test))
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

// TODO 0x10040620 ?_ConformToFloor@MfLocoActivity@@IAEHK@Z: calls, branches and constants
// agree; the original expands the mflinalg/mfutil inlines only in part (it calls the imported
// MfVector3/MfRotation3 constructors and MfRotation3::operator=, and MfDblList::Append through
// the vtable), which shifts the frame by 4 (59% similar)
// Keeps the feet that touch down since the last sample on the floor: a ray
// down from each foot bubble finds the nearest floor below (the actor is
// lowered onto it) and the floor the foot sinks into (the actor is raised out
// of it). Feet without floor get a contour failure message; 2 when no foot
// that was checked found a floor.
int MfLocoActivity::_ConformToFloor(unsigned long sample)
{
    MfVector3 center;
    MfVector3 above;
    MfTransform3 xf;
    int result = 0;
    MfColResultRay ray;
    float radius, height;
    mActor->GetCylinder(radius, height);
    MfVector3 p;
    MfVector3 down;
    MfVector3 hit;
    MfVector3 diff;
    MfVector3 forward;
    float maxRise = -FLT_MAX;
    float minDrop = FLT_MAX;
    MfDblList missed;
    MfVector3 below;
    down = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, -1.0f, 0.0f);
    forward = mActor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    MfLocoSkill *skill = (MfLocoSkill *)mSkill;
    int cur = sample / skill->FootSampleStep();
    int last = mLastSampleTime / skill->FootSampleStep();
    if (mBackwards) {
        if (last < cur)
            last = cur;
    } else if (last > cur)
        last = cur;
    int checked = 0;
    MfDblListItem *pos;
    MfLocoLeg *leg;
    int i;
    for (leg = (MfLocoLeg *)skill->legList().GetFirst(pos); leg; leg = (MfLocoLeg *)skill->legList().GetNext(pos)) {
        MfSkeletonNode *foot = mActor->skeletonTraversal()->node(leg->mNode);
        for (i = 0; i < mBubbleModel->mCount; i++) {
            if (foot != mBubbleModel->mNodes[i])
                continue;
            p = foot->globalTransform() * mBubbleModel->mCenters[i] + (forward - down) * mBubbleModel->mRadii[i];
            int touched = 0;
            float length = md4 - mBubbleModel->mRadii[i] * -2.0f;
            int j;
            if (mBackwards) {
                for (j = last - 1; j >= cur; j--)
                    if (j >= 0 && leg->m0fc[j])
                        touched = 1;
            } else {
                for (j = last + 1; j <= cur; j++)
                    if (leg->m0fc[j])
                        touched = 1;
            }
            if (!touched)
                continue;
            checked++;
            if (((MfAiColNode *)m98)->CheckCollision(p, down, ray, false, length)) {
                hit = ray.pos;
                int found = 1;
                if (mMode == 2 && (MfSkeletonNode *)ray.node->Owner() != mNode)
                    found = 0;
                if (found) {
                    diff = hit - p;
                    float d = diff.length() - (mBubbleModel->mRadii[i] + mBubbleModel->mRadii[i]);
                    if (d > 0.0f && d < length && d < minDrop)
                        minDrop = d;
                }
            } else {
                missed.Append(foot);
            }
        }
    }
    MfVector3 offset;
    int dropped = 0;
    if (minDrop < FLT_MAX / 2) {
        xf.rotation() = mActor->skeleton()->transform().rotation();
        xf.translation() = mActor->skeleton()->transform().translation();
        minDrop = minDrop - radius * 0.01f;
        offset += down * minDrop;
        offset = ~((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform().rotation() * offset;
        xf.translation() += offset;
        mActor->skeleton()->setTransform(xf);
        mActor->skeleton()->syncChildTransforms(0);
        dropped = 1;
    }
    offset = MfVector3(0.0f, 0.0f, 0.0f);
    for (leg = (MfLocoLeg *)skill->legList().GetFirst(pos); leg; leg = (MfLocoLeg *)skill->legList().GetNext(pos)) {
        MfSkeletonNode *foot = mActor->skeletonTraversal()->node(leg->mNode);
        for (i = 0; i < mBubbleModel->mCount; i++) {
            if (foot != mBubbleModel->mNodes[i])
                continue;
            float length;
            if (dropped)
                length = md0 + minDrop;
            else
                length = md0;
            p = foot->globalTransform() * mBubbleModel->mCenters[i] + (forward + down) * mBubbleModel->mRadii[i] - down * length;
            if (!((MfAiColNode *)m98)->CheckCollision(p, down, ray, true, length * 1.5f))
                continue;
            hit = ray.pos;
            int found = 1;
            if (mMode == 2 && (MfSkeletonNode *)ray.node->Owner() != mNode)
                found = 0;
            if (!found)
                continue;
            MfDblListItem *item = missed.Find(foot);
            if (item)
                missed.Delete(item);
            diff = hit - p;
            float d = length - diff.length();
            if (d > 0.0f && d > maxRise)
                maxRise = d;
        }
    }
    MfSkeletonNode *node;
    for (node = (MfSkeletonNode *)missed.GetFirst(pos); node; node = (MfSkeletonNode *)missed.GetNext(pos)) {
        MfMsg *msg = new MfMsg(0x39, 0, 0, mActor, mActor);
        msg->m38 = 1;
        msg->SetData(new MfContourFailMsgData(node));
        g_msgDispatcher->Post(msg);
    }
    if (maxRise > -FLT_MAX / 2) {
        xf = mActor->skeleton()->transform();
        offset -= down * (maxRise - radius * -0.01f);
        offset = ~((MfSkeletonNode *)mActor->skeleton()->parent())->globalTransform().rotation() * offset;
        xf.translation() += offset;
        mActor->skeleton()->setTransform(xf);
        mActor->skeleton()->syncChildTransforms(0);
        return 0;
    }
    if (missed.GetCount() == checked && checked > 0)
        result = 2;
    return result;
}

// 0x100411a0 ?CreateLocoActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateLocoActivity()
{
    return new MfLocoActivity;
}

// 0x10041210 _$E50 (original $E: the static initializer of the constructor entry, with 10041220)
// 0x10041220 _$E49
static MfStreamConstructorEntry constructorEntry(MfLocoActivity::STREAM_TYPE, CreateLocoActivity);

// 0x10041240 ?StreamType@MfLocoActivity@@UBEJXZ
long MfLocoActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10041250 ?WriteToStream@MfLocoActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLocoActivity::WriteToStream(MfStream &stream) const
{
    MfAtomicActivity::WriteToStream(stream);
    WritePath(stream, mPath);
    stream << md0 << md4 << mMode << mCheckCollision << mWithChildren << mBackwards << mLimitCycles << mCycles
           << mMatchGait;
    if (mMode == 2)
        WriteNodeReference(stream, mNode);
    return stream.Status();
}

// 0x10041310 ?ReadFromStream@MfLocoActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLocoActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfAtomicActivity::ReadFromStream(stream);
    if (error)
        return error;
    ReadPath(stream, mPath);
    stream >> md0 >> md4 >> mMode >> mCheckCollision >> mWithChildren >> mBackwards >> mLimitCycles >> mCycles
           >> mMatchGait;
    if (mMode == 2)
        ReadNodeReference(stream, mActor, mNode);
    if (mSkill->Type() == MfLocoSkill::TYPE && mMatchGait)
        mLastSampleTime = ((MfLocoSkill *)mSkill)->BestMatchIndex(mActor);
    else
        mLastSampleTime = 0;
    // the first geometry node above the actor
    MfSkeletonNode *node = (MfSkeletonNode *)mActor->skeleton()->parent();
    while (node && node->GetNodeType())
        node = (MfSkeletonNode *)node->parent();
    mNode = node;
    return stream.Status();
}

// 0x10041430 ?GetCopy2DPath@MfLocoActivity@@UAEPAVMfAiPath2@@XZ
MfAiPath2 *MfLocoActivity::GetCopy2DPath()
{
    return new MfPath2(*mPath);
}

// 0x10007190 ??_EMfLocoActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (10006ca0); the port needs its own definition.
int MfLocoActivity::WillFinish()
{
    return 1;
}
