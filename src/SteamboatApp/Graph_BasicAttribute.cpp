#include "Graph_BasicAttribute.h"
#include "Graph_ActiveNode.h"

// 0x100e1580 ?GetTypeInfo@BasicAttribute@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::BasicAttribute::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e1590 ?GetTypeName@BasicAttribute@Graph@@UBEPBDXZ
char const * Graph::BasicAttribute::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e15a0 ?CanAddChild@BasicAttribute@Graph@@UAE_NPAVBasicNode@2@@Z
bool Graph::BasicAttribute::CanAddChild(Graph::BasicNode *)
{
    return false;
}

// 0x100e15b0 ?ScheduleRefresh@BasicAttribute@Graph@@QAEXXZ
void Graph::BasicAttribute::ScheduleRefresh()
{
    for (int i = 0; i < CountObservers(); i++) {
        Graph::ActiveNode *observer = StdBaseCast(GetObserver(i), (Graph::ActiveNode *)0);
        if (observer && !observer->m_ChildRefreshIsScheduled)
            observer->ScheduleChildRefresh();
    }
}

// 0x100e1610 ??0BasicAttribute@Graph@@IAE@XZ
Graph::BasicAttribute::BasicAttribute()
{
}

// 0x100e1650 ??1BasicAttribute@Graph@@MAE@XZ
Graph::BasicAttribute::~BasicAttribute()
{
}

// 0x100e1660 ?ParentAdded@BasicAttribute@Graph@@MAEXPAVBasicNode@2@@Z
void Graph::BasicAttribute::ParentAdded(Graph::BasicNode *parent)
{
    AddObserver(parent);
    BasicNode::ParentAdded(parent);
}

// 0x100e1680 ?ParentRemoved@BasicAttribute@Graph@@MAEXPAVBasicNode@2@@Z
void Graph::BasicAttribute::ParentRemoved(Graph::BasicNode *parent)
{
    RemoveObserver(FindObserver(parent));
    BasicNode::ParentRemoved(parent);
}


// Matched by this code:
// 0x100e1630 ??_GBasicAttribute@Graph@@MAEPAXI@Z

// the run-time type of Graph::BasicAttribute (values from the original's data section)
StdBase::TypeInfo const Graph::BasicAttribute::s_TypeInfo = { "Graph::BasicAttribute", &Graph::BasicNode::s_TypeInfo };
