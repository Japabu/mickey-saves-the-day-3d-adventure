#pragma once
#include "Graph_BasicNode.h"
#include "Graph_NodeTraverser.h"
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class BasicAttribute; }

namespace Graph {
// A node that can be refreshed: changes schedule a refresh of the node and a
// child refresh of its ancestors, which a BasicRefreshTraverser then carries out.
class ActiveNode : public Graph::BasicNode {
    friend class Graph::BasicAttribute;
    friend class Graph::BasicRefreshTraverser;
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const;
    virtual ~ActiveNode();
    void RefreshImmediately(bool);
    void SetNotifyCallback(void (__cdecl *)(void *), void *);
    virtual Graph::NodeTraverser::Response Traverse(Graph::PassiveTraverser &);
    void ScheduleRefresh(bool);
protected:
    ActiveNode(const Graph::ActiveNode &);
    ActiveNode();
    virtual void Refresh(Graph::BasicRefreshTraverser &);
private:
    bool RefreshIsScheduled();
    bool ChildRefreshIsScheduled();
    void ScheduleChildRefresh();
    bool m_RefreshImmediately;
    bool m_RefreshIsScheduled;
    bool m_ChildRefreshIsScheduled;
    void (__cdecl *m_pNotifyCallback)(void *);
    void *m_pNotifyData;
};
}
