#include "MfSkill.h"
#include <string.h>
#include <math.h>
#include "MfJointKeyList.h"
#include "MfJointKeyframe.h"
#include "MfKChain.h"
#include "MfSkeletonNode.h"
#include "MfKeyList.h"
#include "MfActorPath.h"
#include "MfActor.h"
#include "MfColModel.h"
#include "MfLocoSkill.h"
#include "MfManipSkill.h"
#include "MfPermutation.h"
#include "MfProtoKeyframes.h"
#include "MfRenderNode.h"
#include "MfSkeletonTraversal.h"
#include "MfSkillKeyframes.h"
#include "MfStream.h"
#include "MfTriangleSet.h"

// MfActorLib's load progress (declared in MfActorLib/MfFile.h)
class __declspec(dllimport) MfFileProgress {
public:
    static void UpdateProgress();
};

long const MfSkill::STREAM_TYPE = 'likS';

// Inline members this object emits (the class is exported):
// 0x10008d70 ?SoundTrack@MfSkill@@QAEPAVMfKeyList@@XZ
// 0x10008d90 ?TextureTracks@MfSkill@@QAEAAVMfDblList@@XZ
// 0x10008da0 ?ValidSkill@MfSkill@@QAEHXZ

// 0x1004fef0 ??0MfSkill@@QAE@XZ
MfSkill::MfSkill()
    : mSoundTrack(0), mEnableSounds(1), mEventTrack(0), mSampledPath(0), mLastTime(0), mValidSkill(1)
{
    mSoundTrack = new MfKeyList;
    mEventTrack = new MfKeyList;
}

// 0x1004ffe0 ??_GMfKeyList@@UAEPAXI@Z (local copy for `new MfKeyList`)
// 0x10050010 ??_EMfSkill@@UAEPAXI@Z

// 0x10050070 ??0MfSkill@@QAE@ABV0@@Z
MfSkill::MfSkill(const MfSkill &other)
    : mSoundTrack(0), mEnableSounds(1), mEventTrack(0), mSampledPath(0), mLastTime(other.mLastTime),
      mValidSkill(other.mValidSkill)
{
    Copy(other);
}

// 0x10050110 ??1MfSkill@@UAE@XZ
MfSkill::~MfSkill()
{
    Clear();
    if (mSoundTrack)
        delete mSoundTrack;
    if (mEventTrack)
        delete mEventTrack;
}

// 0x100501a0 ?Clear@MfSkill@@UAEXXZ
void MfSkill::Clear()
{
    MfProtoSkill::Clear();
    if (mSampledPath) {
        delete mSampledPath;
        mSampledPath = 0;
    }
    if (mSoundTrack) {
        delete mSoundTrack;
        mSoundTrack = 0;
    }
    mSoundTrack = new MfKeyList;
    if (mEventTrack) {
        delete mEventTrack;
        mEventTrack = 0;
    }
    mEventTrack = new MfKeyList;
    MfDblListItem *pos;
    for (MfKeyList *track = (MfKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfKeyList *)(&mTextureTracks)->GetNext(pos))
        delete track;
    (&mTextureTracks)->Clear();
}

// 0x100502c0 ?Copy@MfSkill@@UAEXABV1@@Z
void MfSkill::Copy(const MfSkill &other)
{
    MfProtoSkill::Copy(other);
    mSampledPath = other.mSampledPath ? new MfActorPath(*other.mSampledPath) : 0;
    mSoundTrack = other.mSoundTrack ? new MfKeyList(*other.mSoundTrack) : 0;
    mEnableSounds = other.mEnableSounds;
    mEventTrack = other.mEventTrack ? new MfKeyList(*other.mEventTrack) : 0;
    MfDblListItem *pos;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&other.mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&other.mTextureTracks)->GetNext(pos))
        (&mTextureTracks)->Add(new MfTextureKeyList(*track));
    mLastTime = other.mLastTime;
    mValidSkill = other.mValidSkill;
    iSampleRate = other.iSampleRate;
}

// 0x10050490 ?Duplicate@MfSkill@@UAEPAVMfProtoSkill@@XZ
MfProtoSkill *MfSkill::Duplicate()
{
    return new MfSkill(*this);
}

// 0x100504f0 ?CreateFromPrototype@MfSkill@@SAPAV1@ABVMfProtoSkill@@@Z
MfSkill *MfSkill::CreateFromPrototype(const MfProtoSkill &proto)
{
    MfSkill *skill;
    switch (proto.Type()) {
    case MfProtoSkill::GESTURE:
        skill = new MfSkill;
        break;
    case MfProtoSkill::LOCO:
        skill = new MfLocoSkill;
        break;
    case MfProtoSkill::MANIP:
        skill = new MfManipSkill;
        break;
    default:
        return 0;
    }
    skill->MfProtoSkill::Copy(proto);
    skill->UseSubclassData(proto);
    skill->ComputeLastTime();
    return skill;
}

// 0x100505e0 ?SetupFromTraversal@MfSkill@@UAEXPAVMfSkeletonTraversal@@@Z
void MfSkill::SetupFromTraversal(MfSkeletonTraversal *nodes)
{
    if (numJoints != nodes->numNodes()) {
        numJoints = nodes->numNodes();
        pJointInfo = new JointInfo[numJoints];
        for (int i = 0; i < numJoints; i++)
            pJointInfo[i].pKeyframes = 0;
    }
    for (int j = 0; j < numJoints; j++)
        pJointInfo[j].parentIndex = nodes->nodeIndex(nodes->node(j)->parent());
}

