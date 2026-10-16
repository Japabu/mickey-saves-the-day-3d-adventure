#pragma once
#include "Graph_BasicNode.h"

namespace Graph {
// A node that hangs off other nodes as their attribute; it observes its
// parents so that changes to it schedule their refresh.
class BasicAttribute : public Graph::BasicNode {
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
    virtual bool CanAddChild(Graph::BasicNode *);
    void ScheduleRefresh();
protected:
    BasicAttribute(const Graph::BasicAttribute &);
    BasicAttribute();
    virtual ~BasicAttribute();
    virtual void ParentAdded(Graph::BasicNode *);
    virtual void ParentRemoved(Graph::BasicNode *);
};
}
