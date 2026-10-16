// MfAtomicActivity: plays one skill (RuntimeLib.dll 10017f90-10019690).
#include "MfAtomicActivity.h"
#include "MfSpace.h"
#include "MfWorld.h"
#include "MfActorPath.h"
#include "MfColSphere.h"
#include "MfSkill.h"
#include "MfSkillKeyframes.h"
#include "MfStream.h"

// the ignore masks in a stream (defined with the first activities, 1000a920)
MfStreamError WriteMasks(MfStream &stream, int *masks, int count);
MfStreamError ReadMasks(MfStream &stream, int *&masks, int &count);

long const MfAtomicActivity::STREAM_TYPE = 'AtoA';

// the implicit copy members of the exported class
static MfAtomicActivity &(MfAtomicActivity::*s_assign)(const MfAtomicActivity &) = &MfAtomicActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x100062f0 ?SetMaintainTextures@MfAtomicActivity@@QAEXH@Z
// 0x10006300 ?Type@MfAtomicActivity@@UAEHXZ
// 0x10006310 ?IsType@MfAtomicActivity@@UAEHH@Z
// 0x10006340 ?GetIgnoreMasks@MfAtomicActivity@@UAEPAHAAH@Z
// 0x10006370 ?getPlayDirection@MfAtomicActivity@@UBEHXZ
// 0x10006380 ?getCheckColl@MfAtomicActivity@@QBEHXZ
// 0x10006390 ?PrintName@MfAtomicActivity@@UBE?AVMfString@@XZ
// 0x100063c0 ??0MfAtomicActivity@@QAE@ABV0@@Z (implicit copy constructor)
// 0x10006550 ??4MfAtomicActivity@@QAEAAV0@ABV0@@Z (implicit assignment)
// 0x10006690 ??_EMfAtomicActivity@@UAEPAXI@Z
// 0x1000aaf0 ?Type@MfAtomicActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000ab00 ?IsType@MfAtomicActivity@@WBE@AEHH@Z

// 0x10017f90 ??0MfAtomicActivity@@QAE@XZ
MfAtomicActivity::MfAtomicActivity()
    : mSkill(0), mFirstTime(0), mLastTime(1), mPlayDirection(0), mLastSampleTime(0), m90(0),
      mBubbleModel(0), m98(0), mIgnoreMasks(0), mMaintainTextures(0), mLastSound(0), mLastEvent(0)
{
}

// 0x10018020 ??0MfAtomicActivity@@QAE@PAVMfActor@@KPAVMfSkill@@@Z
MfAtomicActivity::MfAtomicActivity(MfActor *actor, unsigned long startTime, MfSkill *skill)
    : MfActivity(actor, startTime), mFirstTime(0), mLastTime(1), mLastSampleTime(0)
{
    m90 = 0;
    mSkill = skill;
    mPlayDirection = 0;
    mMaintainTextures = 0;
    mDuration = 0;
    if (skill) {
        skill->AddRef();
        if (!mSkill->isPresampled())
            skill->Presample(actor);
        mFirstTime = mSkill->firstTime();
        mLastTime = mSkill->lastTime();
        if (mLastTime == 0)
            mLastTime = 1;
        mDuration = mSkill->GetDuration();
    }
    mBubbleModel = 0;
    m98 = 0;
    mIgnoreMasks = 0;
}

// 0x10018130 ??1MfAtomicActivity@@UAE@XZ
MfAtomicActivity::~MfAtomicActivity()
{
    if (mSkill) {
        int actorExists = g_world->ActorExists(mActor);
        if (actorExists && !mMaintainTextures)
            mSkill->RestoreOriginalTextures(mActor->skeletonTraversal());
    }
    _DeleteBubbleModel();
    if (mIgnoreMasks) {
        delete mIgnoreMasks;
        mIgnoreMasks = 0;
    }
    if (mSkill)
        mSkill->Release();
}

