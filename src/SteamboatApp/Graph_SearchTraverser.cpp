#include "Graph_SearchTraverser.h"
#include "Graph_ActiveNode.h"
#include "Graph_BasicNode.h"
#include <string.h>

// 0x100e09f0 ?GetTypeInfo@SearchTraverser@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::SearchTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e0a00 ?GetTypeName@SearchTraverser@Graph@@UBEPBDXZ
char const * Graph::SearchTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e0a10 ??0SearchTraverser@Graph@@QAE@XZ
Graph::SearchTraverser::SearchTraverser()
    : m_Type(0), m_Node(0), m_NameMatch(Exact), m_CaseSensitive(false), m_Find(All), m_Derived(true)
{
}

// 0x100e0ac0 ??1SearchTraverser@Graph@@UAE@XZ
Graph::SearchTraverser::~SearchTraverser()
{
}

// TODO 0x100e0b40 ?Traverse@SearchTraverser@Graph@@QAEXAAVActiveNode@2@@Z: first of two m_FindList clears: original drops the else-if initElements test of setLength
void Graph::SearchTraverser::Traverse(Graph::ActiveNode &node)
{
    if (m_FindList.length() > 0)
        m_FindList.setLength(0);
    m_FindList.setLength(0);
    NodeTraverser::Traverse(node);
}

// EQUIV 0x100e0bb0 ?Visit@SearchTraverser@Graph@@UAE?AW4Response@NodeTraverser@2@AAVBasicNode@2@@Z: reviewed: the code bytes are identical (0 differing, relocations and EH funclets match); only the function's EH tables differ (the original's FuncInfo has 3 unwind states, VC6 here counts more)
Graph::NodeTraverser::Response Graph::SearchTraverser::Visit(Graph::BasicNode &node)
{
    Text name(m_Name);
    Text nodeName(node.GetName());
    if (!m_CaseSensitive) {
        name.toLower();
        nodeName.toLower();
    }
    int length = name.length();
    if ((length && (strcmp(name.getConstPtr(), nodeName.getConstPtr()) == 0 || (m_NameMatch == Contains && nodeName.contains(name.getConstPtr())))) ||
        (m_Node && m_Node == &node) || (m_Derived && node.IsOfType(m_Type)) || node.IsOfExactType(m_Type)) {
        m_FindList.append(GetTraversalPath());
        if (m_Find == First)
            return Terminate;
    }
    return PassiveTraverser::Visit(node);
}

// Matched by this code:
// 0x100e0aa0 ??_GSearchTraverser@Graph@@UAEPAXI@Z
// 0x100e0e60 ??_G?$Vec@VPath@Graph@@@@UAEPAXI@Z
// 0x100e0e80 ??1?$Vec@VPath@Graph@@@@UAE@XZ
// 0x100e0ed0 ?reallocMem@?$Vec@VPath@Graph@@@@IAEXHHH@Z
// 0x100e1060 ?initElements@?$Vec@VPath@Graph@@@@IAEXHH@Z
// 0x100e10f0 ??_EPath@Graph@@UAEPAXI@Z

// the run-time type of Graph::SearchTraverser (values from the original's data section)
StdBase::TypeInfo const Graph::SearchTraverser::s_TypeInfo = { "Graph::SearchTraverser", &Graph::PassiveTraverser::s_TypeInfo };
