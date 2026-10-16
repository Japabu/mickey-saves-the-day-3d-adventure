#include <windows.h>
#include "MfProtoActor.h"
#include "MfProperty.h"
#include "MfStringValue.h"
#include "MfFloatValue.h"
#include "MfVector3Value.h"
#include "MfProtoNode.h"
#include "MfProtoTraversal.h"
#include "MfProtoSkill.h"
#include "MfProtoDB.h"
#include "MfFSMSpaceToBhvr.h"
#include "MfTriangleSet.h"
#include "MfColModel.h"
#include "MfTransform3.h"
#include "MfStream.h"
#include "MfFileStream.h"
#include "MfAiHook.h"
#include "MfLogger.h"
#include "MfCopyFolder.h"
#include "MfCallBack.h"
#include "MfProtoGeomInfo.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

// The reader of the old actor file format (user format below 100).
bool IsOldActorFormat(MfStream *s);
MfStreamError ReadOldActorFormat(MfStream *s, MfProtoActor *actor);

int const MfProtoActor::fileType = 'MFPA';
int const MfProtoActor::fileVersion = 106;
long const MfProtoActor::STREAM_TYPE = 'PtcA';

// 0x10010bf0 ??0MfProtoActor@@QAE@XZ
MfProtoActor::MfProtoActor()
    : refCount(1), userVersion(1), pSkeletonRoot(0), pNodeTraversal(0), pGeomTraversal(0),
      skillDict(53), isDirty(0), inTable(0), hasRenderInfo(0), skillCacheSize(-1), pUserData(0)
{
    Properties().Add(new MfProperty("MF_faceNode", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_eyeVector", new MfVector3Value(MfVector3(1, 0, 0))));
    Properties().Add(new MfProperty("MF_stopSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_locoSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_graspSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_graspPoseSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_touchSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_touchPoseSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_sitSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_putPoseSkill", new MfStringValue(NULL_STRING)));
    Properties().Add(new MfProperty("MF_bndRadius", new MfFloatValue(BoundingRadius())));
}

// 0x10011850 ??0MfProtoActor@@QAE@ABV0@@Z
MfProtoActor::MfProtoActor(const MfProtoActor &a)
    : refCount(1), userVersion(a.userVersion), pSkeletonRoot(0), pNodeTraversal(0),
      pGeomTraversal(0), properties(a.properties), skillDict(53), isDirty(0), inTable(0),
      hasRenderInfo(a.hasRenderInfo), pUserData(0)
{
    sName = MfProtoDB::UniqueName(MfString("Copy of ") + a.sName);
    if (a.pSkeletonRoot)
        SetSkeleton(new MfProtoNode(*a.pSkeletonRoot));
    MfDblListItem *pli;
    MfProtoSkill *skill;
    for (skill = (MfProtoSkill *)a.skills.GetFirst(pli); pli; skill = (MfProtoSkill *)a.skills.GetNext(pli))
        skills.Add(skill->Duplicate());
    MfFSMSpaceToBhvr *map;
    for (map = (MfFSMSpaceToBhvr *)a.behaviorMaps.GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)a.behaviorMaps.GetNext(pli))
        behaviorMaps.Add(new MfFSMSpaceToBhvr(*map));
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)a.proxDefs.GetFirst(pli); def; def = (MfProtoProxDef *)a.proxDefs.GetNext(pli))
        proxDefs.Add(new MfProtoProxDef(*def));
    maxProxDef.SetToMaximum(&proxDefs);
    ComputeBounds();
}

// 0x10011bb0 ??1MfProtoActor@@QAE@XZ
MfProtoActor::~MfProtoActor()
{
    if (pUserData)
        pUserData->Release();
    delete pNodeTraversal;
    delete pGeomTraversal;
    delete pSkeletonRoot;
    MfDblListItem *pli;
    MfProtoSkill *skill;
    for (skill = (MfProtoSkill *)skills.GetFirst(pli); pli; skill = (MfProtoSkill *)skills.GetNext(pli))
        skill->Release();
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)proxDefs.GetFirst(pli); def; def = (MfProtoProxDef *)proxDefs.GetNext(pli))
        delete def;
    MfFSMSpaceToBhvr *map;
    for (map = (MfFSMSpaceToBhvr *)behaviorMaps.GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)behaviorMaps.GetNext(pli))
        delete map;
}

// 0x10011da0 ?AddRef@MfProtoActor@@QAEHXZ
int MfProtoActor::AddRef()
{
    return InterlockedIncrement(&refCount);
}

// 0x10011db0 ?Release@MfProtoActor@@QAEHXZ
int MfProtoActor::Release()
{
    int n = InterlockedDecrement(&refCount);
    if (n == 0)
        delete this;
    return n;
}

