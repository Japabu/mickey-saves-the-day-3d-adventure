#include "MfLocoSkill.h"
#include <math.h>
#include "MfStream.h"
#include "MfPermutation.h"
#include "MfProtoFootData.h"
#include "MfActor.h"
#include "MfLiveActor.h"
#include "MfActorPath.h"
#include "MfSkeletonTraversal.h"
#include "MfString.h"
#include "MfVector2.h"
#include "MfJointKeyList.h"
#include "MfJointKeyframe.h"

long const MfLocoSkill::STREAM_TYPE = 'lkSL';

// Inline members this object emits (the class is exported):
// 0x100036e0 ?Type@MfLocoSkill@@UAEHXZ
// 0x100036f0 ?IsType@MfLocoSkill@@UAEHH@Z
// 0x10003720 ?Duplicate@MfLocoSkill@@UAEPAVMfProtoSkill@@XZ
// 0x10003780 ?legList@MfLocoSkill@@QAEAAVMfDblList@@XZ
// 0x10003790 ?GetNumFootSamples@MfLocoSkill@@QAEHXZ
// 0x100037a0 ?GetRadius@MfLocoSkill@@UAEMXZ
// 0x100037b0 ?SetRadius@MfLocoSkill@@UAEXM@Z
// 0x10008390 ?FootSampleStep@MfLocoSkill@@QAEKXZ

// 0x100414a0 ??0MfLocoSkill@@QAE@XZ
MfLocoSkill::MfLocoSkill()
    : mac(0), mFootPath(0), mNumFootSamples(0)
{
}

// 0x10041550 ??0MfLocoSkill@@QAE@ABV0@@Z
MfLocoSkill::MfLocoSkill(const MfLocoSkill &other)
    : mac(0), mFootPath(0), mNumFootSamples(0)
{
    Copy(other);
}

// 0x100415e0 ??1MfLocoSkill@@UAE@XZ
MfLocoSkill::~MfLocoSkill()
{
    Clear();
}

// 0x10041650 ?Clear@MfLocoSkill@@MAEXXZ
void MfLocoSkill::Clear()
{
    MfSkill::Clear();
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&mLegList)->GetNext(pos))
        delete leg;
    (&mLegList)->Clear();
    if (mFootPath) {
        delete [] mFootPath;
        mFootPath = 0;
    }
}

// 0x100416e0 ??1MfLocoLeg@@QAE@XZ (implicit)

// 0x10041700 ?Copy@MfLocoSkill@@UAEXABVMfSkill@@@Z
void MfLocoSkill::Copy(const MfSkill &skill)
{
    MfSkill::Copy(skill);
    if (((MfSkill &)skill).Type() != TYPE)
        return;
    const MfLocoSkill &other = (const MfLocoSkill &)skill;
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&other.mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&other.mLegList)->GetNext(pos))
        (&mLegList)->Add(new MfLocoLeg(*leg));
    mac = other.mac;
    if (mac) {
        mFootPath = new MfVector2[mac];
        for (int i = 0; i < mac; i++)
            mFootPath[i] = other.mFootPath[i];
    } else
        mFootPath = 0;
    m98 = other.m98;
    mRadius = other.mRadius;
    ma0 = other.ma0;
    ma4 = other.ma4;
    ma8 = other.ma8;
}

// 0x10042bb0 ?PermuteJoints@MfLocoSkill@@UAEXPAVMfProtoActor@@PAVMfPermutation@@@Z
// The legs follow the new node order; legs of removed nodes are deleted.
void MfLocoSkill::PermuteJoints(MfProtoActor *proto, MfPermutation *permutation)
{
    MfSkill::PermuteJoints(proto, permutation);
    MfDblList removed;
    MfDblList *list = &removed;
    MfDblListItem *pos;
    MfLocoLeg *leg;
    for (leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg; leg = (MfLocoLeg *)(&mLegList)->GetNext(pos)) {
        leg->mNode = permutation->mMap[leg->mNode];
        if (leg->mNode == -1)
            list->Append(leg);
    }
    for (leg = (MfLocoLeg *)removed.GetFirst(pos); leg; leg = (MfLocoLeg *)removed.GetNext(pos)) {
        MfDblListItem *item = (&mLegList)->Find(leg);
        (&mLegList)->Delete(item);
        delete leg;
    }
}

