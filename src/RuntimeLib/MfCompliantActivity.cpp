// MfCompliantActivity: keeps the end of a chain of the actor's nodes at a
// node of another actor (RuntimeLib.dll 1001fc10-10020460).
#include "MfCompliantActivity.h"
#include "MfActor.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfStream.h"

// references to nodes in a stream (defined with the first activities,
// 1000a6e0/1000a760)
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);

long const MfCompliantActivity::STREAM_TYPE = 'CmpA';

// the implicit copy members of the exported class
static MfCompliantActivity &(MfCompliantActivity::*s_assign)(const MfCompliantActivity &) =
    &MfCompliantActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10007d80 ?Type@MfCompliantActivity@@UAEHXZ
// 0x10007d90 ?IsType@MfCompliantActivity@@UAEHH@Z
// 0x10007dc0 ?TargetActor@MfCompliantActivity@@QAEPAVMfActor@@XZ
// 0x10007dd0 ?PrintName@MfCompliantActivity@@UBE?AVMfString@@XZ
// 0x1000ab30 ?Type@MfCompliantActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000ab40 ?IsType@MfCompliantActivity@@WBE@AEHH@Z
// EQUIV 0x10007e00 ??0MfCompliantActivity@@QAE@ABV0@@Z: reviewed: the implicit copy constructor; two independent loads in the expanded MfKChain copy come in the other order and registers; nothing else differs
// 0x100080a0 ??4MfCompliantActivity@@QAEAAV0@ABV0@@Z

// 0x1001fc10 ??0MfCompliantActivity@@QAE@XZ
MfCompliantActivity::MfCompliantActivity()
    : mTargetNode(0), mTargetActor(0)
{
    mNodes = 0;
    mInitialTransforms = 0;
    mLinkTransforms = 0;
}

// 0x1001fcc0 ??0MfCompliantActivity@@QAE@PAVMfActor@@K@Z
MfCompliantActivity::MfCompliantActivity(MfActor *actor, unsigned long startTime)
    : MfActivity(actor, startTime), mTargetNode(0), mTargetActor(0)
{
    mNodes = 0;
    mInitialTransforms = 0;
    mLinkTransforms = 0;
}

// 0x1001fd80 ??1MfCompliantActivity@@UAE@XZ
MfCompliantActivity::~MfCompliantActivity()
{
    if (mTargetActor)
        mTargetActor->Release();
    if (mNodes)
        delete mNodes;
    if (mInitialTransforms)
        delete [] mInitialTransforms;
    if (mLinkTransforms)
        delete [] mLinkTransforms;
}

// 0x1001fe60 ?SetupTarget@MfCompliantActivity@@QAEXPAVMfSkeletonNode@@VMfTransform3@@@Z
void MfCompliantActivity::SetupTarget(MfSkeletonNode *node, MfTransform3 offset)
{
    mTargetActor = node->GetOwner();
    if (mTargetActor)
        mTargetActor->AddRef();
    mTargetNode = node;
    mOffset = offset;
    float radius, height;
    mActor->GetCylinder(radius, height);
}

// EQUIV 0x1001ff20 ?SetupChain@MfCompliantActivity@@QAEXPAVMfSkeletonNode@@H@Z: reviewed: the copy loop computes the element address through other registers (one mov more); the loop is otherwise the same
void MfCompliantActivity::SetupChain(MfSkeletonNode *node, int chainLength)
{
    if (chainLength <= 0)
        chainLength = 1;
    // no more than the nodes up to the hip
    int depth = 1;
    for (MfTreeNode *parent = node; parent != mActor->skeleton() && parent != mActor->skeletonTraversal()->node(1);
         parent = parent->parent())
        depth++;
    if (chainLength > depth)
        chainLength = depth;
    mChain.create(node, chainLength, MfVector3(0.0f, 0.0f, 0.0f));
    mNodes = new MfSkeletonTraversal(node, MfSkeletonNode::TYPE, 1);
    mInitialTransforms = mNodes->CopyTransforms();
    mLinkTransforms = new MfTransform3[chainLength];
    MfSkeletonNode *link = node;
    for (int i = 0; i < mChain.numLinks(); i++) {
        mLinkTransforms[i] = link->transform();
        link = (MfSkeletonNode *)link->parent();
    }
}

// 0x100200b0 ?Update@MfCompliantActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfCompliantActivity::Update(unsigned long time)
{
    if (mTargetNode) {
        MfTransform3 goal = mTargetNode->globalTransform() * mOffset;
        // start from the links' last configuration
        MfSkeletonNode *node = mChain.linkNode(mChain.numLinks() - 1);
        int i;
        for (i = 0; i < mChain.numLinks(); i++) {
            node->setTransform(mLinkTransforms[i]);
            node = (MfSkeletonNode *)node->parent();
        }
        node->syncChildTransforms(0);
        mChain.updateChain();
        mChain.moveTo(goal);
        mChain.updateSkeleton();
        node = mChain.linkNode(mChain.numLinks() - 1);
        for (i = 0; i < mChain.numLinks(); i++) {
            mLinkTransforms[i] = node->transform();
            node = (MfSkeletonNode *)node->parent();
        }
    }
    for (int i = 1; i < mNodes->numNodes(); i++)
        mNodes->node(i)->setTransform(mInitialTransforms[i]);
    return MfActivity::Update(time);
}

// 0x10020220 ?CreateCompliantActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateCompliantActivity()
{
    return new MfCompliantActivity;
}

// 0x10020290 _$E39 (original $E: the static initializer of the constructor entry, with 100202a0)
// 0x100202a0 _$E38
static MfStreamConstructorEntry constructorEntry(MfCompliantActivity::STREAM_TYPE, CreateCompliantActivity);

// 0x100202c0 ?StreamType@MfCompliantActivity@@UBEJXZ
long MfCompliantActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x100202d0 ?WriteToStream@MfCompliantActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfCompliantActivity::WriteToStream(MfStream &stream) const
{
    MfActivity::WriteToStream(stream);
    stream.WriteBytes(sizeof(MfTransform3), &mOffset);
    WriteNodeReference(stream, mTargetNode);
    WriteNodeReference(stream, mNodes->node(0));
    stream << mChain.numLinks();
    return stream.Status();
}

// 0x10020330 ?ReadFromStream@MfCompliantActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfCompliantActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfActivity::ReadFromStream(stream);
    if (error)
        return error;
    MfActor *actor = mActor;
    stream.ReadBytes(sizeof(MfTransform3), &mOffset);
    ReadNodeReference(stream, actor, mTargetNode);
    if (mTargetNode)
        SetupTarget(mTargetNode, mOffset);
    MfSkeletonNode *chainEnd;
    int chainLength;
    ReadNodeReference(stream, actor, chainEnd);
    stream >> chainLength;
    if (chainEnd)
        SetupChain(chainEnd, chainLength);
    return stream.Status();
}

// 0x10020410 ?PrepareForExecution@MfCompliantActivity@@UAEXXZ
void MfCompliantActivity::PrepareForExecution()
{
    // the target actor has to be updated first
    if (mActor && mTargetNode && mActor->mSpace)
        mActor->mSpace->ReorderActorUpdate(mActor, mTargetActor);
}

// 0x100082c0 ??_EMfCompliantActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (1001fc00); the port needs its own definition.
int MfCompliantActivity::HasMotion()
{
    return 1;
}
