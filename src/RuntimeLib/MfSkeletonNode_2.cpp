// MfSkeletonNode (RuntimeLib.dll 1004ce20-1004f950).
#include <string.h>
#include "MfSkeletonNode.h"
#include "MfActor.h"
#include "MfSkeletonTraversal.h"
#include "MfColModel.h"
#include "MfProperty.h"
#include "MfPoActorNode.h"
#include "MfScDataHandle.h"
#include "MfRenderNode.h"
#include "MfTriangleSet.h"
#include "MfCameraNode.h"
#include "MfFloor.h"
#include "MfHandle.h"
#include "MfLightNode.h"
#include "MfProtoGeomInfo.h"
#include "MfRenderScene.h"
#include "MfSoundManager.h"
#include "MfSpace.h"
#include "MfSplineNode.h"
#include "MfRenderer.h"
#include "MfHookManager.h"
#include "MfProtoActor.h"
#include "MfSpline.h"
#include "MfLiveActor.h"

// Compiler-generated members of the exported MfRenderScene this object emits:
// 0x1004cd00 ??0MfRenderScene@@QAE@ABV0@@Z
// 0x1004cd60 ??4MfRenderScene@@QAEAAV0@ABV0@@Z

// held while nodes change parents
// 0x1004cdc0 _$E55
// 0x1004cdd0 _$E51
// 0x1004cdf0 _$E54
// 0x1004ce00 _$E52 (the destructor entry: ~MfLock is inline and empty)
MfLock MfSkeletonNode::parentLock;

// 0x1004ce20 ??0MfSkeletonNode@@QAE@XZ
MfSkeletonNode::MfSkeletonNode()
    : m18(0), m1c(15), mProto(0), mOwner(0), mTransformDirty(0), mAllowTransform(1),
      mIsSprite(0), mColNode(0), mLastPickedTriIndex(-1), mSpace(0), mRenderNode(0),
      mLocalColModel(0), mOwnsLocalColModel(false), mLocalName(NULL_STRING), mPiccoloHandle(0)
{
    mParent = 0;
    mDestroysChildrenOnDestruct = 0;
}

// EQUIV 0x1004cfc0 ??0MfSkeletonNode@@QAE@ABV0@@Z: reviewed: helper inlined vs called: the original copies mLocalHomeTransform's rotation with the imported MfRotation3 copy constructor (a plain 4-float copy, as the inline one here), with two more EH states and their funclets for the half-built transform; identical up to there, the rest the same with shifted state numbers
MfSkeletonNode::MfSkeletonNode(const MfSkeletonNode &other)
    : m18(0), m1c(15), mProto(other.mProto), mOwner(0), mTransform(other.mTransform),
      mGlobalTransform(other.mGlobalTransform), mTransformDirty(0),
      mAllowTransform(other.mAllowTransform), mIsSprite(other.mIsSprite), mColNode(0),
      mLastPickedTriIndex(-1), mSpace(0), mRenderNode(0), mLocalColModel(0),
      mOwnsLocalColModel(false), mLocalName(other.mLocalName),
      mLocalHomeTransform(other.mLocalHomeTransform), mPiccoloHandle(other.mPiccoloHandle)
{
    mDestroysChildrenOnDestruct = other.mDestroysChildrenOnDestruct;
    for (MfSkeletonNode *child = (MfSkeletonNode *)other.mFirstChild; child; child = (MfSkeletonNode *)child->mNextSibling) {
        if (child->mOwner == other.mOwner)
            child->createCopy()->changeParent(this, 0);
    }
}

// 0x1004d170 ??1MfSkeletonNode@@UAE@XZ
// Deletes the children, tells the script object the node is gone and drops
// the render node (out of the space's scene) and an owned collision model.
MfSkeletonNode::~MfSkeletonNode()
{
    MfTreeNode *child = mFirstChild;
    while (child) {
        MfTreeNode *next = child->nextSibling();
        delete child;
        child = next;
    }
    if (mPiccoloHandle) {
        MfPoActorNode *po = dynamic_cast<MfPoActorNode *>(mPiccoloHandle->GetData());
        if (po)
            po->SetNodeDeleted();
        mPiccoloHandle->Release();
        mPiccoloHandle = 0;
    }
    if (mRenderNode) {
        if (mSpace)
            mSpace->GetRenderScene()->RemoveNode(mRenderNode);
        delete mRenderNode;
        mRenderNode = 0;
    }
    if (mLocalColModel) {
        if (mOwnsLocalColModel && mLocalColModel)
            delete mLocalColModel;
        mLocalColModel = 0;
    }
}