// 0x10050650 ?InsertTracks@MfSkill@@QAEXPAV1@PAVMfSkeletonTraversal@@1H@Z
// Copies the joint and texture tracks of other's nodes onto the matching
// nodes here (and with all its sound and event tracks).
void MfSkill::InsertTracks(MfSkill *other, MfSkeletonTraversal *otherNodes, MfSkeletonTraversal *nodes, int all)
{
    for (int i = 0; i < otherNodes->numNodes(); i++) {
        int j = nodes->nodeIndex(otherNodes->node(i));
        if (j >= 0) {
            if (pJointInfo[j].pKeyframes)
                delete pJointInfo[j].pKeyframes;
            if (other->pJointInfo[i].pKeyframes)
                pJointInfo[j].pKeyframes = new MfJointKeyList(*other->pJointInfo[i].pKeyframes);
            else
                pJointInfo[j].pKeyframes = new MfJointKeyList;
        }
    }
    MfDblListItem *pos;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&other->mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&other->mTextureTracks)->GetNext(pos)) {
        int node = nodes->nodeIndex(otherNodes->node(track->mNodeIndex));
        if (node >= 0) {
            MfTextureKeyList *copy = new MfTextureKeyList(*track);
            MfKeyListNode *kpos;
            for (MfTextureKeyframe *key = (MfTextureKeyframe *)copy->GetFirst(kpos); key;
                 key = (MfTextureKeyframe *)copy->GetNext(kpos))
                key->mNodeIndex = node;
            copy->mNodeIndex = node;
            (&mTextureTracks)->Add(copy);
        }
    }
    if (all) {
        if (mSoundTrack)
            delete mSoundTrack;
        mSoundTrack = other->mSoundTrack ? new MfKeyList(*other->mSoundTrack) : new MfKeyList;
        if (mEventTrack)
            delete mEventTrack;
        mEventTrack = other->mEventTrack ? new MfKeyList(*other->mEventTrack) : new MfKeyList;
    }
}

// 0x10050960 ??_GMfJointKeyList@@UAEPAXI@Z (local copy for `new MfJointKeyList`)

// 0x10050980 ?UseSubclassData@MfSkill@@UAEXABVMfProtoSkill@@@Z
void MfSkill::UseSubclassData(const MfProtoSkill &proto)
{
    if (!proto.subclassData)
        return;
    MfDblList *objects = &proto.subclassData->objects;
    MfDblListItem *pos;
    for (MfStreamObject *object = (MfStreamObject *)objects->GetFirst(pos); object;
         object = (MfStreamObject *)objects->GetNext(pos)) {
        if (object->StreamType() == MfProtoTextureKeyframe::STREAM_TYPE)
            AddProtoTextureKeyframe(*(MfProtoTextureKeyframe *)object);
        else if (object->StreamType() == MfProtoSoundKeyframe::STREAM_TYPE)
            AddProtoSoundKeyframe(*(MfProtoSoundKeyframe *)object);
        else if (object->StreamType() == MfProtoEventKeyframe::STREAM_TYPE)
            AddProtoEventKeyframe(*(MfProtoEventKeyframe *)object);
    }
}

// 0x10050a10 ?AddProtoTextureKeyframe@MfSkill@@IAEXABUMfProtoTextureKeyframe@@@Z
void MfSkill::AddProtoTextureKeyframe(const MfProtoTextureKeyframe &proto)
{
    MfDblListItem *pos;
    MfTextureKeyList *track;
    for (track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos)) {
        if (track->mNodeIndex == proto.nodeIndex && track->mMaterialIndex == proto.matIndex)
            break;
    }
    if (!track) {
        MfString name;
        name << "Material " << proto.matIndex << " of Node " << proto.nodeIndex;
        track = new MfTextureKeyList;
        track->mName = name;
        track->mMaterialIndex = proto.matIndex;
        track->mNodeIndex = proto.nodeIndex;
        (&mTextureTracks)->Add(track);
    }
    MfTextureKeyframe *key = new MfTextureKeyframe;
    {
        MfString texture(NULL_STRING);
        key->SetMaterial(texture, proto.material);
    }
    key->mMaterialIndex = proto.matIndex;
    key->mNodeIndex = proto.nodeIndex;
    track->Insert(key);
}

// 0x10050bb0 ?AddProtoSoundKeyframe@MfSkill@@IAEXABUMfProtoSoundKeyframe@@@Z
void MfSkill::AddProtoSoundKeyframe(const MfProtoSoundKeyframe &proto)
{
    MfSoundKeyframe *key = new MfSoundKeyframe;
    key->setTime(proto.time);
    key->SetName(proto.filename);
    key->SetVolume(proto.volume);
    key->mMute = proto.mute;
    key->mAmbient = proto.ambient;
    key->mStream = proto.stream;
    mSoundTrack->Insert(key);
}

// 0x10050c60 ?AddProtoEventKeyframe@MfSkill@@IAEXABUMfProtoEventKeyframe@@@Z
void MfSkill::AddProtoEventKeyframe(const MfProtoEventKeyframe &proto)
{
    MfEventKeyframe *key = new MfEventKeyframe;
    key->setTime(proto.time);
    key->mMessage = proto.eventName;
    key->mBroadcast = proto.useBroadcast;
    mEventTrack->Insert(key);
}