// 0x10011de0 ?SetName@MfProtoActor@@QAEXVMfString@@@Z
// A prototype in the database is filed under its name: take it out while renaming.
void MfProtoActor::SetName(MfString name)
{
    int wasInTable = inTable;
    if (wasInTable) {
        AddRef();
        MfProtoDB::RemovePrototype(this);
    }
    sName = name;
    if (wasInTable)
        MfProtoDB::AddPrototype(this);
}

// 0x10011e60 ?SetSkeleton@MfProtoActor@@QAEXPAVMfProtoNode@@@Z
void MfProtoActor::SetSkeleton(MfProtoNode *root)
{
    if (pNodeTraversal) {
        delete pNodeTraversal;
        pNodeTraversal = 0;
        delete pGeomTraversal;
        pGeomTraversal = 0;
    }
    if (pSkeletonRoot && pSkeletonRoot != root)
        delete pSkeletonRoot;
    pSkeletonRoot = root;
    if (root) {
        pNodeTraversal = new MfProtoTraversal(pSkeletonRoot, 0);
        pGeomTraversal = new MfProtoTraversal(pSkeletonRoot, MfProtoNode::GEOMETRY);
        for (int i = 0; i < pNodeTraversal->NumNodes(); i++)
            pNodeTraversal->Node(i)->mActor = this;
    }
    ComputeBounds();
}

// 0x10011f80 ?RemoveSubtree@MfProtoActor@@QAEXPAVMfProtoNode@@@Z
void MfProtoActor::RemoveSubtree(MfProtoNode *node)
{
    MfProtoNode *parent = (MfProtoNode *)node->parent();
    if (parent) {
        MfProtoTraversal *oldGeom = new MfProtoTraversal(pSkeletonRoot, MfProtoNode::GEOMETRY);
        parent->removeChild(node);
        SetSkeleton(pSkeletonRoot);
        MfProtoTraversal *removed = new MfProtoTraversal(node, 0);
        for (int i = 0; i < removed->NumNodes(); i++)
            removed->Node(i)->mActor = 0;
        delete removed;
        MfPermutation *perm = new MfPermutation(oldGeom, pGeomTraversal);
        PermuteSkills(perm);
        ComputeBounds();
        delete oldGeom;
        delete perm;
        isDirty = 1;
    }
}

// 0x100120f0 ?AddSubtree@MfProtoActor@@QAEXPAVMfProtoNode@@0@Z
void MfProtoActor::AddSubtree(MfProtoNode *node, MfProtoNode *parent)
{
    MfProtoTraversal *oldGeom = new MfProtoTraversal(pSkeletonRoot, MfProtoNode::GEOMETRY);
    parent->addChild(node);
    SetSkeleton(pSkeletonRoot);
    MfPermutation *perm = new MfPermutation(oldGeom, pGeomTraversal);
    PermuteSkills(perm);
    ComputeBounds();
    delete oldGeom;
    delete perm;
    isDirty = 1;
}

// 0x100121e0 ?MoveSubtree@MfProtoActor@@QAEXPAVMfProtoNode@@0@Z
// Keeps the node where it is in the world: its home transform becomes relative
// to the new parent.
void MfProtoActor::MoveSubtree(MfProtoNode *node, MfProtoNode *newParent)
{
    MfProtoNode *oldParent = (MfProtoNode *)node->parent();
    if (oldParent) {
        MfProtoTraversal *oldGeom = new MfProtoTraversal(pSkeletonRoot, MfProtoNode::GEOMETRY);
        MfTransform3 toNode, toParent;
        node->GetRelativeHomeTransform(pSkeletonRoot, toNode);
        newParent->GetRelativeHomeTransform(pSkeletonRoot, toParent);
        node->SetHomeTransform(~toParent * toNode);
        oldParent->removeChild(node);
        newParent->addChild(node);
        SetSkeleton(pSkeletonRoot);
        MfPermutation *perm = new MfPermutation(oldGeom, pGeomTraversal);
        PermuteSkills(perm);
        delete oldGeom;
        delete perm;
        isDirty = 1;
    }
}

// 0x10012400 ?TransformNode@MfProtoActor@@QAEXPAVMfProtoNode@@ABVMfTransform3@@@Z
void MfProtoActor::TransformNode(MfProtoNode *node, const MfTransform3 &xf)
{
    node->TransformPivot(xf);
    int index = pGeomTraversal->NodeIndex(node);
    MfDblListItem *pli;
    for (MfProtoSkill *skill = (MfProtoSkill *)skills.GetFirst(pli); skill;
         skill = (MfProtoSkill *)skills.GetNext(pli))
        skill->TransformJoint(index, xf, 1);
    isDirty = 1;
}

// 0x10012460 ?ScaleSubtree@MfProtoActor@@QAEXPAVMfProtoNode@@ABVMfVector3@@@Z
void MfProtoActor::ScaleSubtree(MfProtoNode *node, const MfVector3 &scale)
{
    if (pNodeTraversal->NodeIndex(node) >= 0) {
        node->Scale(scale);
        ComputeBounds();
        int index = pGeomTraversal->NodeIndex(node);
        if (index >= 0) {
            MfDblListItem *pli;
            for (MfProtoSkill *skill = (MfProtoSkill *)skills.GetFirst(pli); skill;
                 skill = (MfProtoSkill *)skills.GetNext(pli))
                skill->ScaleJoint(index, scale);
        }
        isDirty = 1;
    }
}