// 0x1004d3f0 ?CreateFromPrototype@MfSkeletonNode@@SAPAV1@PAVMfProtoNode@@@Z
// Builds the node tree of a prototype subtree: the node class follows the
// prototype's style; every node starts at its home transform with the
// prototype's collision model.
MfSkeletonNode *MfSkeletonNode::CreateFromPrototype(MfProtoNode *proto)
{
    MfSkeletonNode *node;
    switch (proto->Style()) {
    case MfProtoNode::GEOMETRY:
        node = new MfSkeletonNode;
        node->mIsSprite = (int)proto->GeomInfo()->mTriangles;
        break;
    case MfProtoNode::HANDLE:
        node = new MfHandle;
        break;
    case MfProtoNode::FLOOR:
        node = new MfFloor;
        break;
    case MfProtoNode::CAMERA:
        node = new MfCameraNode(proto);
        break;
    case MfProtoNode::LIGHT:
        node = new MfLightNode(proto);
        break;
    case MfProtoNode::SPLINE:
        node = new MfSplineNode;
        break;
    }
    node->mProto = proto;
    node->setTransform(proto->mHomeTransform);
    node->mColNode.ColModel() = proto->ColModel();
    node->mColNode.Owner() = node;
    for (MfTreeNode *child = proto->firstChild(); child; child = child->nextSibling())
        node->addChild(CreateFromPrototype((MfProtoNode *)child));
    return node;
}

// 0x1004d8b0 ?setGeometry@MfSkeletonNode@@QAEXPAVMfTriangleSet@@VMfString@@@Z
// Creates the render node for the triangles (textures searched in
// texturePath); sprites take the camera-facing settings of the prototype,
// a light node also lights its render node.
void MfSkeletonNode::setGeometry(MfTriangleSet *triangles, MfString texturePath)
{
    if (mRenderNode)
        return;
    if (texturePath != NULL_STRING)
        MfRenderer::Instance()->PushSearchPath(texturePath);
    mRenderNode = MfRenderer::Instance()->CreateNode();
    if (MfRenderer::Instance()->GetInfo() && !MfRenderer::Instance()->GetInfo()->m0c)
        mRenderTransform = &mTransform;
    else
        mRenderTransform = &mGlobalTransform;
    MfRenderNodeInfo info;
    info.mID = triangles ? triangles->GetID() : 0;
    info.mTriangles = triangles;
    info.m08 = (int)&mInstProperties;
    info.m0c = mIsSprite != 0;
    if (mIsSprite && mProto) {
        info.m0d = mProto->GeomInfo()->mFacesCamera == 1;
        info.m10 = mProto->GeomInfo()->mE0;
        info.m14 = mProto->GeomInfo()->mE4;
    } else {
        info.m0d = false;
        info.m14 = 0;
        info.m10 = 0;
    }
    if (mRenderNode) {
        mRenderNode->Create(&info);
        mRenderNode->SetTransform(*mRenderTransform);
        if (Type() == MfLightNode::TYPE) {
            mRenderNode->SetupLight(&((MfLightNode *)this)->mLight);
            mRenderNode->EnableLight(true);
        }
    }
    if (texturePath != NULL_STRING)
        MfRenderer::Instance()->PopSearchPath();
}

// 0x1004dc70 ?SetRenderStyle@MfSkeletonNode@@UAEXH@Z
// 0 solid, 1 wire frame, 2 hidden (announced with MF_HOOK_MISC_DATA, kind 4).
void MfSkeletonNode::SetRenderStyle(int style)
{
    if (!mRenderNode)
        return;
    MfMiscHookData data;
    data.pActor = mOwner;
    data.m18 = (int)(MfAiActorNode *)this;
    data.delay = style;
    data.kind = 4;
    gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    if (style == 0) {
        mRenderNode->SetWireframe(false);
        mRenderNode->Enable(true);
    } else if (style == 2) {
        mRenderNode->Enable(false);
    } else if (style == 1) {
        mRenderNode->Enable(true);
        mRenderNode->SetWireframe(true);
    }
    if (mOwner)
        mOwner->SetStateChangeFlag(1);
}

// 0x1004e080 ?CreateRenderNode@MfSkeletonNode@@QAEXXZ
// The render node of the node's geometry (textures from the actor's texture
// path); in a space it joins the scene under the parent's render node, the
// nodes without visible geometry (floors, handles, cameras, lights) hidden.
void MfSkeletonNode::CreateRenderNode()
{
    if (mRenderNode)
        return;
    MfTriangleSet *triangles = colModel() ? colModel()->mTriangleSet : 0;
    MfString texturePath(NULL_STRING);
    if (mProto && mProto->GetActor())
        texturePath = mProto->GetActor()->TexturePath();
    setGeometry(triangles, texturePath);
    if (mSpace) {
        int type = Type();
        if (type == MfFloor::TYPE || type == MfHandle::TYPE || type == MfCameraNode::TYPE || type == MfLightNode::TYPE)
            mRenderNode->Enable(false);
        mRenderNode->SetParent(parent() ? ((MfSkeletonNode *)parent())->GetRenderNode() : 0);
        mSpace->GetRenderScene()->AddNode(mRenderNode);
    }
}

