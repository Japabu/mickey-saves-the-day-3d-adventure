#pragma once
#include "MfActivity.h"
#include "MfKChain.h"
#include "MfTransform3.h"
#include "enums.h"
class MfActor;
class MfColNode;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfStream;
class MfString;

class __declspec(dllexport) MfHoldActivity : public MfActivity {
public:
    enum { TYPE = 'HolA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfHoldActivity();
    virtual MfString PrintName() const { return MfString("Hold"); }
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    MfActor *TargetActor() { return mTargetActor; }
    MfHoldActivity();
    MfHoldActivity(MfActor *, MfActor *, int, unsigned long);
    static long const STREAM_TYPE;
protected:
    void MakeColModel(int);
public:
    MfTransform3 mGrip;                                     // 0x60 the hand node in the actor's frame
    MfTransform3 *mInitialTransforms;                       // 0x7c of mHeldNodes
    MfKChain mChain;                                        // 0x80
    int mChainLength;                                       // 0x430
    MfSkeletonNode *mHandNode;                              // 0x434 the node that holds
    MfSkeletonTraversal *mHeldNodes;                        // 0x438 the hand node's subtree
    MfActor *mTargetActor;                                  // 0x43c what is held
    MfColNode *mEnvironment;                                // 0x440 the space's static environment
    MfColNode *mColNode;                                    // 0x444
    float m448;                                             // 0x448
};