// 0x100124e0 ?Collapse@MfProtoActor@@QAEXPAVMfProtoNode@@@Z
// Merges the geometry of a geometry node's subtree into the node and removes
// the children.
void MfProtoActor::Collapse(MfProtoNode *node)
{
    if (node->Style() == MfProtoNode::GEOMETRY) {
        MfProtoTraversal *trav = new MfProtoTraversal(node, MfProtoNode::GEOMETRY);
        MfTriangleSet *set = 0;
        MfTransform3 xf;
        for (int i = 0; i < trav->NumNodes(); i++) {
            if (trav->Node(i)->ColModel()) {
                trav->Node(i)->GetRelativeHomeTransform(node, xf);
                if (!set) {
                    set = new MfTriangleSet(*trav->Node(i)->ColModel()->GetTriangleSet());
                    set->ApplyTransform(xf);
                } else
                    set->Merge(*trav->Node(i)->ColModel()->GetTriangleSet(), xf);
            }
        }
        delete trav;
        set->AssignAverageNormals();
        node->SetColModel(set);
        node->SetModelFormat(2);
        MfProtoNode *child = (MfProtoNode *)node->firstChild();
        while (child) {
            MfProtoNode *next = (MfProtoNode *)child->nextSibling();
            RemoveSubtree(child);
            child = next;
        }
        isDirty = 1;
    }
}

// 0x10012660 ?GetSkill@MfProtoActor@@QAEPAVMfProtoSkill@@VMfString@@H@Z
MfProtoSkill *MfProtoActor::GetSkill(MfString name, int bLoad)
{
    name.ConvertToUpper();
    MfProtoSkill *skill = (MfProtoSkill *)skillDict.Find(name);
    if (!skill)
        skill = LoadSkill(name, bLoad);
    return skill;
}

// 0x100127a0 ??_GMfStringRef@@UAEPAXI@Z

// 0x100127d0 ?AddSkill@MfProtoActor@@QAEXPAVMfProtoSkill@@@Z
void MfProtoActor::AddSkill(MfProtoSkill *skill)
{
    MfString name(skill->Name());
    name.ConvertToUpper();
    skills.Add(skill);
    skillDict.Add(name, skill);
    isDirty = 1;
}

// 0x10012930 ?RenameSkill@MfProtoActor@@QAEXPAVMfProtoSkill@@ABVMfString@@@Z
void MfProtoActor::RenameSkill(MfProtoSkill *skill, const MfString &newName)
{
    MfString name(skill->Name());
    name.ConvertToUpper();
    skillDict.Delete(name);
    skillDict.Find(name);
    skill->SetName(newName);
    MfString oldFile = SkillPath() + name + MfString(".skl");
    MfString newFile = SkillPath() + newName + MfString(".skl");
    rename(oldFile, newFile);
    name = newName;
    name.ConvertToUpper();
    skillDict.Add(name, skill);
    isDirty = 1;
}

// 0x10012c90 ?ReadSkillFile@MfProtoActor@@SAPAVMfProtoSkill@@VMfString@@@Z
MfProtoSkill *MfProtoActor::ReadSkillFile(MfString fileName)
{
    MfProtoSkill *skill = 0;
    MfFileStream s(fileName, MF_FILE_READ, true);
    if (s.Status() == MF_STREAM_OK) {
        int type;
        s >> type;
        if (type != MfProtoSkill::fileType) {
            // An old file: version, then two words
            int version, a, b;
            s >> version >> a >> b;
            s.userFormat = (version >> 16) & 0xffff;
        }
        skill = (MfProtoSkill *)s.ReadObject(0);
    }
    return skill;
}

// 0x10012d70 ?WriteSkillFile@MfProtoActor@@SAHVMfString@@PAVMfProtoSkill@@@Z
int MfProtoActor::WriteSkillFile(MfString fileName, MfProtoSkill *skill)
{
    MfFileStream s(fileName, MF_FILE_WRITE, true);
    if (s.Status() == MF_STREAM_OK) {
        s << MfProtoSkill::fileType;
        s.WriteObject(skill);
    }
    return s.Status() == MF_STREAM_OK;
}