// 0x1004e1e0 ?ReplaceRenderNode@MfSkeletonNode@@QAEXXZ
// A new render node for a leaf node whose geometry changed (keeps whether it
// was shown).
void MfSkeletonNode::ReplaceRenderNode()
{
    if (mFirstChild || !mRenderNode)
        return;
    bool enabled = mRenderNode->mEnabled;
    if (mSpace)
        mSpace->GetRenderScene()->RemoveNode(mRenderNode);
    delete mRenderNode;
    mRenderNode = 0;
    MfTriangleSet *triangles = colModel() ? colModel()->mTriangleSet : 0;
    if (triangles)
        triangles->ChangeID();
    CreateRenderNode();
    if (mRenderNode) {
        mRenderNode->SetParent(parent() ? ((MfSkeletonNode *)parent())->GetRenderNode() : 0);
        mRenderNode->Enable(enabled);
    }
}

// 0x1004e2c0 ?DeleteRenderNode@MfSkeletonNode@@QAEXXZ
void MfSkeletonNode::DeleteRenderNode()
{
    if (!mRenderNode)
        return;
    for (MfSkeletonNode *child = (MfSkeletonNode *)mFirstChild; child;
         child = (MfSkeletonNode *)child->nextSibling())
        child->mRenderNode->SetParent(0);
    if (mSpace)
        mSpace->GetRenderScene()->RemoveNode(mRenderNode);
    delete mRenderNode;
    mRenderNode = 0;
}

// EQUIV 0x1004e340 ?changeParent@MfSkeletonNode@@QAEXPAV1@PAVMfTransform3@@@Z: reviewed: helpers inlined vs called: the original calls the imported MfRotation3() (identity) twice and MfVector3::operator= in the first branch where this expands them, and expands the translation copy in the last branch where this calls the operator= import (all plain member copies); the EH state numbers shift with the extra states of the calls. Calls, conditions and stores otherwise the same in the same order
// Moves the node under newParent (0: out of the tree), the actors of both
// sides locked. The node gets xform as its new local transform when given,
// else it keeps its global transform (its local one when it leaves the tree).
void MfSkeletonNode::changeParent(MfSkeletonNode *newParent, MfTransform3 *xform)
{
    MfTransform3 relative;
    MfTransform3 local;
    MfSkeletonNode *oldParent = (MfSkeletonNode *)parent();
    if (!oldParent) {
        if (newParent) {
            if (mOwner)
                mOwner->Lock();
            if (newParent->mOwner)
                newParent->mOwner->Lock();
            parentLock.Lock(-1);
            newParent->addChild(this);
            parentLock.Unlock();
            if (newParent->mOwner)
                newParent->mOwner->Unlock();
            if (mOwner)
                mOwner->Unlock();
            if (xform) {
                xform->rotation().normalize();
                setTransform(*xform);
            }
        }
    } else {
        if (xform) {
            local = *xform;
        } else if (newParent) {
            relative = newParent->mGlobalTransform;
            relative = ~relative;
            local.Multiply(relative, mGlobalTransform);
        } else {
            local = mTransform;
        }
        if (mOwner)
            mOwner->Lock();
        if (oldParent->mOwner && oldParent->mOwner != mOwner)
            oldParent->mOwner->Lock();
        parentLock.Lock(-1);
        oldParent->removeChild(this);
        parentLock.Unlock();
        if (oldParent->mOwner && oldParent->mOwner != mOwner)
            oldParent->mOwner->Unlock();
        if (newParent && newParent->mOwner && newParent->mOwner != mOwner)
            newParent->mOwner->Lock();
        parentLock.Lock(-1);
        if (newParent)
            newParent->addChild(this);
        parentLock.Unlock();
        if (newParent && newParent->mOwner && newParent->mOwner != mOwner)
            newParent->mOwner->Unlock();
        if (mOwner)
            mOwner->Unlock();
        local.rotation().normalize();
        setTransform(local);
    }
    if (mRenderNode)
        mRenderNode->SetParent(newParent ? newParent->GetRenderNode() : 0);
}

// 0x1004e760 ?TransformPivot@MfSkeletonNode@@UAEXABVMfTransform3@@@Z
// Moves the node's pivot by pivot: the geometry and the children stay where
// they are.
void MfSkeletonNode::TransformPivot(const MfTransform3 &pivot)
{
    MfTransform3 inverse = ~pivot;
    if (mRenderNode) {
        MfVector3 scale(1.0f, 1.0f, 1.0f);
        mRenderNode->TransformVertices(inverse, scale);
    }
    setTransform(mTransform * pivot);
    for (MfSkeletonNode *child = (MfSkeletonNode *)mFirstChild; child;
         child = (MfSkeletonNode *)child->nextSibling()) {
        child->setTransform(inverse * child->mTransform);
        child->syncChildTransforms(0);
    }
}

