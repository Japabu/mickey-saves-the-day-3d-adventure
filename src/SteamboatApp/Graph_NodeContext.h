#pragma once
#include <vector>

namespace Graph {
// A value of type V per node of type T, indexed by the node's type handle.
template <class T, class V>
class NodeContext {
public:
    V &GetValue(T *node) { return m_Values.at(node->GetTypeHandle()); }
private:
    std::vector<V> m_Values;
};
}