// 0x10050cf0 ?CreateSubclassData@MfSkill@@MAEXXZ
// Turns the texture, sound and event tracks back into prototype keyframes.
void MfSkill::CreateSubclassData()
{
    DeleteSubclassData();
    MfDblListItem *pos;
    MfKeyListNode *node;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos)) {
        for (MfTextureKeyframe *key = (MfTextureKeyframe *)track->GetFirst(node); key;
             key = (MfTextureKeyframe *)track->GetNext(node)) {
            MfProtoTextureKeyframe *proto = new MfProtoTextureKeyframe;
            proto->matIndex = track->mMaterialIndex;
            proto->nodeIndex = track->mNodeIndex;
            proto->material = key->mMaterial;
            AddTextureKeyframe(proto);
        }
    }
    for (MfSoundKeyframe *sound = (MfSoundKeyframe *)mSoundTrack->GetFirst(node); sound;
         sound = (MfSoundKeyframe *)mSoundTrack->GetNext(node)) {
        MfProtoSoundKeyframe *proto = new MfProtoSoundKeyframe;
        proto->time = sound->m08;
        proto->filename = sound->Name();
        proto->volume = sound->Volume();
        proto->mute = sound->mMute;
        proto->ambient = sound->mAmbient;
        proto->stream = sound->mStream;
        AddSoundKeyframe(proto);
    }
    for (MfEventKeyframe *event = (MfEventKeyframe *)mEventTrack->GetFirst(node); event;
         event = (MfEventKeyframe *)mEventTrack->GetNext(node)) {
        MfProtoEventKeyframe *proto = new MfProtoEventKeyframe;
        proto->time = event->m08;
        proto->eventName = event->mMessage;
        proto->useBroadcast = event->mBroadcast;
        AddEventKeyframe(proto);
    }
}

// 0x10050f30 ?normalize@MfSkill@@MAEXXZ
// Moves the sampled root path so that the skill starts with the hip above
// the origin (the height is kept).
void MfSkill::normalize()
{
    MfTransform3 start;
    MfTransform3 key;
    MfVector3 offset;
    if (numJoints < 2)
        return;
    MfJointPath *root = mSampledPath->Track(0);
    MfJointPath *hip = mSampledPath->Track(1);
    if (!root || !hip)
        return;
    int numKeys = root->NumKeys();
    start = root->Key(0) * hip->Key(0);
    offset = -start.translation();
    offset.y() = 0.0f;
    for (int i = 0; i < numKeys; i++) {
        key = root->Key(i);
        key.translation() = key.translation() + offset;
        root->SetKey(i, key);
    }
}

// TODO 0x10051140 ?factor@MfSkill@@MAEXXZ: the last assignment (hip = ~root * global) calls the
// imported MfTransform3 operator= and destructors where this inlines them, and the key temporaries
// take other frame slots (80% similar)
// Splits the hip's motion: the root path takes the horizontal position and
// the heading (relative to the first key), the hip the rest.
void MfSkill::factor()
{
    MfTransform3 hipKey;
    MfTransform3 rootKey;
    MfTransform3 global;
    if (numJoints < 2)
        return;
    MfJointPath *root = mSampledPath->Track(0);
    MfJointPath *hip = mSampledPath->Track(1);
    if (!root || !hip)
        return;
    int numKeys = root->NumKeys();
    float first;
    for (int i = 0; i < numKeys; i++) {
        rootKey = root->Key(i);
        hipKey = hip->Key(i);
        global = ~fwdFacingDir * rootKey * hipKey;
        rootKey.x() = global.x();
        rootKey.y() = 0.0f;
        rootKey.z() = global.z();
        MfVector3 dir;
        dir = global.rotation() * MfVector3(0.0f, 0.0f, 1.0f);
        if ((dir.y() < 0.0f ? -dir.y() : dir.y()) > 0.98)
            dir = global.rotation() * MfVector3(0.0f, 0.1f, 0.0f);
        float angle = atan2(dir.x(), dir.z());
        if (i == 0)
            first = angle;
        rootKey.rotation().setValue(angle - first, MfVector3(0.0f, 1.0f, 0.0f));
        hipKey = ~rootKey * global;
        root->SetKey(i, rootKey);
        hip->SetKey(i, hipKey);
    }
}

// EQUIV 0x10051550 ?apply@MfSkill@@UBEHKPAVMfSkeletonTraversal@@H@Z: reviewed: the code bytes are
// identical; only the place of the 1.0f constant in CONST differs (the original packs it between
// factor's 3.0f and 0.98 without padding)
// Sets the joints to their keys at a time (an IK key moves its chain to the
// key's place in the actor); joints without a key go back home. Returns
// whether a key was found.
int MfSkill::apply(unsigned long time, MfSkeletonTraversal *nodes, int skipRoot) const
{
    MfJointKeyframe key;
    int applied = 0;
    MfTransform3 base(((MfSkeletonNode *)nodes->node(0)->parent())->globalTransform());
    MfKChain chain;
    for (int i = skipRoot != 0; i < numJoints; i++) {
        MfSkeletonNode *node = nodes->node(i);
        if (i > 0 && ((MfSkeletonNode *)node->parent())->Type() != MfSkeletonNode::TYPE)
            ((MfSkeletonNode *)node->parent())->setTransform(((MfSkeletonNode *)node->parent())->mTransform);
        if (pJointInfo[i].pKeyframes && pJointInfo[i].pKeyframes->SampleAtTime(time, key, 1)) {
            applied = 1;
            node->setTransform(key.mXform);
            if (key.mFlags & 1) {
                if (chain.create(node, key.mUserData, key.mOffset)) {
                    chain.setStiffness(0.6f);
                    chain.moveTo(base * key.mWorld);
                    chain.updateSkeleton();
                }
            }
        } else
            node->setTransform(node->mTransform);
    }
    for (int j = 0; j < nodes->numNodes(); j++) {
        MfTransform3 local(nodes->node(j)->mTransform);
        local.rotation().normalize();
        nodes->node(j)->SetTransform(local);
    }
    return applied;
}