// 0x1004ed80 ?SetTransform@MfSkeletonNode@@UAEXABVMfTransform3@@@Z
// The application sets the local transform (announced with
// MF_HOOK_MISC_DATA, kind 6).
void MfSkeletonNode::SetTransform(const MfTransform3 &transform)
{
    MfMiscHookData data;
    data.pActor = mOwner;
    data.kind = 6;
    data.m18 = (int)(MfAiActorNode *)this;
    data.rotation = transform.rotation();
    data.position = transform.translation();
    gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    setTransform(transform);
    syncChildTransforms(0);
    if (mOwner)
        mOwner->SetStateChangeFlag(1);
}

// 0x1004ef80 ?SetMaterial@MfSkeletonNode@@UAEHHPAVMfMaterial@@@Z
// Replaces a material of the render node (textures from the actor's texture
// path); 0 without a render node.
int MfSkeletonNode::SetMaterial(int index, MfMaterial *material)
{
    if (mRenderNode) {
        MfString texturePath;
        if (mOwner && mOwner->Prototype())
            texturePath = mOwner->Prototype()->TexturePath();
        if (texturePath != NULL_STRING)
            MfRenderer::Instance()->PushSearchPath(texturePath);
        mRenderNode->ChangeMaterial(index, material);
        mOwner->SetStateChangeFlag(1);
        if (texturePath != NULL_STRING)
            MfRenderer::Instance()->PopSearchPath();
        return 1;
    }
    return 0;
}

// 0x1004f0a0 ?OverrideMaterials@MfSkeletonNode@@QAEXABVMfMaterial@@@Z
// Renders every material of the geometry with one material.
void MfSkeletonNode::OverrideMaterials(const MfMaterial &material)
{
    if (!mRenderNode || !colModel())
        return;
    MfTriangleSet *triangles = colModel()->mTriangleSet;
    for (int i = 0; i < triangles->GetNumMaterials(); i++)
        mRenderNode->ChangeMaterial(i, (MfMaterial *)&material);
}

// 0x1004f110 ?RestoreMaterials@MfSkeletonNode@@QAEXXZ
// Gives the render node the geometry's own materials back.
void MfSkeletonNode::RestoreMaterials()
{
    if (!mRenderNode || !colModel())
        return;
    MfString texturePath;
    if (mOwner && mOwner->Prototype())
        texturePath = mOwner->Prototype()->TexturePath();
    if (texturePath != NULL_STRING)
        MfRenderer::Instance()->PushSearchPath(texturePath);
    MfTriangleSet *triangles = colModel()->mTriangleSet;
    for (int i = 0; i < triangles->GetNumMaterials(); i++)
        mRenderNode->ChangeMaterial(i, &triangles->Material(i));
    if (texturePath != NULL_STRING)
        MfRenderer::Instance()->PopSearchPath();
}

// 0x1004f250 ?GetPrevTransform@MfSkeletonNode@@UAEHAAVMfTransform3@@@Z
// The global transform before the owner's last update.
int MfSkeletonNode::GetPrevTransform(MfTransform3 &transform)
{
    MfLiveActor *owner = (MfLiveActor *)mOwner;
    if (owner) {
        int index = owner->skeletonTraversal()->nodeIndex(this);
        transform = owner->mLastConfig[index];
        return 1;
    }
    return 0;
}

// 0x1004f330 ?GetSplineStart@MfSkeletonNode@@UAEHAAVMfTransform3@@@Z
// The start of a spline node's curve in global coordinates (the info of a
// spline prototype node is its MfSpline).
int MfSkeletonNode::GetSplineStart(MfTransform3 &transform)
{
    if (IsType(MfSplineNode::TYPE)) {
        ((MfSpline *)mProto->SplineInfo())->InterpolateAtTime(0, transform);
        transform = mGlobalTransform * transform;
        return 1;
    }
    return 0;
}

// 0x1004f3c0 ?GetSplineEnd@MfSkeletonNode@@UAEHAAVMfTransform3@@@Z
int MfSkeletonNode::GetSplineEnd(MfTransform3 &transform)
{
    if (IsType(MfSplineNode::TYPE)) {
        MfSpline *spline = (MfSpline *)mProto->SplineInfo();
        spline->InterpolateAtTime(spline->LengthInTime(), transform);
        transform = mGlobalTransform * transform;
        return 1;
    }
    return 0;
}

