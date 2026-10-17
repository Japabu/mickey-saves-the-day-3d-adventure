#include "MfManipSkill.h"
#include <math.h>
#include "MfStream.h"
#include "MfHandle.h"
#include "MfSkill.h"
#include "MfProtoNode.h"
#include "MfPermutation.h"
#include "MfProtoAnchorData.h"
#include "MfProtoHandData.h"
#include "MfProtoManipData.h"
#include "MfSkeletonNode.h"
#include "MfActor.h"
#include "MfSkeletonTraversal.h"
#include "MfColNode.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfManipPlanner.h"
#include "MfMaterial.h"
#include "MfTriangleSet.h"

long const MfManipSkill::STREAM_TYPE = 'lkSM';

// Inline members this object emits (the class is exported):
// 0x100037c0 ?Type@MfManipSkill@@UAEHXZ
// 0x100037d0 ?IsType@MfManipSkill@@UAEHH@Z
// 0x10003800 ?Duplicate@MfManipSkill@@UAEPAVMfProtoSkill@@XZ
// 0x10003860 ?HandList@MfManipSkill@@QAEAAVMfDblList@@XZ
// 0x10003870 ?AnchorList@MfManipSkill@@QAEAAVMfDblList@@XZ
// 0x10003880 ?ManipType@MfManipSkill@@QAEAAHXZ
// 0x10003890 ?ContactForce@MfManipSkill@@QAEAAVMfVector3@@XZ
// 0x100038a0 ?MeanDist@MfManipSkill@@QAEMXZ
// 0x100038b0 ?GraspOrientation@MfManipSkill@@QAEMXZ
// 0x100038c0 ?HandleWrtTrace@MfManipSkill@@QAEAAVMfTransform3@@XZ

// TODO 0x100464f0 ?ScaleJoint@MfManipSkill@@UAEXHABVMfVector3@@@Z: the original copies the home transform
// in inline and back with the imported member assignments (here the other way round) and keeps the
// loop count on the stack; 44% similar
// Scaling the root scales the handles with it.
void MfManipSkill::ScaleJoint(int joint, const MfVector3 &scale)
{
    MfSkill::ScaleJoint(joint, scale);
    MfTransform3 home;
    if (joint == 0) {
        for (int i = 0; i < 3; i++) {
            (&mHandleProto)[i]->Scale(scale);
            home = (&mHandleProto)[i]->mHomeTransform;
            home.translation() *= scale;
            (&mHandleProto)[i]->mHomeTransform = home;
            (&mHandleProto)[i]->mHomeTransform.rotation().normalize();
        }
        if (mHandle)
            mHandle->ScaleSubtree(scale, 0, 1);
    }
}

// 0x10046710 ?Clear@MfManipSkill@@MAEXXZ
void MfManipSkill::Clear()
{
    MfSkill::Clear();
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos))
        delete hand;
    (&mHandList)->Clear();
    for (MfManipAnchor *anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(pos))
        delete anchor;
    (&mAnchorList)->Clear();
    for (int i = 0; i < 3; i++) {
        delete (&mHandleProto)[i];
        (&mHandleProto)[i] = 0;
    }
    if (mHandle) {
        mHandle->changeParent(0, 0);
        delete mHandle;
        mHandle = 0;
    }
}

// 0x100467f0 ?Copy@MfManipSkill@@UAEXABVMfSkill@@@Z
void MfManipSkill::Copy(const MfSkill &skill)
{
    MfSkill::Copy(skill);
    int i;
    if (((MfSkill &)skill).Type() != TYPE) {
        for (i = 0; i < 3; i++) {
            (&mHandleProto)[i] = new MfProtoNode;
            (&mHandleProto)[i]->SetStyle(MfProtoNode::HANDLE);
        }
        if (!mHandleProto->mColModel)
            mHandleProto->CreateColModel();
        mHandle = (MfHandle *)MfSkeletonNode::CreateFromPrototype(mHandleProto);
        mHandle->setTransform(mHandleProto->mHomeTransform);
        return;
    }
    const MfManipSkill &other = (const MfManipSkill &)skill;
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&other.mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&other.mHandList)->GetNext(pos))
        (&mHandList)->Add(new MfManipHand(*hand));
    for (MfManipAnchor *anchor = (MfManipAnchor *)(&other.mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&other.mAnchorList)->GetNext(pos))
        (&mAnchorList)->Add(new MfManipAnchor(*anchor));
    for (i = 0; i < 3; i++)
        (&mHandleProto)[i] = new MfProtoNode(*(&other.mHandleProto)[i]);
    mHandle = (MfHandle *)MfSkeletonNode::CreateFromPrototype(mHandleProto);
    mHandle->setTransform(mHandleProto->mHomeTransform);
    mManipType = other.mManipType;
    mContactForce = other.mContactForce;
}

