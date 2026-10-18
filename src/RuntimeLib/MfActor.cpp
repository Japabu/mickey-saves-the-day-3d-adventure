#include "MfActor.h"
#include "MfSkeletonTraversal.h"
#include "MfCameraNode.h"
#include "MfSpace.h"
#include "MfWorld.h"
#include "MfProtoActor.h"
#include "MfProtoDB.h"
#include "MfProtoNode.h"
#include "MfProtoTraversal.h"
#include <string.h>
#include <math.h>
#include "MfHookManager.h"
#include "MfKChain.h"
#include "MfRenderer.h"
#include "MfSoundManager.h"
#include "MfStringValue.h"
#include "MfSkill.h"
#include "MfActorPath.h"
#include "MfIntValue.h"
#include "MfRay.h"
#include "MfColResults.h"
#include "MfAiColNode.h"

#include "MfDataPath.h"

// MfActorLib's load progress (declared in MfActorLib/MfFile.h)
class __declspec(dllimport) MfFileProgress {
public:
    static void SetProgress(int, MfString);
};

// 0x100025f0 ??0MfActor@@IAE@ABV0@@Z
// (an inline of the original header: it lies among the inline copies of the
// first object; as an inline here it changes VC5's register choices in
// MfJointPath::Key and MfLocoSkill::CycleLength)
MfActor::MfActor(const MfActor &) : mNodeDictionary(0x35)
{
}

// 0x10008d80 ?Space@MfActor@@QAEPAVMfSpace@@XZ
MfSpace * MfActor::Space()
{
    return mSpace;
}

// 0x1000e5b0 ?GetInterfaceName@MfActor@@UAEXPADHAAH1@Z
void MfActor::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiActor", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1000e5f0 ?AddRef@MfActor@@UAEHXZ
int MfActor::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}

// 0x1000e600 ?Release@MfActor@@UAEHXZ
int MfActor::Release()
{
    long count = InterlockedDecrement(&mRefCount);
    if (count == 0) {
        MfActorHookData data;
        data.pActor = this;
        data.pSpace = 0;
        gHookManager->CallHooks('WDAc', &data);
        delete this;
    }
    return count;
}

// 0x1000e6a0 ??1MfActorHookData@@QAE@XZ (compiler-generated, from Release)

// 0x1000e6b0 ?GetProperty@MfActor@@UAEHABVMfString@@AAVMfProperty@@@Z
int MfActor::GetProperty(const MfString &name, MfProperty &property)
{
    return getProperty(name, property);
}

// 0x1000e6e0 ?SetProperty@MfActor@@UAEXAAVMfProperty@@@Z
void MfActor::SetProperty(MfProperty &property)
{
    setProperty(property);
}