// TODO 0x10041930 ?normalize@MfLocoSkill@@MAEXXZ: (81% similar) setting the root key to (turn, -start) calls the imported
// MfRotation3/MfVector3 operator= in the original (as the nested members of an inline MfTransform3
// assignment do), here the two assignments are expanded; the rest follows (65% similar)
// Moves the sampled root path so that the hip starts above the origin and
// turns it so that the cycle runs along z; the turn becomes the conversion.
void MfLocoSkill::normalize()
{
    MfTransform3 hipKey;
    MfTransform3 rootKey;
    MfTransform3 key;
    MfRotation3 turn;
    MfVector3 start;
    MfVector3 dir;
    MfJointPath *root = mSampledPath->Track(0);
    MfJointPath *hip = mSampledPath->Track(1);
    if (!hip || !root)
        return;
    int numKeys = root->NumKeys();
    rootKey = root->Key(0);
    hipKey = hip->Key(0);
    key = rootKey * hipKey;
    start.x() = key.x();
    start.y() = 0.0f;
    start.z() = key.z();
    rootKey = root->Key(numKeys - 1);
    hipKey = hip->Key(numKeys - 1);
    key = rootKey * hipKey;
    dir.x() = key.x() - start.x();
    turn = MfRotation3();
    dir.z() = key.z() - start.z();
    rootKey.rotation() = turn;
    rootKey.translation() = -start;
    int i;
    for (i = 0; i < numKeys; i++) {
        key = root->Key(i);
        root->SetKey(i, rootKey * key);
    }
    turn.setValue(-(float)atan2(dir.x(), dir.z()), MfVector3(0.0f, 1.0f, 0.0f));
    rootKey.setValue(turn, MfVector3());
    for (i = 0; i < numKeys; i++) {
        key = root->Key(i);
        root->SetKey(i, rootKey * key);
    }
    conversion = turn;
}

// 0x10042390 ?ComputeFootTrajectories@MfLocoSkill@@IAEXPAVMfActor@@@Z
// Samples the feet (at most 20 times over the skill) relative to the actor
// and marks the samples at which a foot is planted.
void MfLocoSkill::ComputeFootTrajectories(MfActor *actor)
{
    MfTransform3 toActor;
    MfTransform3 unused;
    unsigned long last = lastTime();
    mFootSampleStep = 50;
    mNumFootSamples = last / 50 + 1;
    if (mNumFootSamples > 20) {
        mNumFootSamples = 20;
        mFootSampleStep = last / 19;
    }
    int i = 0;
    unsigned long time = 0;
    for (; i < mNumFootSamples; i++) {
        sample(time, actor->mSkeletonTraversal, 0);
        toActor = ~actor->mSkeleton->globalTransform();
        MfDblListItem *pos;
        for (MfLocoLeg *leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg;
             leg = (MfLocoLeg *)(&mLegList)->GetNext(pos)) {
            leg->mFootPositions[i] =
                (toActor * actor->mSkeletonTraversal->node(leg->mNode)->globalTransform()).translation();
            MfJointKeyList *track = pJointInfo[leg->mNode].pKeyframes;
            if (track) {
                MfJointKeyframe *before = (MfJointKeyframe *)track->LastKeyframeUntil(time);
                MfJointKeyframe *after = (MfJointKeyframe *)track->FirstKeyframeAfter(time);
                int planted = 0;
                if (before && after && (before->mFlags & 4) && (after->mFlags & 4))
                    planted = 1;
                else if (i > 0 && before && (before->mFlags & 4) && !leg->m0fc[i - 1] &&
                         (unsigned long)before->m08 > time - mFootSampleStep)
                    planted = 1;
                leg->m0fc[i] = planted;
            }
        }
        time += mFootSampleStep;
    }
}

