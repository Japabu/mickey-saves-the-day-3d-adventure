#pragma once
#include "MfActivity.h"
#include "MfVector3.h"
#include "enums.h"
class MfActor;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfStream;
class MfString;
class MfTransform3;

class __declspec(dllexport) MfOrientActivity : public MfActivity {
public:
    enum { TYPE = 'OriA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfOrientActivity();
    virtual void setInitialConfig(MfSkeletonTraversal *, unsigned long);
    virtual void setInitialConfig(MfTransform3 *, unsigned long);
    virtual MfString PrintName() const { return MfString("Orient"); }
    virtual void PrepareForExecution();
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    MfOrientActivity();
    MfOrientActivity(MfActor *, unsigned long);
    MfSkeletonNode * getPointNode() const;
    MfSkeletonNode * getTargetNode() const;
    MfVector3 const & getNodeOffset() const;
    int getChainLength() const;
    void setup(MfSkeletonNode *, MfVector3 const &, MfSkeletonNode *, int);
    void setup(MfSkeletonNode *, MfVector3 const &, MfVector3 const &, int);
    static long const STREAM_TYPE;
    MfSkeletonNode *mPointNode;                             // 0x60 the node that is turned
    MfSkeletonNode *mTargetNode;                            // 0x64 what it points at, or
    MfActor *mTargetActor;                                  // 0x68 (the owner of mTargetNode)
    MfVector3 mNodeOffset;                                  // 0x6c the pointing direction in the node
    MfVector3 mTargetPoint;                                 // 0x78 a point
    int mChainLength;                                       // 0x84 how many parents turn with it
    int mPointNodeIndex;                                    // 0x88
};