// 0x100518b0 ?lastTime@MfSkill@@QBEKXZ
unsigned long MfSkill::lastTime() const
{
    return mLastTime;
}

// 0x100518c0 ?ComputeLastTime@MfSkill@@QAEXXZ
// The last keyframe time of all tracks; a sampled skill keeps its time.
void MfSkill::ComputeLastTime()
{
    if (dataFormat == MF_SKILL_FILE_SAMPLES)
        return;
    mLastTime = 0;
    unsigned long time;
    if (pJointInfo) {
        for (int i = 0; i < numJoints; i++) {
            if (pJointInfo[i].pKeyframes) {
                time = pJointInfo[i].pKeyframes->LastTime();
                if (time > mLastTime)
                    mLastTime = time;
            }
        }
    }
    if (mSoundTrack) {
        time = mSoundTrack->LastTime();
        if (time > mLastTime)
            mLastTime = time;
    }
    MfDblListItem *pos;
    for (MfKeyList *track = (MfKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfKeyList *)(&mTextureTracks)->GetNext(pos)) {
        time = track->LastTime();
        if (time > mLastTime)
            mLastTime = time;
    }
    if (mEventTrack) {
        time = mEventTrack->LastTime();
        if (time > mLastTime)
            mLastTime = time;
    }
}

// 0x10051980 ?InitSounds@MfSkill@@QAEXPAVMfActor@@@Z
void MfSkill::InitSounds(MfActor *)
{
    MfKeyListNode *pos;
    for (MfSoundKeyframe *key = (MfSoundKeyframe *)mSoundTrack->GetFirst(pos); key;
         key = (MfSoundKeyframe *)mSoundTrack->GetNext(pos)) {
        if (!key->Load()) {
            MfString message("Unable to load the sound file \"");
            message << key->Name();
            message << MfString("\" for the skill \"") << sName;
            message << MfString("\".\nThe sound file is not in the actor's sound folder.");
        }
    }
}

// 0x10051ab0 ?ApplyTextures@MfSkill@@QAEXKPAVMfSkeletonTraversal@@@Z
void MfSkill::ApplyTextures(unsigned long time, MfSkeletonTraversal *nodes)
{
    MfDblListItem *pos;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos)) {
        MfTextureKeyframe *key = (MfTextureKeyframe *)track->LastKeyframeUntil(time);
        if (key)
            key->Apply(nodes);
        else if (track->NumKeyframes())
            ApplyOriginalMaterial(track, nodes);
    }
}

// 0x10051b20 ?RestoreOriginalTextures@MfSkill@@QAEXPAVMfSkeletonTraversal@@@Z
void MfSkill::RestoreOriginalTextures(MfSkeletonTraversal *nodes)
{
    MfDblListItem *pos;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos))
        ApplyOriginalMaterial(track, nodes);
}

// 0x10051b60 ?ApplyOriginalMaterial@MfSkill@@IAEXPAVMfTextureKeyList@@PAVMfSkeletonTraversal@@@Z
void MfSkill::ApplyOriginalMaterial(MfTextureKeyList *track, MfSkeletonTraversal *nodes)
{
    if (track->mNodeIndex != -1) {
        MfSkeletonNode *node = nodes->node(track->mNodeIndex);
        if (node && node->ProtoNode()) {
            MfMaterial *material = &node->ProtoNode()->mColModel->GetTriangleSet()->Material(track->mMaterialIndex);
            MfRenderNode *renderNode = node->GetRenderNode();
            if (renderNode && material)
                renderNode->ChangeMaterial(track->mMaterialIndex, material);
        }
    }
}

// 0x10051bc0 ?InitRenderMaterials@MfSkill@@UAEXXZ
void MfSkill::InitRenderMaterials()
{
    MfDblListItem *pos;
    for (MfTextureKeyList *track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos))
        track->InitMaterials();
}

// 0x10051bf0 ?CreateSkill@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
MfStreamObject *CreateSkill()
{
    return new MfSkill;
}

// 0x10051c50 _$E34 (original $E: the static initializer of the constructor entry, with 10051c60)
// 0x10051c60 _$E33
static MfStreamConstructorEntry constructorEntry(MfSkill::STREAM_TYPE, CreateSkill);

// 0x10051c80 ?StreamType@MfSkill@@UBEJXZ
long MfSkill::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10051540 ?IsValid@MfSkill@@UAEHAAVMfString@@PAVMfActor@@@Z
int MfSkill::IsValid(MfString &, MfActor *)
{
    return mValidSkill = 1;
}

// 0x10051820 ?sample@MfSkill@@UBEHKPAVMfSkeletonTraversal@@PAH@Z
int MfSkill::sample(unsigned long time, MfSkeletonTraversal *nodes, int *masks) const
{
    return mSampledPath->Sample(time, nodes, masks, bInterpolate);
}

// 0x10051840 ?sample@MfSkill@@UBEHKPAVMfSkeletonTraversal@@H@Z
int MfSkill::sample(unsigned long time, MfSkeletonTraversal *nodes, int flag) const
{
    return mSampledPath->Sample(time, nodes, bInterpolate, flag != 0, 0);
}

