#include "StateScriptFlags.h"

// 0x1007dd40 ?SetFlag@StateScriptFlags@@SAXH@Z
void __cdecl StateScriptFlags::SetFlag(int flag)
{
    iFlags |= flag;
}

// 0x1007dd60 ?ClearFlag@StateScriptFlags@@SAXH@Z
void __cdecl StateScriptFlags::ClearFlag(int flag)
{
    iFlags &= ~flag;
}

// 0x1007dd80 ?GetFlags@StateScriptFlags@@SAHXZ
int __cdecl StateScriptFlags::GetFlags()
{
    return iFlags;
}

// 0x1007dd90 ??4StateScriptFlags@@QAEAAV0@ABV0@@Z
StateScriptFlags & StateScriptFlags::operator=(StateScriptFlags const & rhs)
{
    field_0 = rhs.field_0;
    return *this;
}