// EQUIV 0x1000e6f0 ?SetProperty@MfActor@@UAEXPAVMfProperty@@@Z: reviewed: helper called vs inlined: the original calls the imported MfProperty::Name() (three calls through ebp; it returns the name member at +4, as the MfProperty.h inline), so it keeps found in a stack slot and the frame is 4 bytes larger; the blocks, calls, conditions and stores are the same in the same order
// A property of the prototype is replaced there (MF_bndRadius also sets the
// bounding cylinder's radius), one of the actor replaced, else it is added to
// the actor's properties. With the network enabled the change is announced
// first (MF_HOOK_MISC_DATA, kind 0).
void MfActor::SetProperty(MfProperty *property)
{
    if (MfWorld::enableNetwork) {
        MfMiscHookData data;
        data.pActor = this;
        data.kind = 0;
        data.property = *property;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
    int found = 0;
    Lock();
    if (mPrototype && mPrototype->Properties().Find(property->Name())) {
        mPrototype->Properties().Replace(property);
        found = 1;
        if (property->Name() == "MF_bndRadius")
            mPrototype->BoundingRadius() = *property;
    } else if (mProperties.Find(property->Name())) {
        mProperties.Replace(property);
        found = 1;
    }
    if (!found)
        mProperties.Add(property);
    Unlock();
}

// EQUIV 0x1000e880 ??1MfMiscHookData@@QAE@XZ: reviewed: the code is identical; the unwind funclet reads the object from another frame slot ([ebp-0x14] vs [ebp-0x10])

// 0x1000e8d0 ?GetProperty@MfActor@@UAEHABVMfString@@PAPAVMfProperty@@@Z
int MfActor::GetProperty(const MfString &name, MfProperty **property)
{
    *property = 0;
    if (mPrototype && (*property = mPrototype->Properties().Find(name)) != 0)
        return 1;
    *property = mProperties.Find(name);
    return *property != 0;
}

// 0x1000e920 ?GetCylinder@MfActor@@UAEXAAM0@Z
void MfActor::GetCylinder(float &height, float &radius)
{
    if (mPrototype)
        mPrototype->GetBoundingCylinder(radius, height);
}

// 0x1000e940 ?GetSphere@MfActor@@UAEXAAMAAVMfVector3@@@Z
void MfActor::GetSphere(float &radius, MfVector3 &center)
{
    if (mPrototype)
        mPrototype->GetBoundingSphere(center, radius);
}

// 0x1000e960 ?SetActorAttributes@MfActor@@UAEXH@Z
void MfActor::SetActorAttributes(int attributes)
{
    setActorAttributes(attributes);
}

// 0x1000ec80 ?ignoreTriangleCollisionCheck@MfActor@@QAEXH@Z (register choice depends on unrelated header content)
void MfActor::ignoreTriangleCollisionCheck(int ignore)
{
    Lock();
    for (int i = 0; i < mSkeletonTraversal->numNodes(); i++) {
        int style = mSkeletonTraversal->node(i)->colNode().GetCheckStyle();
        if (ignore)
            style = (style & ~1) | (style << 8) | 0x20;
        else if (style & 0x20)
            style >>= 8;
        mSkeletonTraversal->node(i)->colNode().SetCheckStyle(style);
    }
    Unlock();
}

// EQUIV 0x1000e970 ?setActorAttributes@MfActor@@QAEXH@Z: reviewed: helper inlined vs called: the original constructs the hook data's MfVector3 with the imported MfVector3() (zeroes x, y, z, as the inline one here), with one more EH state around it; the rest is the same code with independent loads ordered differently
// The render style of the nodes follows the attributes (8 visible, 4 all
// nodes as boxes, 0x20 wire frame), 0x10 switches off the collision checks of
// the skeleton; 1 (static) and 2 (dynamic) exclude each other, and a change
// from dynamic to static (or back) rebuilds the space's static environment.
void MfActor::setActorAttributes(int attributes)
{
    MfMiscHookData data;
    if (MfWorld::enableNetwork) {
        data.pActor = this;
        data.kind = 1;
        data.delay = attributes;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
    int old = mActorAttributes;
    Lock();
    mActorAttributes = attributes;
    Unlock();
    mStateChangeFlag = 1;
    if (mAllTraversal) {
        int i;
        if (attributes & 8) {
            mActorAttributes &= ~0x24;
            for (i = 0; i < skeletonTraversal()->numNodes(); i++)
                skeletonTraversal()->node(i)->SetRenderStyle(0);
        }
        if (attributes & 4) {
            mActorAttributes = (mActorAttributes & ~0x28) | 4;
            for (i = 0; i < allTraversal()->numNodes(); i++)
                allTraversal()->node(i)->SetRenderStyle(2);
        }
        if (attributes & 0x20) {
            mActorAttributes = (mActorAttributes & ~0x0c) | 0x20;
            for (i = 0; i < skeletonTraversal()->numNodes(); i++)
                skeletonTraversal()->node(i)->SetRenderStyle(1);
        }
        Lock();
        if (attributes & 0x10) {
            for (i = 0; i < skeletonTraversal()->numNodes(); i++) {
                skeletonTraversal()->node(i)->colNode().SetCheckStyle(0);
                data.m18 = (int)(MfAiActorNode *)skeletonTraversal()->node(i);
                data.kind = 3;
                data.delay = 0;
                gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
            }
        } else if (old & 0x10) {
            for (i = 0; i < skeletonTraversal()->numNodes(); i++) {
                skeletonTraversal()->node(i)->colNode().SetCheckStyle(0xf);
                data.m18 = (int)(MfAiActorNode *)skeletonTraversal()->node(i);
                data.kind = 3;
                data.delay = 0xf;
                gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
            }
        }
        Unlock();
    }
    Lock();
    int current = mActorAttributes;
    if (current & 1)
        mActorAttributes = current & ~2;
    else if (current & 2)
        mActorAttributes = current & ~1;
    else if (old & 1)
        mActorAttributes = current | 1;
    else if (old & 2)
        mActorAttributes = current | 2;
    Unlock();
    if ((attributes & 1) && !(old & 1) && mSpace)
        mSpace->MakeStaticEnv();
    if ((attributes & 2) && (old & 1) && mSpace)
        mSpace->MakeStaticEnv();
}

// 0x1000ed00 ?GetNodeUnderRoot@MfActor@@UAEPAVMfAiActorNode@@XZ
// The node a ray from the middle of the actor's bounding cylinder straight
// down (along the root's -y) hits first, or 0.
MfAiActorNode *MfActor::GetNodeUnderRoot()
{
    MfRay ray;
    float height, radius;
    GetCylinder(height, radius);
    Lock();
    MfTransform3 transform = mSkeleton->mGlobalTransform;
    Unlock();
    MfVector3 up = transform.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    transform.translation() += up * (height * 0.5f);
    ray.mDirection = -up;
    ray.mOrigin = transform.translation();
    MfColResultRay result;
    if (ray.Cast(mSpace, mSkeleton, result, 0))
        return result.node->Owner();
    return 0;
}

// 0x1000eed0 ?GetRootNode@MfActor@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfActor::GetRootNode()
{
    return mSkeleton;
}

// 0x1000eee0 ?GetPrimaryNode@MfActor@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfActor::GetPrimaryNode()
{
    if (mSkeletonTraversal && mSkeletonTraversal->numNodes() > 1)
        return mSkeletonTraversal->node(1);
    return 0;
}

// 0x1000ef00 ?GetNodeByName@MfActor@@UAEPAVMfAiActorNode@@ABVMfString@@@Z
// The node dictionary is keyed by the upper-case name.
MfAiActorNode *MfActor::GetNodeByName(const MfString &name)
{
    MfString key(name);
    key.ConvertToUpper();
    return (MfSkeletonNode *)mNodeDictionary.Find(key);
}

// 0x1000f030 ?GetNodeByIndex@MfActor@@UAEPAVMfAiActorNode@@H@Z
MfAiActorNode *MfActor::GetNodeByIndex(int index)
{
    if (index < mAllTraversal->numNodes() && index >= 0)
        return mAllTraversal->node(index);
    return 0;
}

// 0x1000f060 ?GetNumNodes@MfActor@@UAEHXZ
int MfActor::GetNumNodes()
{
    if (mAllTraversal)
        return mAllTraversal->numNodes();
    return 0;
}

// 0x1000f070 ?SetUserData@MfActor@@UAEXPAVMfAi@@@Z
void MfActor::SetUserData(MfAi * pUser)
{
    mUserData = pUser;
}

// 0x1000f080 ?GetUserData@MfActor@@UAEPAVMfAi@@XZ
MfAi * MfActor::GetUserData()
{
    return mUserData;
}

// 0x1000f090 ??0MfActor@@QAE@XZ
// Every actor has the property MF_replanPriority (an int, 0) and a unique ID.
MfActor::MfActor()
    : mTempCopy(0), mPrototype(0), mInstanceIndex(0), mActorAttributes(8), mSkeleton(0),
      mSkeletonTraversal(0), mAllTraversal(0), mNodeDictionary(0x35), mSpace(0), mRefCount(1),
      mUserData(0), mStateChangeFlag(0)
{
    MfProperty *property = new MfProperty("MF_replanPriority", 0);
    property->SetValue(new MfIntValue);
    mProperties.Add(property);
    mUniqueID = MfWorld::CreateUniqueNumber();
}

// 0x1000f230 ??_GMfIntValue@@UAEPAXI@Z (local copy, after `new MfIntValue`)

// 0x1000f250 ??1MfActor@@UAE@XZ
// Leaves the space and the world; nodes of other actors attached to the
// skeleton move to the skeleton's parent before it is deleted.
MfActor::~MfActor()
{
    ClearFilter();
    if (mSpace && !mTempCopy && mSkeleton)
        mSpace->RemoveActorSync(this, 1);
    g_world->UnregisterActor(this);
    if (mSkeleton) {
        MfSkeletonTraversal *traversal = new MfSkeletonTraversal(mSkeleton, 'Skel', 0);
        for (int i = 0; i < traversal->numNodes(); i++) {
            MfSkeletonNode *node = traversal->node(i);
            if (node->mOwner != this && ((MfSkeletonNode *)node->parent())->mOwner == this)
                node->changeParent((MfSkeletonNode *)mSkeleton->parent(), 0);
        }
        delete traversal;
        mSkeleton->changeParent(0, 0);
    }
    DetachFromPrototype();
    delete mSkeleton;
    delete mSkeletonTraversal;
    delete mAllTraversal;
    if (mUserData)
        mUserData->Release();
}

// EQUIV 0x1000f3e0 ?setSkeleton@MfActor@@QAEXPAVMfSkeletonNode@@H@Z: reviewed: one stack slot: the two new results (EH cleanup) sit in a local slot in the original, in the dead skeleton parameter's slot here; every instruction is otherwise the same
// The actor takes the skeleton: new traversals, the node dictionary (by
// upper-case node name) and the owner of every node; with render set the
// nodes get their render nodes.
void MfActor::setSkeleton(MfSkeletonNode *skeleton, int render)
{
    Lock();
    if (mSkeleton && mSkeleton != skeleton)
        delete mSkeleton;
    mSkeleton = skeleton;
    delete mSkeletonTraversal;
    mSkeletonTraversal = new MfSkeletonTraversal(mSkeleton, 'Skel', 1);
    delete mAllTraversal;
    mAllTraversal = new MfSkeletonTraversal(mSkeleton, 0, 1);
    mNodeDictionary.Clear();
    {
        MfString name;
        for (int i = 0; i < mAllTraversal->numNodes(); i++) {
            name = mAllTraversal->node(i)->GetNodeName();
            name.ConvertToUpper();
            mNodeDictionary.Add(name, mAllTraversal->node(i));
            mAllTraversal->node(i)->SetOwner(this, 0);
            if (render)
                mAllTraversal->node(i)->CreateRenderNode();
        }
    }
    Unlock();
}

// 0x1000f610 ?CreateRenderNodes@MfActor@@QAEXXZ
void MfActor::CreateRenderNodes()
{
    for (int i = 0; i < mAllTraversal->numNodes(); i++)
        mAllTraversal->node(i)->CreateRenderNode();
    MfRenderer::Instance()->PushSearchPath(mPrototype->TexturePath());
    mPrototype->InitRenderMaterials();
    MfRenderer::Instance()->PopSearchPath();
}

// 0x1000f6b0 ?GotoHomeConfig@MfActor@@QAEXH@Z
void MfActor::GotoHomeConfig(int all)
{
    if (!mPrototype)
        return;
    for (int i = all == 0; i < mAllTraversal->numNodes(); i++) {
        MfSkeletonNode *node = mAllTraversal->node(i);
        node->setTransform(node->homeTransform());
        Lock();
        for (MfSkeletonNode *child = (MfSkeletonNode *)node->firstChild(); child;
             child = (MfSkeletonNode *)child->nextSibling()) {
            if (child->GetOwner() != this)
                child->_syncToTransform();
        }
        Unlock();
    }
}

// 0x1000f770 ?GotoHomeConfig@MfActor@@QAEXABVMfTransform3@@@Z
void MfActor::GotoHomeConfig(const MfTransform3 &transform)
{
    mSkeleton->setTransform(transform);
    GotoHomeConfig(0);
}

// 0x1000f790 ?GetPropertyNames@MfActor@@UAEXPAVMfStringList@@@Z
void MfActor::GetPropertyNames(MfStringList *names)
{
    if (mPrototype)
        mPrototype->Properties().GetPropertyNames(*names, 0);
}

// 0x1000f7b0 ?getProperty@MfActor@@QAEHVMfString@@AAVMfProperty@@@Z
int MfActor::getProperty(MfString name, MfProperty &property)
{
    MfProperty *found;
    if ((mPrototype && (found = mPrototype->Properties().Find(name)) != 0) ||
        (found = mProperties.Find(name)) != 0) {
        property = *found;
        return 1;
    }
    return 0;
}

// TODO 0x1000f860 ?setProperty@MfActor@@QAEXAAVMfProperty@@@Z: as SetProperty(MfProperty *): the original calls the imported MfProperty::Name() where MfProperty.h expands the inline accessor; here the else-if (instance property) block also moves to the end of the function, the original keeps it in line. Calls, conditions and stores along each path are the same (66% similar)
// As SetProperty(MfProperty *), but copying: an existing property (of the
// prototype or the actor) takes the value, else the actor gets a copy.
void MfActor::setProperty(MfProperty &property)
{
    if (MfWorld::enableNetwork) {
        MfMiscHookData data;
        data.pActor = this;
        data.kind = 0;
        data.property = property;
        gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    }
    Lock();
    int found = 0;
    MfProperty *existing;
    if (mPrototype && (existing = mPrototype->Properties().Find(property.Name())) != 0) {
        *existing = property;
        found = 1;
        if (property.Name() == "MF_bndRadius")
            mPrototype->BoundingRadius() = *existing;
    } else if ((existing = mProperties.Find(property.Name())) != 0) {
        *existing = property;
        found = 1;
    }
    if (!found)
        mProperties.Add(new MfProperty(property));
    Unlock();
}

// 0x1000fa30 ?property@MfActor@@QAEAAVMfProperty@@VMfString@@@Z
MfProperty &MfActor::property(MfString name)
{
    MfProperty *found = mPrototype->Properties().Find(name);
    if (found)
        return *found;
    MfPropertyTable *table = &mProperties;
    found = table->Find(name);
    if (!found) {
        found = new MfProperty(name, 0);
        Lock();
        table->Add(found);
        Unlock();
    }
    return *found;
}

// 0x1000fb20 ?SetSkillToProperty@MfActor@@QAEXABVMfString@@0@Z
void MfActor::SetSkillToProperty(const MfString &name, const MfString &skill)
{
    MfProperty &prop = property(name);
    Lock();
    prop.SetValue(new MfStringValue(skill));
    Unlock();
}

// 0x1000fbf0 ??_GMfStringValue@@UAEPAXI@Z (local copy, after `new MfStringValue`)

// 0x1000fc10 ?SetSkillToProperty@MfActor@@QAEXABVMfString@@HPAVMfSkill@@@Z
void MfActor::SetSkillToProperty(const MfString &name, int type, MfSkill *skill)
{
    MfProperty &prop = property(name);
    Lock();
    if (!skill)
        prop.SetValue(new MfStringValue);
    else if (skill->Type() == type)
        prop.SetValue(new MfStringValue(skill->Name()));
    Unlock();
}

// 0x1000fd50 ?GetSkillFromProperty@MfActor@@QAEPAVMfSkill@@ABVMfString@@@Z
MfSkill *MfActor::GetSkillFromProperty(const MfString &name)
{
    MfProperty property;
    MfSkill *skill;
    if (getProperty(name, property))
        skill = getSkill(property);
    else
        skill = getSkill(name);
    if (skill && !skill->isPresampled())
        skill->Presample(this);
    return skill;
}

// 0x1000fe10 ?getSkill@MfActor@@QAEPAVMfSkill@@VMfString@@@Z
MfSkill *MfActor::getSkill(MfString name)
{
    MfSkill *skill = mPrototype ? (MfSkill *)mPrototype->GetSkill(name, 0) : 0;
    if (skill) {
        if (!skill->isPresampled())
            skill->Presample(this);
        theSoundManager->SetActorPath(mPrototype->SoundPath());
        skill->InitSounds(this);
    }
    return skill;
}

// 0x1000fed0 ?GetChildActors@MfActor@@UAEXPAVMfDblList@@@Z
void MfActor::GetChildActors(MfDblList *list)
{
    MfSkeletonTraversal *traversal = new MfSkeletonTraversal(mSkeleton, 0, 0);
    for (int i = 0; i < traversal->numNodes(); i++) {
        MfAiActor *actor = traversal->node(i)->GetOwner();
        if (actor && actor != this && !list->Find(actor))
            list->Add(actor);
    }
    delete traversal;
}

// 0x1000ffc0 ?LoadSkill@MfActor@@UAEHABVMfString@@H@Z
int MfActor::LoadSkill(const MfString &name, int load)
{
    if (mPrototype) {
        MfSkill *skill = (MfSkill *)mPrototype->GetSkill(name, load);
        if (skill) {
            if (!skill->isPresampled())
                skill->Presample(this);
            theSoundManager->SetActorPath(mPrototype->SoundPath());
            skill->InitSounds(this);
            return 1;
        }
    }
    return 0;
}

// 0x1000ffa0 ?GetNumSkills@MfActor@@UAEHXZ
int MfActor::GetNumSkills()
{
    if (mPrototype && mPrototype->Skills())
        return mPrototype->Skills()->GetCount();
    return 0;
}

// 0x10010090 ?FlushSkill@MfActor@@UAEXABVMfString@@@Z
void MfActor::FlushSkill(const MfString &name)
{
    if (mPrototype)
        mPrototype->FlushSkill(name);
}

// 0x100100c0 ?FlushAllSkills@MfActor@@UAEXXZ
void MfActor::FlushAllSkills()
{
    if (mPrototype)
        mPrototype->FlushAllSkills();
}

// 0x100100d0 ?LoadAllSkills@MfActor@@UAEXXZ
void MfActor::LoadAllSkills()
{
    if (mPrototype)
        mPrototype->LoadAllSkills();
}

// 0x100100e0 ?SetupSkillCache@MfActor@@UAEXH@Z
void MfActor::SetupSkillCache(int size)
{
    if (mPrototype)
        mPrototype->SetSkillCacheSize(size);
}

// 0x10011870 ?AddSkill@MfActor@@SAXPAVMfProtoActor@@PAVMfSkill@@@Z
void MfActor::AddSkill(MfProtoActor *proto, MfSkill *skill)
{
    proto->AddSkill(skill);
}

// included here, not at the top: their inline members change VC5's register
// choice in ignoreTriangleCollisionCheck when declared before it
#include "MfHPTimer.h"
#include "MfFloor.h"
#include "MfRoadMap.h"
#include "MfTriangleSet.h"
#include "MfColModel.h"
#include "MfRenderNode.h"
#include "MfProtoGeomInfo.h"
#include "MfTransform2.h"
#include "MfMetaColModel.h"
#include "MfProtoProxDef.h"
#include "MfProximityManager.h"
#include "MfFilterItem.h"

// 0x10010100 ?presampleAllSkills@MfActor@@QAEXH@Z
// Samples every skill of the prototype once (only while one actor uses it);
// a skill already sampled only gets its info computed again unless resample
// is set.
void MfActor::presampleAllSkills(int resample)
{
    if (mPrototype && mPrototype->Instances()->GetCount() <= 1) {
        int memory = 0;
        MfHPTimer timer;
        timer.Start();
        MfDblList *skills = mPrototype->Skills();
        theSoundManager->SetActorPath(mPrototype->SoundPath());
        MfDblListItem *pos;
        for (MfSkill *skill = (MfSkill *)skills->GetFirst(pos); skill; skill = (MfSkill *)skills->GetNext(pos)) {
            if (!resample && skill->isPresampled())
                skill->ComputeInfo(this);
            else
                skill->Presample(this);
            memory += skill->sampledPath()->MemorySize();
            skill->InitSounds(this);
        }
        timer.Stop();
    }
}

// 0x10010200 ?AttachToPrototype@MfActor@@QAEXPAVMfProtoActor@@H@Z
void MfActor::AttachToPrototype(MfProtoActor *proto, int flags)
{
    proto->AddRef();
    mPrototype = proto;
    mSkeleton = MfSkeletonNode::CreateFromPrototype(proto->RootNode());
    setSkeleton(mSkeleton, flags);
    proto->Instances()->Add(this);
    mInstanceIndex = proto->Instances()->GetCount() - 1;
}

// 0x10010250 ?Load@MfActor@@QAEHVMfString@@HH@Z
// Loads the prototype <data path>\Actors\<name>\<name>.act (\Exports\ for
// playback actors; a relative result is taken from the current directory)
// and attaches the actor to it. Returns 0, or 7 when there is no prototype.
int MfActor::Load(MfString name, int render, int share)
{
    MfString file(name);
    file << ".act";
    MfString path = GetDataPath();
    if (usePlaybackActors)
        path << "\\Exports\\" << name;
    else
        path << "\\Actors\\" << name;
    MfString fullPath = path + MfString("\\") + file;
    MfString drive, dir, ext, fname;
    fullPath.SplitPath(&drive, &dir, &fname, &ext);
    if (dir.Length() == 0) {
        char cwd[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, cwd);
        MfString relative(fullPath);
        fullPath = cwd;
        fullPath << "\\";
        fullPath << relative;
    }
    MfFileProgress::SetProgress(0, fname);
    int loaded;
    MfProtoActor *proto = MfProtoDB::GetPrototype(fullPath, loaded, share);
    if (loaded) {
        MfActorRWHookData data;
        data.filePath = fullPath;
        data.pActor = 0;
        data.pProto = proto;
        gHookManager->CallHooks('WRPa', &data);
    }
    MfFileProgress::SetProgress(2, fname);
    if (proto) {
        AttachToPrototype(proto, render);
        ShowNodes('Hndl', 0);
        ShowNodes('Cmra', 0);
        ShowNodes('Flor', 0);
        MfActorRWHookData data;
        data.filePath = proto->Path();
        data.pActor = this;
        data.pProto = 0;
        gHookManager->CallHooks('WRac', &data);
        g_world->RegisterActor(this);
        return 0;
    }
    return 7;
}

// 0x10010690 ?Save@MfActor@@QAEHW4MfSkillFormat@@@Z
int MfActor::Save(MfSkillFormat format)
{
    if (mPrototype) {
        MfActorRWHookData data;
        data.filePath = mPrototype->Path();
        data.pActor = this;
        data.pProto = 0;
        gHookManager->CallHooks('WWAc', &data);
        return mPrototype->Save(format);
    }
    return 0;
}

// 0x10010680 ??1MfActorRWHookData@@QAE@XZ (compiler-generated, from Save)

// 0x10010760 ?DetachFromPrototype@MfActor@@QAEXXZ
void MfActor::DetachFromPrototype()
{
    if (!mPrototype)
        return;
    for (int i = 0; i < mAllTraversal->numNodes(); i++)
        mAllTraversal->node(i)->mProto = 0;
    MfDblListItem *pos = mPrototype->Instances()->Find(this);
    if (pos)
        mPrototype->Instances()->Delete(pos);
    int index = 0;
    if (!mTempCopy) {
        for (MfActor *actor = (MfActor *)mPrototype->Instances()->GetFirst(pos); pos;
             actor = (MfActor *)mPrototype->Instances()->GetNext(pos)) {
            actor->mInstanceIndex = index;
            index++;
        }
    }
    mPrototype->Release();
    if (mPrototype->Instances()->GetCount() == 0)
        MfProtoDB::RemovePrototype(mPrototype);
    mPrototype = 0;
}

// 0x10010830 ?RemoveSubtree@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@@Z
void MfActor::RemoveSubtree(MfProtoActor *proto, MfProtoNode *node)
{
    int index = proto->NodeTraversal()->NodeIndex(node);
    proto->RemoveSubtree(node);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->RemoveSubtree(index);
    MfActor *first = (MfActor *)instances->GetFirst(pos);
    first->presampleAllSkills(1);
}

// 0x100108b0 ?RemoveSubtree@MfActor@@IAEXH@Z
// Removes the subtree under node index: its cameras and floors leave the
// space (and the road map), then the nodes are deleted.
void MfActor::RemoveSubtree(int index)
{
    MfSkeletonNode *node = mAllTraversal->node(index);
    int i;
    if (mSpace) {
        MfSkeletonTraversal *cameras = new MfSkeletonTraversal(node, 'Cmra', 1);
        for (i = 0; i < cameras->numNodes(); i++)
            mSpace->RemoveCamera((MfCameraNode *)cameras->node(i));
        delete cameras;
    }
    MfRoadMapBuilder builder;
    if (mSpace) {
        MfSkeletonTraversal *floors = new MfSkeletonTraversal(node, 'Flor', 1);
        for (i = 0; i < floors->numNodes(); i++)
            builder.RemoveFloor(floors->node(i), mSpace->RoadMap());
        delete floors;
    }
    node->changeParent(0, 0);
    if (mSpace)
        node->RemoveSpace(mSpace, 1);
    delete node;
    setSkeleton(mSkeleton, 0);
}

// TODO 0x10010a20 ?RenameNode@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@VMfString@@@Z: inline budget: the original calls the imported MfString::CopyOnWrite(int) in both ConvertToUpper expansions where MfString.h expands it (65% similar)
// Renames a prototype node; every instance re-keys the node in its dictionary
// (by upper-case name).
void MfActor::RenameNode(MfProtoActor *proto, MfProtoNode *node, MfString name)
{
    MfString oldName(node->Name());
    oldName.ConvertToUpper();
    node->SetName(name);
    name.ConvertToUpper();
    int index = proto->NodeTraversal()->NodeIndex(node);
    if (index < 0)
        return;
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos)) {
        actor->mNodeDictionary.Delete(oldName);
        actor->mNodeDictionary.Add(name, actor->mAllTraversal->node(index));
    }
}