// 0x10051870 ?sample@MfSkill@@UBEHKPAVMfSkeletonTraversal@@ABVMfTransform3@@@Z
int MfSkill::sample(unsigned long time, MfSkeletonTraversal *nodes, const MfTransform3 &base) const
{
    return mSampledPath->Sample(time, nodes, bInterpolate, 0, &base);
}

// 0x10051890 ?sample@MfSkill@@QAEHKPAVMfSkeletonNode@@H@Z
int MfSkill::sample(unsigned long time, MfSkeletonNode *node, int index)
{
    return mSampledPath->Sample(time, node, index, bInterpolate);
}

// 0x10051c90 ?ReadFromStream@MfSkill@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSkill::ReadFromStream(MfStream &stream)
{
    MfProtoSkill::ReadFromStream(stream);
    MfFileProgress::UpdateProgress();
    if (stream.objectType == MfProtoSkill::STREAM_TYPE)
        return stream.Status();
    if (dataFormat == MF_SKILL_FILE_SAMPLES) {
        mSampledPath = new MfActorPath(numJoints);
        for (int i = 0; i < numJoints; i++) {
            int hasTrack;
            stream >> hasTrack;
            if (hasTrack)
                mSampledPath->NewTrack(i)->ReadFromStream(stream);
        }
    }
    int hasSounds;
    int hasEvents;
    stream >> hasSounds;
    if (hasSounds) {
        if (mSoundTrack)
            delete mSoundTrack;
        mSoundTrack = new MfKeyList;
        mSoundTrack->ReadFromStream(stream);
    }
    if (stream.userFormat > 40) {
        stream >> hasEvents;
        if (hasEvents) {
            if (mEventTrack)
                delete mEventTrack;
            mEventTrack = new MfKeyList;
            mEventTrack->ReadFromStream(stream);
        }
    }
    int numTextureTracks;
    stream >> numTextureTracks;
    for (int j = 0; j < numTextureTracks; j++) {
        MfTextureKeyList *track = new MfTextureKeyList;
        track->ReadFromStream(stream);
        (&mTextureTracks)->Add(track);
    }
    if (stream.userFormat > 25)
        stream >> (int &)mLastTime;
    if (stream.userFormat < 34)
        mValidSkill = 1;
    else
        stream >> mValidSkill;
    if (dataFormat != MF_SKILL_FILE_SAMPLES)
        ComputeLastTime();
    return stream.Status();
}

// 0x10051f00 ?WriteToStream@MfSkill@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSkill::WriteToStream(MfStream &stream) const
{
    MfProtoSkill::WriteToStream(stream);
    if (dataFormat == MF_SKILL_FILE_SAMPLES) {
        for (int i = 0; i < numJoints; i++) {
            MfJointPath *track = mSampledPath->Track(i);
            if (track)
                stream << 1 << *track;
            else
                stream << 0;
        }
    }
    stream << (int)(mSoundTrack != 0);
    if (mSoundTrack)
        stream << *mSoundTrack;
    stream << (int)(mEventTrack != 0);
    if (mEventTrack)
        stream << *mEventTrack;
    stream << mTextureTracks.GetCount();
    MfDblListItem *pos;
    for (MfKeyList *track = (MfKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfKeyList *)(&mTextureTracks)->GetNext(pos))
        stream << *track;
    stream << (int)mLastTime << mValidSkill;
    return stream.Status();
}

// 0x10051ff0 ?GetInterfaceName@MfSkill@@UAEXPADHAAH1@Z
void MfSkill::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiSkill", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10052030 ?GetStartTime@MfSkill@@UAEKXZ
unsigned long MfSkill::GetStartTime()
{
    return firstTime();
}

// 0x10052040 ?GetDuration@MfSkill@@UAEKXZ
unsigned long MfSkill::GetDuration()
{
    unsigned long duration = lastTime();
    duration -= firstTime();
    return duration;
}

// 0x10052060 ?Sample@MfSkill@@UAEXPBVMfAiActor@@ABVMfString@@K@Z
// Sets the subtree below the named node to the skill at a time.
void MfSkill::Sample(const MfAiActor *aiActor, const MfString &rootName, unsigned long time)
{
    MfActor *actor = (MfActor *)aiActor;
    int root = actor->skeletonTraversal()->NodeIndex(rootName);
    if (root >= 0 && root < actor->mSkeletonTraversal->numNodes()) {
        if (mSampledPath)
            mSampledPath->Sample(time, actor->mSkeletonTraversal, bInterpolate, root, 0);
    } else {
        MfString message;
        message = MfString("Skill.Sample()   Bad node name used as the root of the sample.");
        MfString source("Skill");
    }
}

// EQUIV 0x10052170 ?Sample@MfSkill@@UAEHAAVMfTransform3@@PBVMfAiActor@@ABVMfString@@K@Z: reviewed: stack
// slots only: the original gives the strings of the two message branches separate slots, so its frame
// is 8 bytes larger; code and EH states are the same
// The local transform of the named node at a time.
int MfSkill::Sample(MfTransform3 &xform, const MfAiActor *aiActor, const MfString &nodeName, unsigned long time)
{
    MfActor *actor = (MfActor *)aiActor;
    int node = actor->skeletonTraversal()->NodeIndex(nodeName);
    if (node < 0 || node >= actor->mSkeletonTraversal->numNodes()) {
        MfString message;
        message = MfString("Skill.Sample()   Bad node name.");
        MfString source("Skill");
    } else if (mSampledPath) {
        MfJointPath *track = mSampledPath->Track(node);
        if (track) {
            xform = track->Sample(time, bInterpolate);
            return 1;
        }
    } else {
        MfString message;
        message = MfString("Skill.Sample()   The skill is not presampled.");
        MfString source("Skill");
    }
    return 0;
}

