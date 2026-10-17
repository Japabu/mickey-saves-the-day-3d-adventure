// MfActorLib.dll 0x10018a00-0x10019800: MfProtoSkill.
#include "MfProtoSkill.h"
#include "MfProtoActor.h"
#include "MfProtoTraversal.h"
#include "MfPermutation.h"
#include "MfProtoNode.h"
#include "MfJointKeyList.h"
#include "MfJointKeyframe.h"
#include "MfStream.h"
#include "MfProtoTextureKeyframe.h"
#include "MfProtoSoundKeyframe.h"
#include "MfProtoEventKeyframe.h"
#include "MfProtoFootData.h"
#include "MfProtoHandData.h"
#include "MfProtoAnchorData.h"
#include "MfProtoManipData.h"
#include "MfStreamConstructorEntry.h"

const int MfProtoSkill::fileVersion = 106;
const int MfProtoSkill::fileType = 'MFSk';

// 0x10018a00 ??0MfProtoSkill@@QAE@XZ
MfProtoSkill::MfProtoSkill()
    : dataFormat(MF_SKILL_FILE_KEYFRAMES), type(0), userVersion(1), numJoints(0),
      pJointInfo(0), refCnt(1), locked(0), iSampleRate(33), bInterpolate(0),
      subclassData(0)
{
}

// 0x10018a80 ??_EMfProtoSkill@@UAEPAXI@Z (compiler-generated)

// 0x10018ad0 ??0MfProtoSkill@@QAE@ABV0@@Z
MfProtoSkill::MfProtoSkill(const MfProtoSkill &skill)
    : type(skill.type), userVersion(1), numJoints(0), pJointInfo(0), refCnt(1),
      locked(0), subclassData(0)
{
    Copy(skill);
}

// 0x10018b80 ??1MfProtoSkill@@UAE@XZ
MfProtoSkill::~MfProtoSkill()
{
    Clear();
}

// 0x10018be0 ?Clear@MfProtoSkill@@MAEXXZ
void MfProtoSkill::Clear()
{
    if (pJointInfo) {
        for (int i = 0; i < numJoints; i++)
            delete pJointInfo[i].pKeyframes;
        delete pJointInfo;
        pJointInfo = 0;
        numJoints = 0;
    }
    DeleteSubclassData();
}

// 0x10018c40 ?Copy@MfProtoSkill@@UAEXABV1@@Z
void MfProtoSkill::Copy(const MfProtoSkill &skill)
{
    Clear();
    type = skill.type;
    sName = skill.sName;
    userVersion = skill.userVersion;
    bInterpolate = skill.bInterpolate;
    numJoints = skill.numJoints;
    pJointInfo = new JointInfo[numJoints];
    for (int i = 0; i < numJoints; i++) {
        pJointInfo[i].parentIndex = skill.pJointInfo[i].parentIndex;
        if (skill.pJointInfo[i].pKeyframes)
            pJointInfo[i].pKeyframes = new MfJointKeyList(*skill.pJointInfo[i].pKeyframes);
        else
            pJointInfo[i].pKeyframes = 0;
    }
    dataFormat = skill.dataFormat;
}

// 0x10018d50 ?Duplicate@MfProtoSkill@@UAEPAV1@XZ
MfProtoSkill *MfProtoSkill::Duplicate()
{
    return new MfProtoSkill(*this);
}

// EQUIV 0x10018db0 ?SetupFromPrototype@MfProtoSkill@@UAEXPAVMfProtoActor@@@Z: reviewed: order of independent instructions: numJoints is stored before or after the byte count is computed for new
// One (empty) track per node of the actor's skeleton.
void MfProtoSkill::SetupFromPrototype(MfProtoActor *actor)
{
    int n = actor->GeomTraversal()->NumNodes();
    if (numJoints != n) {
        numJoints = n;
        pJointInfo = new JointInfo[numJoints];
        for (int i = 0; i < numJoints; i++)
            pJointInfo[i].pKeyframes = 0;
    }
    SetParentIndices(actor);
}

// 0x10018e00 ?SetParentIndices@MfProtoSkill@@QAEXPAVMfProtoActor@@@Z
void MfProtoSkill::SetParentIndices(MfProtoActor *actor)
{
    MfProtoTraversal *nodes = actor->GeomTraversal();
    for (int i = 0; i < numJoints; i++)
        pJointInfo[i].parentIndex = nodes->NodeIndex((MfProtoNode *)nodes->Node(i)->parent());
}

// 0x10018e40 ?InitJoint@MfProtoSkill@@QAEPAVMfJointKeyList@@H@Z
MfJointKeyList *MfProtoSkill::InitJoint(int i)
{
    if (!pJointInfo[i].pKeyframes)
        pJointInfo[i].pKeyframes = new MfJointKeyList;
    return pJointInfo[i].pKeyframes;
}

