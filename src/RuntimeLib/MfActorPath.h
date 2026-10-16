// MfActorPath: a sampled path of an actor (MfSkill::sampledPath), one
// MfJointPath per node. Only what other units use is declared so far.
#ifndef RUNTIMELIB_MFACTORPATH_H
#define RUNTIMELIB_MFACTORPATH_H
#include "MfStreamObject.h"
#include "MfRotation3.h"
#include "MfTransform3.h"
#include "MfVector3.h"

class MfActor;
class MfSkeletonNode;
class MfStream;
class MfSkeletonTraversal;

// the sampled transforms of one node (unit 1002ff40-10030b90); the
// translations and rotations are kept apart and a track may end early (its
// last value holds)
class MfJointPath : public MfStreamObject {
public:
    enum { STREAM_TYPE = 'JPat' };
    MfJointPath();
    MfJointPath(const MfJointPath &path);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &stream);
    virtual MfStreamError WriteToStream(MfStream &stream) const;
    virtual ~MfJointPath();
    void Allocate(int numKeys);
    void SetKey(int i, const MfTransform3 &xform);
    // drops the repeated values at the end of the tracks
    void Compact();
    void Scale(const MfVector3 &scale);
    // the transform at a time, interpolated between the samples or not (100302c0)
    MfTransform3 Sample(unsigned long time, int interpolate);
    int NumKeys() { return mNumKeys; }
    MfTransform3 Key(int i)
    {
        int r = i >= mNumRotations ? mNumRotations - 1 : i;
        int t = i < mNumTranslations ? i : mNumTranslations - 1;
        return MfTransform3(mRotations[r], mTranslations[t]);
    }

    unsigned long mInterval;                                // 0x04 between samples
    int mNumTranslations;                                   // 0x08
    int mNumRotations;                                      // 0x0c
    int mNumKeys;                                           // 0x10
    MfVector3 *mTranslations;                               // 0x14
    MfRotation3 *mRotations;                                // 0x18
};

class MfActorPath {
public:
    // one empty track per node of the actor (10012f90)
    MfActorPath(MfActor *actor);
    MfActorPath(int numTracks);                                                               // 10012fe0
    MfActorPath(const MfActorPath &path);                                                     // 10013020
    ~MfActorPath();                                                                           // 100130e0
    MfJointPath *Track(int i);                                                                // 100131a0
    // a new empty track in place of track i (10013120)
    MfJointPath *NewTrack(int i);
    void Compact();                                                                           // 10013530
    void ScaleTrack(int i, const MfVector3 &scale);                                           // 100134e0
    // set the nodes to the sampled configuration at a time
    int Sample(unsigned long time, MfSkeletonTraversal *nodes, int *masks, int interpolate);   // 100131b0
    int Sample(unsigned long time, MfSkeletonNode *node, int index, int interpolate);         // 10013260
    // the subtree under node start (the whole skeleton for start 0 or 1),
    // the start node put at base * its sample when base is given
    int Sample(unsigned long time, MfSkeletonTraversal *nodes, int interpolate, int start,
               const MfTransform3 *base);                                                     // 100132d0
    // the number of samples of the longest track (10013500)
    unsigned int MaxSamples();
    // the memory the samples take; returns 0 (folded at 1000de60)
    int MemorySize();

    int mNumTracks;                                         // 0x00
    MfJointPath **mTracks;                                  // 0x04
};

#endif
