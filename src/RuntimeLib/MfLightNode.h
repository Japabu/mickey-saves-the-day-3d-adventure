// MfLightNode: a skeleton node that carries a light. Layout from
// ??0MfLightNode@@QAE@XZ; vtable order from ??_7MfLightNode@@6BMfTreeNode@@@ and
// ??_7MfLightNode@@6BMfAiActorNode@@@.
#ifndef RUNTIMELIB_MFLIGHTNODE_H
#define RUNTIMELIB_MFLIGHTNODE_H
#include "MfLight.h"
#include "MfSkeletonNode.h"
class MfProtoNode;
class MfScDataHandle;

class __declspec(dllexport) MfLightNode : public MfSkeletonNode {
public:
    enum { TYPE = 'Lght' };
    MfLightNode() {}
    MfLightNode(MfProtoNode *proto);
    MfLightNode(const MfLightNode &);
    virtual ~MfLightNode();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfSkeletonNode::IsType(type);
    }
    virtual MfSkeletonNode *createCopy() { return new MfLightNode(*this); }
protected:
    virtual void _syncToTransform();
public:
    // MfAiActorNode
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual MfLight *GetLight() { return &mLight; }
    virtual void SetupLight(int on);

    MfLight &Light() { return mLight; }

    MfLight mLight;                                         // 0x10c
};

#endif
