#pragma once
#include "StdBase.h"
#include "Graph_Path.h"
namespace Graph { class BasicNode; }

namespace Graph {
// Walks a node graph, keeping the path from the root to the node visited.
class NodeTraverser : public StdBase {
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
    enum Response {
        Terminate = 0,
        Continue = 1,
        Prune = 2
    };
    int GetNodeCount() const { return m_NodeCount; }
    const Graph::Path &GetTraversalPath() { return m_TraversalPath; }
    void SetFrameTime(unsigned long time) { m_FrameTime = time; }
    unsigned long GetFrameTime() { return m_FrameTime; }
    void SetElapsedTime(unsigned long time) { m_ElapsedTime = time; }
    unsigned long GetElapsedTime() { return m_ElapsedTime; }
    virtual ~NodeTraverser();
    void Traverse(Graph::BasicNode &);
    void PushNode(Graph::BasicNode &node) { m_TraversalPath.Push(&node); }
    void PopNode() { m_TraversalPath.Pop(); }
    virtual Response Visit(Graph::BasicNode &);
protected:
    NodeTraverser(const Graph::NodeTraverser &);
    NodeTraverser();
private:
    unsigned int m_NodeCount;
    Graph::Path m_TraversalPath;
    unsigned long m_FrameTime;
    unsigned long m_ElapsedTime;
};
}
