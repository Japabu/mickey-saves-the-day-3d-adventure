// MfSplineNode (RuntimeLib.dll 100538e0-100539b0).
#include "MfSplineNode.h"

// Inline members this object emits (the class is exported):
// 0x10006140 ?createCopy@MfSplineNode@@UAEPAVMfSkeletonNode@@XZ
// 0x100061a0 ?Type@MfSplineNode@@UAEHXZ
// 0x100061b0 ?IsType@MfSplineNode@@UAEHH@Z

// 0x100538e0 ??0MfSplineNode@@QAE@XZ
MfSplineNode::MfSplineNode()
{
}

// 0x10053900 ??_EMfSplineNode@@UAEPAXI@Z

// 0x10053960 ??0MfSplineNode@@QAE@ABV0@@Z
MfSplineNode::MfSplineNode(const MfSplineNode &other) : MfSkeletonNode(other)
{
}

// 0x10053980 ??1MfSplineNode@@UAE@XZ
MfSplineNode::~MfSplineNode()
{
}

// 0x100539a0 ?GetNodeType@MfSplineNode@@UAE?AW4MfActorNodeType@@XZ
MfActorNodeType MfSplineNode::GetNodeType()
{
    return MF_NODE_SPLINE;
}
