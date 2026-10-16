#include "Graph_ActiveNode.h"
#include "Graph_PassiveTraverser.h"
#include "Graph_BasicRefreshTraverser.h"

// 0x100df740 ?GetTypeInfo@ActiveNode@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::ActiveNode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100df750 ?GetTypeName@ActiveNode@Graph@@UBEPBDXZ
char const * Graph::ActiveNode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100df760 ??1ActiveNode@Graph@@UAE@XZ
Graph::ActiveNode::~ActiveNode()
{
}

// TODO 0x100df790 ?Traverse@ActiveNode@Graph@@UAE?AW4Response@NodeTraverser@2@AAVPassiveTraverser@2@@Z: PushNode/PopNode: original calls reallocMem out of line (inline budget) and keeps &path in ebx
Graph::NodeTraverser::Response Graph::ActiveNode::Traverse(Graph::PassiveTraverser &traverser)
{
    if (StdBaseCast(&traverser, (Graph::BasicRefreshTraverser *)0))
        m_ChildRefreshIsScheduled = false;
    NodeIterator it = GetBeginChildIterator();
    while (it != GetEndChildIterator()) {
        Graph::BasicNode *child = *it++;
        traverser.PushNode(*child);
        Graph::NodeTraverser::Response response = traverser.Visit(*child);
        traverser.PopNode();
        if (response == Graph::NodeTraverser::Terminate)
            return Graph::NodeTraverser::Terminate;
    }
    return Graph::NodeTraverser::Continue;
}

// 0x100df900 ?ScheduleRefresh@ActiveNode@Graph@@QAEX_N@Z
void Graph::ActiveNode::ScheduleRefresh(bool observersOnly)
{
    if (!observersOnly) {
        m_RefreshIsScheduled = true;
        if (!m_RefreshImmediately) {
            for (int i = 0; i < CountParents(); i++) {
                if (!((Graph::ActiveNode *)GetParent(i))->m_ChildRefreshIsScheduled)
                    ((Graph::ActiveNode *)GetParent(i))->ScheduleChildRefresh();
            }
        } else {
            Refresh(Graph::BasicRefreshTraverser());
        }
    }
    for (int i = 0; i < CountObservers(); i++) {
        Graph::ActiveNode *observer = StdBaseCast(GetObserver(i), (Graph::ActiveNode *)0);
        if (observer && !observer->m_RefreshIsScheduled)
            observer->ScheduleRefresh(false);
    }
}

// 0x100df9f0 ??0ActiveNode@Graph@@IAE@XZ
Graph::ActiveNode::ActiveNode()
    : m_RefreshImmediately(false), m_RefreshIsScheduled(true), m_ChildRefreshIsScheduled(true),
      m_pNotifyCallback(0), m_pNotifyData(0)
{
}

// 0x100dfa20 ??0ActiveNode@Graph@@IAE@ABV01@@Z
Graph::ActiveNode::ActiveNode(const Graph::ActiveNode &node)
    : Graph::BasicNode(node),
      m_RefreshImmediately(false), m_RefreshIsScheduled(true), m_ChildRefreshIsScheduled(true),
      m_pNotifyCallback(node.m_pNotifyCallback), m_pNotifyData(node.m_pNotifyData)
{
}

// 0x100dfa60 ?Refresh@ActiveNode@Graph@@MAEXAAVBasicRefreshTraverser@2@@Z
void Graph::ActiveNode::Refresh(Graph::BasicRefreshTraverser &)
{
    m_RefreshIsScheduled = false;
    if (m_pNotifyCallback)
        m_pNotifyCallback(m_pNotifyData);
}

// 0x100dfa80 ?ScheduleChildRefresh@ActiveNode@Graph@@AAEXXZ
void Graph::ActiveNode::ScheduleChildRefresh()
{
    m_ChildRefreshIsScheduled = true;
    for (int i = 0; i < CountParents(); i++) {
        if (!((Graph::ActiveNode *)GetParent(i))->m_ChildRefreshIsScheduled)
            ((Graph::ActiveNode *)GetParent(i))->ScheduleChildRefresh();
    }
}


// Matched by this code:
// 0x100df770 ??_GActiveNode@Graph@@UAEPAXI@Z
// 0x100dfac0 ?reallocMem@?$Vec@PAVBasicNode@Graph@@@@IAEXHHH@Z

// the run-time type of Graph::ActiveNode (values from the original's data section)
StdBase::TypeInfo const Graph::ActiveNode::s_TypeInfo = { "Graph::ActiveNode", &Graph::BasicNode::s_TypeInfo };
