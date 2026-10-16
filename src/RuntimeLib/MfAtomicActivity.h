// MfAtomicActivity: an activity that plays one skill on the actor's skeleton.
// Layout from the constructors (RuntimeLib.dll 10017f90-10019690); the
// inline members are emitted by every unit (the class is exported).
#ifndef RUNTIMELIB_MFATOMICACTIVITY_H
#define RUNTIMELIB_MFATOMICACTIVITY_H
#include "MfActivity.h"
#include "MfDblList.h"
#include "MfSkeletonTraversal.h"
#include "MfTransform3.h"
#include "MfVector3.h"
#include "enums.h"
class MfActor;
class MfKeyframe;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfSkill;
class MfStream;
class MfString;

// The collision bubbles of the nodes an atomic activity moves: one sphere per
// collision sphere of the moving nodes' collision models.
class MfBubbleModel {
public:
    MfBubbleModel()
    {
        mCount = 0;
        mNodes = 0;
        mRadii = 0;
        mCenters = 0;
        mGlobalCenters = 0;
    }
    ~MfBubbleModel();
    void Allocate(int count);

    MfSkeletonNode **mNodes;                                // 0x00
    MfVector3 *mCenters;                                    // 0x04 in the node's frame
    MfVector3 *mGlobalCenters;                              // 0x08
    float *mRadii;                                          // 0x0c
    int *mHits;                                             // 0x10
    int mCount;                                             // 0x14
    MfDblList mColliders;                                   // 0x18
};

class __declspec(dllexport) MfAtomicActivity : public MfActivity {
public:
    enum { TYPE = 'AtoA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfAtomicActivity();
    virtual void setInitialConfig(MfSkeletonTraversal *, unsigned long);
    virtual void setInitialConfig(MfTransform3 *, unsigned long);
    virtual MfString PrintName() const { return MfString("Atomic"); }
protected:
    virtual void blendFromInitialConfig(unsigned long, int);
public:
    virtual void setSkill(MfSkill *);
    virtual void setPlayDirection(int);
    virtual int getPlayDirection() const { return mPlayDirection; }
protected:
    virtual void _InitBubbleModel(MfSkill *, MfActor *, int);
    virtual void _DeleteBubbleModel();
    virtual void _CheckBubble(unsigned long &) {}
public:
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int *GetIgnoreMasks(int &count)
    {
        if (mIgnoreMasks && mActor)
            count = mActor->skeletonTraversal()->numNodes();
        return mIgnoreMasks;
    }
    virtual int AnimateSubtree(MfString const &);
    virtual int HasMotion();
    virtual int IgnoreSubtree(MfString const &);
    virtual int WillFinish();
    virtual long StreamType() const;
    MfAtomicActivity();
    MfAtomicActivity(MfActor *actor, unsigned long startTime, MfSkill *skill);
    MfSkill *getSkill() const { return mSkill; }
    int getCheckColl() const { return mBubbleModel != 0; }
    void SetMaintainTextures(int maintain) { mMaintainTextures = maintain; }
    static long const STREAM_TYPE;
protected:
    void setInitialStates();
    void updateExtraTracks(unsigned long, unsigned long);
public:
    MfSkill *mSkill;                                        // 0x60
    unsigned long mFirstTime;                               // 0x64 of the skill
    unsigned long mLastTime;                                // 0x68 of the skill, at least 1
    int mPlayDirection;                                     // 0x6c 1: backwards
    MfTransform3 mSkillOffset;                              // 0x70 initial config * ~skill start
    unsigned long mLastSampleTime;                          // 0x8c
    int m90;                                                // 0x90
    MfBubbleModel *mBubbleModel;                            // 0x94 collision check
    int m98;                                                // 0x98
    int *mIgnoreMasks;                                      // 0x9c per node, 1: not animated
    int mMaintainTextures;                                  // 0xa0
    MfKeyframe *mLastSound;                                 // 0xa4
    MfKeyframe *mLastEvent;                                 // 0xa8
};

#endif