// 0x10010b50 ?AddSubtree@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@1@Z
void MfActor::AddSubtree(MfProtoActor *proto, MfProtoNode *subtree, MfProtoNode *parent)
{
    int index = proto->NodeTraversal()->NodeIndex(parent);
    proto->AddSubtree(subtree, parent);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->AddSubtree(subtree, index);
    MfActor *first = (MfActor *)instances->GetFirst(pos);
    first->presampleAllSkills(1);
}

// 0x10010bd0 ?AddSubtree@MfActor@@IAEXPAVMfProtoNode@@H@Z
// Builds the nodes of a prototype subtree under node index; in a space the
// new nodes get render nodes, the space and its cameras.
void MfActor::AddSubtree(MfProtoNode *proto, int index)
{
    MfSkeletonNode *parent = mAllTraversal->node(index);
    MfSkeletonNode *node = MfSkeletonNode::CreateFromPrototype(proto);
    node->changeParent(parent, 0);
    node->SetOwner(this, 1);
    setSkeleton(mSkeleton, 1);
    if (mSpace) {
        MfSkeletonTraversal *nodes = new MfSkeletonTraversal(node, 0, 0);
        for (int i = 0; i < nodes->numNodes(); i++) {
            if (nodes->node(i)->Type() == MfSkeletonNode::TYPE)
                nodes->node(i)->CreateRenderNode();
            if (nodes->node(i)->Type() == MfFloor::TYPE)
                nodes->node(i)->CreateRenderNode();
            nodes->node(i)->SetSpace(mSpace, 0);
            if (nodes->node(i)->Type() == MfCameraNode::TYPE)
                mSpace->AddCamera((MfCameraNode *)nodes->node(i));
        }
        delete nodes;
    }
    parent->syncChildTransforms(0);
}

