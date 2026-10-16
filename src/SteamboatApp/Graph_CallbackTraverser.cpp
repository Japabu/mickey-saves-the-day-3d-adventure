#include "Graph_CallbackTraverser.h"
#include "Graph_ActiveNode.h"

// 0x1012fec0 ?GetTypeInfo@CallbackTraverser@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::CallbackTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x1012fed0 ?GetTypeName@CallbackTraverser@Graph@@UBEPBDXZ
char const * Graph::CallbackTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x1012fee0 ??0CallbackTraverser@Graph@@QAE@XZ
Graph::CallbackTraverser::CallbackTraverser()
    : m_Match(MatchAll), m_Type(0), m_Node(0), m_pPreData(0), m_pPostData(0),
      m_pPreCallback(0), m_pPostCallback(0)
{
}

// 0x1012ff30 ??1CallbackTraverser@Graph@@UAE@XZ
Graph::CallbackTraverser::~CallbackTraverser()
{
}

// 0x1012ff40 ?SetPreCallback@CallbackTraverser@Graph@@QAEXP6A?AW4Response@NodeTraverser@2@PAV12@PAVBasicNode@2@PAX@Z2@Z
void Graph::CallbackTraverser::SetPreCallback(Callback callback, void *data)
{
    m_pPreCallback = callback;
    m_pPreData = data;
}

// 0x1012ff60 ?Traverse@CallbackTraverser@Graph@@QAEXAAVActiveNode@2@@Z
void Graph::CallbackTraverser::Traverse(Graph::ActiveNode &node)
{
    NodeTraverser::Traverse(node);
}

// 0x1012ff70 ?CallPreCallback@CallbackTraverser@Graph@@QAE?AW4Response@NodeTraverser@2@PAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::CallbackTraverser::CallPreCallback(Graph::BasicNode *node)
{
    if (m_pPreCallback == 0)
        return Continue;
    return m_pPreCallback(this, node, m_pPreData);
}

// 0x1012ffa0 ?CallPostCallback@CallbackTraverser@Graph@@QAE?AW4Response@NodeTraverser@2@PAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::CallbackTraverser::CallPostCallback(Graph::BasicNode *node)
{
    if (m_pPostCallback == 0)
        return Continue;
    return m_pPostCallback(this, node, m_pPostData);
}

// 0x1012ffd0 ?Visit@CallbackTraverser@Graph@@UAE?AW4Response@NodeTraverser@2@AAVBasicNode@2@@Z
Graph::NodeTraverser::Response Graph::CallbackTraverser::Visit(Graph::BasicNode &node)
{
    Response response;
    if (m_Match == MatchAll) {
        response = CallPreCallback(&node);
        if (response == Terminate)
            return Terminate;
        if (response == Prune)
            return Continue;
    } else if (m_Match == MatchType) {
        if (node.IsOfType(m_Type)) {
            response = CallPreCallback(&node);
            if (response == Terminate)
                return Terminate;
            if (response == Prune)
                return Continue;
        }
    } else if (m_Match == MatchNode && m_Node == &node) {
        response = CallPreCallback(&node);
        if (response == Terminate)
            return Terminate;
        if (response == Prune)
            return Continue;
    }
    PassiveTraverser::Visit(node);
    if (m_Match == MatchType) {
        if (node.IsOfType(m_Type)) {
            if (CallPostCallback(&node) == Terminate)
                return Terminate;
        }
    } else if (m_Match == MatchNode && m_Node == &node) {
        if (CallPostCallback(&node) == Terminate)
            return Terminate;
    }
    return Continue;
}


// Matched by this code:
// 0x1012ff10 ??_GCallbackTraverser@Graph@@UAEPAXI@Z

// the run-time type of Graph::CallbackTraverser (values from the original's data section)
StdBase::TypeInfo const Graph::CallbackTraverser::s_TypeInfo = { "Graph::CallbackTraverser", &Graph::PassiveTraverser::s_TypeInfo };