// EQUIV 0x10042ce0 ?ScaleGait@MfLocoSkill@@QAEXM@Z: reviewed: the rotation of the unused transform is built
// with the imported MfRotation3 constructor in the original (mflinalg's sets w = 1, x = y = z = 0), inline
// here, so the original has one more EH state; nothing else differs
// Scales the stride: the forward (z) offsets of the root's keys and of every
// joint's keys in the actor.
void MfLocoSkill::ScaleGait(float scale)
{
    MfTransform3 xform;
    MfKeyListNode *node;
    MfJointKeyframe *key;
    int i;
    MfJointKeyList *root = pJointInfo[0].pKeyframes;
    if (root) {
        for (key = (MfJointKeyframe *)root->GetFirst(node); key; key = (MfJointKeyframe *)root->GetNext(node)) {
            key->mXform.z() = scale * key->mXform.z();
            for (i = 0; i < 2; i++)
                key->mXformCtrl[i].z() = scale * key->mXformCtrl[i].z();
        }
    }
    for (int j = 0; j < numJoints; j++) {
        MfJointKeyList *track = pJointInfo[j].pKeyframes;
        if (track) {
            for (key = (MfJointKeyframe *)track->GetFirst(node); key; key = (MfJointKeyframe *)track->GetNext(node)) {
                key->mWorld.z() = scale * key->mWorld.z();
                for (i = 0; i < 2; i++)
                    key->mWorldCtrl[i].z() = scale * key->mWorldCtrl[i].z();
            }
        }
    }
}

// 0x10042df0 ?UseSubclassData@MfLocoSkill@@UAEXABVMfProtoSkill@@@Z
// A leg for every foot of the prototype.
void MfLocoSkill::UseSubclassData(const MfProtoSkill &proto)
{
    if (proto.subclassData) {
        MfDblList *objects = &proto.subclassData->objects;
        MfDblListItem *pos;
        for (MfStreamObject *object = (MfStreamObject *)objects->GetFirst(pos); object;
             object = (MfStreamObject *)objects->GetNext(pos)) {
            if (object->StreamType() == MfProtoFootData::STREAM_TYPE) {
                MfProtoFootData *foot = (MfProtoFootData *)object;
                MfLocoLeg *leg = new MfLocoLeg;
                leg->mNode = foot->footIndex;
                leg->m04 = foot->hipIndex;
                leg->m08 = foot->soleTriIndex;
                (&mLegList)->Append(leg);
            }
        }
        MfSkill::UseSubclassData(proto);
    }
}

// 0x10042ee0 ?CreateSubclassData@MfLocoSkill@@UAEXXZ
// A prototype foot for every leg.
void MfLocoSkill::CreateSubclassData()
{
    MfSkill::CreateSubclassData();
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&mLegList)->GetNext(pos)) {
        MfProtoFootData *foot = new MfProtoFootData;
        foot->footIndex = leg->mNode;
        foot->hipIndex = leg->m04;
        foot->soleTriIndex = leg->m08;
        AddFoot(foot);
    }
    type = MfProtoSkill::LOCO;
}

// 0x10042fa0 ?CreateLocoSkill@@YAPAVMfStreamObject@@XZ
static MfStreamObject *CreateLocoSkill()
{
    return new MfLocoSkill;
}

// 0x10043010 _$E36
// 0x10043000 _$E37
static MfStreamConstructorEntry constructorEntry(MfLocoSkill::STREAM_TYPE, CreateLocoSkill);

// 0x10043040 ?ReadFromStream@MfLocoSkill@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLocoSkill::ReadFromStream(MfStream &stream)
{
    MfSkill::ReadFromStream(stream);
    if (stream.userFormat >= 23) {
        stream >> m98 >> mRadius >> ma0;
        stream >> ma4 >> ma8 >> mac;
        mFootPath = new MfVector2[mac];
        stream.ReadBytes(mac * sizeof(MfVector2), mFootPath);
    }
    int numLegs;
    stream >> numLegs;
    if (stream.userFormat < 17)
        mNumFootSamples = 0;
    else {
        int step;
        stream >> mNumFootSamples >> step;
        mFootSampleStep = step;
    }
    for (int i = 0; i < numLegs; i++) {
        MfLocoLeg *leg = new MfLocoLeg;
        if (stream.userFormat < 17)
            stream.ReadBytes(12, leg);
        else if (stream.userFormat <= 23)
            stream.ReadBytes(0xfc, leg);
        else
            stream.ReadBytes(sizeof(MfLocoLeg), leg);
        (&mLegList)->Add(leg);
    }
    return stream.Status();
}