// 0x10018ec0 ?PermuteJoints@MfProtoSkill@@UAEXPAVMfProtoActor@@PAVMfPermutation@@@Z
// Sets the skill up for the actor's new skeleton and moves each track to the
// joint it maps to; the tracks of joints that are gone are deleted.
void MfProtoSkill::PermuteJoints(MfProtoActor *actor, MfPermutation *perm)
{
    int oldNumJoints = numJoints;
    JointInfo *oldJointInfo = pJointInfo;
    pJointInfo = 0;
    numJoints = 0;
    SetupFromPrototype(actor);
    int i;
    for (i = 0; i < numJoints; i++) {
        if (perm->mToToFrom[i] != -1) {
            pJointInfo[i].pKeyframes = oldJointInfo[perm->mToToFrom[i]].pKeyframes;
            oldJointInfo[perm->mToToFrom[i]].pKeyframes = 0;
        }
    }
    for (i = 0; i < oldNumJoints; i++)
        delete oldJointInfo[i].pKeyframes;
    delete oldJointInfo;
}

// EQUIV 0x10018f50 ?TransformJoint@MfProtoSkill@@UAEXHABVMfTransform3@@H@Z: reviewed: MfRotation3/MfVector3::operator= inlined in one and called through the import in the other; register choice
// Applies t to the joint's keys; with moveChildren the keys of the child
// joints get the inverse so that they stay where they are.
void MfProtoSkill::TransformJoint(int joint, const MfTransform3 &t, int moveChildren)
{
    MfTransform3 xform;     // not used
    MfTransform3 inverse = ~t;
    MfJointKeyList *keys = pJointInfo[joint].pKeyframes;
    MfKeyListNode *node;
    MfJointKeyframe *key;
    if (keys) {
        for (key = keys->GetFirst(node); key; key = keys->GetNext(node)) {
            key->mWorld = key->mWorld * t;
            key->mXform = key->mXform * t;
            for (int i = 0; i < 2; i++) {
                key->mWorldCtrl[i] = key->mWorldCtrl[i] * t;
                key->mXformCtrl[i] = key->mXformCtrl[i] * t;
                key->mOffset = inverse * key->mOffset;
            }
        }
    }
    if (moveChildren) {
        for (int i = 0; i < numJoints; i++) {
            if (pJointInfo[i].parentIndex == joint && pJointInfo[i].pKeyframes) {
                MfJointKeyList *child = pJointInfo[i].pKeyframes;
                for (key = child->GetFirst(node); key; key = child->GetNext(node))
                    key->mXform = inverse * key->mXform;
            }
        }
    }
}

// 0x10019240 ?ScaleJoint@MfProtoSkill@@UAEXHABVMfVector3@@@Z
// Scales the track of the joint and of all joints below it.
void MfProtoSkill::ScaleJoint(int joint, const MfVector3 &scale)
{
    for (int i = joint; i < numJoints; i++) {
        int parent = i;
        while (parent > joint)
            parent = pJointInfo[parent].parentIndex;
        if (parent != joint)
            break;
        MfJointKeyList *keys = pJointInfo[i].pKeyframes;
        if (keys) {
            MfKeyListNode *node;
            for (MfJointKeyframe *key = keys->GetFirst(node); key; key = keys->GetNext(node))
                key->Scale(scale);
        }
    }
}

// 0x100192b0 ?DeleteSubclassData@MfProtoSkill@@QAEXXZ
void MfProtoSkill::DeleteSubclassData()
{
    if (subclassData) {
        MfDblListItem *pli;
        for (MfStreamObject *object = (MfStreamObject *)subclassData->objects.GetFirst(pli); object;
             object = (MfStreamObject *)subclassData->objects.GetNext(pli))
            delete object;
        delete subclassData;
        subclassData = 0;
    }
}

// 0x10019320 ?AddSubclassObject@MfProtoSkill@@IAEXQAVMfStreamObject@@@Z
void MfProtoSkill::AddSubclassObject(MfStreamObject *const object)
{
    if (!subclassData)
        subclassData = new SubclassData;
    subclassData->objects.Add(object);
}

// 0x10019380 ?AddAnchor@MfProtoSkill@@QAEXQAUMfProtoAnchorData@@@Z
// (AddTextureKeyframe ... AddManip are the same code; the linker keeps one copy)
void MfProtoSkill::AddAnchor(MfProtoAnchorData *const anchor)
{
    AddSubclassObject(anchor);
}

void MfProtoSkill::AddTextureKeyframe(MfProtoTextureKeyframe *const key)
{
    AddSubclassObject(key);
}

