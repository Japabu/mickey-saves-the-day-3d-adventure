#include "MfHandle.h"
#include "MfSkeletonNode.h"
#include "MfLiveActor.h"
#include "MfSkeletonTraversal.h"

// Inline members this object emits (the class is exported):
// 0x10005f20 ??_EMfTransform3@@QAEPAXI@Z (for the delete [] of the transform arrays)
// 0x10005f80 ?createCopy@MfHandle@@UAEPAVMfSkeletonNode@@XZ
// 0x10005fe0 ?Type@MfHandle@@UAEHXZ
// 0x10005ff0 ?IsType@MfHandle@@UAEHH@Z
// 0x10006030 ?GetStyle@MfHandle@@QAEHXZ
// 0x10006050 ?GetRadius@MfHandle@@QAEMXZ
// 0x10006070 ?GetLength@MfHandle@@QAEMXZ
// 0x10006090 ?SetSymmetry@MfHandle@@QAEXH@Z
// 0x100060c0 ?IsSymmetric@MfHandle@@QAEHXZ
// 0x100060e0 ?GetSkillName@MfHandle@@QAE?AVMfString@@XZ
// 0x10006110 ?SetOccupied@MfHandle@@QAEXPAVMfSkeletonNode@@@Z
// 0x10006120 ?IsOccupied@MfHandle@@QAEHXZ
// 0x10006130 ?CurrentOccupant@MfHandle@@QAEPAVMfSkeletonNode@@XZ

// 0x1002ec80 ??0MfHandle@@QAE@XZ
MfHandle::MfHandle()
    : mSkillName(NULL_STRING)
{
    mOccupied = 0;
    mFixXformCount = 0;
    mFixXforms = 0;
    mTargetXforms = 0;
}

// 0x1002ed80 ??1MfHandle@@UAE@XZ
MfHandle::~MfHandle()
{
    if (mFixXforms)
        delete [] mFixXforms;
    if (mTargetXforms)
        delete [] mTargetXforms;
}

// 0x1002ee40 ??0MfHandle@@QAE@ABV0@@Z
MfHandle::MfHandle(const MfHandle &other)
    : MfSkeletonNode(other), mSkillName(other.mSkillName)
{
    Copy(other);
    mTargetXforms = 0;
}

// 0x1002eee0 ?Copy@MfHandle@@QAEXABV1@@Z
void MfHandle::Copy(const MfHandle &other)
{
    mOccupied = other.mOccupied;
    mGraspRotation = other.mGraspRotation;
    mFixXformCount = other.mFixXformCount;
    if (mFixXformCount > 0) {
        mFixXforms = new MfTransform3[mFixXformCount];
        for (int i = 0; i < mFixXformCount; i++)
            mFixXforms[i] = other.mFixXforms[i];
    } else {
        mFixXforms = 0;
    }
}

// 0x1002f010 ?SetFixXformForGrasp@MfHandle@@QAEXHPAVMfTransform3@@@Z
void MfHandle::SetFixXformForGrasp(int count, MfTransform3 *xforms)
{
    mFixXformCount = count;
    if (mFixXforms)
        delete [] mFixXforms;
    if (mFixXformCount) {
        mFixXforms = new MfTransform3[mFixXformCount];
        for (int i = 0; i < mFixXformCount; i++)
            mFixXforms[i] = xforms[i];
    }
}

// 0x1002f130 ?ClearFixXformForGrasp@MfHandle@@QAEXXZ
void MfHandle::ClearFixXformForGrasp()
{
    mFixXformCount = 0;
    if (mFixXforms)
        delete [] mFixXforms;
    mFixXforms = 0;
}

// 0x1002f180 ?SetStyle@MfHandle@@QAEXH@Z
// A new kind of handle gets the default size of its kind.
void MfHandle::SetStyle(int style)
{
    int oldStyle = mProto->HandleInfo()->mStyle;
    if (style == oldStyle)
        return;
    mProto->HandleInfo()->mStyle = (MfHandleNodeStyle)style;
    float radius = mProto->HandleInfo()->mRadius;
    float length = mProto->HandleInfo()->mLength;
    switch (style) {
    case MF_HANDLE_NODE_CYLINDER:
    case MF_HANDLE_NODE_ORIENTED_CYLINDER:
        if (oldStyle != MF_HANDLE_NODE_CYLINDER && oldStyle != MF_HANDLE_NODE_ORIENTED_CYLINDER) {
            length = 0.2f;
            radius = 0.05f;
        }
        break;
    case MF_HANDLE_NODE_DISC:
    case MF_HANDLE_NODE_ORIENTED_DISC:
        if (oldStyle != MF_HANDLE_NODE_DISC && oldStyle != MF_HANDLE_NODE_ORIENTED_DISC) {
            length = 0.01f;
            radius = 0.1f;
        }
        break;
    default:
        length = 0.4f;
        radius = 0.025f;
        break;
    }
    mProto->HandleInfo()->mRadius = radius;
    mProto->HandleInfo()->mLength = length;
    if (mProto->mActor) {
        MfActor::ReplaceColModel(mProto->mActor, mProto, 0);
    } else {
        mProto->CreateColModel();
        ReplaceRenderNode();
    }
}

// 0x1002f290 ?SetRadius@MfHandle@@QAEXM@Z
void MfHandle::SetRadius(float radius)
{
    float scale = radius / GetRadius();
    MfVector3 factor(scale, 1.0f, scale);
    if (mProto->mActor) {
        MfActor::ScaleSubtree(mProto->mActor, mProto, factor);
    } else {
        mProto->Scale(factor);
        ScaleSubtree(factor, 0, 1);
    }
}

// 0x1002f340 ?SetLength@MfHandle@@QAEXM@Z
void MfHandle::SetLength(float length)
{
    float scale = length / GetLength();
    MfVector3 factor(1.0f, scale, 1.0f);
    if (mProto->mActor) {
        MfActor::ScaleSubtree(mProto->mActor, mProto, factor);
    } else {
        mProto->Scale(factor);
        ScaleSubtree(factor, 0, 1);
    }
}

// 0x1002f400 ?CacheAwayTargetXforms@MfHandle@@QAEXPAVMfLiveActor@@@Z
void MfHandle::CacheAwayTargetXforms(MfLiveActor *actor)
{
    if (mTargetXforms)
        delete [] mTargetXforms;
    mTargetXforms = actor->skeletonTraversal()->CopyTransforms();
}

// 0x1002f450 ?SetTargetXforms@MfHandle@@QAEXPAVMfLiveActor@@@Z
void MfHandle::SetTargetXforms(MfLiveActor *actor)
{
    if (mTargetXforms)
        actor->skeletonTraversal()->SetTransforms(mTargetXforms);
}

// 0x1002ed20 ??_EMfHandle@@UAEPAXI@Z (compiler-generated vector deleting destructor)
