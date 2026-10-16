// MfActorPath: a sampled path of an actor, one MfJointPath per node
// (RuntimeLib.dll 10012f90-10013560).
#include "MfActorPath.h"
#include "MfSkeletonTraversal.h"
#include "MfActor.h"

// 0x10012f90 ??0MfActorPath@@QAE@PAVMfActor@@@Z
MfActorPath::MfActorPath(MfActor *actor)
{
    mNumTracks = actor->skeletonTraversal()->numNodes();
    mTracks = new MfJointPath *[mNumTracks];
    for (int i = 0; i < mNumTracks; i++)
        mTracks[i] = 0;
}

// 0x10012fe0 ??0MfActorPath@@QAE@H@Z
MfActorPath::MfActorPath(int numTracks)
{
    mNumTracks = numTracks;
    mTracks = new MfJointPath *[mNumTracks];
    for (int i = 0; i < mNumTracks; i++)
        mTracks[i] = 0;
}

// 0x10013020 ??0MfActorPath@@QAE@ABV0@@Z
MfActorPath::MfActorPath(const MfActorPath &path)
{
    mNumTracks = path.mNumTracks;
    mTracks = new MfJointPath *[mNumTracks];
    for (int i = 0; i < mNumTracks; i++)
        mTracks[i] = path.mTracks[i] ? new MfJointPath(*path.mTracks[i]) : 0;
}

// 0x100130e0 ??1MfActorPath@@QAE@XZ
MfActorPath::~MfActorPath()
{
    for (int i = 0; i < mNumTracks; i++)
        if (mTracks[i])
            delete mTracks[i];
    delete mTracks;
}

// 0x10013120 ?NewTrack@MfActorPath@@QAEPAVMfJointPath@@H@Z
MfJointPath *MfActorPath::NewTrack(int i)
{
    if (mTracks[i])
        delete mTracks[i];
    mTracks[i] = new MfJointPath;
    return mTracks[i];
}

// 0x100131a0 ?Track@MfActorPath@@QAEPAVMfJointPath@@H@Z
MfJointPath *MfActorPath::Track(int i)
{
    return mTracks[i];
}

// 0x100131b0 ?Sample@MfActorPath@@QAEHKPAVMfSkeletonTraversal@@PAHH@Z
int MfActorPath::Sample(unsigned long time, MfSkeletonTraversal *nodes, int *masks, int interpolate)
{
    int sampled = 0;
    for (int i = 0; i < nodes->numNodes(); i++) {
        if (!masks[i] && mTracks[i]) {
            sampled = 1;
            nodes->node(i)->setTransform(mTracks[i]->Sample(time, interpolate));
        }
    }
    return sampled;
}

// 0x10013260 ?Sample@MfActorPath@@QAEHKPAVMfSkeletonNode@@HH@Z
int MfActorPath::Sample(unsigned long time, MfSkeletonNode *node, int index, int interpolate)
{
    int sampled = 0;
    if (mTracks[index]) {
        sampled = 1;
        node->setTransform(mTracks[index]->Sample(time, interpolate));
    }
    return sampled;
}

// 0x100132d0 ?Sample@MfActorPath@@QAEHKPAVMfSkeletonTraversal@@HHPBVMfTransform3@@@Z
int MfActorPath::Sample(unsigned long time, MfSkeletonTraversal *nodes, int interpolate, int start,
                        const MfTransform3 *base)
{
    int end;
    if (start <= 1) {
        end = nodes->numNodes();
    } else {
        for (end = start + 1; end < nodes->numNodes(); end++) {
            if (!nodes->node(start)->hasDescendant(nodes->node(end)))
                break;
        }
    }
    MfSkeletonNode *node = nodes->node(start);
    MfTransform3 xform;
    if (mTracks[start])
        xform = mTracks[start]->Sample(time, interpolate);
    else
        xform = node->transform();
    if (base)
        xform = *base * xform;
    node->setTransform(xform);
    int sampled = mTracks[start] || base;
    for (int i = start + 1; i < end; i++) {
        node = nodes->node(i);
        if (mTracks[i]) {
            sampled = 1;
            node->setTransform(mTracks[i]->Sample(time, interpolate));
        } else {
            node->setTransform(node->transform());
        }
    }
    return sampled;
}

// 0x100134e0 ?ScaleTrack@MfActorPath@@QAEXHABVMfVector3@@@Z
void MfActorPath::ScaleTrack(int i, const MfVector3 &scale)
{
    if (mTracks[i])
        mTracks[i]->Scale(scale);
}

// 0x10013500 ?MaxSamples@MfActorPath@@QAEIXZ
unsigned int MfActorPath::MaxSamples()
{
    unsigned int max = 0;
    for (int i = 0; i < mNumTracks; i++) {
        if (mTracks[i] && mTracks[i]->mInterval * (mTracks[i]->mNumKeys + 1) > max)
            max = mTracks[i]->mInterval * (mTracks[i]->mNumKeys + 1);
    }
    return max;
}

// 0x1000de60 ?MemorySize@MfActorPath@@QAEHXZ (folded with MfActivity::AddRef)
int MfActorPath::MemorySize()
{
    return 0;
}

// 0x10013530 ?Compact@MfActorPath@@QAEXXZ
void MfActorPath::Compact()
{
    for (int i = 0; i < mNumTracks; i++)
        if (mTracks[i])
            mTracks[i]->Compact();
}