// TODO 0x10046b30 ?ComputeInfo@MfManipSkill@@UAEXPAVMfActor@@@Z: the calls agree but for one MfVector3
// constructor this build inlines; the original's frame is 0x14 bytes larger and it keeps other
// registers through the hand loop (68% similar)
// With the actor at the start of the skill: where the handle is relative to
// the actor, the hands relative to the handle (and for discs the side
// each hand approaches from), the finger data and the handle sizes the skill
// can take.
void MfManipSkill::ComputeInfo(MfActor *actor)
{
    MfSkill::ComputeInfo(actor);
    MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
    mMaxHandleProto->HandleInfo()->mLength = 0.1f;
    mMaxHandleProto->HandleInfo()->mRadius = 0.1f;
    mMinHandleProto->HandleInfo()->mLength = 0.1f;
    mMinHandleProto->HandleInfo()->mRadius = 0.1f;
    int ok = 1;
    if (!sample(firstTime(), actor->mSkeletonTraversal, 0))
        ok = 0;
    if (!ok) {
        actor->mSkeletonTraversal->SetTransforms(saved);
        delete[] saved;
        return;
    }
    MfSkeletonNode *parent = (MfSkeletonNode *)actor->mSkeleton->parent();
    GetHandle();
    mHandle->changeParent(parent, 0);
    mHandle->setTransform(mHandleProto->mHomeTransform);
    MfTransform3 handle(mHandle->globalTransform());
    MfTransform3 root(actor->mSkeleton->globalTransform());
    mHandleWrtTrace = ~root * handle;
    if (mManipType == 4) {
        mValidSkill = 1;
        actor->mSkeletonTraversal->SetTransforms(saved);
        delete[] saved;
        mHandle->changeParent(0, 0);
        return;
    }
    {
        _ComputeMeanDistance(actor, handle);
        MfVector3 offset = mHandleWrtTrace.translation();
        mGraspOrientation = (float)atan2(offset.x(), offset.z());
        MfTransform3 node;
        MfTransform3 toHandle;
        toHandle = ~handle;
        MfVector3 side;
        MfDblListItem *pos;
        for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
             hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
            node = actor->mSkeletonTraversal->node(hand->mNode)->globalTransform();
            hand->m10 = toHandle * node;
            MfProtoHandleInfo *info = mHandleProto->HandleInfo();
            if (info->mStyle == MF_HANDLE_NODE_DISC || info->mStyle == MF_HANDLE_NODE_ORIENTED_DISC) {
                side = toHandle * actor->mSkeleton->globalTransform().translation();
                side.y() = 0.0f;
                MfVector3 axis;
                axis = toHandle.rotation() * actor->mSkeleton->globalTransform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
                axis.y() = 0.0f;
                if (axis.dot(side) < 0.0f)
                    side = -side;
                float length = (float)sqrt(side.lengthSquared());
                side.mX /= length;
                side.mY /= length;
                side.mZ /= length;
                hand->m48 = MfVector3(side.x(), 0.0f, side.z());
            }
        }
        mValidSkill = 1;
        if (mHandleProto->HandleInfo()->mStyle != 4 && mManipType != 2 && mManipType != 4) {
            ComputeFingerData(actor);
            ComputeMinMaxHandle(actor);
        } else {
            mMaxHandleProto->HandleInfo()->mLength = 0.1f;
            mMaxHandleProto->HandleInfo()->mRadius = 0.1f;
            mMinHandleProto->HandleInfo()->mLength = 0.1f;
            mMinHandleProto->HandleInfo()->mRadius = 0.1f;
        }
    }
    actor->mSkeletonTraversal->SetTransforms(saved);
    delete[] saved;
    mHandle->changeParent(0, 0);
}