// TODO 0x1004f520 ?SetProperty@MfSkeletonNode@@UAEXAAVMfProperty@@@Z: the original calls the imported MfProperty::Name() where MfProperty.h expands the inline accessor, and keeps the two branches' *existing = property calls apart (each followed by found = 1 and a jump), where this tree cross-jumps them into one call; the block structure differs, so not reviewed-equivalent. Calls, conditions and stores along each path are the same (61% similar)
// A property of the prototype node or of the instance takes the value, else
// the instance gets a copy (announced with MF_HOOK_MISC_DATA, kind 5).
void MfSkeletonNode::SetProperty(MfProperty &property)
{
    MfMiscHookData data;
    data.pActor = mOwner;
    data.m18 = (int)(MfAiActorNode *)this;
    data.property = property;
    data.kind = 5;
    gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    int found = 0;
    MfProperty *existing;
    if (mProto && (existing = mProto->Properties().Find(property.Name())) != 0) {
        *existing = property;
        found = 1;
    } else if ((existing = (&mInstProperties)->Find(property.Name())) != 0) {
        *existing = property;
        found = 1;
    }
    if (!found)
        (&mInstProperties)->Add(new MfProperty(property));
}

// TODO 0x1004f6b0 ?SetProperty@MfSkeletonNode@@UAEXPAVMfProperty@@@Z: the original calls the imported MfProperty::Name() where MfProperty.h expands the inline accessor, and keeps the two branches' Replace calls apart (each with its own jump), where this tree cross-jumps them into one call; the block structure differs, so not reviewed-equivalent. Calls, conditions and stores along each path are the same (61% similar)
// As SetProperty(MfProperty &), but the property itself replaces the old one
// or is added.
void MfSkeletonNode::SetProperty(MfProperty *property)
{
    MfMiscHookData data;
    data.pActor = mOwner;
    data.m18 = (int)(MfAiActorNode *)this;
    data.property = *property;
    data.kind = 5;
    gHookManager->CallHooks(MF_HOOK_MISC_DATA, &data);
    int found = 0;
    if (mProto && mProto->Properties().Find(property->Name())) {
        mProto->Properties().Replace(property);
        found = 1;
    } else if ((&mInstProperties)->Find(property->Name())) {
        (&mInstProperties)->Replace(property);
        found = 1;
    }
    if (!found)
        (&mInstProperties)->Add(property);
}

// TODO 0x1004e840 ?ScaleSubtree@MfSkeletonNode@@UAEXVMfVector3@@HH@Z: the expanded MfRotation3::normalize spills the length to the stack in the original (this tree keeps it on the FPU) and the rotated scale's temporary keeps its EH state one store longer; calls, conditions and stores are the same (78% similar)
// Scales the subtree (with sameOwner set only the nodes of the same actor):
// the first node's translation scales in the parent's frame, the geometry in
// the node's own (the magnitudes of the scale turned into it). The render
// vertices of an actor without prototype are scaled directly, an owned
// collision model is copied and scaled.
void MfSkeletonNode::ScaleSubtree(MfVector3 scale, int sameOwner, int isChild)
{
    MfVector3 nodeScale;
    if (!isChild) {
        mTransform.translation() *= scale;
        mTransform.rotation().normalize();
        scale = ~mTransform.rotation() * scale;
        nodeScale = scale;
        if (scale.x() < 0.0f)
            nodeScale.x() = -scale.x();
        if (scale.y() < 0.0f)
            nodeScale.y() = -scale.y();
        if (scale.z() < 0.0f)
            nodeScale.z() = -scale.z();
    } else {
        nodeScale = scale;
    }
    _syncToTransform();
    if (mRenderNode && mOwner && !mOwner->Prototype()) {
        mRenderNode->TransformVertices(MfTransform3::identity(), nodeScale);
        MfVector3 one(1.0f, 1.0f, 1.0f);
        mRenderNode->SetApparentScale(one);
    }
    if (mLocalColModel && mOwnsLocalColModel) {
        MfTriangleSet *triangles = new MfTriangleSet(*mLocalColModel->mTriangleSet);
        triangles->ApplyScale(nodeScale);
        SetLocalColModel(triangles);
        if (mRenderNode && mOwner && mOwner->Prototype()) {
            MfTriangleSet *current = colModel() ? colModel()->mTriangleSet : 0;
            if (current)
                current->ChangeID();
            bool enabled = mRenderNode->mEnabled;
            DeleteRenderNode();
            CreateRenderNode();
            mRenderNode->Enable(enabled);
        }
    }
    for (MfSkeletonNode *child = (MfSkeletonNode *)mFirstChild; child;
         child = (MfSkeletonNode *)child->nextSibling()) {
        if (!sameOwner || child->mOwner == mOwner)
            child->ScaleSubtree(scale, sameOwner, 0);
    }
}

// 0x1004d3e0 ?GetJointLimit@MfSkeletonNode@@UAEPAVMfJLimit@@XZ
MfJLimit *MfSkeletonNode::GetJointLimit()
{
    if (mProto)
        return mProto->mJointLimit;
    return 0;
}

// 0x1004eea0 ?GetRenderNode@MfSkeletonNode@@UAEPAVMfRenderNode@@XZ
MfRenderNode *MfSkeletonNode::GetRenderNode()
{
    return GetRenderNode(0);
}

