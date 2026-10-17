// MfHoldActivity: keeps a node of the actor (a hand) where it holds another
// actor (RuntimeLib.dll 1002f470-1002ff40).
#include "MfHoldActivity.h"
#include "MfActor.h"
#include "MfColNode.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfJLimit.h"
#include "MfProtoNode.h"
#include "MfSpace.h"
#include "MfSkeletonTraversal.h"
#include "MfStream.h"

// references to actors and nodes in a stream (defined with the first
// activities, 1000a6e0-1000a8b0)
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);
MfStreamError WriteActorReference(MfStream &stream, MfActor *actor);
MfStreamError ReadActorReference(MfStream &stream, MfActor *&actor);

long const MfHoldActivity::STREAM_TYPE = 'HolA';

// the implicit copy members of the exported class
static MfHoldActivity &(MfHoldActivity::*s_assign)(const MfHoldActivity &) = &MfHoldActivity::operator=;

// Inline members this object emits (the class is exported):
// 0x10007760 ?Type@MfHoldActivity@@UAEHXZ
// 0x10007770 ?IsType@MfHoldActivity@@UAEHH@Z
// 0x100077a0 ?TargetActor@MfHoldActivity@@QAEPAVMfActor@@XZ
// 0x100077b0 ?PrintName@MfHoldActivity@@UBE?AVMfString@@XZ
// 0x1000aa90 ?Type@MfHoldActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x1000aaa0 ?IsType@MfHoldActivity@@WBE@AEHH@Z
// 0x100077e0 ??0MfHoldActivity@@QAE@ABV0@@Z (the implicit copy constructor)
// 0x10007a70 ??0Link@MfKChain@@QAE@ABU01@@Z (implicit)
// 0x10007b00 ??4MfHoldActivity@@QAEAAV0@ABV0@@Z

// 0x1002f470 ??0MfHoldActivity@@QAE@XZ
MfHoldActivity::MfHoldActivity()
    : MfActivity(0, 0)
{
}

// 0x1002f4f0 ??0MfHoldActivity@@QAE@PAVMfActor@@0HK@Z
MfHoldActivity::MfHoldActivity(MfActor *actor, MfActor *target, int handIndex, unsigned long startTime)
    : MfActivity(actor, startTime)
{
    mHandNode = actor->skeletonTraversal()->node(handIndex);
    mTargetActor = target;
    mHeldNodes = new MfSkeletonTraversal(mHandNode, MfSkeletonNode::TYPE, 1);
    mInitialTransforms = mHeldNodes->CopyTransforms();
    mGrip = ~actor->skeleton()->globalTransform() * mHandNode->globalTransform();
    // the chain that moves the hand: the actor's own nodes, single children
    // and at least three nodes, up to the hip
    mChainLength = 1;
    MfSkeletonNode *node = (MfSkeletonNode *)mHandNode->parent();
    while (node->GetOwner() == actor && (node->numChildren(0) == 1 || mChainLength < 3) &&
           node != mActor->skeletonTraversal()->node(1)) {
        node = (MfSkeletonNode *)node->parent();
        mChainLength++;
    }
    MakeColModel(handIndex);
}

// 0x1002f6b0 ??1MfHoldActivity@@UAE@XZ
MfHoldActivity::~MfHoldActivity()
{
    if (mHeldNodes)
        delete mHeldNodes;
    if (mInitialTransforms)
        delete [] mInitialTransforms;
    if (mColNode)
        delete mColNode;
}

// 0x1002f770 ?MakeColModel@MfHoldActivity@@IAEXH@Z
void MfHoldActivity::MakeColModel(int handIndex)
{
    // the collision model of the hand's subtree in the frame of its parent
    MfTransform3 saved = mHandNode->transform();
    MfTransform3 parent = ((MfSkeletonNode *)mHandNode->parent())->globalTransform();
    mHandNode->setTransform(~parent);
    mHandNode->syncChildTransforms(0);
    mColNode = mActor->MakeSubTreeColNode(handIndex);
    mHandNode->setTransform(saved);
    mHandNode->syncChildTransforms(0);
    m448 = mGrip.y() * 0.2f;
}

