// AttributeStack.obj (Graphics.lib): the attribute stacks of DisplayQuality,
// ShadowAttribute and VertexClusterAttribute.
#include "Gfx_AttributeStack.h"

namespace Gfx {

// 0x100fb490 ??0AttributeStack@Gfx@@QAE@H@Z
AttributeStack::AttributeStack(int n) : m_Stack(n)
{
}

// 0x100fb530 ??_GAttributeStack@Gfx@@UAEPAXI@Z

// 0x100fb550 ??1AttributeStack@Gfx@@UAE@XZ
AttributeStack::~AttributeStack()
{
}

// 0x100fb5b0 ?GetTop@AttributeStack@Gfx@@QAEPBVAttribute@2@XZ
Attribute const *AttributeStack::GetTop()
{
    Attribute const *top = 0;
    Attribute const **e = m_Stack;
    if (m_Stack.length())
        top = e[m_Stack.length() - 1];
    return top;
}

// TODO 0x100fb5d0 ?Push@AttributeStack@Gfx@@QAEXPBVAttribute@2@@Z: 92% similar: the inlined Vec::append; the original forms len + 1 with lea and picks the larger size into eax, VC6 keeps it in edi (grown < n tried in Vec.h: 84%)
void AttributeStack::Push(Attribute const *attribute)
{
    m_Stack.append(attribute);
}

// 0x100fb660 ?Pop@AttributeStack@Gfx@@QAEXXZ
void AttributeStack::Pop()
{
    if (m_Stack.length() > 0)
        m_Stack.setLength(m_Stack.length() - 1);
}

}

// Matched by this code (template and inline instances):
// 0x100fb700 ??_G?$Vec@PBVAttribute@Gfx@@@@UAEPAXI@Z
// 0x100fb720 ??1?$Vec@PBVAttribute@Gfx@@@@UAE@XZ