// 0x10052320 ?SampleWrtTrace@MfSkill@@UAEHAAVMfTransform3@@PBVMfAiActor@@ABVMfString@@K@Z
// The transform of the named node relative to the actor's root at a time
// (the skeleton is put back afterwards).
int MfSkill::SampleWrtTrace(MfTransform3 &xform, const MfAiActor *aiActor, const MfString &nodeName,
                            unsigned long time)
{
    MfActor *actor = (MfActor *)aiActor;
    if (mSampledPath) {
        MfSkeletonTraversal *nodes = actor->mSkeletonTraversal;
        MfTransform3 *saved = nodes->CopyTransforms();
        mSampledPath->Sample(time, nodes, bInterpolate, 0, 0);
        MfSkeletonNode *node = actor->skeletonTraversal()->Node(nodeName);
        if (node) {
            MfTransform3 root(actor->mSkeleton->globalTransform());
            xform = ~root * node->globalTransform();
        } else {
            MfString message;
            message = MfString("Skill.Sample()   Bad node name.");
            MfString source("Skill");
        }
        nodes->SetTransforms(saved);
        delete[] saved;
        if (node)
            return 1;
    }
    return 0;
}

// EQUIV 0x10052510 ?GetHandNodes@MfSkill@@UAEHAAVMfDblList@@PBVMfAiActor@@@Z: reviewed: in the loop the traversal and the hand index swap edx/ecx; every instruction is the same otherwise
int MfSkill::GetHandNodes(MfDblList &nodes, const MfAiActor *actor)
{
    MfManipSkill *manip = (MfManipSkill *)this;
    if (manip->IsType(MfManipSkill::TYPE) && actor) {
        MfDblList *hands = &manip->mHandList;
        MfDblListItem *pos;
        for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(pos); hand;
             hand = (MfManipHand *)hands->GetNext(pos))
            nodes.Add((MfAiActorNode *)((MfActor *)actor)->skeletonTraversal()->node(hand->mNode));
        return 1;
    }
    return 0;
}

// EQUIV 0x10052590 ?GetFootNodes@MfSkill@@UAEHAAVMfDblList@@PBVMfAiActor@@@Z: reviewed: in both loops the node index and the traversal (two independent loads) come in the other order, with edx/eax swapped; nothing else differs
int MfSkill::GetFootNodes(MfDblList &nodes, const MfAiActor *actor)
{
    nodes.Clear();
    MfSkill *skill = this;
    if (skill->IsType(MfManipSkill::TYPE) && actor) {
        MfDblList *anchors = &((MfManipSkill *)skill)->mAnchorList;
        MfDblListItem *pos;
        for (int *anchor = (int *)anchors->GetFirst(pos); anchor; anchor = (int *)anchors->GetNext(pos))
            nodes.Add((MfAiActorNode *)((MfActor *)actor)->skeletonTraversal()->node(*anchor));
        return 1;
    }
    if (skill->IsType(MfLocoSkill::TYPE) && actor) {
        MfDblList *legs = &((MfLocoSkill *)skill)->mLegList;
        MfDblListItem *pos;
        for (int *leg = (int *)legs->GetFirst(pos); leg; leg = (int *)legs->GetNext(pos))
            nodes.Add((MfAiActorNode *)((MfActor *)actor)->skeletonTraversal()->node(*leg));
        return 1;
    }
    return 0;
}

// TODO 0x100526d0 ?CheckContact@MfSkill@@UAEHPBVMfAiActorNode@@KK@Z: the wrap of the two times into the
// cycle keeps the product in eax and the time in ecx (this keeps them the other way round and spills
// the quotient), `from -= q * d`, `from = from - ...` give the same; the rest follows (83% similar)
// Whether the foot node of a locomotion skill touches the ground between two
// times (taken within the cycle).
int MfSkill::CheckContact(const MfAiActorNode *aiNode, unsigned long from, unsigned long to)
{
    if (!IsType(MfLocoSkill::TYPE) || !aiNode)
        return 0;
    MfLocoSkill *loco = (MfLocoSkill *)this;
    unsigned long step = loco->mFootSampleStep;
    MfActor *actor = (MfActor *)((MfAiActorNode *)aiNode)->GetActor();
    if (from > GetDuration())
        from -= from / GetDuration() * GetDuration();
    if (to > GetDuration())
        to -= to / GetDuration() * GetDuration();
    int first = from / step;
    int last = to / step;
    int numSamples = loco->mNumFootSamples;
    if (from < to) {
        last++;
        if (last >= numSamples)
            last = numSamples - 1;
    }
    if (first == last) {
        first++;
        if (first > numSamples)
            first = numSamples;
    }
    MfSkeletonNode *node = (MfSkeletonNode *)aiNode;
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&loco->mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&loco->mLegList)->GetNext(pos)) {
        if (node == actor->mSkeletonTraversal->node(leg->mNode)) {
            int i;
            if (from >= to) {
                for (i = last; i <= first; i++)
                    if (leg->m0fc[i])
                        return 1;
            } else {
                for (i = last; i < numSamples; i++)
                    if (leg->m0fc[i])
                        return 1;
                for (i = 0; i <= first; i++)
                    if (leg->m0fc[i])
                        return 1;
            }
        }
    }
    return 0;
}

