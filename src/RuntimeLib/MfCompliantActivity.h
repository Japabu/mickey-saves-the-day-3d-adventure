#pragma once
#include "MfActivity.h"
#include "MfKChain.h"
#include "MfSkeletonNode.h"
#include "MfTransform3.h"
#include "enums.h"
class MfActor;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfStream;
class MfString;
class MfTransform3;

class __declspec(dllexport) MfCompliantActivity : public MfActivity {
public:
    enum { TYPE = 'CmpA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfCompliantActivity();
    virtual MfString PrintName() const { return MfString("Compliant"); }
    virtual void PrepareForExecution();
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int HasMotion();
    virtual long StreamType() const;
    MfActor *TargetActor() { return mTargetNode->mOwner; }
    MfCompliantActivity();
    MfCompliantActivity(MfActor *, unsigned long);
    void SetupChain(MfSkeletonNode *, int);
    void SetupTarget(MfSkeletonNode *, MfTransform3);
    static long const STREAM_TYPE;
    MfKChain mChain;                                        // 0x60
    MfSkeletonNode *mTargetNode;                            // 0x410 the chain's end follows it
    MfActor *mTargetActor;                                  // 0x414 (the owner of mTargetNode)
    MfTransform3 mOffset;                                   // 0x418 of the end from the target node
    MfSkeletonTraversal *mNodes;                            // 0x434 the chain's end node and its subtree
    MfTransform3 *mInitialTransforms;                       // 0x438 of mNodes
    MfTransform3 *mLinkTransforms;                          // 0x43c the chain's links before the move
};
