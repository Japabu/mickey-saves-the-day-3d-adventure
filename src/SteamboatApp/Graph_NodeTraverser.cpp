#include "Graph_NodeTraverser.h"
#include "Graph_BasicNode.h"
#include "StdBase.h"

// 0x100e1ec0 ?GetTypeInfo@NodeTraverser@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::NodeTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e1ed0 ?GetTypeName@NodeTraverser@Graph@@UBEPBDXZ
char const * Graph::NodeTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e1ee0 ??0NodeTraverser@Graph@@IAE@XZ
Graph::NodeTraverser::NodeTraverser() : m_NodeCount(0)
{
}

// 0x100e1f50 ??1NodeTraverser@Graph@@UAE@XZ
Graph::NodeTraverser::~NodeTraverser()
{
}

// TODO 0x100e1fa0 ?Traverse@NodeTraverser@Graph@@QAEXAAVBasicNode@2@@Z: Vec::remove shift loop: original increments index before the body; append loads len via lea
void Graph::NodeTraverser::Traverse(Graph::BasicNode &node)
{
    m_NodeCount = 0;
    PushNode(node);
    Visit(node);
    PopNode();
}

// 0x100e20e0 ?Visit@NodeTraverser@Graph@@UAE?AW4Response@12@AAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::NodeTraverser::Visit(Graph::BasicNode &)
{
    m_NodeCount++;
    return Continue;
}


// Matched by this code:
// 0x100e1f30 ??_GNodeTraverser@Graph@@UAEPAXI@Z
// 0x100e20f0 ?initElements@?$Vec@PAVBasicNode@Graph@@@@IAEXHH@Z