// 0x10010d10 ?Collapse@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@@Z
void MfActor::Collapse(MfProtoActor *proto, MfProtoNode *node)
{
    int index = proto->NodeTraversal()->NodeIndex(node);
    proto->Collapse(node);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->Collapse(index);
}

// 0x10010d70 ?Collapse@MfActor@@IAEXH@Z
void MfActor::Collapse(int index)
{
    MfSkeletonNode *node = mAllTraversal->node(index);
    MfTreeNode *child = node->firstChild();
    while (child) {
        MfTreeNode *next = child->nextSibling();
        RemoveSubtree(mAllTraversal->nodeIndex(child));
        child = next;
    }
    node->ReplaceRenderNode();
    node->colNode().ColModel() = node->mProto->ColModel();
}

// 0x10010dd0 ?MoveSubtree@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@1@Z
void MfActor::MoveSubtree(MfProtoActor *proto, MfProtoNode *node, MfProtoNode *parent)
{
    int index = proto->NodeTraversal()->NodeIndex(node);
    int parentIndex = proto->NodeTraversal()->NodeIndex(parent);
    proto->MoveSubtree(node, parent);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->MoveSubtree(index, parentIndex);
}

// 0x10010e50 ?MoveSubtree@MfActor@@IAEXHH@Z
void MfActor::MoveSubtree(int index, int parentIndex)
{
    MfSkeletonNode *node = mAllTraversal->node(index);
    MfSkeletonNode *parent = mAllTraversal->node(parentIndex);
    MfTransform3 global = node->globalTransform();
    node->changeParent(parent, 0);
    setSkeleton(mSkeleton, 0);
    MfTransform3 parentGlobal = parent->globalTransform();
    node->setTransform(~parentGlobal * global);
}

