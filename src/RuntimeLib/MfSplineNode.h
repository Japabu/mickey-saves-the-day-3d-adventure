// MfSplineNode: a skeleton node of a spline (prototype node style SPLINE).
// Unit 100538e0-100539b0; the inline members are emitted by every unit that
// includes this header (the class is exported).
#ifndef RUNTIMELIB_MFSPLINENODE_H
#define RUNTIMELIB_MFSPLINENODE_H
#include "MfSkeletonNode.h"
#include "enums.h"

class __declspec(dllexport) MfSplineNode : public MfSkeletonNode {
public:
    enum { TYPE = 'Spln' };

    MfSplineNode();
    MfSplineNode(const MfSplineNode &);
    virtual ~MfSplineNode();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfSkeletonNode::IsType(type);
    }
    virtual MfSkeletonNode *createCopy() { return new MfSplineNode(*this); }
    virtual MfActorNodeType GetNodeType();
};

#endif
