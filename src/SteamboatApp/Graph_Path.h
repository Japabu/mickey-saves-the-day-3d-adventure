#pragma once
#include "Vec.h"
namespace Graph { class BasicNode; }

namespace Graph {
// The chain of nodes from the root of a traversal down to the current node.
class Path {
public:
    Path(const Graph::Path &);
    Path();
    virtual ~Path();
    int GetLength() const { return m_NodeList.length(); }
    Graph::BasicNode *GetTop() const;
    Graph::BasicNode *GetNode(int) const;
    int FindNode(Graph::BasicNode *) const;
    Graph::BasicNode *operator[](int) const;
    void Push(Graph::BasicNode *node) { m_NodeList.append(node); }
    void Pop() { m_NodeList.remove(m_NodeList.length() - 1, 1); }
    void Clear() { m_NodeList.setLength(0); }
    Graph::Path &operator=(const Graph::Path &);
    bool operator==(const Graph::Path &) const;
protected:
    Vec<Graph::BasicNode *> m_NodeList;
};
}