// TODO 0x10010f60 ?SetHomeTransforms@MfActor@@SAXPAVMfProtoActor@@PAVMfTransform3@@@Z: inline budget: the original calls the imported MfTransform3::operator=, MfRotation3::normalize and the MfVector3/MfRotation3 default constructors where this tree expands them (the frame and every later offset shift with it); calls, conditions and stores are the same in the same order (73% similar)
// Gives the prototype's geometry nodes new home transforms (one per node of
// the geometry traversal; the root's translation is cleared) and moves every
// pivot to its node's origin. Without a collision model on the first node the
// hip is centred over the bounding box and the root put at its bottom. The
// instances keep their place (with y = 0) and get new render nodes.
void MfActor::SetHomeTransforms(MfProtoActor *proto, MfTransform3 *transforms)
{
    proto->isDirty = 1;
    transforms[0].translation() = MfVector3(0.0f, 0.0f, 0.0f);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    MfActor *actor;
    for (actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->GotoHomeConfig(0);
    int i;
    for (i = 0; i < proto->GeomTraversal()->mNumNodes; i++) {
        MfProtoNode *node = proto->GeomTraversal()->mNodes[i];
        node->mHomeTransform = transforms[i];
        node->mHomeTransform.rotation().normalize();
    }
    MfTransform3 pivot;
    for (i = 0; i < proto->GeomTraversal()->mNumNodes; i++) {
        proto->GeomTraversal()->mNodes[i]->GetRelativeHomeTransform(0, pivot);
        pivot.rotation() = ~pivot.rotation();
        pivot.translation() = MfVector3(0.0f, 0.0f, 0.0f);
        TransformPivot(proto, proto->GeomTraversal()->mNodes[i], pivot);
    }
    proto->ComputeBounds();
    MfVector3 boxMin(0.0f, 0.0f, 0.0f);
    MfVector3 boxMax(0.0f, 0.0f, 0.0f);
    proto->GetBoundingBox(boxMin, boxMax);
    int count = proto->Instances()->GetCount();
    MfTransform3 *places = 0;
    if (count) {
        places = new MfTransform3[count];
        MfTransform3 *place = places;
        for (actor = (MfActor *)instances->GetFirst(pos); actor; actor = (MfActor *)instances->GetNext(pos))
            *place++ = actor->GetRootNode()->GetLocalTransform();
    }
    MfTransform3 hip;
    if (!proto->GeomTraversal()->mNodes[1]->mColModel) {
        MfVector3 center = (boxMin + boxMax) / 2.0f;
        MfTransform3 offset;
        hip = proto->GeomTraversal()->mNodes[1]->mHomeTransform;
        hip.translation() = center;
        offset = ~proto->GeomTraversal()->mNodes[1]->mHomeTransform * hip;
        TransformPivot(proto, proto->GeomTraversal()->mNodes[1], offset);
    }
    MfVector3 &home = proto->GeomTraversal()->mNodes[1]->mHomeTransform.translation();
    hip.translation() = MfVector3(home.x(), boxMin.y(), home.z());
    TransformPivot(proto, proto->GeomTraversal()->mNodes[0], hip);
    proto->GeomTraversal()->mNodes[0]->mHomeTransform = MfTransform3::identity();
    proto->GeomTraversal()->mNodes[0]->mHomeTransform.rotation().normalize();
    proto->ComputeBounds();
    MfTransform3 root;
    MfTransform3 *place = places;
    for (actor = (MfActor *)instances->GetFirst(pos); actor; actor = (MfActor *)instances->GetNext(pos)) {
        root.translation() = place->translation();
        root.translation().y() = 0.0f;
        place++;
        root.rotation() = MfRotation3::identity();
        actor->GotoHomeConfig(root);
        for (i = 0; i < actor->mAllTraversal->numNodes(); i++) {
            bool enabled = actor->mAllTraversal->node(i)->GetRenderNode()->mEnabled;
            MfSkeletonNode *node = actor->mAllTraversal->node(i);
            MfTriangleSet *triangles = node->colModel() ? node->colModel()->mTriangleSet : 0;
            if (triangles)
                triangles->ChangeID();
            actor->mAllTraversal->node(i)->DeleteRenderNode();
            actor->mAllTraversal->node(i)->CreateRenderNode();
            actor->mAllTraversal->node(i)->GetRenderNode()->Enable(enabled);
        }
    }
    delete [] places;
}

// 0x10011570 ?TransformPivot@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@ABVMfTransform3@@@Z
void MfActor::TransformPivot(MfProtoActor *proto, MfProtoNode *node, const MfTransform3 &pivot)
{
    int index = proto->GeomTraversal()->NodeIndex(node);
    proto->TransformNode(node, pivot);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->TransformPivot(index, pivot);
}

// 0x100115e0 ?TransformPivot@MfActor@@IAEXHABVMfTransform3@@@Z
void MfActor::TransformPivot(int index, const MfTransform3 &pivot)
{
    mSkeletonTraversal->node(index)->TransformPivot(pivot);
}

// 0x10011600 ?ScaleSubtree@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@ABVMfVector3@@@Z
void MfActor::ScaleSubtree(MfProtoActor *proto, MfProtoNode *node, const MfVector3 &scale)
{
    int index = proto->NodeTraversal()->NodeIndex(node);
    if (index < 0)
        return;
    proto->ScaleSubtree(node, scale);
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos))
        actor->ScaleSubtree(index, scale);
    MfActor *first = (MfActor *)instances->GetFirst(pos);
    first->presampleAllSkills(0);
}

// 0x10011680 ?ScaleSubtree@MfActor@@IAEXHABVMfVector3@@@Z
void MfActor::ScaleSubtree(int index, const MfVector3 &scale)
{
    MfSkeletonNode *node = mAllTraversal->node(index);
    node->ScaleSubtree(scale, 1, 1);
}

// 0x100116c0 ?ChangeMaterial@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@HPAVMfMaterial@@@Z
// Replaces a material of a geometry node (material 0 when the node has its
// own triangles) and of the render nodes of every instance.
void MfActor::ChangeMaterial(MfProtoActor *proto, MfProtoNode *node, int index, MfMaterial *material)
{
    if (node->Style() != MfProtoNode::GEOMETRY)
        return;
    if (node->GeomInfo()->mTriangles)
        index = 0;
    if (!node->ColModel() || !node->ColModel()->mTriangleSet)
        return;
    node->ColModel()->mTriangleSet->Material(index) = *material;
    int nodeIndex = proto->NodeTraversal()->NodeIndex(node);
    MfRenderer::Instance()->PushSearchPath(proto->TexturePath());
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos)) {
        MfRenderNode *render = actor->mAllTraversal->node(nodeIndex)->GetRenderNode();
        if (render)
            render->ChangeMaterial(index, material);
    }
    MfRenderer::Instance()->PopSearchPath();
}

// 0x100117e0 ?ReplaceColModel@MfActor@@SAXPAVMfProtoActor@@PAVMfProtoNode@@PAVMfTriangleSet@@@Z
// A new collision model for a prototype node (built from its geometry when
// triangles is 0); the instances' nodes get new render nodes.
void MfActor::ReplaceColModel(MfProtoActor *proto, MfProtoNode *node, MfTriangleSet *triangles)
{
    int index = proto->NodeTraversal()->NodeIndex(node);
    if (index < 0)
        return;
    if (triangles)
        node->SetColModel(triangles);
    else
        node->CreateColModel();
    MfDblList *instances = proto->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); pos; actor = (MfActor *)instances->GetNext(pos)) {
        MfSkeletonNode *instanceNode = actor->mAllTraversal->node(index);
        if (instanceNode)
            instanceNode->ReplaceRenderNode();
    }
}

// 0x10011880 ?traceTransform2D@MfActor@@QAE?AVMfTransform2@@XZ
// The root's place on the ground: x/y are the root's z/x, the angle its turn
// about y.
MfTransform2 MfActor::traceTransform2D()
{
    MfTransform3 root = mSkeleton->transform();
    MfTransform2 trace;
    MfVector3 axis(0.0f, 0.0f, 0.0f);
    trace.translation() = MfVector2(root.z(), root.x());
    float angle;
    root.rotation().getValue(angle, axis);
    if (axis.y() < 0.0f)
        angle = -angle;
    trace.rotation() = angle;
    return trace;
}

// 0x10011960 ?getFloor@MfActor@@QAEPAVMfTriangleSet@@XZ
// The triangles of the floor the actor stands on (the root's parent), or 0.
MfTriangleSet *MfActor::getFloor()
{
    if (((MfSkeletonNode *)mSkeleton->parent())->Type() == MfFloor::TYPE)
        return ((MfSkeletonNode *)mSkeleton->parent())->colModel()->mTriangleSet;
    return 0;
}

