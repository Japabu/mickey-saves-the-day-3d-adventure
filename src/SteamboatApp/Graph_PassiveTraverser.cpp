#include "Graph_PassiveTraverser.h"
#include "Graph_ActiveNode.h"

// 0x100e1e30 ?GetTypeInfo@PassiveTraverser@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::PassiveTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e1e40 ?GetTypeName@PassiveTraverser@Graph@@UBEPBDXZ
char const * Graph::PassiveTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e1e50 ??0PassiveTraverser@Graph@@IAE@XZ
Graph::PassiveTraverser::PassiveTraverser()
{
}

// 0x100e1e90 ??1PassiveTraverser@Graph@@UAE@XZ
Graph::PassiveTraverser::~PassiveTraverser()
{
}

// 0x100e1ea0 ?Visit@PassiveTraverser@Graph@@UAE?AW4Response@NodeTraverser@2@AAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::PassiveTraverser::Visit(Graph::BasicNode &node)
{
    NodeTraverser::Visit(node);
    return ((Graph::ActiveNode &)node).Traverse(*this);
}


// Matched by this code:
// 0x100e1e70 ??_GPassiveTraverser@Graph@@UAEPAXI@Z

// the run-time type of Graph::PassiveTraverser (values from the original's data section)
StdBase::TypeInfo const Graph::PassiveTraverser::s_TypeInfo = { "Graph::PassiveTraverser", &Graph::NodeTraverser::s_TypeInfo };