// 0x10018200 ?Update@MfAtomicActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfAtomicActivity::Update(unsigned long time)
{
    if (time < mStartTime)
        return (MfActivityStatus)0;
    unsigned long skillTime = time - mStartTime;
    if (skillTime > mLastTime)
        mFinished = 1;
    if (mPlayDirection == 1)
        skillTime = mLastTime - skillTime;
    if (mFinished) {
        if (mPlayDirection == 1)
            skillTime = mFirstTime;
        else
            skillTime = mLastTime;
    }
    if (mInitialConfig)
        mSkill->sample(skillTime, mActor->skeletonTraversal(), mSkillOffset);
    else
        mSkill->sample(skillTime, mActor->skeletonTraversal(), 1);
    blendFromInitialConfig(time, 0);
    updateExtraTracks(mLastSampleTime, skillTime);
    mLastSampleTime = skillTime;
    return MfActivity::Update(time);
}

// 0x100182b0 ?updateExtraTracks@MfAtomicActivity@@IAEXKK@Z
// Plays the sounds and sends the events of the skill between two sample
// times; a range that wraps around the end of the skill is split in two.
void MfAtomicActivity::updateExtraTracks(unsigned long from, unsigned long to)
{
    if (mPlayDirection == 0 && to <= from) {
        if (to == from) {
            updateExtraTracks(from, to + 1);
            return;
        }
        if (from < mLastTime)
            updateExtraTracks(from, mLastTime);
        if (to > 0)
            updateExtraTracks(0, to);
        return;
    }
    if (mPlayDirection == 1 && to >= from) {
        if (to == from) {
            updateExtraTracks(from + 1, to);
            return;
        }
        if (from > 0)
            updateExtraTracks(from, 0);
        if (to < mLastTime)
            updateExtraTracks(mLastTime, to);
        return;
    }

    MfSoundKeyframe *sound = 0;
    if (mPlayDirection == 0)
        sound = mSkill->soundEvent(from, to);
    else if (mPlayDirection == 1)
        sound = mSkill->soundEvent(to, from);
    if (sound && sound != mLastSound)
        sound->Play(mActor->skeleton());
    mLastSound = sound;

    MfKeyframe *last = 0;
    MfDblList events;
    if (mPlayDirection == 0)
        mSkill->triggerEvent(from, to, events);
    else if (mPlayDirection == 1)
        mSkill->triggerEvent(to, from, events);
    MfDblListItem *pos;
    for (MfEventKeyframe *event = (MfEventKeyframe *)events.GetFirst(pos); event;
         event = (MfEventKeyframe *)events.GetNext(pos)) {
        last = event;
        if (event && event->mBroadcast && event != mLastEvent)
            mActor->SendEvent(0, event->mMessage, 0);
        else if (event && event != mLastEvent)
            mActor->SendEvent(mActor, event->mMessage, 0);
    }
    mLastEvent = last;
    mSkill->ApplyTextures(to, mActor->skeletonTraversal());
}

// 0x10018510 ?setSkill@MfAtomicActivity@@UAEXPAVMfSkill@@@Z
void MfAtomicActivity::setSkill(MfSkill *skill)
{
    mSkill = skill;
    mFirstTime = mSkill->firstTime();
    mLastTime = mSkill->lastTime();
    if (mLastTime == 0)
        mLastTime = 1;
}

// 0x10018540 ?setPlayDirection@MfAtomicActivity@@UAEXH@Z
void MfAtomicActivity::setPlayDirection(int direction)
{
    mPlayDirection = direction;
}

// 0x10018550 ?setInitialConfig@MfAtomicActivity@@UAEXPAVMfSkeletonTraversal@@K@Z
void MfAtomicActivity::setInitialConfig(MfSkeletonTraversal *traversal, unsigned long blendTime)
{
    if (mFirstTime != mLastTime) {
        MfActivity::setInitialConfig(traversal, blendTime);
        setInitialStates();
    }
}