// EQUIV 0x100471c0 ?_ComputeMeanDistance@MfManipSkill@@IAEXPAVMfActor@@ABVMfTransform3@@@Z: reviewed: the
// count's `inc edi` comes after the GetNext call in the original, before it here; nothing else differs
// The horizontal distance from the mean of the anchors to the handle.
void MfManipSkill::_ComputeMeanDistance(MfActor *actor, const MfTransform3 &handle)
{
    MfTransform3 toParent(((MfSkeletonNode *)actor->mSkeleton->parent())->globalTransform());
    toParent = ~toParent;
    MfVector3 center;
    int count = 0;
    MfDblListItem *pos;
    for (MfManipAnchor *anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(pos)) {
        center += actor->mSkeletonTraversal->node(anchor->mNode)->globalTransform().translation();
        count++;
    }
    center = center / (float)count;
    MfVector3 target = handle.translation();
    MfVector3 offset = toParent.rotation() * (target - center);
    offset.y() = 0.0f;
    mMeanDist = offset.length();
}

// EQUIV 0x10047370 ?_InitialKeyFrameOK@MfManipSkill@@IAEHPAVMfActor@@@Z: reviewed: the unused transform's
// vector is built with the imported MfVector3 constructor in the original (mflinalg's zeroes x, y, z),
// zeroed inline here, and two stack slots trade places; nothing else differs
// Whether the actor at the start of the skill keeps clear of the handle with
// every node but those of the hands.
int MfManipSkill::_InitialKeyFrameOK(MfActor *actor)
{
    if (!mSampledPath)
        Presample(actor);
    sample(firstTime(), actor->mSkeletonTraversal, 0);
    MfTransform3 unused;
    MfSkeletonNode *oldParent = (MfSkeletonNode *)mHandle->parent();
    if (oldParent != actor->mSkeleton->parent())
        mHandle->changeParent((MfSkeletonNode *)actor->mSkeleton->parent(), 0);
    mHandle->setTransform(mHandleProto->mHomeTransform);
    MfColNode *body = actor->MakeSubTreeColNode(0);
    int ok = 1;
    int i;
    for (i = 0; i < actor->mAllTraversal->numNodes(); i++)
        (&((MfSkeletonNode *)actor->mAllTraversal->node(i))->mColNode)->SetCheckStyle(0);
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
        MfSkeletonNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
        for (i = 0; i < actor->mAllTraversal->numNodes(); i++)
            if (handNode->hasDescendant(actor->mAllTraversal->node(i)))
                (&((MfSkeletonNode *)actor->mAllTraversal->node(i))->mColNode)->SetCheckStyle(15);
    }
    MfColResultNode result;
    if ((&mHandle->mColNode)->CheckCollision(body, result))
        ok = 0;
    for (i = 0; i < actor->mAllTraversal->numNodes(); i++)
        (&((MfSkeletonNode *)actor->mAllTraversal->node(i))->mColNode)->SetCheckStyle(15);
    mHandle->changeParent(oldParent, 0);
    delete body;
    return ok;
}

// EQUIV 0x10047550 ?ComputeFingerData@MfManipSkill@@IAEHPAVMfActor@@@Z: reviewed: the first unused
// rotation is built with the imported MfRotation3 constructor in the original (mflinalg's sets w = 1,
// x = y = z = 0), inline here, so the EH state count before the loop is one lower; nothing else
// differs. A do/while hand loop reaches 96% but tests the first hand differently.
// The fingers of each hand: for a manipulation of type 3 the children of
// the hand that lead to no anchor, else the chains that end in the leaves
// below the hand.
int MfManipSkill::ComputeFingerData(MfActor *actor)
{
    MfTransform3 unused1;
    MfTransform3 unused2;
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
        MfSkeletonNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
        MfDblListItem *fpos;
        for (MfManipFinger *finger = (MfManipFinger *)(&hand->mFingers)->GetFirst(fpos); finger;
             finger = (MfManipFinger *)(&hand->mFingers)->GetNext(fpos))
            delete finger;
        (&hand->mFingers)->Clear();
        if (mManipType == 3) {
            for (MfTreeNode *child = handNode->firstChild(); child; child = child->nextSibling()) {
                MfManipAnchor *anchor;
                MfDblListItem *apos;
                for (anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(apos); anchor;
                     anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(apos))
                    if (child->hasDescendant(actor->mSkeletonTraversal->node(anchor->mNode)))
                        break;
                if (!anchor) {
                    int index = actor->mSkeletonTraversal->nodeIndex(child);
                    if (index >= 0) {
                        MfManipFinger *finger = new MfManipFinger;
                        finger->mNode = index;
                        (&hand->mFingers)->Add(finger);
                    }
                }
            }
        } else {
            MfSkeletonTraversal *below = new MfSkeletonTraversal(handNode, MfSkeletonNode::TYPE, 1);
            if (below->numNodes() > 1) {
                for (int i = 0; i < below->numNodes(); i++) {
                    if (below->node(i)->numChildren(MfSkeletonNode::TYPE) == 0) {
                        MfTreeNode *node = below->node(i);
                        while (node->parent()->numChildren(MfSkeletonNode::TYPE) == 1 && node != handNode)
                            node = node->parent();
                        int index = actor->mSkeletonTraversal->nodeIndex(node);
                        if (index >= 0) {
                            MfManipFinger *finger = new MfManipFinger;
                            finger->mNode = index;
                            (&hand->mFingers)->Add(finger);
                        }
                    }
                }
            }
            delete below;
        }
    }
    return 1;
}

