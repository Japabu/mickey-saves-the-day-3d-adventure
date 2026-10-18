// MfOrientActivity: turns a node (and some of its parents) towards a point
// or a node (RuntimeLib.dll 10048d90-10049810).
#include <math.h>
#include "MfOrientActivity.h"
#include "MfActor.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfStream.h"
#include "MfKChain.h"

// writes a reference to a node to the stream (1000a6e0)
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
// reads it back, finding the node in the actor (1000a760)
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);

long const MfOrientActivity::STREAM_TYPE = 'OriA';

// the implicit copy members of the exported class
static MfOrientActivity &(MfOrientActivity::*s_assign)(const MfOrientActivity &) = &MfOrientActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10008d30 ?Type@MfOrientActivity@@UAEHXZ
// 0x10008d40 ?IsType@MfOrientActivity@@UAEHH@Z
// 0x10008db0 ?PrintName@MfOrientActivity@@UBE?AVMfString@@XZ
// 0x10008de0 ??0MfOrientActivity@@QAE@ABV0@@Z
// 0x10008f10 ??4MfOrientActivity@@QAEAAV0@ABV0@@Z
// 0x1000aab0 ?Type@MfOrientActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000aac0 ?IsType@MfOrientActivity@@WBE@AEHH@Z

// 0x10048d90 ??0MfOrientActivity@@QAE@XZ
MfOrientActivity::MfOrientActivity()
{
    mPointNode = 0;
    mTargetNode = 0;
    mPointNodeIndex = 0;
    mTargetActor = 0;
}

// 0x10048de0 ??0MfOrientActivity@@QAE@PAVMfActor@@K@Z
MfOrientActivity::MfOrientActivity(MfActor *actor, unsigned long startTime)
    : MfActivity(actor, startTime)
{
    mPointNode = 0;
    mTargetNode = 0;
    mPointNodeIndex = 0;
    mTargetActor = 0;
}

// 0x10048e40 ??1MfOrientActivity@@UAE@XZ
MfOrientActivity::~MfOrientActivity()
{
    if (mTargetActor)
        mTargetActor->Release();
}

// 0x10048eb0 ?setup@MfOrientActivity@@QAEXPAVMfSkeletonNode@@ABVMfVector3@@0H@Z
void MfOrientActivity::setup(MfSkeletonNode *pointNode, const MfVector3 &nodeOffset, MfSkeletonNode *targetNode,
                             int chainLength)
{
    mPointNode = pointNode;
    mTargetNode = targetNode;
    mNodeOffset = nodeOffset;
    mChainLength = chainLength;
    if (chainLength <= 0)
        mChainLength = 1;
    // no more than the nodes up to the hip
    int depth = 1;
    for (MfTreeNode *node = mPointNode; node != mActor->skeleton() && node != mActor->skeletonTraversal()->node(1);
         node = node->parent())
        depth++;
    if (mChainLength > depth)
        mChainLength = depth;
    mTargetActor = targetNode->GetOwner();
    if (mTargetActor)
        mTargetActor->AddRef();
    float length = (float)sqrt(mNodeOffset.mY * mNodeOffset.mY + mNodeOffset.mZ * mNodeOffset.mZ +
                               mNodeOffset.mX * mNodeOffset.mX);
    mNodeOffset.mX /= length;
    mNodeOffset.mY /= length;
    mNodeOffset.mZ /= length;
}

// 0x10048fa0 ?setup@MfOrientActivity@@QAEXPAVMfSkeletonNode@@ABVMfVector3@@1H@Z
void MfOrientActivity::setup(MfSkeletonNode *pointNode, const MfVector3 &nodeOffset, const MfVector3 &targetPoint,
                             int chainLength)
{
    mTargetNode = 0;
    mPointNode = pointNode;
    mNodeOffset = nodeOffset;
    mChainLength = chainLength;
    if (chainLength <= 0)
        mChainLength = 1;
    int depth = 1;
    for (MfTreeNode *node = mPointNode; node != mActor->skeleton() && node != mActor->skeletonTraversal()->node(1);
         node = node->parent())
        depth++;
    if (mChainLength > depth)
        mChainLength = depth;
    mTargetPoint = targetPoint;
    float length = (float)sqrt(mNodeOffset.mY * mNodeOffset.mY + mNodeOffset.mZ * mNodeOffset.mZ +
                               mNodeOffset.mX * mNodeOffset.mX);
    mNodeOffset.mX /= length;
    mNodeOffset.mY /= length;
    mNodeOffset.mZ /= length;
}

// 0x10049090 ?setInitialConfig@MfOrientActivity@@UAEXPAVMfSkeletonTraversal@@K@Z
void MfOrientActivity::setInitialConfig(MfSkeletonTraversal *traversal, unsigned long blendTime)
{
    MfActivity::setInitialConfig(traversal, blendTime);
    mPointNodeIndex = traversal->nodeIndex(mPointNode);
}