// 0x10012e20 ?LoadSkill@MfProtoActor@@IAEPAVMfProtoSkill@@VMfString@@H@Z
// Reads <path>Skills\<name>.skl; with a skill cache size, flushes the unlocked
// skills over the size.
MfProtoSkill *MfProtoActor::LoadSkill(MfString name, int bLock)
{
    if (name == NULL_STRING)
        return 0;
    MfString fileName = SkillPath() + name + MfString(".skl");
    MfProtoSkill *skill = ReadSkillFile(fileName);
    if (skill && skill->numJoints != pGeomTraversal->NumNodes()) {
        delete skill;
        return 0;
    }
    name.ConvertToUpper();
    if (skill && !skillDict.Find(name)) {
        skill->SetParentIndices(this);
        skills.Add(skill);
        skillDict.Add(name, skill);
    }
    if (skillCacheSize > 0 && skills.GetCount() > skillCacheSize) {
        int n = skillCacheSize;
        MfDblListItem *pli;
        MfProtoSkill *s;
        for (s = (MfProtoSkill *)skills.GetFirst(pli); s; s = (MfProtoSkill *)skills.GetNext(pli))
            if (s->locked)
                n++;
        if (n > 0 && skills.GetCount() > n) {
            for (s = (MfProtoSkill *)skills.GetFirst(pli); s; s = (MfProtoSkill *)skills.GetNext(pli))
                if (!s->locked) {
                    FlushSkill(s->Name());
                    break;
                }
        }
    }
    if (skill)
        skill->locked = bLock;
    return skill;
}

// EQUIV 0x100130f0 ?LoadAllSkills@MfProtoActor@@QAEXXZ: reviewed: stack slots of the MfString locals; cmp eax, ebx with ebx == 0 equals test eax, eax
void MfProtoActor::LoadAllSkills()
{
    MfString drive, dir, ext, name;
    MfString pattern = SkillPath();
    pattern << "*.skl";
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(pattern, &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        MfString fileName = SkillPath();
        fileName << fd.cFileName;
        MfProtoSkill *skill = ReadSkillFile(fileName);
        MfFileProgress::UpdateProgress();
        if (skill && skill->numJoints == pGeomTraversal->NumNodes()) {
            MfString skillName(skill->sName);
            skillName.ConvertToUpper();
            fileName.SplitPath(&drive, &dir, &name, &ext);
            MfString fileKey(name);
            fileKey.ConvertToUpper();
            if (fileKey != skillName) {
                skill->SetName(name);
                skillName = fileKey;
            }
            if (!skillDict.Find(skillName)) {
                skill->SetParentIndices(this);
                skills.Add(skill);
                skillDict.Add(skillName, skill);
            }
            continue;
        }
        delete skill;
    } while (FindNextFile(hFind, &fd));
    FindClose(hFind);
}

// 0x100135c0 ?DeleteSkill@MfProtoActor@@QAEXPAVMfProtoSkill@@@Z
void MfProtoActor::DeleteSkill(MfProtoSkill *skill)
{
    MfDblListItem *pli = skills.Find(skill);
    if (pli) {
        MfString name(skill->Name());
        name.ConvertToUpper();
        skillDict.Delete(name);
        skills.Delete(pli);
        skill->Release();
        MfString fileName = SkillPath() + name + MfString(".skl");
        DeleteFile(fileName);
    }
}

// 0x100137b0 ?FlushSkill@MfProtoActor@@QAEXVMfString@@@Z
void MfProtoActor::FlushSkill(MfString name)
{
    name.ConvertToUpper();
    MfProtoSkill *skill = (MfProtoSkill *)skillDict.Delete(name);
    MfDblListItem *pli = skills.Find(skill);
    if (pli)
        skills.Delete(pli);
    if (skill)
        skill->Release();
}

// 0x10013900 ?FlushAllSkills@MfProtoActor@@QAEXXZ
void MfProtoActor::FlushAllSkills()
{
    MfDblList list;
    list.Copy(skills);
    MfDblListItem *pli;
    for (MfProtoSkill *skill = (MfProtoSkill *)list.GetFirst(pli); skill;
         skill = (MfProtoSkill *)list.GetNext(pli))
        if (!skill->locked)
            FlushSkill(skill->Name());
}

// 0x100139c0 ?SetSkillCacheSize@MfProtoActor@@QAEXH@Z
void MfProtoActor::SetSkillCacheSize(int aSize)
{
    skillCacheSize = aSize;
}

// 0x100139d0 ?SaveSkills@MfProtoActor@@QAEHW4MfSkillFormat@@@Z
int MfProtoActor::SaveSkills(MfSkillFormat format)
{
    int ok = 1;
    MfString path = SkillPath();
    CreateDirectory(path, 0);
    MfDblList *list = Skills();
    MfDblListItem *pli;
    for (MfProtoSkill *skill = (MfProtoSkill *)list->GetFirst(pli); skill;
         skill = (MfProtoSkill *)list->GetNext(pli)) {
        skill->dataFormat = format;
        MfString fileName = path + skill->sName + MfString(".skl");
        if (!WriteSkillFile(fileName, skill)) {
            MfString msg("Failed to write the ");
            msg << skill->sName;
            msg << ".skl skill file.";
            MfLogger::Log(MF_LOG_INFORMATION, "Motivate", sName, msg);
            ok = 0;
        }
    }
    return ok;
}