// TODO 0x10047820 ?UseSubclassData@MfManipSkill@@UAEXABVMfProtoSkill@@@Z: the original keeps the handle
// loop count in ebx and the handle array in edi (here on the stack) and normalizes the home rotation
// through stack temporaries (a frame 4 bytes larger); 61% similar
// The hands, anchors and the handle of the prototype.
void MfManipSkill::UseSubclassData(const MfProtoSkill &proto)
{
    if (!proto.subclassData)
        return;
    for (int i = 0; i < 3; i++) {
        (&mHandleProto)[i] = new MfProtoNode;
        (&mHandleProto)[i]->SetStyle(MfProtoNode::HANDLE);
        (&mHandleProto)[i]->HandleInfo()->mStyle = (MfHandleNodeStyle)0;
    }
    mHandleProto->HandleInfo()->mLength = 0.3f;
    mHandleProto->HandleInfo()->mRadius = 0.1f;
    mMaxHandleProto->HandleInfo()->mLength = 0.0f;
    mMaxHandleProto->HandleInfo()->mRadius = 0.0f;
    mMinHandleProto->HandleInfo()->mLength = 0.0f;
    mMinHandleProto->HandleInfo()->mRadius = 0.0f;
    MfDblList *objects = &proto.subclassData->objects;
    MfDblListItem *pos;
    for (MfStreamObject *object = (MfStreamObject *)objects->GetFirst(pos); object;
         object = (MfStreamObject *)objects->GetNext(pos)) {
        if (object->StreamType() == MfProtoHandData::STREAM_TYPE) {
            MfManipHand *hand = new MfManipHand;
            hand->mNode = ((MfProtoHandData *)object)->handIndex;
            (&mHandList)->Append(hand);
        } else if (object->StreamType() == MfProtoAnchorData::STREAM_TYPE) {
            MfManipAnchor *anchor = new MfManipAnchor;
            anchor->mNode = ((MfProtoAnchorData *)object)->anchorIndex;
            (&mAnchorList)->Append(anchor);
        } else if (object->StreamType() == MfProtoManipData::STREAM_TYPE) {
            MfProtoManipData *data = (MfProtoManipData *)object;
            mContactForce = data->contactForce;
            mManipType = data->manipType;
            mHandleProto->HandleInfo()->mStyle = (MfHandleNodeStyle)data->handleStyle;
            mHandleProto->HandleInfo()->mRadius = data->radius;
            mHandleProto->HandleInfo()->mLength = data->length;
            mHandleProto->mHomeTransform = data->homeXform;
            mHandleProto->mHomeTransform.rotation().normalize();
        }
    }
    MfSkill::UseSubclassData(proto);
}