// 0x10018580 ?setInitialConfig@MfAtomicActivity@@UAEXPAVMfTransform3@@K@Z
void MfAtomicActivity::setInitialConfig(MfTransform3 *config, unsigned long blendTime)
{
    MfActivity::setInitialConfig(config, blendTime);
    setInitialStates();
}

// 0x100185a0 ?setInitialStates@MfAtomicActivity@@IAEXXZ
void MfAtomicActivity::setInitialStates()
{
    MfTransform3 initial(mInitialConfig[0]);
    unsigned long time = mPlayDirection == 0 ? mFirstTime : mLastTime;
    MfTransform3 skillStart;
    if (mSkill && mSkill->mSampledPath->Track(0))
        skillStart = mSkill->mSampledPath->Track(0)->Sample(time, mSkill->GetInterpolate());
    mSkillOffset = initial * ~skillStart;
    if (mBlendTime > mLastTime && mSkill)
        mBlendTime = mLastTime;
    else if (mDuration > 0 && mBlendTime > mDuration)
        mBlendTime = mDuration;
}

// 0x10018730 ?HasMotion@MfAtomicActivity@@UAEHXZ
// Whether the skill moves a node that holds a handle.
int MfAtomicActivity::HasMotion()
{
    MfSkeletonTraversal *handles = new MfSkeletonTraversal(mActor->skeleton(), 'Hndl', 1);
    for (int i = 0; i < mActor->skeletonTraversal()->numNodes(); i++) {
        if (mSkill && mSkill->sampledPath()->Track(i)) {
            MfSkeletonNode *node = mActor->skeletonTraversal()->node(i);
            int j;
            for (j = 0; j < handles->numNodes(); j++) {
                if (node->hasDescendant(handles->node(j)))
                    break;
            }
            if (mSkill && j < handles->numNodes()) {
                for (int k = 1; k < mSkill->sampledPath()->Track(i)->NumKeys(); k++) {
                    float distance = (mSkill->sampledPath()->Track(i)->Key(k).translation() -
                                      mSkill->sampledPath()->Track(i)->Key(k - 1).translation()).lengthSquared();
                    bool moved = !(mSkill->sampledPath()->Track(i)->Key(k).rotation() ==
                                   mSkill->sampledPath()->Track(i)->Key(k - 1).rotation()) ||
                                 distance > 0.00001f;
                    if (moved)
                        return 1;
                }
            }
        }
    }
    return 0;
}

// 0x10018990 ?Key@MfJointPath@@QAE?AVMfTransform3@@H@Z

// 0x10018a00 ?blendFromInitialConfig@MfAtomicActivity@@MAEXKH@Z
// As MfActivity's, but nodes the skill does not animate keep their transform.
void MfAtomicActivity::blendFromInitialConfig(unsigned long time, int skipRoot)
{
    if (mInitialConfig && time - mStartTime < mBlendTime && !mFinished) {
        float t = (float)(time - mStartTime) / (float)mBlendTime;
        MfSkeletonTraversal *nodes = mActor->skeletonTraversal();
        MfTransform3 current;
        MfTransform3 blended;
        for (int i = skipRoot != 0; i < nodes->numNodes(); i++) {
            if (mIgnoreMasks && mIgnoreMasks[i])
                continue;
            current = nodes->node(i)->transform();
            blended.interpolate(mInitialConfig[i], current, t);
            nodes->node(i)->setTransform(blended);
        }
    }
}

// 0x10018bc0 ?_DeleteBubbleModel@MfAtomicActivity@@MAEXXZ
void MfAtomicActivity::_DeleteBubbleModel()
{
    if (mBubbleModel) {
        if (mBubbleModel->mColliders.GetCount()) {
            MfDblListItem *pos;
            for (MfAi *collider = (MfAi *)(&mBubbleModel->mColliders)->GetFirst(pos); collider;
                 collider = (MfAi *)(&mBubbleModel->mColliders)->GetNext(pos))
                collider->Release();
        }
        if (mActor->mSpace)
            mActor->mSpace->UpdateEnvironmentModel(0);
        delete mBubbleModel;
        mBubbleModel = 0;
    }
}