// 0x10043220 ?WriteToStream@MfLocoSkill@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLocoSkill::WriteToStream(MfStream &stream) const
{
    MfSkill::WriteToStream(stream);
    stream << m98 << mRadius << ma0;
    stream << ma4 << ma8 << mac;
    stream.WriteBytes(mac * sizeof(MfVector2), mFootPath);
    stream << mLegList.GetCount() << mNumFootSamples << (int)mFootSampleStep;
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&mLegList)->GetNext(pos))
        stream.WriteBytes(sizeof(MfLocoLeg), leg);
    return stream.Status();
}

// 0x10043030 ?StreamType@MfLocoSkill@@UBEJXZ
long MfLocoSkill::StreamType() const
{
    return 'lkSL';
}

// 0x100418f0 ?apply@MfLocoSkill@@UBEHKPAVMfSkeletonTraversal@@H@Z
int MfLocoSkill::apply(unsigned long time, MfSkeletonTraversal *nodes, int flag) const
{
    return MfSkill::apply(time, nodes, flag);
}

// 0x10041910 ?Presample@MfLocoSkill@@UAEXPAVMfActor@@@Z
void MfLocoSkill::Presample(MfActor *actor)
{
    MfSkill::Presample(actor);
    ComputeGaitInfo(actor);
}

// 0x10042330 ?ComputeInfo@MfLocoSkill@@UAEXPAVMfActor@@@Z
void MfLocoSkill::ComputeInfo(MfActor *actor)
{
    MfSkill::ComputeInfo(actor);
    MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
    ComputeFootTrajectories(actor);
    actor->mSkeletonTraversal->SetTransforms(saved);
    if (saved)
        delete [] saved;
}

// 0x100438f0 ?getLocoPlannerData@MfLocoSkill@@QAEXAAM0000AAHPAPAVMfVector2@@@Z
// The gait data the locomotion planner works with; path gets a new copy of
// the foot path (count points) that the caller deletes.
void MfLocoSkill::getLocoPlannerData(float &a, float &radius, float &c, float &d, float &e, int &count,
                                     MfVector2 **path)
{
    a = m98;
    radius = mRadius;
    c = ma0;
    d = ma4;
    e = ma8;
    count = mac;
    *path = new MfVector2[count];
    count = mac;
    for (int i = 0; i < count; i++)
        (*path)[i] = mFootPath[i];
}

// 0x100439f0 ?IsValid@MfLocoSkill@@UAEHAAVMfString@@PAVMfActor@@@Z
int MfLocoSkill::IsValid(MfString &message, MfActor *)
{
    if (mLegList.GetCount() == 0) {
        mValidSkill = 0;
        message << "Invalid loco skill:\n\n";
        message << "Loco skills must have specified feet nodes.";
        message << "\nDo you really want to close the timeline?\n";
        return 0;
    }
    return mValidSkill = 1;
}

// 0x10043a40 ?CycleLength@MfLocoSkill@@QAEMXZ
// The distance the root moves in one cycle of the skill.
float MfLocoSkill::CycleLength()
{
    if (mSampledPath) {
        MfJointPath *root = mSampledPath->Track(0);
        if (root) {
            unsigned long end = root->mInterval * root->mNumKeys;
            return (root->Sample(end, bInterpolate).translation() - root->Sample(0, bInterpolate).translation()).length();
        }
    }
    return 0.0f;
}