// 0x1004d3a0 ?GetContainer@MfSkeletonNode@@UAEHAAVMfVector3@@0@Z
int MfSkeletonNode::GetContainer(MfVector3 &min, MfVector3 &max)
{
    MfColModel *model = colModel();
    if (model && model->GetTriangleSet()) {
        model->GetTriangleSet()->FindBoundingBox(min, max);
        return 1;
    }
    return 0;
}

// 0x1004dc40 ?GetRenderNode@MfSkeletonNode@@QAEPAVMfRenderNode@@H@Z
MfRenderNode *MfSkeletonNode::GetRenderNode(int create)
{
    if (create && !mRenderNode)
        CreateRenderNode();
    return mRenderNode;
}

// 0x1004de10 ?SetOwner@MfSkeletonNode@@QAEXPAVMfActor@@H@Z
void MfSkeletonNode::SetOwner(MfActor *owner, int recurse)
{
    mOwner = owner;
    if (recurse) {
        for (MfSkeletonNode *child = (MfSkeletonNode *)firstChild(); child; child = (MfSkeletonNode *)child->nextSibling())
            child->SetOwner(owner, recurse);
    }
}

// 0x1004de40 ?SetSpace@MfSkeletonNode@@UAEHPAVMfSpace@@H@Z
int MfSkeletonNode::SetSpace(MfSpace *space, int recurse)
{
    if (!space)
        return 0;
    if (mSpace == space)
        return 1;
    if (mSpace)
        RemoveSpace(mSpace, recurse);
    mSpace = space;
    if (mRenderNode) {
        int type = Type();
        if (type == MfFloor::TYPE || type == MfHandle::TYPE || type == MfCameraNode::TYPE ||
            type == MfLightNode::TYPE)
            mRenderNode->Enable(false);
        MfSkeletonNode *parent = (MfSkeletonNode *)mParent;
        mRenderNode->SetParent(parent ? parent->GetRenderNode() : 0);
        mSpace->GetRenderScene()->AddNode(mRenderNode);
    }
    if (mProto && mProto->Style() == MfProtoNode::GEOMETRY && mProto->GeomInfo()->mFacesCamera == 1) {
        mSpace->AddNodeWrtCamera(this);
        mColNode.SetCheckStyle(0);
    }
    int result = 1;
    if (recurse) {
        for (MfSkeletonNode *child = (MfSkeletonNode *)firstChild(); child; child = (MfSkeletonNode *)child->nextSibling())
            result = result && child->SetSpace(mSpace, recurse);
    }
    return result;
}

// 0x1004df70 ?RemoveSpace@MfSkeletonNode@@UAEHPAVMfSpace@@H@Z
int MfSkeletonNode::RemoveSpace(MfSpace *space, int recurse)
{
    if (!mSpace)
        return 0;
    if (mSpace != space)
        return 0;
    if (theSoundManager && theSoundManager->GetListener() == this)
        theSoundManager->SetListener(0);
    if (mProto && mProto->Style() == MfProtoNode::GEOMETRY && mProto->GeomInfo()->mFacesCamera == 1) {
        mSpace->RemoveNodeWrtCamera(this);
        mColNode.SetCheckStyle(0xf);
    }
    if (mRenderNode)
        mSpace->GetRenderScene()->RemoveNode(mRenderNode);
    int result = 1;
    if (recurse) {
        for (MfSkeletonNode *child = (MfSkeletonNode *)firstChild(); child; child = (MfSkeletonNode *)child->nextSibling())
            result = result && child->RemoveSpace(mSpace, recurse);
    }
    mSpace = 0;
    return result;
}

// 0x1004ebe0 ?GetInterfaceName@MfSkeletonNode@@UAEXPADHAAH1@Z
void MfSkeletonNode::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiActorNode", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1004ec20 ?GetNodeName@MfSkeletonNode@@UAEABVMfString@@XZ
const MfString &MfSkeletonNode::GetNodeName()
{
    return name();
}

// 0x1004eca0 ?Parent@MfSkeletonNode@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfSkeletonNode::Parent()
{
    return (MfSkeletonNode *)parent();
}

// 0x1004ecb0 ?FirstChild@MfSkeletonNode@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfSkeletonNode::FirstChild()
{
    return (MfSkeletonNode *)firstChild();
}

// 0x1004ecc0 ?NextSibling@MfSkeletonNode@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfSkeletonNode::NextSibling()
{
    return (MfSkeletonNode *)nextSibling();
}

// 0x1004eeb0 ?FlushColModel@MfSkeletonNode@@UAEXXZ
void MfSkeletonNode::FlushColModel()
{
    if (mProto && mProto->ColModel())
        mProto->SetModelFormat(1);
}