// 0x10052690 ?BestMatchIndex@MfSkill@@UAEKPBVMfAiActor@@@Z
unsigned long MfSkill::BestMatchIndex(const MfAiActor *actor)
{
    if (IsType(MfLocoSkill::TYPE) && actor)
        return ((MfLocoSkill *)this)->BestMatchIndex((MfActor *)actor);
    return 0;
}

// EQUIV 0x10052920 ?NextContact@MfSkill@@UAEHKPBVMfAiActor@@AAKAAVMfDblList@@@Z: reviewed: only the
// `mov ebx, esi` of best = i moves (before the Clear here, right before the Add in the original);
// every other instruction is the same
// The next time after time at which a foot of a locomotion skill touches
// down that was not in contact before; nodes gets the feet that do.
int MfSkill::NextContact(unsigned long time, const MfAiActor *aiActor, unsigned long &next, MfDblList &nodes)
{
    MfDblList previous;
    previous.Copy(nodes);
    nodes.Clear();
    if (!IsType(MfLocoSkill::TYPE))
        return 0;
    MfLocoSkill *loco = (MfLocoSkill *)this;
    MfActor *actor = (MfActor *)aiActor;
    int step = time / loco->mFootSampleStep + 1;
    int numSamples = loco->mNumFootSamples;
    int best = numSamples;
    int noneAhead = 1;
    MfDblListItem *pos;
    for (MfLocoLeg *leg = (MfLocoLeg *)(&loco->mLegList)->GetFirst(pos); leg;
         leg = (MfLocoLeg *)(&loco->mLegList)->GetNext(pos)) {
        MfAiActorNode *node = actor->mSkeletonTraversal->node(leg->mNode);
        if (previous.Find(node))
            continue;
        int i;
        for (i = step; i < numSamples; i++) {
            if (leg->m0fc[i]) {
                noneAhead = 0;
                if (i <= best) {
                    if (i < best)
                        best = i;
                    nodes.Clear();
                    nodes.Add(node);
                }
                if (best < step) {
                    best = i;
                    nodes.Clear();
                    nodes.Add(node);
                }
                break;
            }
        }
        if (noneAhead) {
            for (i = 0; i < step; i++) {
                if (leg->m0fc[i]) {
                    if (i <= best) {
                        if (i < best)
                            nodes.Clear();
                        nodes.Add(node);
                        best = i;
                    }
                    break;
                }
            }
        }
    }
    if (best < numSamples) {
        next = best * loco->mFootSampleStep;
        return 1;
    }
    return 0;
}

// 0x100528a0 ?GetHandleTransform@MfSkill@@UAEXAAVMfTransform3@@@Z
void MfSkill::GetHandleTransform(MfTransform3 &xform)
{
    MfManipSkill *manip = (MfManipSkill *)this;
    if (manip->IsType(MfManipSkill::TYPE))
        xform = manip->mHandleWrtTrace;
    else
        xform = MfTransform3();
}

// EQUIV 0x10052b40 ?PermuteJoints@MfSkill@@UAEXPAVMfProtoActor@@PAVMfPermutation@@@Z: reviewed: in the key
// loop the address of the node cursor goes through eax/ecx instead of edx/eax and the store of the
// node index swaps with that lea; nothing else differs
// The texture tracks follow the new node order; those of removed nodes are
// dropped.
void MfSkill::PermuteJoints(MfProtoActor *proto, MfPermutation *permutation)
{
    MfProtoSkill::PermuteJoints(proto, permutation);
    MfDblList removed;
    MfDblList *list = &removed;
    MfDblListItem *pos;
    MfTextureKeyList *track;
    for (track = (MfTextureKeyList *)(&mTextureTracks)->GetFirst(pos); track;
         track = (MfTextureKeyList *)(&mTextureTracks)->GetNext(pos)) {
        int node = track->mNodeIndex;
        int newNode;
        if (node >= 0 && node < permutation->field_0 && (newNode = permutation->mMap[node]) != -1) {
            track->mNodeIndex = newNode;
            MfKeyListNode *kpos;
            for (MfTextureKeyframe *key = (MfTextureKeyframe *)track->GetFirst(kpos); key;
                 key = (MfTextureKeyframe *)track->GetNext(kpos))
                key->mNodeIndex = newNode;
        } else
            list->Append(track);
    }
    for (track = (MfTextureKeyList *)removed.GetFirst(pos); track;
         track = (MfTextureKeyList *)removed.GetNext(pos)) {
        MfDblListItem *item = (&mTextureTracks)->Find(track);
        if (item)
            (&mTextureTracks)->Delete(item);
    }
}

// 0x10052c80 ?ScaleJoint@MfSkill@@UAEXHABVMfVector3@@@Z
// Scales the sampled path of the joint and of every joint below it.
void MfSkill::ScaleJoint(int joint, const MfVector3 &scale)
{
    MfProtoSkill::ScaleJoint(joint, scale);
    if (mSampledPath) {
        // the joints below come right after the joint
        for (int i = joint; i < numJoints; i++) {
            int ancestor = i;
            while (ancestor > joint)
                ancestor = pJointInfo[ancestor].parentIndex;
            if (ancestor != joint)
                break;
            mSampledPath->ScaleTrack(i, scale);
        }
    }
}