// 0x10047bd0 ?CreateSubclassData@MfManipSkill@@UAEXXZ
void MfManipSkill::CreateSubclassData()
{
    MfSkill::CreateSubclassData();
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
        MfProtoHandData *data = new MfProtoHandData;
        data->handIndex = hand->mNode;
        AddHand(data);
    }
    for (MfManipAnchor *anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(pos)) {
        MfProtoAnchorData *data = new MfProtoAnchorData;
        data->anchorIndex = anchor->mNode;
        AddAnchor(data);
    }
    MfProtoManipData *manip = new MfProtoManipData;
    manip->contactForce = mContactForce;
    manip->manipType = mManipType;
    manip->handleStyle = mHandleProto->HandleInfo()->mStyle;
    manip->radius = mHandleProto->HandleInfo()->mRadius;
    manip->length = mHandleProto->HandleInfo()->mLength;
    manip->homeXform = mHandleProto->mHomeTransform;
    AddManip(manip);
    type = MfProtoSkill::MANIP;
}

// 0x10047dc0 ?ComputeMinMaxHandle@MfManipSkill@@IAEHPAVMfActor@@@Z
// The largest handle is the one the skill was made with, the smallest a
// point of its style.
int MfManipSkill::ComputeMinMaxHandle(MfActor *)
{
    *mMaxHandleProto->HandleInfo() = *mHandleProto->HandleInfo();
    mMinHandleProto->HandleInfo()->mRadius = 0.0f;
    mMinHandleProto->HandleInfo()->mLength = 0.0f;
    mMinHandleProto->HandleInfo()->mStyle = mHandleProto->HandleInfo()->mStyle;
    return 1;
}

// 0x10047e80 ?PermuteJoints@MfManipSkill@@UAEXPAVMfProtoActor@@PAVMfPermutation@@@Z
// The hands and anchors follow the new node order; those of removed nodes
// are deleted.
void MfManipSkill::PermuteJoints(MfProtoActor *proto, MfPermutation *permutation)
{
    MfSkill::PermuteJoints(proto, permutation);
    MfDblList removed;
    MfDblList *list = &removed;
    MfDblListItem *pos;
    MfManipHand *hand;
    for (hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand; hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
        hand->mNode = permutation->mMap[hand->mNode];
        if (hand->mNode == -1)
            list->Append(hand);
    }
    for (hand = (MfManipHand *)removed.GetFirst(pos); hand; hand = (MfManipHand *)removed.GetNext(pos)) {
        MfDblListItem *item = (&mHandList)->Find(hand);
        (&mHandList)->Delete(item);
        delete hand;
    }
    removed.Clear();
    MfManipAnchor *anchor;
    for (anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(pos)) {
        anchor->mNode = permutation->mMap[anchor->mNode];
        if (anchor->mNode == -1)
            list->Append(anchor);
    }
    for (anchor = (MfManipAnchor *)removed.GetFirst(pos); anchor; anchor = (MfManipAnchor *)removed.GetNext(pos)) {
        MfDblListItem *item = (&mAnchorList)->Find(anchor);
        (&mAnchorList)->Delete(item);
        delete anchor;
    }
}

// 0x10048040 ?CreateManipSkill@@YAPAVMfStreamObject@@XZ
static MfStreamObject *CreateManipSkill()
{
    return new MfManipSkill;
}

// 0x100480b0 _$E27
// 0x100480a0 _$E28
static MfStreamConstructorEntry constructorEntry(MfManipSkill::STREAM_TYPE, CreateManipSkill);

// 0x100480d0 ?StreamType@MfManipSkill@@UBEJXZ
long MfManipSkill::StreamType() const
{
    return STREAM_TYPE;
}

