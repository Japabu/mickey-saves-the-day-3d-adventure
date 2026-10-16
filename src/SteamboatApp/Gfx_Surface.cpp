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

// the run-time type of Gfx::Surface (values from the original's data section)
StdBase::TypeInfo const Gfx::Surface::s_TypeInfo = { "Gfx::Surface", &StdBase::s_TypeInfo };