// TODO 0x10052ce0 ?Presample@MfSkill@@UAEXPAVMfActor@@@Z: the original sets each track's interval with the
// out-of-line MfJointPath::SetInterval (10030100; declaring it flips MfJointPath::Key/SetKey, see
// NOTES), this stores mInterval; it also keeps the key count in memory where this keeps it in ebx
// (77% similar)
// Samples the joint tracks into a path (at least five keys) and splits the
// hip's motion into the root path.
void MfSkill::Presample(MfActor *actor)
{
    MfSkeletonTraversal *nodes = actor->mSkeletonTraversal;
    int interval = iSampleRate;
    if (mSampledPath) {
        delete mSampledPath;
        mSampledPath = 0;
    }
    if (!numJoints)
        return;
    int last = lastTime();
    int numKeys = last / interval + 1;
    if (numKeys < 5) {
        numKeys = 5;
        interval = last / 4;
    }
    MfTransform3 *saved = nodes->CopyTransforms();
    mSampledPath = new MfActorPath(numJoints);
    int i;
    for (i = 0; i < numJoints; i++) {
        if (pJointInfo[i].pKeyframes && pJointInfo[i].pKeyframes->NumKeyframes()) {
            MfJointPath *track = mSampledPath->NewTrack(i);
            track->mInterval = interval;
            track->Allocate(numKeys);
        }
    }
    int time = 0;
    for (int k = 0; k < numKeys; k++) {
        apply(k == numKeys - 1 ? last : time, nodes, 0);
        for (i = 0; i < numJoints; i++) {
            MfJointPath *track = mSampledPath->Track(i);
            if (track)
                track->SetKey(k, nodes->node(i)->mTransform);
        }
        time += interval;
    }
    nodes->SetTransforms(saved);
    delete[] saved;
    factor();
    mSampledPath->Compact();
    ComputeInfo(actor);
    actor->mSkeleton->syncChildTransforms(0);
}

// 0x10052ed0 ?ComputeInfo@MfSkill@@UAEXPAVMfActor@@@Z
void MfSkill::ComputeInfo(MfActor *)
{
    ComputeLastTime();
}

// the sound and the event last started (so that a key sampled twice plays once)
static MfSoundKeyframe *sLastSound;
static MfEventKeyframe *sLastEvent;

// 0x10052ee0 ?UpdateExtraTracks@MfSkill@@UAEXPAVMfAiActor@@HKK@Z
// Plays the sounds, sends the events and puts on the textures of the keys
// between two times, forward or backward (a range over the end of the skill
// is split in two).
void MfSkill::UpdateExtraTracks(MfAiActor *aiActor, int forward, unsigned long from, unsigned long to)
{
    MfActor *actor = (MfActor *)aiActor;
    if (forward && to <= from) {
        if (to == from) {
            UpdateExtraTracks(aiActor, forward, from, to + 1);
            return;
        }
        if (from < lastTime())
            UpdateExtraTracks(aiActor, forward, from, lastTime());
        if (to > 0)
            UpdateExtraTracks(aiActor, forward, 0, to);
        return;
    }
    if (!forward && to >= from) {
        if (to == from) {
            UpdateExtraTracks(aiActor, forward, from + 1, to);
            return;
        }
        if (from > 0)
            UpdateExtraTracks(aiActor, 0, from, 0);
        if (to < lastTime())
            UpdateExtraTracks(aiActor, 0, lastTime(), to);
        return;
    }
    MfSoundKeyframe *sound = forward ? soundEvent(from, to) : soundEvent(to, from);
    if (sound && sound != sLastSound)
        sound->Play(actor->skeleton());
    sLastSound = sound;
    MfEventKeyframe *event = 0;
    MfDblList events;
    if (forward)
        triggerEvent(from, to, events);
    else
        triggerEvent(to, from, events);
    MfDblListItem *pos;
    for (MfEventKeyframe *key = (MfEventKeyframe *)events.GetFirst(pos); key;
         key = (MfEventKeyframe *)events.GetNext(pos)) {
        event = key;
        if (event && event->mBroadcast && event != sLastEvent)
            actor->SendEvent(0, event->mMessage, 0);
        else if (event && event != sLastEvent)
            actor->SendEvent(actor, event->mMessage, 0);
    }
    sLastEvent = event;
    ApplyTextures(to, actor->mSkeletonTraversal);
}

// 0x10053130 ?ConvertSkillSubclassData@@YAPAVMfProtoSkill@@ABV1@@Z
// The runtime skill of a prototype (with its tracks).
MfProtoSkill *ConvertSkillSubclassData(const MfProtoSkill &proto)
{
    MfSkill *skill;
    switch (proto.Type()) {
    case MfProtoSkill::GESTURE:
        skill = new MfSkill;
        break;
    case MfProtoSkill::LOCO:
        skill = new MfLocoSkill;
        break;
    case MfProtoSkill::MANIP:
        skill = new MfManipSkill;
        break;
    default:
        return 0;
    }
    skill->MfProtoSkill::Copy(proto);
    skill->UseSubclassData(proto);
    skill->ComputeLastTime();
    return skill;
}

// Not claimed: in the original this member is folded with an identical function
// (1007dfb0); the port needs its own definition.
void MfSkill::SetRadius(float)
{
}

// Not claimed: in the original this member is folded with an identical function
// (1000de60); the port needs its own definition.
unsigned long MfSkill::firstTime() const
{
    return 0;
}