void MfProtoSkill::AddSoundKeyframe(MfProtoSoundKeyframe *const key)
{
    AddSubclassObject(key);
}

void MfProtoSkill::AddEventKeyframe(MfProtoEventKeyframe *const key)
{
    AddSubclassObject(key);
}

void MfProtoSkill::AddFoot(MfProtoFootData *const foot)
{
    AddSubclassObject(foot);
}

void MfProtoSkill::AddHand(MfProtoHandData *const hand)
{
    AddSubclassObject(hand);
}

void MfProtoSkill::AddManip(MfProtoManipData *const manip)
{
    AddSubclassObject(manip);
}

const long MfProtoSkill::STREAM_TYPE = 'SklP';

// 0x10019390 ?CreateProtoSkill@@YAPAVMfStreamObject@@XZ (stream constructor for 'SklP')
static MfStreamObject *CreateProtoSkill()
{
    return new MfProtoSkill;
}

// 0x100193f0 _$E31 (jumps to _$E30)
// 0x10019400 _$E30
static MfStreamConstructorEntry s_entry(MfProtoSkill::STREAM_TYPE, CreateProtoSkill);

// 0x10019420 ?StreamType@MfProtoSkill@@UBEJXZ
long MfProtoSkill::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10019430 ?ReadFromStream@MfProtoSkill@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
// Skill files before version 38 start with a 16 byte header of their own.
MfStreamError MfProtoSkill::ReadFromStream(MfStream &s)
{
    int i, flag;
    if (s.userFormat > 0 && s.userFormat < 38) {
        int unused, format;
        s.Seek(-16);
        s >> unused >> format >> unused >> unused;
        dataFormat = (MfSkillFormat)format;
    } else {
        s >> s.userFormat >> (int &)dataFormat;
    }
    s >> userVersion >> sName;
    if (s.userFormat >= 102)
        s >> type;
    else
        type = 0;
    s >> numJoints;
    pJointInfo = new JointInfo[numJoints];
    if (dataFormat == MF_SKILL_FILE_KEYFRAMES) {
        for (i = 0; i < numJoints && s.Status() == MF_STREAM_OK; i++) {
            s >> flag;
            if (flag) {
                pJointInfo[i].pKeyframes = new MfJointKeyList;
                s >> *pJointInfo[i].pKeyframes;
            } else {
                pJointInfo[i].pKeyframes = 0;
            }
        }
    } else {
        for (i = 0; i < numJoints; i++)
            pJointInfo[i].pKeyframes = 0;
    }
    if (s.userFormat > 36)
        s.ReadBytes(sizeof(fwdFacingDir), &fwdFacingDir);
    if (s.userFormat > 37)
        s >> iSampleRate >> bInterpolate;
    DeleteSubclassData();
    if (s.userFormat >= 102) {
        int hasSubclassData;
        s >> hasSubclassData;
        if (hasSubclassData) {
            subclassData = new SubclassData;
            int n;
            s >> n;
            for (i = 0; i < n; i++)
                subclassData->objects.Append(s.ReadObject(0));
        }
    }
    if (s.userFormat >= 103)
        s.ReadBytes(sizeof(conversion), &conversion);
    return s.Status();
}

// 0x100196a0 ?WriteToStream@MfProtoSkill@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoSkill::WriteToStream(MfStream &s) const
{
    s.userFormat = fileVersion;
    s << fileVersion;
    s << (int)dataFormat;
    s << userVersion << sName;
    s << type;
    s << numJoints;
    if (dataFormat == MF_SKILL_FILE_KEYFRAMES) {
        for (int i = 0; i < numJoints && s.Status() == MF_STREAM_OK; i++) {
            int hasKeyframes = pJointInfo[i].pKeyframes != 0;
            s << hasKeyframes;
            if (hasKeyframes)
                s << *pJointInfo[i].pKeyframes;
        }
    }
    s.WriteBytes(sizeof(fwdFacingDir), &fwdFacingDir);
    s << iSampleRate << bInterpolate;
    s << (int)(subclassData != 0);
    if (subclassData) {
        s << subclassData->objects.GetCount();
        MfDblListItem *pli;
        for (MfStreamObject *object = (MfStreamObject *)subclassData->objects.GetFirst(pli); object;
             object = (MfStreamObject *)subclassData->objects.GetNext(pli))
            s.WriteObject(object);
    }
    s.WriteBytes(sizeof(conversion), &conversion);
    return s.Status();
}

// 0x100197d0 ?AddRef@MfProtoSkill@@UAEHXZ
int MfProtoSkill::AddRef()
{
    return ++refCnt;
}

// 0x100197e0 ?Release@MfProtoSkill@@UAEHXZ
int MfProtoSkill::Release()
{
    int count = refCnt--;
    if (refCnt == 0)
        delete this;
    return count;
}