// 0x100119a0 ?SetSpace@MfActor@@UAEHPAVMfSpace@@@Z
int MfActor::SetSpace(MfSpace *space)
{
    if (mSpace)
        return 0;
    Lock();
    mSpace = space;
    int i;
    for (i = 0; i < mAllTraversal->numNodes(); i++)
        mAllTraversal->node(i)->SetSpace(mSpace, 0);
    Unlock();
    for (i = 0; i < mAllTraversal->numNodes(); i++) {
        if (mAllTraversal->node(i)->Type() == MfCameraNode::TYPE)
            mSpace->AddCamera((MfCameraNode *)mAllTraversal->node(i));
    }
    presampleAllSkills(0);
    return 1;
}

// 0x10011a50 ?RemoveSpace@MfActor@@UAEHPAVMfSpace@@@Z
int MfActor::RemoveSpace(MfSpace *space)
{
    int result = 1;
    if (mSpace != space)
        return 0;
    Lock();
    int i;
    for (i = 0; i < mAllTraversal->numNodes(); i++) {
        MfSkeletonNode *node = mAllTraversal->node(i);
        if (node && !node->RemoveSpace(mSpace, 0))
            result = 0;
    }
    Unlock();
    for (i = 0; i < mAllTraversal->numNodes(); i++) {
        if (mAllTraversal->node(i)->Type() == MfCameraNode::TYPE)
            mSpace->RemoveCamera((MfCameraNode *)mAllTraversal->node(i));
    }
    Lock();
    mSpace = 0;
    Unlock();
    return result;
}

// 0x10011b20 ?GetSpace@MfActor@@UAEPAVMfAiSpace@@XZ
MfAiSpace *MfActor::GetSpace()
{
    return mSpace;
}

// 0x10011b30 ?ShowNodes@MfActor@@QAEXHH@Z
// Shows or hides the render nodes of the given node type (geometry nodes of
// style 2 stay hidden).
void MfActor::ShowNodes(int type, int show)
{
    for (int i = 0; i < mAllTraversal->numNodes(); i++) {
        MfSkeletonNode *node = mAllTraversal->node(i);
        if (node->Type() == type) {
            MfRenderNode *render = node->GetRenderNode();
            bool on = show ? true : false;
            if (type == MfSkeletonNode::TYPE && node->m18 == 2)
                on = false;
            if (render)
                render->Enable(on);
        }
    }
    if (mSpace)
        MfRenderer::Instance()->RenderAllViews();
}

// TODO 0x10011bc0 ?MakeSubTreeColNode@MfActor@@QAEPAVMfColNode@@H@Z: the open sprite-test shape (NOTES): the original keeps the constant 1 in ebx for the whole function (EH state, the owned flag, the check-style test, the style compares) and the new MfColNode in a stack slot; this tree loads the style into a register and threads the null-collision-node jump past the re-test of the node's collision node, which the original repeats (a control-flow difference, so not reviewed-equivalent). Calls, conditions and stores are the same (89% similar)
// A collision node whose model collects the checked skeleton nodes of the
// subtree under node index (sprites left out).
MfColNode *MfActor::MakeSubTreeColNode(int index)
{
    MfColNode *colNode = new MfColNode(0);
    MfMetaColModel *meta = new MfMetaColModel;
    colNode->OwnColModel(meta);
    MfMetaColModel::constructLock.Lock(-1);
    Lock();
    MfSkeletonNode *root = mSkeletonTraversal->node(index);
    MfSkeletonTraversal *nodes = new MfSkeletonTraversal(root, 0, 0);
    for (int i = 0; i < nodes->numNodes(); i++) {
        if (!(~nodes->node(i)->colNode().GetCheckStyle() & 1)) {
            MfSkeletonNode *node = nodes->node(i);
            MfProtoNode *proto = node->mProto;
            if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles
                && proto->GeomInfo()->mFacesCamera == 1)
                continue;
            if (node->colModel() && node->Type() == MfSkeletonNode::TYPE)
                meta->AddColNode(&nodes->node(i)->mColNode);
        }
    }
    meta->MakeMeta();
    Unlock();
    MfMetaColModel::constructLock.Unlock();
    delete nodes;
    return colNode;
}

// 0x10011d80 ?SetProximityDefinition@MfActor@@UAEHABVMfString@@MMMH@Z
// Sets the proximity definition of the prototype with the given name (case
// does not matter; the null string names the default one). In a space the
// instances are then registered with the proximity manager when any
// definition is enabled, else removed. Returns 0 when there is no such
// definition.
int MfActor::SetProximityDefinition(const MfString &name, float radius, float width, float length, int enabled)
{
    int result = 0;
    Lock();
    MfDblList *defs = mPrototype->ProximityDefs();
    MfString key;
    MfString defName;
    key = name;
    if (key != NULL_STRING)
        key.ConvertToUpper();
    MfDblListItem *pos;
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)defs->GetFirst(pos); def; def = (MfProtoProxDef *)defs->GetNext(pos)) {
        if (def->protoName == NULL_STRING && key == NULL_STRING)
            break;
        defName = def->protoName;
        defName.ConvertToUpper();
        if (key == defName)
            break;
    }
    if (def) {
        def->fRadius = radius;
        def->fRadiusSquared = radius * radius;
        def->fBCWidth = width;
        def->fBCLength = length;
        def->enabled = enabled;
        result = 1;
    }
    if (mSpace) {
        MfDblListItem *pli;
        defs = mPrototype->ProximityDefs();
        for (def = (MfProtoProxDef *)defs->GetFirst(pli); def; def = (MfProtoProxDef *)defs->GetNext(pli)) {
            if (def->enabled)
                break;
        }
        for (MfLiveActor *actor = (MfLiveActor *)mPrototype->Instances()->GetFirst(pli); actor;
             actor = (MfLiveActor *)mPrototype->Instances()->GetNext(pli)) {
            if (!def)
                mSpace->ProximityManager()->RemoveActor(actor);
            else
                mSpace->ProximityManager()->AddActor(actor);
        }
    }
    Unlock();
    return result;
}