// 0x10013b80 ?Save@MfProtoActor@@QAEHW4MfSkillFormat@@@Z
// Writes <path><name>.act (creating the actor folder with its Textures,
// Behaviors, Sounds and Skills folders) and, unless told not to, the skills.
int MfProtoActor::Save(MfSkillFormat format)
{
    MfString fileName = sPath + sName + MfString(".act");
    if (MfCopyFolder::Exists(sPath) != 1) {
        if (!MfCopyFolder::CreateFolder(sPath))
            return 0;
        MfString folder;
        folder = sPath + MfString("Textures\\");
        if (!MfCopyFolder::CreateFolder(folder))
            return 0;
        folder = sPath + MfString("Behaviors\\");
        if (!MfCopyFolder::CreateFolder(folder))
            return 0;
        folder = sPath + MfString("Sounds\\");
        if (!MfCopyFolder::CreateFolder(folder))
            return 0;
        folder = sPath + MfString("Skills\\");
        if (!MfCopyFolder::CreateFolder(folder))
            return 0;
    }
    for (int i = 0; i < pGeomTraversal->NumNodes(); i++) {
        MfColModel *model = pGeomTraversal->Node(i)->ColModel();
        if (model && model->GetTriangleSet()) {
            model->GetTriangleSet()->RemoveDegenerateTriangles();
            model->GetTriangleSet()->RemoveUnusedVertices();
        }
    }
    int ok = WriteActorFile(fileName, this);
    if (!ok) {
        MfString msg("Failed to write the ");
        msg << sName;
        msg << ".act actor file.";
        MfLogger::Log(ok, "Motivate", sName, msg);
    }
    if (format != MF_SKILL_NO_SAVE) {
        if (SaveSkills(format) && ok)
            ok = 1;
        else
            ok = 0;
    }
    isDirty = 0;
    return ok;
}

// 0x10013ef0 ?PermuteSkills@MfProtoActor@@IAEXPAVMfPermutation@@@Z
void MfProtoActor::PermuteSkills(MfPermutation *perm)
{
    MfDblListItem *pli;
    for (MfProtoSkill *skill = (MfProtoSkill *)skills.GetFirst(pli); skill;
         skill = (MfProtoSkill *)skills.GetNext(pli))
        skill->PermuteJoints(this, perm);
}

// 0x10013f30 ?ComputeBounds@MfProtoActor@@QAEXXZ
// The box around the visible geometry (a small cube without any), the sphere
// around the box and the vertical cylinder around it.
void MfProtoActor::ComputeBounds()
{
    int bGeometry = 0;
    int i;
    int n = pNodeTraversal->NumNodes();
    for (i = 0; i < n; i++) {
        MfProtoNode *node = pNodeTraversal->Node(i);
        if (node->Style() == MfProtoNode::GEOMETRY && node->GeomInfo()->mTriangles == 0 && node->ColModel()
            && node->ColModel()->GetRoot() && node->ColModel()->GetTriangleSet()) {
            bGeometry = 1;
            break;
        }
    }
    if (!bGeometry) {
        minBox.setValue(-0.1f, -0.1f, -0.1f);
        maxBox = -minBox;
    } else {
        minBox.setValue(FLT_MAX, FLT_MAX, FLT_MAX);
        maxBox.setValue(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }
    for (i = 0; i < pNodeTraversal->NumNodes(); i++) {
        MfProtoNode *node = pNodeTraversal->Node(i);
        if (node->Style() == MfProtoNode::GEOMETRY && node->GeomInfo()->mTriangles == 0 && node->ColModel()
            && node->ColModel()->GetRoot()) {
            MfTransform3 xf;
            MfVector3 min, max;
            node->GetRelativeHomeTransform(0, xf);
            node->ColModel()->GetTriangleSet()->FindBoundingBox(min, max);
            min = xf * min;
            max = xf * max;
            if (min[0] < minBox[0])
                minBox[0] = min[0];
            if (min[1] < minBox[1])
                minBox[1] = min[1];
            if (min[2] < minBox[2])
                minBox[2] = min[2];
            if (max[0] > maxBox[0])
                maxBox[0] = max[0];
            if (max[1] > maxBox[1])
                maxBox[1] = max[1];
            if (max[2] > maxBox[2])
                maxBox[2] = max[2];
        }
    }
    float dz = maxBox[2] - minBox[2];
    float dx = maxBox[0] - minBox[0];
    cylinderRadius = (float)sqrt(dx * dx + dz * dz) * 0.5f;
    cylinderHeight = maxBox[1] - minBox[1];
    sphereCenter = (minBox + maxBox) / 2.0f;
    sphereRadius = (maxBox - minBox).length() * 0.5f;
    properties.SetValue("MF_bndRadius", new MfFloatValue(BoundingRadius()));
}

// 0x10014300 ?GetBoundingBox@MfProtoActor@@QAEXAAVMfVector3@@0@Z
void MfProtoActor::GetBoundingBox(MfVector3 &min, MfVector3 &max)
{
    min = minBox;
    max = maxBox;
}

// 0x10014340 ?GetBoundingSphere@MfProtoActor@@QAEXAAVMfVector3@@AAM@Z
void MfProtoActor::GetBoundingSphere(MfVector3 &center, float &radius)
{
    center = sphereCenter;
    radius = sphereRadius;
}

// 0x10014370 ?GetBoundingCylinder@MfProtoActor@@QAEXAAM0@Z
void MfProtoActor::GetBoundingCylinder(float &radius, float &height)
{
    radius = cylinderRadius;
    height = cylinderHeight;
}

// 0x10014390 ?GetDefaultProximityDef@MfProtoActor@@QAEPAVMfProtoProxDef@@XZ
// The definition without a proto name; made from the bounds when missing.
MfProtoProxDef *MfProtoActor::GetDefaultProximityDef()
{
    MfDblListItem *pli;
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)proxDefs.GetFirst(pli); def; def = (MfProtoProxDef *)proxDefs.GetNext(pli))
        if (def->protoName == NULL_STRING)
            return def;
    def = new MfProtoProxDef;
    float radius = cylinderRadius;
    def->fRadius = radius;
    def->fRadiusSquared = radius * radius;
    def->fBCWidth = (maxBox.x() - minBox.x()) * 1.5f;
    def->fBCLength = def->fBCWidth * 2;
    proxDefs.Add(def);
    return def;
}

