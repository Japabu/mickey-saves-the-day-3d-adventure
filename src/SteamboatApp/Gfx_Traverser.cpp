#include "Gfx_Traverser.h"
#include "MathConstants.h"
#include "Gfx_Node.h"

// 0x1010d4e0 ?GetTypeInfo@Traverser@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Traverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x1010d4f0 ?GetTypeName@Traverser@Gfx@@UBEPBDXZ
char const * Gfx::Traverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x1010d500 ??0Traverser@Gfx@@QAE@XZ
Gfx::Traverser::Traverser() : m_pView(0), m_LODScale(1.0f)
{
}

// 0x1010d540 ??1Traverser@Gfx@@UAE@XZ
Gfx::Traverser::~Traverser()
{
}

// 0x1010d550 ?Traverse@Traverser@Gfx@@UAEXAAVNode@2@@Z
void Gfx::Traverser::Traverse(Gfx::Node &node)
{
    m_LODScale = 1.11111f;
    NodeTraverser::Traverse(node);
}


// Matched by this code:
// 0x1010d520 ??_GTraverser@Gfx@@UAEPAXI@Z// 0x1010d3f0 _$E10
// 0x1010d400 _$E9
// 0x1010d420 _$E13
// 0x1010d430 _$E12
// 0x1010d450 _$E16
// 0x1010d460 _$E15
// 0x1010d480 _$E19
// 0x1010d490 _$E18
// 0x1010d4b0 _$E22
// 0x1010d4c0 _$E21

// the run-time type of Gfx::Traverser (values from the original's data section)
StdBase::TypeInfo const Gfx::Traverser::s_TypeInfo = { "Gfx::Traverser", &Graph::PassiveTraverser::s_TypeInfo };