// EQUIV 0x10012010 ?SetForwardFacingDirection@MfActor@@UAEXABVMfRotation3@@@Z: reviewed: helper inlined vs called: in root = mSkeleton->globalTransform() the original expands the MfRotation3 copy and calls the imported MfVector3::operator=, this tree calls the MfRotation3::operator= import and expands the MfVector3 copy (both plain member copies, mflinalg); the rest is identical
// Turns the root to face along direction (in the parent's frame) without
// moving the hip.
void MfActor::SetForwardFacingDirection(const MfRotation3 &direction)
{
    MfTransform3 hip = skeletonTraversal()->node(1)->globalTransform();
    MfTransform3 root = mSkeleton->transform();
    MfTransform3 parent = ((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
    MfRotation3 facing = ~parent.rotation() * direction;
    root.rotation() = facing;
    mSkeleton->setTransform(root);
    root = mSkeleton->globalTransform();
    hip = ~root * hip;
    skeletonTraversal()->node(1)->setTransform(hip);
}

// EQUIV 0x100121f0 ?SnapTraceToSurface@MfActor@@UAEHABVMfVector3@@H0@Z: reviewed: stack slots and the order of independent stores: the ray and forward vector swap slots, and the two zero stores of point and the EH state byte are scheduled around the origin copy (different slots, no call in between); calls, arguments and conditions are the same
// Casts a ray along direction (from origin when useOrigin is set, else from
// the hip) and stands the actor on the surface it hits: the root goes to the
// hit point with its y axis along the surface normal (facing the ray), the
// hip keeps its place, and the forward direction is kept as far as it can.
// Returns 0 when nothing is hit.
int MfActor::SnapTraceToSurface(const MfVector3 &direction, int useOrigin, const MfVector3 &origin)
{
    MfRay ray;
    ray.mOrigin = useOrigin ? origin : skeletonTraversal()->node(1)->globalTransform().translation();
    MfVector3 point(0.0f, 0.0f, 0.0f);
    ray.mDirection = direction;
    MfColResultRay result;
    if (ray.Cast(mSpace, mSkeleton, result, 0)) {
        MfSkeletonNode *node = (MfSkeletonNode *)result.node->Owner();
        point = result.pos;
        MfVector3 normal = result.node->ColModel()->mTriangleSet->FindTriangleNormal(result.triIndex);
        normal = node->globalTransform().rotation() * normal;
        if (ray.mDirection.dot(normal) >= 0.0f)
            normal = -normal;
        MfTransform3 hip = skeletonTraversal()->node(1)->globalTransform();
        MfTransform3 root;
        root.translation() = point;
        root.rotation().setValue(MfVector3(0.0f, 1.0f, 0.0f), normal);
        MfTransform3 parent = ((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
        root = ~parent * root;
        mSkeleton->setTransform(root);
        hip = ~mSkeleton->globalTransform() * hip;
        skeletonTraversal()->node(1)->setTransform(hip);
        MfVector3 forward = skeletonTraversal()->node(1)->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
        MfVector3 local = ~mSkeleton->globalTransform().rotation() * forward;
        if (local.y() > 0.98f || local.y() < -0.98f) {
            forward = skeletonTraversal()->node(1)->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
            local = ~mSkeleton->globalTransform().rotation() * forward;
        }
        MfRotation3 facing = mSkeleton->globalTransform().rotation();
        facing = facing * MfRotation3((float)atan2(local.x(), local.z()), MfVector3(0.0f, 1.0f, 0.0f));
        SetForwardFacingDirection(facing);
        return 1;
    }
    return 0;
}

// 0x10012780 ?CreateKChain@MfActor@@UAEPAVMfAiKChain@@ABVMfString@@HABVMfVector3@@@Z
MfAiKChain *MfActor::CreateKChain(const MfString &endNodeName, int numLinks, const MfVector3 &endOffset)
{
    MfKChain *chain = new MfKChain;
    MfSkeletonNode *endNode = (MfSkeletonNode *)GetNodeByName(endNodeName);
    if (!endNode)
        return 0;
    if (numLinks <= 0)
        return 0;
    chain->create(endNode, numLinks, endOffset);
    return chain;
}

// TODO 0x10012840 ?SetInstanceName@MfActor@@UAEHABVMfString@@@Z: the original re-tests mPrototype in the name's ?: (VC5 folds it here, the MfSpace::RemoveActorSync shape), keeps the MfActor pointer in a stack slot and expands the two ConvertToUpper calls differently (the second with an inline MfStringRef constructor); calls, conditions and stores are the same (61% similar)
// Renames the actor (the space's actor dictionary is keyed by the upper-case
// instance name). An actor without a name first gets the default name
// <prototype name><instance index>. Returns 0 when the space already has an
// actor of that name.
int MfActor::SetInstanceName(const MfString &name)
{
    if (mSpace && mSpace->GetActor(name))
        return 0;
    if (mInstanceName == NULL_STRING) {
        if (mPrototype)
            mInstanceName = MfString(mPrototype ? mPrototype->Name() : mName) + MfString(mInstanceIndex);
        else
            mInstanceName = MfString(mInstanceIndex);
    }
    MfString key(mInstanceName);
    key.ConvertToUpper();
    if (mSpace)
        mSpace->mActorDictionary.Delete(key);
    key = mInstanceName = name;
    key.ConvertToUpper();
    if (mSpace)
        mSpace->mActorDictionary.Add(key, this);
    return 1;
}

// 0x10012b00 ?DeleteFilterItem@MfActor@@UAEHPAUMfFilterItem@@@Z
// Removes the filter entry for the same node or actor; 0 when there is none.
int MfActor::DeleteFilterItem(MfFilterItem *item)
{
    MfFilterHookData data;
    data.pActor = this;
    data.item = item;
    data.kind = 2;
    gHookManager->CallHooks(MF_HOOK_FILTER_EDIT, &data);
    Lock();
    MfDblList *filter = &mFilter;
    MfDblListItem *pos;
    for (MfFilterItem *entry = (MfFilterItem *)filter->GetFirst(pos); entry;
         entry = (MfFilterItem *)filter->GetNext(pos)) {
        if (entry->type == item->type) {
            if (item->type >= 3 && item->type != 6) {
                if (entry->actor == item->actor) {
                    filter->Delete(pos);
                    delete entry;
                    Unlock();
                    return 1;
                }
            } else if (entry->node == item->node) {
                filter->Delete(pos);
                delete entry;
                Unlock();
                return 1;
            }
        }
    }
    Unlock();
    return 0;
}

// 0x10012bf0 ?AddFilterItem@MfActor@@UAEHPAUMfFilterItem@@@Z
// Adds an entry to the filter (the actor takes it); 0 when the node or actor
// is already there.
int MfActor::AddFilterItem(MfFilterItem *item)
{
    MfFilterHookData data;
    data.pActor = this;
    data.item = item;
    data.kind = 0;
    gHookManager->CallHooks(MF_HOOK_FILTER_EDIT, &data);
    Lock();
    MfDblList *filter = &mFilter;
    MfDblListItem *pos;
    for (MfFilterItem *entry = (MfFilterItem *)filter->GetFirst(pos); entry;
         entry = (MfFilterItem *)filter->GetNext(pos)) {
        if (entry->type == item->type) {
            if (item->type >= 3 && item->type != 6) {
                if (entry->actor == item->actor) {
                    Unlock();
                    return 0;
                }
            } else if (entry->node == item->node) {
                Unlock();
                return 0;
            }
        }
    }
    filter->Add(item);
    Unlock();
    return 1;
}

// 0x10012cd0 ?ClearFilter@MfActor@@UAEXXZ
void MfActor::ClearFilter()
{
    MfFilterHookData data;
    data.pActor = this;
    data.item = 0;
    data.kind = 1;
    gHookManager->CallHooks(MF_HOOK_FILTER_EDIT, &data);
    Lock();
    MfDblList *filter = &mFilter;
    MfDblListItem *pos;
    for (MfFilterItem *entry = (MfFilterItem *)filter->GetFirst(pos); entry;
         entry = (MfFilterItem *)filter->GetNext(pos))
        delete entry;
    filter->Clear();
    Unlock();
}

// 0x10012d60 ?UseFilter@MfActor@@QAEXHHHAAVMfDblList@@@Z
// Switches off the collision checks of the filtered nodes (entries of
// nodeType) and of every geometry node of the filtered actors (entries of
// actorType), saving the check styles in styles; with save 0 the styles are
// restored from the list in the same order.
void MfActor::UseFilter(int save, int nodeType, int actorType, MfDblList &styles)
{
    MfDblListItem *stylePos;
    void *style;
    if (!save)
        style = styles.GetFirst(stylePos);
    Lock();
    MfDblList *filter = &mFilter;
    MfDblListItem *pos;
    for (MfFilterItem *entry = (MfFilterItem *)filter->GetFirst(pos); entry;
         entry = (MfFilterItem *)filter->GetNext(pos)) {
        if (entry->type == nodeType) {
            entry->node->GetActor()->Lock();
            if (save) {
                styles.Add((void *)entry->node->ColNode().GetCheckStyle());
                entry->node->ColNode().SetCheckStyle(0);
            } else {
                entry->node->ColNode().SetCheckStyle((int)style);
                style = styles.GetNext(stylePos);
            }
            entry->node->GetActor()->Unlock();
        }
        if (entry->type == actorType) {
            MfActor *actor = (MfActor *)entry->actor;
            for (int i = 0; i < actor->skeletonTraversal()->numNodes(); i++) {
                actor->skeletonTraversal()->node(i)->GetActor()->Lock();
                if (save) {
                    styles.Add((void *)actor->skeletonTraversal()->node(i)->ColNode().GetCheckStyle());
                    actor->skeletonTraversal()->node(i)->ColNode().SetCheckStyle(0);
                } else {
                    actor->skeletonTraversal()->node(i)->ColNode().SetCheckStyle((int)style);
                    style = styles.GetNext(stylePos);
                }
                actor->skeletonTraversal()->node(i)->GetActor()->Unlock();
            }
        }
    }
    Unlock();
}

// 0x10012f50 ?GetGeomNodeByIndex@MfActor@@UAEPAVMfAiActorNode@@H@Z
MfAiActorNode *MfActor::GetGeomNodeByIndex(int index)
{
    if (index < mSkeletonTraversal->numNodes() && index >= 0) {
        MfSkeletonNode *node = mSkeletonTraversal->node(index);
        if (node)
            return node;
    }
    return 0;
}

// 0x10012f80 ?GetNumGeomNodes@MfActor@@UAEHXZ
int MfActor::GetNumGeomNodes()
{
    return mSkeletonTraversal->numNodes();
}

// 0x1007dfa0 ?CorrectPlacement@MfActor@@UAEXAAVMfTransform3@@ABVMfVector3@@@Z
void MfActor::CorrectPlacement(MfTransform3 &, MfVector3 const &)
{
    return;
}

// 0x1007dfd0 ?CreateOverlayActivity@MfActor@@UAEPAVMfAiActivity@@XZ
MfAiActivity * MfActor::CreateOverlayActivity()
{
    return 0;
}

// 0x1007f9e0 ?ActivateBehavior@MfActor@@UAEHABVMfString@@@Z
int MfActor::ActivateBehavior(MfString const &)
{
    return 0;
}

// 0x1007fa30 ?CreateHoldActivity@MfActor@@UAEPAVMfAiActivity@@PBVMfAiActor@@H@Z
MfAiActivity * MfActor::CreateHoldActivity(MfAiActor const *, int)
{
    return 0;
}

// 0x1007fe80 ?CreateGestureActivity@MfActor@@UAEPAVMfAiActivity@@ABVMfString@@HH@Z
MfAiActivity * MfActor::CreateGestureActivity(MfString const &, int, int)
{
    return 0;
}

// ---- members without a unit of their own -----------------------------------
// In the original these are inline members of the exported class, emitted with
// it in the MfActor_1 unit (MfActor_1.cpp, its own declaration of the class),
// and the empty virtuals are folded with the stubs above (xor eax, eax / ret n,
// see the vtable at 100b1dcc). This unit's MfActor.h declares them out of line,
// so they are defined here; they are not claimed.

int MfActor::usePlaybackActors;

const MfString &MfActor::Name()
{
    if (mPrototype)
        return mPrototype->sName;
    return mName;
}

const MfString &MfActor::InstanceName()
{
    if (mInstanceName == NULL_STRING) {
        if (mPrototype)
            mInstanceName = MfString(Name()) + MfString(mInstanceIndex);
        else
            mInstanceName = MfString(mInstanceIndex);
    }
    return mInstanceName;
}

const MfString &MfActor::GetClassName()
{
    if (mPrototype)
        return mPrototype->sName;
    return mName;
}

MfString MfActor::GetInstanceName()
{
    return InstanceName();
}

MfAiSkill *MfActor::GetSkill(const MfString &name)
{
    return getSkill(name);
}

// The original returns a reference to a temporary copy of NULL_STRING that it has
// already destroyed; an empty string that stays alive serves the callers the same.
const MfString &MfActor::GetLastMessage()
{
    static MfString empty;
    return empty;
}

MfDblList *MfActor::Filter()
{
    return &mFilter;
}

int MfActor::DeactivateBehavior(const MfString &)
{
    return 0;
}

int MfActor::LoadBehavior(const MfString &, int)
{
    return 0;
}

int MfActor::UnloadBehavior(const MfString &)
{
    return 0;
}

int MfActor::DetachTarget(const MfAiActor *, int)
{
    return 0;
}

int MfActor::AddActivity(const MfAiActivity *)
{
    return 0;
}

int MfActor::SetActivity(const MfAiActivity *)
{
    return 0;
}

MfAiActivity * MfActor::GetRootActivity()
{
    return 0;
}

int MfActor::RemoveActivity(const MfAiActivity *, int)
{
    return 0;
}

int MfActor::RemoveActivity(int, int)
{
    return 0;
}

int MfActor::SendMessage(const MfAiActor *, const MfString &)
{
    return 0;
}

int MfActor::StopTimer(int)
{
    return 0;
}

int MfActor::GetLastEventID()
{
    return 0;
}

MfAiActivity * MfActor::GetLastActivity()
{
    return 0;
}

const MfCollisionData * MfActor::GetLastCollisionInfo()
{
    return 0;
}

const MfProximityData * MfActor::GetLastProximityInfo()
{
    return 0;
}

const MfPickData * MfActor::GetLastPickInfo()
{
    return 0;
}

const MfContourFailDownData * MfActor::GetLastContourFailDownInfo()
{
    return 0;
}

int MfActor::RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, int)
{
    return 0;
}

int MfActor::RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int)
{
    return 0;
}

int MfActor::RequestLocoPlan(const MfAiActor *, const MfString &, const MfString &, float, float, float, float, int)
{
    return 0;
}

int MfActor::RequestGraspPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int)
{
    return 0;
}

int MfActor::RequestPlacePlan(const MfAiActor *, const MfVector3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int)
{
    return 0;
}

int MfActor::RequestPlacePlan(const MfAiActor *, const MfTransform3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int)
{
    return 0;
}

int MfActor::RequestSitPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int)
{
    return 0;
}

