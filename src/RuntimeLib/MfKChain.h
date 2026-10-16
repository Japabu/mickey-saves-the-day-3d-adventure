// MfKChain: an inverse-kinematics chain from a base node down to an end node
// of an actor's skeleton. Layout from the constructor ??0MfKChain@@QAE@XZ;
// the inline members are emitted by every unit (the class is exported).
#ifndef RUNTIMELIB_MFKCHAIN_H
#define RUNTIMELIB_MFKCHAIN_H
#include "MfAiKChain.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfSkeletonNode;

class __declspec(dllexport) MfKChain : public MfAiKChain {
public:
    enum { MAX_STATIC_LINKS = 10 };

    // one joint of the chain (0x54 bytes)
    struct Link {
        MfTransform3 mXform;                                // 0x00 current local transform
        MfTransform3 mHome;                                 // 0x1c home (rest) transform
        MfVector3 mLocalGoal;                               // 0x38 the goal in this link's frame
        float mStiffness;                                   // 0x44
        float mReturnFactor;                                // 0x48
        MfSkeletonNode *mNode;                              // 0x4c
        int mTranslate;                                     // 0x50 joint slides instead of turning
    };

    MfKChain();
    ~MfKChain();

    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual float MoveTo(const MfTransform3 &xform) { return moveTo(xform); }
    virtual void UpdateChain() { updateChain(); }
    virtual void UpdateActor() const { updateSkeleton(); }
    virtual void SetLinkStiffness(int i, float stiffness) { mLinks[i].mStiffness = stiffness; }
    virtual int GetNumLinks() { return mNumLinks; }
    virtual float &ThresholdSquare() { return mThresholdSquare; }
    virtual int &MaxIterations() { return mMaxIterations; }

    int create(MfSkeletonNode *, int, const MfVector3 &);
    void setNumLinks(int);
    void setStiffness(float);
    void setReturnFactor(float);
    void setBaseXform(const MfTransform3 &);
    void setEndOffset(const MfVector3 &);
    void setRigidity(float rigidity) { mRigidity = rigidity; }
    void setThreshold(float threshold) { mThresholdSquare = threshold * threshold; }
    void setMaxIterations(int iterations) { mMaxIterations = iterations; }
    int numLinks() const { return mNumLinks; }
    int nodeIndex(MfSkeletonNode *);
    MfSkeletonNode *linkNode(int i) { return mLinks[i].mNode; }
    MfTransform3 &linkXform(int i) { return mLinks[i].mXform; }
    MfTransform3 &linkHome(int i) { return mLinks[i].mHome; }
    float &linkStiffness(int i) { return mLinks[i].mStiffness; }
    int &linkTranslate(int i) { return mLinks[i].mTranslate; }
    const MfTransform3 &baseXform() const { return mBaseXform; }
    const MfVector3 &endOffset() const { return mEndOffset; }
    void floatBase(int on) { mFloatBase = on; }
    float moveTo(const MfTransform3 &);
    float moveTo(const MfVector3 &, int);
    void updateChain();
    void updateHome();
    void updateSkeleton() const;

protected:
    int mNumLinks;                                          // 0x04
    Link *mLinks;                                           // 0x08 mLinkBuffer or a new array
    Link mLinkBuffer[MAX_STATIC_LINKS];                     // 0x0c
    MfTransform3 mBaseXform;                                // 0x354 global transform of the base node
    MfTransform3 mBaseInverse;                              // 0x370 ~mBaseXform
    int mHasEndOffset;                                      // 0x38c
    int mFloatBase;                                         // 0x390
    MfVector3 mEndOffset;                                   // 0x394
    float mThresholdSquare;                                 // 0x3a0
    float mRigidity;                                        // 0x3a4
    int mMaxIterations;                                     // 0x3a8
    long mRefCount;                                         // 0x3ac
};

#endif