// 0x10014470 ?InitRenderMaterials@MfProtoActor@@QAEXXZ
void MfProtoActor::InitRenderMaterials()
{
    if (!hasRenderInfo) {
        MfDblListItem *pli;
        for (MfProtoSkill *skill = (MfProtoSkill *)skills.GetFirst(pli); skill;
             skill = (MfProtoSkill *)skills.GetNext(pli))
            skill->InitRenderMaterials();
        hasRenderInfo = 1;
    }
}

// 0x100144c0 ?ReadActorFile@MfProtoActor@@SAPAV1@VMfString@@@Z
MfProtoActor *MfProtoActor::ReadActorFile(MfString fileName)
{
    MfProtoActor *actor = 0;
    MfFileStream s(fileName, MF_FILE_READ, true);
    if (s.Status() == MF_STREAM_OK) {
        int type;
        s >> type;
        if (type != fileType) {
            int version, a, b;
            s >> version >> a >> b;
            s.userFormat = (version >> 16) & 0xffff;
        }
        actor = (MfProtoActor *)s.ReadObject(0);
    }
    if (actor) {
        MfString drive, dir;
        fileName.SplitPath(&drive, &dir, 0, 0);
        actor->Path() << drive << dir;
    }
    return actor;
}

// 0x10014610 ?WriteActorFile@MfProtoActor@@SAHVMfString@@PAV1@@Z
int MfProtoActor::WriteActorFile(MfString fileName, MfProtoActor *actor)
{
    MfFileStream s(fileName, MF_FILE_WRITE, true);
    if (s.Status() == MF_STREAM_OK) {
        s << fileType;
        s.WriteObject(actor);
    }
    return s.Status() == MF_STREAM_OK;
}

// 0x100146c0 ?fn_100146c0@@YAPAVMfStreamObject@@XZ
static MfStreamObject *fn_100146c0()
{
    return new MfProtoActor;
}
// 0x10014720 _$E30
// 0x10014730 _$E29
static MfStreamConstructorEntry s_entry(MfProtoActor::STREAM_TYPE, fn_100146c0);

// 0x10014750 ?StreamType@MfProtoActor@@UBEJXZ
long MfProtoActor::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10014760 ?ReadFromStream@MfProtoActor@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoActor::ReadFromStream(MfStream &s)
{
    if (s.userFormat == 0)
        s >> s.userFormat;
    if (IsOldActorFormat(&s))
        return ReadOldActorFormat(&s, this);
    s >> userVersion >> sName;
    MfProtoNode *root = new MfProtoNode;
    s >> *root;
    SetSkeleton(root);
    int i;
    for (i = 0; i < pNodeTraversal->NumNodes(); i++)
        if (pNodeTraversal->Node(i)->Style() == MfProtoNode::HANDLE)
            pNodeTraversal->Node(i)->CreateColModel();
    ComputeBounds();
    s >> properties;
    int numDefs;
    s >> numDefs;
    for (i = 0; i < numDefs; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfProtoProxDef *def = new MfProtoProxDef;
        s >> *def;
        if (s.Status() == MF_STREAM_OK)
            proxDefs.Add(def);
    }
    maxProxDef.SetToMaximum(&proxDefs);
    int numMaps;
    s >> numMaps;
    for (i = 0; i < numMaps; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfFSMSpaceToBhvr *map = new MfFSMSpaceToBhvr;
        s >> *map;
        if (s.Status() == MF_STREAM_OK)
            behaviorMaps.Add(map);
    }
    return s.Status();
}