// 0x1002f890 ?Update@MfHoldActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfHoldActivity::Update(unsigned long time)
{
    MfTransform3 goal;
    MfRotation3 from;
    MfRotation3 to;
    MfTransform3 halfway;
    MfDblList children;
    MfDblList filtered;
    mActor->UseFilter(1, 2, 5, filtered);
    mEnvironment = mActor->mSpace->mSnapshotEnv;
    mActor->ignoreTriangleCollisionCheck(1);
    mActor->GetChildActors(&children);
    MfDblListItem *pli;
    MfAiActor *child;
    for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
        ((MfActor *)child)->ignoreTriangleCollisionCheck(1);

    // move the hand to the grip; while it collides with the environment,
    // try a little higher
    for (int attempt = 0; attempt < 4; attempt++) {
        goal = mActor->skeleton()->globalTransform() * mGrip;
        MfSkeletonNode *node = mHandNode;
        for (int i = 0; i < mChainLength; i++) {
            if (node->jointLimit()) {
                if (node->jointLimit()->GetType() == MF_JL_HINGE) {
                    int mode;
                    ((MfJLHinge *)node->jointLimit())->GetValue(from, to, mode);
                    halfway = node->transform();
                    halfway.rotation().interpolate(from, to, 0.5f, mode ? -1 : 0);
                    node->setTransform(halfway);
                } else if (node->jointLimit()->GetType() == MF_JL_EULER) {
                }
            }
            node = (MfSkeletonNode *)node->parent();
        }
        mChain.create(mHandNode, mChainLength, MfVector3(0.0f, 0.0f, 0.0f));
        mChain.setThreshold(0.0f);
        mChain.moveTo(goal);
        mChain.updateSkeleton();
        mColNode->Transform() = mHandNode->globalTransform();
        MfColResultNode result;
        int collides = mEnvironment->CheckCollision(mColNode, result);
        if (!collides)
            break;
        mGrip.y() += m448;
    }

    for (int i = 1; i < mHeldNodes->numNodes(); i++)
        mHeldNodes->node(i)->setTransform(mInitialTransforms[i]);
    mActor->ignoreTriangleCollisionCheck(0);
    for (child = (MfAiActor *)children.GetFirst(pli); child; child = (MfAiActor *)children.GetNext(pli))
        ((MfActor *)child)->ignoreTriangleCollisionCheck(0);
    mActor->UseFilter(0, 2, 5, filtered);
    return MfActivity::Update(time);
}

// 0x1002fcc0 ?CreateHoldActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateHoldActivity()
{
    return new MfHoldActivity;
}

// 0x1002fd30 _$E40 (original $E: the static initializer of the constructor entry, with 1002fd40)
// 0x1002fd40 _$E39
static MfStreamConstructorEntry constructorEntry(MfHoldActivity::STREAM_TYPE, CreateHoldActivity);

// 0x1002fd60 ?StreamType@MfHoldActivity@@UBEJXZ
long MfHoldActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1002fd70 ?WriteToStream@MfHoldActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfHoldActivity::WriteToStream(MfStream &stream) const
{
    MfActivity::WriteToStream(stream);
    WriteActorReference(stream, mTargetActor);
    WriteNodeReference(stream, mHandNode);
    return stream.Status();
}

// 0x1002fdb0 ?ReadFromStream@MfHoldActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfHoldActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfActivity::ReadFromStream(stream);
    if (error)
        return error;
    ReadActorReference(stream, mTargetActor);
    ReadNodeReference(stream, mActor, mHandNode);
    if (stream.Status() == MF_STREAM_OK) {
        mHeldNodes = new MfSkeletonTraversal(mHandNode, MfSkeletonNode::TYPE, 1);
        mInitialTransforms = mHeldNodes->CopyTransforms();
        mGrip = ~mActor->skeleton()->globalTransform() * mHandNode->globalTransform();
        // the chain that moves the hand: single children and at least
        // three nodes, up to the hip
        mChainLength = 1;
        MfTreeNode *node = mHandNode->parent();
        while ((node->numChildren(0) == 1 || mChainLength < 3) && node != mActor->skeletonTraversal()->node(1)) {
            node = node->parent();
            mChainLength++;
        }
        MakeColModel(mActor->skeletonTraversal()->nodeIndex(mHandNode));
    }
    return stream.Status();
}

// 0x10007d20 ??_EMfHoldActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)
