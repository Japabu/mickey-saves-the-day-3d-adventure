#pragma once
#include "Graph_NodeTraverser.h"
namespace Graph { class BasicNode; }

namespace Graph {
// A traverser that lets each active node drive the descent into its children.
class PassiveTraverser : public Graph::NodeTraverser {
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
    virtual ~PassiveTraverser();
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
protected:
    PassiveTraverser(const Graph::PassiveTraverser &);
    PassiveTraverser();
};
}