// TODO 0x100480e0 ?ReadFromStream@MfManipSkill@@UAE?AW4MfStreamError@@AAVMfStream@@@Z: the hand's vector
// is zeroed inline (imported constructor here), the old-format home transform is built from float
// constants and normalized through stack temporaries; 60% similar
// Hands of the same node as an earlier hand are dropped; skills of the old
// format store the handles as home transform, handle info and names and get
// a cylinder as the handle model.
MfStreamError MfManipSkill::ReadFromStream(MfStream &stream)
{
    MfSkill::ReadFromStream(stream);
    stream >> mManipType;
    stream.ReadBytes(sizeof(MfVector3), &mContactForce);
    int numHands;
    stream >> numHands;
    MfDblListItem *pos;
    for (int i = 0; i < numHands; i++) {
        MfManipHand *hand = new MfManipHand;
        hand->ReadFromStream(stream);
        for (MfManipHand *other = (MfManipHand *)(&mHandList)->GetFirst(pos); other;
             other = (MfManipHand *)(&mHandList)->GetNext(pos)) {
            if (other->mNode == hand->mNode) {
                delete hand;
                hand = 0;
                break;
            }
        }
        if (hand)
            (&mHandList)->Add(hand);
    }
    int numAnchors;
    stream >> numAnchors;
    for (int j = 0; j < numAnchors; j++) {
        MfManipAnchor *anchor = new MfManipAnchor;
        stream.ReadBytes(sizeof(MfManipAnchor), anchor);
        (&mAnchorList)->Add(anchor);
    }
    int k;
    if (dataFormat == MF_SKILL_FILE_KEYFRAMES || dataFormat == MF_SKILL_FILE_SAMPLES) {
        for (k = 0; k < 3; k++)
            (&mHandleProto)[k]->ReadFromStream(stream);
    } else {
        for (k = 0; k < 3; k++) {
            MfTransform3 home;
            stream.ReadBytes(sizeof(MfTransform3), &home);
            MfProtoNode *proto = (&mHandleProto)[k];
            proto->mHomeTransform = home;
            proto->mHomeTransform.rotation().normalize();
            stream.ReadBytes(sizeof(MfProtoHandleInfo), (&mHandleProto)[k]->HandleInfo());
            MfString name;
            stream >> name;
            if (stream.userFormat > 11)
                stream >> name;
            MfProtoHandleInfo *info = (&mHandleProto)[k]->HandleInfo();
            MfMaterial material;
            material.m_ambient.Set(0.9f, 0.7f, 0.7f, 0.6f);
            material.m_diffuse.Set(0.9f, 0.7f, 0.7f, 0.6f);
            (&mHandleProto)[k]->SetColModel(new MfTriCylinder(material, info->mRadius, info->mLength, 12));
        }
    }
    stream >> mMeanDist >> mGraspOrientation;
    if (stream.userFormat > 8 && stream.userFormat < 34)
        stream >> mValidSkill;
    else if (stream.userFormat <= 8)
        mValidSkill = 1;
    return stream.Status();
}

// 0x100485c0 ?WriteToStream@MfManipSkill@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManipSkill::WriteToStream(MfStream &stream) const
{
    MfSkill::WriteToStream(stream);
    stream << mManipType;
    stream.WriteBytes(sizeof(MfVector3), &mContactForce);
    stream << mHandList.GetCount();
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos))
        hand->WriteToStream(stream);
    stream << mAnchorList.GetCount();
    for (MfManipAnchor *anchor = (MfManipAnchor *)(&mAnchorList)->GetFirst(pos); anchor;
         anchor = (MfManipAnchor *)(&mAnchorList)->GetNext(pos))
        stream.WriteBytes(sizeof(MfManipAnchor), anchor);
    for (int i = 0; i < 3; i++)
        (&mHandleProto)[i]->WriteToStream(stream);
    stream << mMeanDist << mGraspOrientation;
    return stream.Status();
}

