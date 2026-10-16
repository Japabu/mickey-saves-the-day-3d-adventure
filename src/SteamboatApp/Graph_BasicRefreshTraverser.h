#pragma once
#include "Graph_PassiveTraverser.h"
namespace Graph { class ActiveNode; }
namespace Graph { class BasicNode; }

namespace Graph {
// Calls Refresh on every active node that has a refresh scheduled.
class BasicRefreshTraverser : public Graph::PassiveTraverser {
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
    enum RefreshType {
        Full = 0,
        Partial = 1
    };
    BasicRefreshTraverser(const Graph::BasicRefreshTraverser &);
    BasicRefreshTraverser();
    virtual ~BasicRefreshTraverser();
    void SetRefreshType(RefreshType type) { m_RefreshType = type; }
    RefreshType GetRefreshType() { return m_RefreshType; }
    void Traverse(Graph::ActiveNode &, RefreshType);
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
private:
    RefreshType m_RefreshType;
};
}