// 0x10018c50 ??1MfBubbleModel@@QAE@XZ
MfBubbleModel::~MfBubbleModel()
{
    if (mNodes)
        delete mNodes;
    if (mRadii)
        delete mRadii;
    if (mCenters)
        delete [] mCenters;
    if (mGlobalCenters)
        delete [] mGlobalCenters;
}

// 0x10018d10 ?_InitBubbleModel@MfAtomicActivity@@MAEXPAVMfSkill@@PAVMfActor@@H@Z
void MfAtomicActivity::_InitBubbleModel(MfSkill *skill, MfActor *actor, int withChildren)
{
    mBubbleModel = new MfBubbleModel;
    MfSkeletonTraversal *nodes;
    if (withChildren) {
        actor->GetChildActors(&mBubbleModel->mColliders);
        if (mBubbleModel->mColliders.GetCount()) {
            MfDblListItem *pos;
            for (MfAi *child = (MfAi *)(&mBubbleModel->mColliders)->GetFirst(pos); child;
                 child = (MfAi *)(&mBubbleModel->mColliders)->GetNext(pos))
                child->AddRef();
        }
        nodes = new MfSkeletonTraversal(actor->skeleton(), MfSkeletonNode::TYPE, 0);
    } else {
        nodes = actor->skeletonTraversal();
    }

    int count = 0;
    int i;
    for (i = 0; i < nodes->numNodes(); i++) {
        if (nodes->node(i)->colSpheres())
            count += nodes->node(i)->colSpheres()->GetCount();
    }
    if (count == 0) {
        delete mBubbleModel;
        mBubbleModel = 0;
        return;
    }
    mBubbleModel->Allocate(count);

    MfVector3 center;
    int bubble = 0;
    for (i = 0; i < nodes->numNodes(); i++) {
        if (!nodes->node(i)->colSpheres())
            continue;
        MfDblList *spheres = nodes->node(i)->colSpheres();
        MfDblListItem *pos;
        for (MfColSphere *sphere = (MfColSphere *)spheres->GetFirst(pos); sphere;
             sphere = (MfColSphere *)spheres->GetNext(pos)) {
            mBubbleModel->mNodes[bubble] = nodes->node(i);
            mBubbleModel->mRadii[bubble] = sphere->mRadius;
            mBubbleModel->mCenters[bubble] = sphere->mCenter;
            center = mBubbleModel->mNodes[bubble]->globalTransform() * mBubbleModel->mCenters[bubble];
            mBubbleModel->mHits[bubble] = 0;
            mBubbleModel->mGlobalCenters[bubble] = center;
            bubble++;
        }
    }
    mActor->mSpace->UpdateEnvironmentModel(1);
    if (withChildren && nodes)
        delete nodes;
}

// 0x10019020 ?Allocate@MfBubbleModel@@QAEXH@Z
void MfBubbleModel::Allocate(int count)
{
    if (count) {
        mCount = count;
        mNodes = new MfSkeletonNode *[mCount];
        mRadii = new float[mCount];
        mCenters = new MfVector3[mCount];
        mGlobalCenters = new MfVector3[mCount];
        mHits = new int[mCount];
    }
}

// 0x10019130 ?IgnoreSubtree@MfAtomicActivity@@UAEHABVMfString@@@Z
int MfAtomicActivity::IgnoreSubtree(const MfString &name)
{
    MfAiActorNode *root = mActor->skeletonTraversal()->Node(name);
    if (!root)
        return 0;
    if (mActor->skeletonTraversal()->numNodes() < 2)
        return 0;
    if (!mIgnoreMasks) {
        mIgnoreMasks = new int[mActor->skeletonTraversal()->numNodes()];
        for (int i = 0; i < mActor->skeletonTraversal()->numNodes(); i++)
            mIgnoreMasks[i] = 0;
    }
    MfSkeletonTraversal *subtree = new MfSkeletonTraversal((MfSkeletonNode *)root, MfSkeletonNode::TYPE, 1);
    for (int j = 0; j < subtree->numNodes(); j++)
        mIgnoreMasks[mActor->skeletonTraversal()->nodeIndex(subtree->node(j))] = 1;
    if (subtree)
        delete subtree;
    return 1;
}