// TODO 0x100486c0 ?IsValid@MfManipSkill@@UAEHAAVMfString@@PAVMfActor@@@Z: VC5 merges the repeated message
// tails of the failure cases here (96 bytes shorter); the original keeps each case whole (54% similar)
// Checks what a manipulation skill needs; message gets the reason when it
// is not valid.
int MfManipSkill::IsValid(MfString &message, MfActor *actor)
{
    if (mManipType == 4)
        return mValidSkill = 1;
    if (mHandleProto->HandleInfo()->mStyle != MF_HANDLE_NODE_POINT && mManipType != 2) {
        MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
        if (!_InitialKeyFrameOK(actor)) {
            mValidSkill = 0;
            message << "Invalid manip skill:\n\n";
            message << "The initial keyframe has the actor in collision with the handle.\n";
            message << "\nDo you really want to close the timeline?\n";
            actor->mSkeletonTraversal->SetTransforms(saved);
            delete[] saved;
            return 0;
        }
        actor->mSkeletonTraversal->SetTransforms(saved);
        delete[] saved;
    }
    if (mManipType == 3 && !MfManipPlanner::HandOnAnchorChain(this, actor)) {
        mValidSkill = 0;
        message << "Invalid manip skill:\n\n";
        message << "Sit skills must be authored with the special IK option.\n";
        message << "\nDo you really want to close the timeline?\n";
        return 0;
    }
    if (mManipType == 3) {
        if (mHandleProto->HandleInfo()->mStyle != MF_HANDLE_NODE_DISC &&
            mHandleProto->HandleInfo()->mStyle != MF_HANDLE_NODE_ORIENTED_DISC) {
            mValidSkill = 0;
            message << "Invalid manip skill:\n\n";
            message << "Sit skills must be authored with Disc handles.\n";
            message << "\nDo you really want to close the timeline?\n";
            return 0;
        }
    } else if (mHandleProto->HandleInfo()->mStyle == MF_HANDLE_NODE_DISC ||
               mHandleProto->HandleInfo()->mStyle == MF_HANDLE_NODE_ORIENTED_DISC) {
        mValidSkill = 0;
        message << "Invalid manip skill:\n\n";
        message << "Only sit skills recognize Disc handles.\n";
        message << "\nDo you really want to close the timeline?\n";
        return 0;
    }
    if (mHandList.GetCount() == 0) {
        mValidSkill = 0;
        message << "Invalid manip skill:\n\n";
        message << "Manip skills must have specified hands.\n";
        message << "\nDo you really want to close the timeline?\n";
        return 0;
    }
    if (mAnchorList.GetCount() == 0) {
        mValidSkill = 0;
        message << "Invalid manip skill:\n\n";
        message << "Manip skills must have specified anchors.\n";
        message << "\nDo you really want to close the timeline?\n";
        return 0;
    }
    MfDblListItem *pos;
    for (MfManipHand *hand = (MfManipHand *)(&mHandList)->GetFirst(pos); hand;
         hand = (MfManipHand *)(&mHandList)->GetNext(pos)) {
        MfSkeletonNode *node = actor->mSkeletonTraversal->node(hand->mNode);
        if (!node->colModel()) {
            mValidSkill = 0;
            message << "Invalid manip skill:\n\n";
            message << "The hand nodes for a manip skill must have geometry associated\n";
            message << "with it.  This is for detecting collision between the hand node\n";
            message << "and the target.\n";
            message << "\nDo you really want to close the timeline?\n";
            return 0;
        }
        if (mManipType != 3 &&
            (node == actor->mSkeleton || node == actor->mSkeletonTraversal->node(1) ||
             node->parent() == actor->mSkeletonTraversal->node(1))) {
            mValidSkill = 0;
            message << "Invalid manip skill:\n\n";
            message << "The hand nodes for a manip skill (except in the case of a sit\n";
            message << "manip skill) must be at least two levels below the primary node\n";
            message << "(the first node under the root node of the skeletal hierarchy).\n";
            message << "\nDo you really want to close the timeline?\n";
            return 0;
        }
    }
    return mValidSkill = 1;
}

// 0x10048a60 ?factor@MfManipSkill@@UAEXXZ
void MfManipSkill::factor()
{
    MfSkill::factor();
}

// 0x10046110 ??0MfManipSkill@@QAE@XZ
// The three handle prototypes (as made, smallest and largest) start as
// handles of no style; the made one is 0.3 long and 0.1 thick.
MfManipSkill::MfManipSkill()
    : mHandle(0), mManipType(1)
{
    for (int i = 0; i < 3; i++) {
        (&mHandleProto)[i] = new MfProtoNode;
        (&mHandleProto)[i]->SetStyle(MfProtoNode::HANDLE);
        (&mHandleProto)[i]->HandleInfo()->mStyle = (MfHandleNodeStyle)0;
    }
    mHandleProto->HandleInfo()->mLength = 0.3f;
    mHandleProto->HandleInfo()->mRadius = 0.1f;
    mMaxHandleProto->HandleInfo()->mLength = 0.0f;
    mMaxHandleProto->HandleInfo()->mRadius = 0.0f;
    mMinHandleProto->HandleInfo()->mLength = 0.0f;
    mMinHandleProto->HandleInfo()->mRadius = 0.0f;
}

// 0x10046350 ??_GMfProtoNode@@UAEPAXI@Z (local copy for `new MfProtoNode`)

// 0x10046370 ??0MfManipSkill@@QAE@ABV0@@Z
MfManipSkill::MfManipSkill(const MfManipSkill &other)
    : mHandle(0), mManipType(1)
{
    for (int i = 0; i < 3; i++)
        (&mHandleProto)[i] = 0;
    Copy(other);
}

// 0x10046470 ??1MfManipSkill@@UAE@XZ
MfManipSkill::~MfManipSkill()
{
    Clear();
}