// 0x100149c0 ?WriteToStream@MfProtoActor@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoActor::WriteToStream(MfStream &s) const
{
    s.userFormat = fileVersion;
    s << fileVersion;
    s << userVersion << sName;
    s << *pSkeletonRoot;
    s << properties;
    s << proxDefs.GetCount();
    MfDblListItem *pli;
    MfStreamObject *o;
    for (o = (MfProtoProxDef *)proxDefs.GetFirst(pli); pli; o = (MfProtoProxDef *)proxDefs.GetNext(pli))
        s << *o;
    s << behaviorMaps.GetCount();
    for (o = (MfFSMSpaceToBhvr *)behaviorMaps.GetFirst(pli); pli;
         o = (MfFSMSpaceToBhvr *)behaviorMaps.GetNext(pli))
        s << *o;
    return s.Status();
}

// 0x10014ab0 ?SkillPath@MfProtoActor@@QAE?AVMfString@@XZ
MfString MfProtoActor::SkillPath()
{
    return sPath + MfString("Skills\\");
}

// 0x10014b30 ?TexturePath@MfProtoActor@@QAE?AVMfString@@XZ
MfString MfProtoActor::TexturePath()
{
    return sPath + MfString("Textures\\");
}

// 0x10014bb0 ?SoundPath@MfProtoActor@@QAE?AVMfString@@XZ
MfString MfProtoActor::SoundPath()
{
    return sPath + MfString("Sounds\\");
}

// 0x10014c30 ?FindSpaceBhvrMap@MfProtoActor@@QAEPAVMfFSMSpaceToBhvr@@VMfString@@@Z
MfFSMSpaceToBhvr *MfProtoActor::FindSpaceBhvrMap(MfString spaceName)
{
    spaceName.ConvertToUpper();
    MfString name;
    MfDblList *list = &behaviorMaps;
    MfDblListItem *pli;
    for (MfFSMSpaceToBhvr *map = (MfFSMSpaceToBhvr *)list->GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)list->GetNext(pli)) {
        name = map->m_spaceName;
        name.ConvertToUpper();
        if (name == spaceName)
            return map;
    }
    return 0;
}

// 0x10014eb0 ?AddSpaceBhvrMap@MfProtoActor@@QAEXPAVMfFSMSpaceToBhvr@@@Z
void MfProtoActor::AddSpaceBhvrMap(MfFSMSpaceToBhvr *map)
{
    behaviorMaps.Add(map);
}

// 0x10014ed0 ?RemoveSpaceBhvrMap@MfProtoActor@@QAEXVMfString@@@Z
void MfProtoActor::RemoveSpaceBhvrMap(MfString spaceName)
{
    MfString name;
    spaceName.ConvertToUpper();
    MfDblList *list = &behaviorMaps;
    MfDblListItem *pli;
    for (MfFSMSpaceToBhvr *map = (MfFSMSpaceToBhvr *)list->GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)list->GetNext(pli)) {
        name = map->m_spaceName;
        name.ConvertToUpper();
        if (name == spaceName) {
            list->Delete(pli);
            delete map;
            break;
        }
    }
}

// 0x10015120 ?RenameBehaviorForSpaces@MfProtoActor@@QAEXABVMfString@@0@Z
void MfProtoActor::RenameBehaviorForSpaces(const MfString &oldName, const MfString &newName)
{
    MfDblListItem *pli;
    for (MfFSMSpaceToBhvr *map = (MfFSMSpaceToBhvr *)behaviorMaps.GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)behaviorMaps.GetNext(pli))
        if (map->IsActive(oldName)) {
            map->RemoveActiveBhvr(oldName);
            map->AddActiveBhvr(newName);
        }
}

// 0x100151b0 ?RemoveBehaviorFromAllSpaces@MfProtoActor@@QAEXABVMfString@@@Z
void MfProtoActor::RemoveBehaviorFromAllSpaces(const MfString &name)
{
    MfDblListItem *pli;
    for (MfFSMSpaceToBhvr *map = (MfFSMSpaceToBhvr *)behaviorMaps.GetFirst(pli); map;
         map = (MfFSMSpaceToBhvr *)behaviorMaps.GetNext(pli))
        if (map->IsActive(name))
            map->RemoveActiveBhvr(name);
}

// 0x10015220 _$E36 (calls $E32 and $E35)
// 0x10015230 _$E32
// 0x10015240 _$E35
// 0x10015250 _$E33
MfDictionary MfProtoDB::protoTable(53);
// 0x10015270 _$E41 (calls $E38 and $E40)
// 0x10015280 _$E38
// 0x10015290 _$E40
// 0x100152a0 _$E39
MfCriticalSection MfProtoDB::csectDBLock;

