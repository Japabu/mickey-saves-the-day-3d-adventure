#include "Graph_BasicRefreshTraverser.h"
#include "Graph_ActiveNode.h"

// 0x100e1490 ?GetTypeInfo@BasicRefreshTraverser@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::BasicRefreshTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e14a0 ?GetTypeName@BasicRefreshTraverser@Graph@@UBEPBDXZ
char const * Graph::BasicRefreshTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e14b0 ??0BasicRefreshTraverser@Graph@@QAE@XZ
Graph::BasicRefreshTraverser::BasicRefreshTraverser() : m_RefreshType(Partial)
{
}

// 0x100e14f0 ??1BasicRefreshTraverser@Graph@@UAE@XZ
Graph::BasicRefreshTraverser::~BasicRefreshTraverser()
{
}

// 0x100e1500 ?Traverse@BasicRefreshTraverser@Graph@@QAEXAAVActiveNode@2@W4RefreshType@12@@Z
void Graph::BasicRefreshTraverser::Traverse(Graph::ActiveNode &node, RefreshType type)
{
    m_RefreshType = type;
    NodeTraverser::Traverse(node);
}

// 0x100e1520 ?Visit@BasicRefreshTraverser@Graph@@UAE?AW4Response@NodeTraverser@2@AAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::BasicRefreshTraverser::Visit(Graph::BasicNode &node)
{
    Graph::ActiveNode &activeNode = (Graph::ActiveNode &)node;
    if (activeNode.m_RefreshIsScheduled) {
        if (activeNode.m_ChildRefreshIsScheduled) {
            if (PassiveTraverser::Visit(node) == Terminate)
                return Terminate;
        }
        activeNode.Refresh(*this);
        return Continue;
    }
    if (activeNode.m_ChildRefreshIsScheduled)
        return PassiveTraverser::Visit(node) != Terminate ? Continue : Terminate;
    return Prune;
}


// Matched by this code:
// 0x100e14d0 ??_GBasicRefreshTraverser@Graph@@UAEPAXI@Z