// 0x100466c0 ?GetHandle@MfManipSkill@@QAEPAVMfHandle@@XZ
MfHandle *MfManipSkill::GetHandle()
{
    if (!mHandle) {
        if (!mHandleProto->mColModel)
            mHandleProto->CreateColModel();
        mHandle = (MfHandle *)MfSkeletonNode::CreateFromPrototype(mHandleProto);
        mHandle->setTransform(mHandleProto->mHomeTransform);
    }
    return mHandle;
}

// EQUIV 0x10046a60 ?appliesTo@MfManipSkill@@QAEHPAVMfHandle@@@Z: reviewed: the original loads the handle
// radius before the limit's info pointer and reads the limit's node type into eax before comparing it,
// where this compares it in memory after the fld; registers differ, nothing else
// Whether the skill can grasp the handle: a handle of the same kind, not
// larger than the largest the skill was made for.
int MfManipSkill::appliesTo(MfHandle *handle)
{
    if (!handle->IsType(MfHandle::TYPE))
        return 0;
    int style = mHandleProto->HandleInfo()->mStyle;
    int applies = 0;
    if (style == handle->mProto->HandleInfo()->mStyle && style != MF_HANDLE_NODE_POINT) {
        if (!mManipType || style == MF_HANDLE_NODE_DISC || style == MF_HANDLE_NODE_ORIENTED_DISC)
            applies = 1;
        else if (handle->mProto->HandleInfo()->mRadius > mMaxHandleProto->HandleInfo()->mRadius ||
                 handle->mProto->HandleInfo()->mLength < mMaxHandleProto->HandleInfo()->mLength)
            applies = 0;
        else
            applies = 1;
    }
    return applies;
}

// 0x10048a70 ??0MfManipHand@@QAE@ABU0@@Z
MfManipHand::MfManipHand(const MfManipHand &hand)
    : mNode(hand.mNode), m08(hand.m08), m0c(hand.m0c), m10(hand.m10), m48(hand.m48), m54(hand.m54)
{
    MfDblListItem *pos;
    for (MfManipFinger *finger = (MfManipFinger *)(&hand.mFingers)->GetFirst(pos); finger;
         finger = (MfManipFinger *)(&hand.mFingers)->GetNext(pos)) {
        MfManipFinger *copy = new MfManipFinger;
        copy->mNode = finger->mNode;
        copy->m04 = 0;
        copy->m08 = 0;
        copy->m0c = 0;
        (&mFingers)->Add(copy);
    }
}

// 0x10048bc0 ??1MfManipHand@@QAE@XZ
MfManipHand::~MfManipHand()
{
    MfDblListItem *pos;
    for (MfManipFinger *finger = (MfManipFinger *)(&mFingers)->GetFirst(pos); finger;
         finger = (MfManipFinger *)(&mFingers)->GetNext(pos))
        delete finger;
}

// 0x10048c30 ?WriteToStream@MfManipHand@@QBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManipHand::WriteToStream(MfStream &stream) const
{
    stream << mNode << m08 << m0c;
    stream.WriteBytes(sizeof(MfTransform3), &m10);
    stream << mFingers.GetCount();
    MfDblListItem *pos;
    for (MfManipFinger *finger = (MfManipFinger *)(&mFingers)->GetFirst(pos); finger;
         finger = (MfManipFinger *)(&mFingers)->GetNext(pos))
        stream << finger->mNode << finger->m08 << finger->m0c;
    stream.WriteBytes(sizeof(MfVector3), &m48);
    return stream.Status();
}

// 0x10048cd0 ?ReadFromStream@MfManipHand@@QAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManipHand::ReadFromStream(MfStream &stream)
{
    stream >> mNode >> m08 >> m0c;
    stream.ReadBytes(sizeof(MfTransform3), &m10);
    int numFingers;
    stream >> numFingers;
    for (int i = 0; i < numFingers; i++) {
        MfManipFinger *finger = new MfManipFinger;
        stream >> finger->mNode >> finger->m08 >> finger->m0c;
        (&mFingers)->Add(finger);
    }
    stream.ReadBytes(sizeof(MfVector3), &m48);
    return stream.Status();
}

// 0x100462f0 ??_EMfManipSkill@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (100228e0); the port needs its own definition.
void MfManipSkill::normalize()
{
}