int MfActor::RequestPosePlan(const MfAiActor *, const MfString &, int)
{
    return 0;
}

int MfActor::RequestUngraspPlan(const MfAiActor *, const MfString &, int)
{
    return 0;
}

int MfActor::SampleAttachSkill(const MfTransform3 &, const MfString &, unsigned long)
{
    return 0;
}

MfAiActivity * MfActor::CreateComplyActivity(const MfAiActor *, const MfString &, const MfString &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfAiActor *, const MfString &, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfVector3 &, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int, const MfVector3 *, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, const MfAiActorNode *, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateCameraActivity(const MfString &, const MfDblList &, int, const MfVector3 *, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateRandomActivity(int)
{
    return 0;
}

MfAiActivity * MfActor::CreateSequentialActivity(int)
{
    return 0;
}

MfAiActivity * MfActor::CreateLocoActivity(const MfString &, float, float, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateLocoActivity(const MfString &, float, float, float, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateLocoActivity(const MfString &, const MfVector3 &, float, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateLocoActivity(const MfString &, const MfAiPath2 *, float, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, float, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, const MfVector3 &, const MfTransform3 &, const MfTransform3 &, float, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, const MfVector3 &, const MfVector3 &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, MfTransform3 *, int, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, unsigned long, MfTransform3 *, int, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, unsigned long, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, const MfAiActor *, const MfString &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::Create3dPathActivity(const MfString &, unsigned long, const MfAiActor *, const MfString &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreateAttachActivity(const MfTransform3 &, const MfString &, int, int)
{
    return 0;
}

MfAiActivity * MfActor::CreatePluginActivity(const MfAiActivitySpec *, int, int)
{
    return 0;
}

int MfActor::GetLastLocoFailureInfo()
{
    return 0;
}

int MfActor::GetLastManipFailureInfo()
{
    return 0;
}

MfISoundResource * MfActor::PlaySound(const MfString &, MfAiActorNode *)
{
    return 0;
}

int MfActor::ChangeTexture(const MfString &, int, const MfString &, const MfString &)
{
    return 0;
}

int MfActor::GetLastOptionID()
{
    return 0;
}

MfScDataHandle * MfActor::GetPiccoloHandle()
{
    return 0;
}

MfAiController * MfActor::GetController()
{
    return 0;
}

int MfActor::SetAnchorNodeStop(MfAiActorNode *)
{
    return 0;
}

MfAiActorNode * MfActor::GetAnchorNodeStop()
{
    return 0;
}

MfAiActor * MfActor::OwnerActor()
{
    return 0;
}

void MfActor::ClearActivity()
{
    return;
}

void MfActor::EnableBehavior(int)
{
    return;
}

void MfActor::FacePoint(const MfVector3 &)
{
    return;
}

void MfActor::UndoSample()
{
    return;
}

void MfActor::HandleLButtonDown(MfPickData *)
{
    return;
}

void MfActor::HandleLButtonUp(MfPickData *)
{
    return;
}

void MfActor::HandleRButtonDown(MfPickData *)
{
    return;
}

void MfActor::HandleRButtonUp(MfPickData *)
{
    return;
}

void MfActor::HandleMouseMove(MfPickData *)
{
    return;
}

void MfActor::SetOptionTarget(const MfAiActor *)
{
    return;
}

void MfActor::SyncTraceUnderHip()
{
    return;
}

void MfActor::SetPlannedActivityOptions(int, int)
{
    return;
}

void MfActor::SetController(MfAiController *)
{
    return;
}

void MfActor::HandleKeyboardEvent(unsigned int, unsigned int)
{
    return;
}

void MfActor::HandleKeyboardDownEvent(unsigned int, unsigned int)
{
    return;
}

void MfActor::HandleKeyboardUpEvent(unsigned int, unsigned int)
{
    return;
}

void MfActor::HandleKeyboardRepeatEvent(unsigned int, unsigned int)
{
    return;
}

int MfActor::SendEvent(const MfAiActor *, const MfString &, unsigned long)
{
    return 0;
}

int MfActor::SendEvent(const MfAiActor *, MfScDataHandle *, unsigned long)
{
    return -5;
}

int MfActor::StartTimer(int, const MfString &, int, int)
{
    return 0;
}

int MfActor::StartTimer(int, MfScDataHandle *, int, int)
{
    return -5;
}

int MfActor::RequestTouchPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int)
{
    return -1;
}