// 0x10019280 ?AnimateSubtree@MfAtomicActivity@@UAEHABVMfString@@@Z
int MfAtomicActivity::AnimateSubtree(const MfString &name)
{
    MfAiActorNode *root = mActor->skeletonTraversal()->Node(name);
    if (!root)
        return 0;
    if (mActor->skeletonTraversal()->numNodes() < 2)
        return 0;
    if (!mIgnoreMasks) {
        mIgnoreMasks = new int[mActor->skeletonTraversal()->numNodes()];
        for (int i = 0; i < mActor->skeletonTraversal()->numNodes(); i++)
            mIgnoreMasks[i] = 1;
    }
    MfSkeletonTraversal *subtree = new MfSkeletonTraversal((MfSkeletonNode *)root, MfSkeletonNode::TYPE, 1);
    for (int j = 0; j < subtree->numNodes(); j++)
        mIgnoreMasks[mActor->skeletonTraversal()->nodeIndex(subtree->node(j))] = 0;
    if (subtree)
        delete subtree;
    return 1;
}

// 0x100193d0 ?CreateAtomicActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateAtomicActivity()
{
    return new MfAtomicActivity;
}

// 0x10019440 _$E45 (original $E: the static initializer of the constructor entry, with 10019450)
// 0x10019450 _$E44
static MfStreamConstructorEntry constructorEntry(MfAtomicActivity::STREAM_TYPE, CreateAtomicActivity);

// 0x10019470 ?StreamType@MfAtomicActivity@@UBEJXZ
long MfAtomicActivity::StreamType() const
{
    return TYPE;
}

// 0x10019480 ?WriteToStream@MfAtomicActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAtomicActivity::WriteToStream(MfStream &stream) const
{
    MfActivity::WriteToStream(stream);
    stream << mSkill->Name() << mPlayDirection;
    stream << (int)(mBubbleModel != 0);
    stream << (mBubbleModel && mBubbleModel->mColliders.GetCount() ? 1 : 0);
    if (mIgnoreMasks) {
        stream << 1;
        WriteMasks(stream, mIgnoreMasks, mActor->skeletonTraversal()->numNodes());
    } else {
        stream << 0;
    }
    stream << mMaintainTextures;
    return stream.Status();
}

// 0x10019530 ?ReadFromStream@MfAtomicActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAtomicActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfActivity::ReadFromStream(stream);
    if (error)
        return error;
    MfString skillName;
    stream >> skillName;
    mSkill = mActor->getSkill(skillName);
    if (mSkill)
        mSkill->AddRef();
    stream >> mPlayDirection;
    int present;
    int withChildren;
    stream >> present >> withChildren;
    if (present)
        _InitBubbleModel(mSkill, mActor, withChildren);
    stream >> present;
    int count;
    if (present)
        ReadMasks(stream, mIgnoreMasks, count);
    stream >> mMaintainTextures;
    if (mSkill) {
        if (!mSkill->isPresampled())
            mSkill->Presample(mActor);
        mFirstTime = mSkill->firstTime();
        mLastTime = mSkill->lastTime();
        if (mLastTime == 0)
            mLastTime = 1;
        setInitialConfig(mActor->skeletonTraversal(), mBlendTime);
    }
    return stream.Status();
}

// Not claimed: in the original this member is folded with an identical function
// (10006ca0); the port needs its own definition.
int MfAtomicActivity::WillFinish()
{
    return 1;
}