// 0x100152c0 ?GetPrototype@MfProtoDB@@SAPAVMfProtoActor@@VMfString@@AAHH@Z
// The prototype of an actor file, from the database, a hook ('OrPa') or the file.
MfProtoActor *MfProtoDB::GetPrototype(MfString fileName, int &bLoaded, int bLoadSkills)
{
    MfString name;
    csectDBLock.Enter();
    fileName.SplitPath(0, 0, &name, 0);
    name.ConvertToUpper();
    MfProtoActor *found = (MfProtoActor *)protoTable.Find(name);
    if (found) {
        csectDBLock.Leave();
        bLoaded = 0;
        return found;
    }
    MfProtoActor *proto = 0;
    MfAiHookManager *hooks;
    if (MfInterfaceRegistry::GetInterface("MfAiHookManager", (MfAi **)&hooks)) {
        MfActorRWHookData data;
        data.filePath = fileName;
        data.pActor = 0;
        data.pProto = 0;
        if (hooks->CallHooks('OrPa', &data) > 0)
            proto = data.pProto;
        hooks->Release();
    }
    if (!proto) {
        proto = MfProtoActor::ReadActorFile(fileName);
        bLoaded = 1;
        if (!proto) {
            csectDBLock.Leave();
            return 0;
        }
        if (bLoadSkills)
            proto->LoadAllSkills();
    }
    protoTable.Add(name, proto);
    proto->inTable = 1;
    csectDBLock.Leave();
    return proto;
}

// 0x10015520 ??1MfActorRWHookData@@QAE@XZ

// 0x10015530 ?PrototypeExists@MfProtoDB@@SAHVMfString@@@Z
int MfProtoDB::PrototypeExists(MfString name)
{
    csectDBLock.Enter();
    name.ConvertToUpper();
    int exists = protoTable.Find(name) != 0;
    csectDBLock.Leave();
    return exists;
}

// 0x10015670 ?AddPrototype@MfProtoDB@@SAHPAVMfProtoActor@@@Z
int MfProtoDB::AddPrototype(MfProtoActor *proto)
{
    if (PrototypeExists(proto->Name()))
        return 0;
    csectDBLock.Enter();
    MfString name(proto->sName);
    name.ConvertToUpper();
    protoTable.Add(name, proto);
    proto->inTable = 1;
    csectDBLock.Leave();
    return 1;
}

// 0x100157f0 ?RemovePrototype@MfProtoDB@@SAHPAVMfProtoActor@@@Z
int MfProtoDB::RemovePrototype(MfProtoActor *proto)
{
    csectDBLock.Enter();
    MfString name(proto->sName);
    name.ConvertToUpper();
    if (protoTable.Find(name) != proto) {
        csectDBLock.Leave();
        return 0;
    }
    proto->inTable = 0;
    int removed = protoTable.Delete(name) != 0;
    proto->Release();
    csectDBLock.Leave();
    return removed;
}

// Releases the prototypes when the table is cleared.
class MfProtoDBCB : public MfCallBack {
public:
    virtual int CallBack(void *a);
};

// 0x10015980 ?RemoveAllPrototypes@MfProtoDB@@SAHXZ
int MfProtoDB::RemoveAllPrototypes()
{
    csectDBLock.Enter();
    MfProtoDBCB cb;
    protoTable.ClearItems(&cb);
    csectDBLock.Leave();
    return 1;
}

// 0x100159e0 ?CallBack@MfProtoDBCB@@UAEHPAX@Z
int MfProtoDBCB::CallBack(void *a)
{
    ((MfProtoActor *)a)->Release();
    return 1;
}

// 0x10015a00 ?UniqueName@MfProtoDB@@SA?AVMfString@@V2@@Z
// name, else "name 1", "name 2", ... (compared in upper case)
MfString MfProtoDB::UniqueName(MfString base)
{
    MfString key;
    MfString name;
    csectDBLock.Enter();
    key = name = base;
    key.ConvertToUpper();
    for (int n = 1; protoTable.Find(key); n++) {
        key = name = base + MfString(" ") + MfString(n);
        key.ConvertToUpper();
    }
    csectDBLock.Leave();
    return name;
}

// 0x10015d00 ?RegCompiledBhvr@MfProtoActor@@QAEXVMfString@@@Z
void MfProtoActor::RegCompiledBhvr(MfString name)
{
    if (!listCompiledBhvrs.Search(name))
        listCompiledBhvrs.Add(name);
}

// 0x10015d70 ?UnregCompiledBhvr@MfProtoActor@@QAEXVMfString@@@Z
void MfProtoActor::UnregCompiledBhvr(MfString name)
{
    MfDblListItem *pli = listCompiledBhvrs.Search(name);
    if (!pli)
        listCompiledBhvrs.Delete(pli);
}

// 0x10015de0 ?IsBhvrCompiled@MfProtoActor@@QAEHVMfString@@@Z
int MfProtoActor::IsBhvrCompiled(MfString name)
{
    if (!listCompiledBhvrs.Search(name))
        return 0;
    return 1;
}