// 0x100414f0 ??_EMfLocoSkill@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// (defined after CycleLength: in front of it, it flips the operand order of CycleLength's imul)
// 0x10042630 ?BestMatchIndex@MfLocoSkill@@QAEKPAVMfActor@@@Z
// The time in the cycle whose foot positions and motion fit the actor's feet
// now and since the last update best.
unsigned long MfLocoSkill::BestMatchIndex(MfActor *actor)
{
    int numLegs = mLegList.GetCount();
    if (!numLegs || !mNumFootSamples)
        return 0;
    MfTransform3 toActor = ~actor->mSkeleton->globalTransform();
    MfVector3 *current = new MfVector3[numLegs];
    MfVector3 *motion = new MfVector3[numLegs];
    MfLiveActor *live = (MfLiveActor *)actor;
    live->AllocateLastConfig();
    MfTransform3 lastToActor = ~*live->mLastConfig;
    MfVector3 last;
    MfDblListItem *pos;
    int k = 0;
    MfLocoLeg *leg;
    for (leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg; leg = (MfLocoLeg *)(&mLegList)->GetNext(pos)) {
        current[k] = (toActor * actor->mSkeletonTraversal->node(leg->mNode)->globalTransform()).translation();
        last = lastToActor * live->mLastConfig[leg->mNode].translation();
        motion[k] = current[k] - last;
        k++;
    }
    float best = 3.4028235e38f;
    int bestIndex = 0;
    MfVector3 step;
    MfVector3 offset;
    float fraction = 0.0f;
    for (int i = 0; i < mNumFootSamples; i++) {
        float error = 0.0f;
        float total = 0.0f;
        k = 0;
        for (leg = (MfLocoLeg *)(&mLegList)->GetFirst(pos); leg; leg = (MfLocoLeg *)(&mLegList)->GetNext(pos)) {
            if (i == 0)
                step = leg->mFootPositions[0] - leg->mFootPositions[mNumFootSamples - 1];
            else
                step = leg->mFootPositions[i] - leg->mFootPositions[i - 1];
            offset = leg->mFootPositions[i] - current[k];
            if (step.dot(motion[k]) >= 0.0f && step.dot(offset) > 0.0f) {
                total += step.length();
                if (step.lengthSquared() > 0.00001f) {
                    float length = (float)sqrt(step.lengthSquared());
                    step.mX /= length;
                    step.mY /= length;
                    step.mZ /= length;
                    error = step.dot(offset) + error;
                } else
                    error = offset.length() + error;
            } else
                error -= offset.length() * -10000.0f;
            k++;
        }
        if (error < best) {
            best = error;
            bestIndex = i;
            if (error < total && total > 0.0f)
                fraction = error / total;
            else
                fraction = 0.0f;
        }
    }
    delete[] current;
    delete[] motion;
    float index = bestIndex - fraction;
    if (index < 0.0f)
        index = 0.0f;
    return (unsigned long)(mFootSampleStep * index - -0.5f);
}

// (also after CycleLength, see BestMatchIndex)
// TODO 0x10041e40 ?factor@MfLocoSkill@@MAEXXZ: (83% similar) as MfSkill::factor: the last assignment (hip = ~root *
// global) calls the imported MfTransform3 operator= and destructors where this expands them (76% similar)
// Splits the hip's motion: the root path takes the height-free forward
// position and the heading (unwrapped and folded to the nearer of the two
// directions), the hip the rest.
void MfLocoSkill::factor()
{
    MfTransform3 hipKey;
    MfTransform3 rootKey;
    MfTransform3 global;
    if (numJoints < 2)
        return;
    normalize();
    MfJointPath *root = mSampledPath->Track(0);
    MfJointPath *hip = mSampledPath->Track(1);
    if (!hip || !root)
        return;
    int numKeys = root->NumKeys();
    hipKey = root->Key(0) * hip->Key(0);
    MfVector3 forward;
    MfVector3 unused;
    forward = ~hipKey.rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    float last;
    for (int i = 0; i < numKeys; i++) {
        rootKey = root->Key(i);
        hipKey = hip->Key(i);
        global = rootKey * hipKey;
        rootKey.translation() = MfVector3(0.0f, 0.0f, global.z());
        const MfVector3 &dir = global.rotation() * forward;
        float angle = MfVector2(dir.z(), dir.x()).angle();
        if (angle < 0.0f)
            angle -= -6.2831855f;
        if (i == 0)
            last = angle;
        float back = angle - 3.1415927f;
        if (back < 0.0f)
            back -= -6.2831855f;
        float d1 = angle - last;
        if (d1 < 0.0f)
            d1 = -d1;
        float d2 = back - last;
        if (d2 < 0.0f)
            d2 = -d2;
        if (d1 > 3.1415927f)
            d1 = 6.2831855f - d1;
        if (d2 > 3.1415927f)
            d2 = 6.2831855f - d2;
        if (d2 < d1)
            angle = back;
        last = angle;
        rootKey.rotation().setValue(angle, MfVector3(0.0f, 1.0f, 0.0f));
        hipKey = ~rootKey * global;
        root->SetKey(i, rootKey);
        hip->SetKey(i, hipKey);
    }
}