// 0x1004eed0 ?ShareColModel@MfSkeletonNode@@UAEXPAVMfAiActorNode@@@Z
void MfSkeletonNode::ShareColModel(MfAiActorNode *node)
{
    MfColModel *&model = node->ColNode().ColModel();
    mColNode.ColModel() = model;
}

// 0x1004ef00 ?FlushGeometry@MfSkeletonNode@@UAEXXZ
void MfSkeletonNode::FlushGeometry()
{
    if (mProto && mProto->ColModel())
        mProto->SetModelFormat(0);
}

// 0x1004f2b0 ?GetTriangleSet@MfSkeletonNode@@UAEPAVMfTriangleSet@@XZ
MfTriangleSet *MfSkeletonNode::GetTriangleSet()
{
    if (colModel())
        return colModel()->GetTriangleSet();
    return 0;
}

// 0x1004f2f0 ?GetHandleStyle@MfSkeletonNode@@UAE?AW4MfHandleNodeStyle@@XZ
MfHandleNodeStyle MfSkeletonNode::GetHandleStyle()
{
    if (IsType('Hndl'))
        return mProto->HandleInfo()->mStyle;
    return MF_HANDLE_NODE_ERROR;
}

// 0x1004f500 ?GetPropertyNames@MfSkeletonNode@@UAEXPAVMfStringList@@@Z
void MfSkeletonNode::GetPropertyNames(MfStringList *names)
{
    if (mProto)
        mProto->Properties().GetPropertyNames(*names, 0);
}

// 0x1004dde0 ?GetRenderStyle@MfSkeletonNode@@UAEHXZ
int MfSkeletonNode::GetRenderStyle()
{
    if (mRenderNode) {
        if (!mRenderNode->mEnabled)
            return MF_RENDER_INVISIBLE;
        if (mRenderNode->mWireframed)
            return MF_RENDER_WIREFRAME;
    }
    return MF_RENDER_SOLID;
}

// 0x1004ec40 ?GetNodeType@MfSkeletonNode@@UAE?AW4MfActorNodeType@@XZ
MfActorNodeType MfSkeletonNode::GetNodeType()
{
    if (IsType('Flor'))
        return MF_NODE_FLOOR;
    if (IsType('Hndl'))
        return MF_NODE_HANDLE;
    if (IsType('Cmra'))
        return MF_NODE_CAMERA;
    if (IsType('Lght'))
        return MF_NODE_LIGHT;
    return MF_NODE_GEOMETRY;
}

// 0x1004ecd0 ?GetTransform@MfSkeletonNode@@UAEHAAVMfTransform3@@00@Z
int MfSkeletonNode::GetTransform(MfTransform3 &local, MfTransform3 &home, MfTransform3 &global)
{
    local = mTransform;
    home = homeTransform();
    global = mGlobalTransform;
    return mTransformDirty;
}

// 0x1004ef20 ?GetMaterial@MfSkeletonNode@@UAEPBVMfMaterial@@H@Z
const MfMaterial *MfSkeletonNode::GetMaterial(int index)
{
    if (colModel()) {
        MfTriangleSet *triangles = colModel()->GetTriangleSet();
        if (triangles && index >= 0 && index < triangles->GetNumMaterials())
            return &triangles->Material(index);
    }
    return 0;
}

// 0x1004f8f0 ?GetProperty@MfSkeletonNode@@UAEHABVMfString@@PAPAVMfProperty@@@Z
int MfSkeletonNode::GetProperty(const MfString &name, MfProperty **property)
{
    *property = 0;
    if (mProto && (*property = mProto->Properties().Find(name)) != 0)
        return 1;
    *property = mInstProperties.Find(name);
    return *property != 0;
}

// 0x1004d610 ?setTransform@MfSkeletonNode@@QAEXABVMfTransform3@@@Z
void MfSkeletonNode::setTransform(const MfTransform3 &transform)
{
    if (mOwner)
        mOwner->Lock();
    mTransformDirty = 1;
    mTransform = transform;
    _syncToTransform();
    if (mOwner)
        mOwner->Unlock();
}

// 0x1004d680 ?setTranslation@MfSkeletonNode@@QAEXABVMfVector3@@@Z
void MfSkeletonNode::setTranslation(const MfVector3 &translation)
{
    if (mOwner)
        mOwner->Lock();
    mTransformDirty = 1;
    mTransform.translation() = translation;
    _syncToTransform();
    if (mOwner)
        mOwner->Unlock();
}

// 0x1004d6d0 ?setRotation@MfSkeletonNode@@QAEXABVMfRotation3@@@Z
void MfSkeletonNode::setRotation(const MfRotation3 &rotation)
{
    if (mOwner)
        mOwner->Lock();
    mTransformDirty = 1;
    mTransform.rotation() = rotation;
    _syncToTransform();
    if (mOwner)
        mOwner->Unlock();
}