// 0x100490c0 ?setInitialConfig@MfOrientActivity@@UAEXPAVMfTransform3@@K@Z
void MfOrientActivity::setInitialConfig(MfTransform3 *config, unsigned long blendTime)
{
    MfActivity::setInitialConfig(config, blendTime);
    mPointNodeIndex = mActor->skeletonTraversal()->nodeIndex(mPointNode);
}

// TODO 0x100490f0 ?Update@MfOrientActivity@@UAE?AW4MfActivityStatus@@K@Z: (80% similar) this in edi (original
// esi) and the inline budget: the original zeroes the target vector inline, ours calls the
// imported MfVector3() (60% similar)
// Turns the chain above the point node so that the node points at the
// target, blended from the initial configuration over the blend time.
MfActivityStatus MfOrientActivity::Update(unsigned long time)
{
    if (mTargetNode && mTargetNode->mOwner->mSpace != mPointNode->mOwner->mSpace) {
        mFinished = 1;
        return (MfActivityStatus)1;
    }
    if (!mPointNode) {
        mFinished = 1;
        return (MfActivityStatus)1;
    }
    MfTransform3 target;
    if (mTargetNode)
        target = mTargetNode->globalTransform();
    else
        target.translation() = mTargetPoint;
    if (mPointNode == mActor->skeleton() && mNodeOffset.mY == 0.0f) {
        target = ~mPointNode->globalTransform() * target;
        target.translation().mY = 0;
        target = mPointNode->globalTransform() * target;
    }
    MfKChain chain;
    chain.create(mPointNode, mChainLength, mNodeOffset);
    chain.moveTo(target.translation(), -1);
    chain.updateSkeleton();
    MfTransform3 xform;
    MfSkeletonNode *node = mPointNode;
    int i;
    for (i = 0; i < mChainLength; i++) {
        xform.rotation() = node->transform().rotation();
        xform.rotation().normalize();
        xform.translation() = node->transform().translation();
        node->setTransform(xform);
        node = (MfSkeletonNode *)node->parent();
    }
    if (mInitialConfig && time - mStartTime < mBlendTime) {
        float t = (float)(time - mStartTime) / mBlendTime;
        MfTransform3 blended;
        MfTransform3 current;
        node = mPointNode;
        for (i = 0; i < mChainLength; i++) {
            current.rotation() = node->transform().rotation();
            current.translation() = node->transform().translation();
            blended.interpolate(mInitialConfig[mActor->skeletonTraversal()->nodeIndex(node)], current, t);
            blended.rotation().normalize();
            node->setTransform(blended);
            node = (MfSkeletonNode *)node->parent();
        }
        if (node)
            node->syncChildTransforms(0);
    }
    mLastTime = time;
    return MfActivity::Update(time);
}

// 0x100495c0 ?CreateOrientActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateOrientActivity()
{
    return new MfOrientActivity;
}

// 0x10049630 _$E38 (original $E: the static initializer of the constructor entry, with 10049640)
// 0x10049640 _$E37
static MfStreamConstructorEntry constructorEntry(MfOrientActivity::STREAM_TYPE, CreateOrientActivity);

// 0x10049660 ?StreamType@MfOrientActivity@@UBEJXZ
long MfOrientActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10049670 ?WriteToStream@MfOrientActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOrientActivity::WriteToStream(MfStream &stream) const
{
    MfActivity::WriteToStream(stream);
    stream.WriteBytes(sizeof(MfVector3), &mNodeOffset);
    if (mTargetNode) {
        stream << 1;
        WriteNodeReference(stream, mTargetNode);
    } else {
        stream << 0;
        stream.WriteBytes(sizeof(MfVector3), &mTargetPoint);
    }
    WriteNodeReference(stream, mPointNode);
    stream << mChainLength;
    return stream.Status();
}

// 0x100496f0 ?ReadFromStream@MfOrientActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOrientActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfActivity::ReadFromStream(stream);
    if (error)
        return error;
    MfActor *actor = mActor;
    stream.ReadBytes(sizeof(MfVector3), &mNodeOffset);
    int hasTargetNode;
    stream >> hasTargetNode;
    if (hasTargetNode)
        ReadNodeReference(stream, actor, mTargetNode);
    else
        stream.ReadBytes(sizeof(MfVector3), &mTargetPoint);
    ReadNodeReference(stream, actor, mPointNode);
    stream >> mChainLength;
    if (mTargetNode && mPointNode) {
        setup(mPointNode, mNodeOffset, mTargetNode, mChainLength);
        mPointNodeIndex = mActor->skeletonTraversal()->nodeIndex(mPointNode);
    } else if (mPointNode) {
        setup(mPointNode, mNodeOffset, mTargetPoint, mChainLength);
        mPointNodeIndex = mActor->skeletonTraversal()->nodeIndex(mPointNode);
    }
    return stream.Status();
}

// 0x100497d0 ?PrepareForExecution@MfOrientActivity@@UAEXXZ
void MfOrientActivity::PrepareForExecution()
{
    // the target actor has to be updated first
    if (mActor && mTargetNode && mActor->mSpace)
        mActor->mSpace->ReorderActorUpdate(mActor, mTargetActor);
}

// 0x10008ff0 ??_EMfOrientActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)
