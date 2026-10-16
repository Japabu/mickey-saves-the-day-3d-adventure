#pragma once
#include "Vec.h"
namespace Gfx { class Attribute; }

namespace Gfx {
// Stack of the active attributes of one kind during a traversal (no PDB type:
// a vftable with only the destructor, then a Vec at 4; size 0x1c).
class AttributeStack {
public:
    AttributeStack(int n);
    virtual ~AttributeStack();
    Gfx::Attribute const *GetTop();
    void Pop();
    void Push(Gfx::Attribute const *);
private:
    Vec<Gfx::Attribute const *> m_Stack;
};
}
