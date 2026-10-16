#pragma once
#include "Graph_PassiveTraverser.h"
namespace Graph { class ActiveNode; }
namespace Graph { class BasicNode; }

namespace Graph {
// Calls a function before and after descending into each matching node:
// every node, the nodes of one type, or one given node.
class CallbackTraverser : public Graph::PassiveTraverser {
public:
    typedef Graph::NodeTraverser::Response (__cdecl *Callback)(Graph::CallbackTraverser *, Graph::BasicNode *, void *);
    enum Match {
        MatchType = 0,
        MatchNode = 1,
        MatchAll = 2
    };
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    CallbackTraverser();
    virtual ~CallbackTraverser();
    void SetType(const StdBase::TypeInfo *type) { m_Type = type; }
    void SetPreCallback(Callback, void *);
    Graph::NodeTraverser::Response CallPreCallback(Graph::BasicNode *);
    Graph::NodeTraverser::Response CallPostCallback(Graph::BasicNode *);
    void Traverse(Graph::ActiveNode &);
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
private:
    Match m_Match;
    const StdBase::TypeInfo *m_Type;
    Graph::BasicNode *m_Node;
    void *m_pPreData;
    void *m_pPostData;
    Callback m_pPreCallback;
    Callback m_pPostCallback;
};
}
