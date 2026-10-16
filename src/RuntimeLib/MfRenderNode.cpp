#include "MfRenderNode.h"

// the implicit copy members of the exported class
static MfRenderNode &(MfRenderNode::*s_assign)(const MfRenderNode &) = &MfRenderNode::operator=;

// 0x1000e310 ??0MfRenderNode@@QAE@ABV0@@Z (implicit copy constructor)
// 0x1000e380 ??4MfRenderNode@@QAEAAV0@ABV0@@Z (implicit assignment)

// 0x1000e2f0 ?IsEnabled@MfRenderNode@@QAE_NXZ
bool MfRenderNode::IsEnabled()
{
	return mEnabled;
}

// 0x1000e300 ?IsWireframed@MfRenderNode@@QAE_NXZ
bool MfRenderNode::IsWireframed()
{
	return mWireframed;
}