// EQUIV 0x1004d730 ?_syncToTransform@MfSkeletonNode@@MAEXXZ: reviewed: register allocation: this and this+0x44 swap esi/edi; every load/store hits the same address (this+0x50 = (this+0x44)+0xc)
void MfSkeletonNode::_syncToTransform()
{
    if (parent())
        mGlobalTransform.Multiply(((MfSkeletonNode *)parent())->mGlobalTransform, mTransform);
    else
        mGlobalTransform = mTransform;
    mColNode.Transform() = mGlobalTransform;
    if (mRenderNode)
        mRenderNode->SetTransform(*mRenderTransform);
}

// 0x1004d7d0 ?syncChildTransforms@MfSkeletonNode@@QAEXH@Z
void MfSkeletonNode::syncChildTransforms(int dirty)
{
    for (MfSkeletonNode *child = (MfSkeletonNode *)mFirstChild; child; child = (MfSkeletonNode *)child->mNextSibling) {
        if (mOwner)
            mOwner->Lock();
        child->_syncToTransform();
        if (mOwner)
            mOwner->Unlock();
        if (dirty)
            mTransformDirty = 1;
        child->syncChildTransforms(dirty | mTransformDirty);
    }
}

// 0x1004d840 ?gotoHomeTransform@MfSkeletonNode@@QAEXXZ
void MfSkeletonNode::gotoHomeTransform()
{
    setTransform(homeTransform());
    for (MfSkeletonNode *child = (MfSkeletonNode *)mFirstChild; child; child = (MfSkeletonNode *)child->mNextSibling) {
        if (child->mOwner == mOwner)
            child->gotoHomeTransform();
    }
}

// 0x1004ee90 ?GetActor@MfSkeletonNode@@UAEPAVMfAiActor@@XZ
MfAiActor *MfSkeletonNode::GetActor()
{
    return mOwner;
}

// 0x1004dbc0 ?SetLocalColModel@MfSkeletonNode@@QAEPAVMfColModel@@PAV2@_N@Z
MfColModel *MfSkeletonNode::SetLocalColModel(MfColModel *model, bool own)
{
    if (mProto)
        return 0;
    if (mLocalColModel && mLocalColModel != model && mOwnsLocalColModel)
        delete mLocalColModel;
    mLocalColModel = model;
    mColNode.ColModel() = model;
    mColNode.Owner() = this;
    mOwnsLocalColModel = own;
    return mLocalColModel;
}

// 0x1004eb80 ?SetApparentScale@MfSkeletonNode@@UAEXABVMfVector3@@@Z
void MfSkeletonNode::SetApparentScale(const MfVector3 &scale)
{
    if (mRenderNode) {
        MfVector3 apparent(scale);
        mRenderNode->SetApparentScale(apparent);
    }
}

// 0x1004d2e0 ?GetPiccoloHandle@MfSkeletonNode@@UAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfSkeletonNode::GetPiccoloHandle()
{
    if (!mPiccoloHandle)
        mPiccoloHandle = MfScDataHandle::CreateData(new MfPoActorNode(this));
    mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x1004dab0 ?SetLocalColModel@MfSkeletonNode@@QAEPAVMfColModel@@PAVMfTriangleSet@@@Z
MfColModel *MfSkeletonNode::SetLocalColModel(MfTriangleSet *triangles)
{
    if (mProto)
        return 0;
    if (!mLocalColModel)
        mLocalColModel = new MfColModel;
    if (triangles != mLocalColModel->GetTriangleSet())
        mLocalColModel->SetTriangleSet(triangles);
    mLocalColModel->UpdateSphereTree();
    mColNode.ColModel() = mLocalColModel;
    mColNode.Owner() = this;
    mOwnsLocalColModel = true;
    return mLocalColModel;
}

// 0x1004f450 ?GetProperty@MfSkeletonNode@@UAEHVMfString@@AAVMfProperty@@@Z
int MfSkeletonNode::GetProperty(MfString name, MfProperty &property)
{
    MfProperty *found;
    if ((mProto && (found = mProto->Properties().Find(name)) != 0) || (found = mInstProperties.Find(name)) != 0) {
        property = *found;
        return 1;
    }
    return 0;
}

// 0x1004f810 ?Property@MfSkeletonNode@@QAEAAVMfProperty@@VMfString@@@Z
MfProperty &MfSkeletonNode::Property(MfString name)
{
    MfProperty *property;
    if (mProto && (property = mProto->Properties().Find(name)) != 0)
        return *property;
    property = mInstProperties.Find(name);
    if (!property) {
        property = new MfProperty(name, 0);
        mInstProperties.Add(property);
    }
    return *property;
}


// Matched by this code:
// 0x1004cf60 ??_EMfSkeletonNode@@UAEPAXI@Z
// 0x1004d380 ??_GMfPoActorNode@@UAEPAXI@Z
// 0x1004dba0 ??_GMfColModel@@UAEPAXI@Z