// PORT override: VC6's std::vector<bool> is a plain vector of bools; libc++ packs it into bits and cannot hand out a bool &, so the values are kept in a struct.
#pragma once
#include <vector>

namespace Graph {
// A value of type V per node of type T, indexed by the node's type handle.
template <class T, class V>
class NodeContext {
public:
    V &GetValue(T *node) { return m_Values.at(node->GetTypeHandle()).value; }
private:
    struct Slot {
        V value;
    };
    std::vector<Slot> m_Values;
};
}
