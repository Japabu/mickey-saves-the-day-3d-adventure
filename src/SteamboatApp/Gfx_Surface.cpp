#include "Gfx_Surface.h"
#include "StdBase.h"

// 0x100fc9c0 ?GetTypeInfo@Surface@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Surface::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100fc9d0 ?GetTypeName@Surface@Gfx@@UBEPBDXZ
char const * Gfx::Surface::GetTypeName() const
{
    return s_TypeInfo.typeName;
}
