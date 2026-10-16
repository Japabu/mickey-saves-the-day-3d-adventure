// MfFloor: a skeleton node that actors can stand and walk on (prototype node
// style FLOOR). Unit 10020fe0-10021df0; the inline members are emitted by every
// unit that includes this header (the class is exported).
#ifndef RUNTIMELIB_MFFLOOR_H
#define RUNTIMELIB_MFFLOOR_H
#include "MfSkeletonNode.h"
#include "enums.h"
class MfProtoNode;
class MfRay;
class MfTriangleSet;

class __declspec(dllexport) MfFloor : public MfSkeletonNode {
public:
    enum { TYPE = 'Flor' };

    MfFloor();
    MfFloor(const MfFloor &other) : MfSkeletonNode(other) {}
    virtual ~MfFloor();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfSkeletonNode::IsType(type);
    }
    virtual MfActorNodeType GetNodeType();

    static void __cdecl CreateFloor(MfProtoNode *, MfTriangleSet *, int, MfSkeletonNode *, MfRay &);
    static void __cdecl CreateProjectedFloor(MfProtoNode *, MfTriangleSet *, MfSkeletonNode *);
};

#endif
