#include "StdBase.h"

// 0x10112750 ?GetTypeInfo@StdBase@@UBEPBUTypeInfo@1@XZ
StdBase::TypeInfo const * StdBase::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10112760 ?GetTypeName@StdBase@@UBEPBDXZ
char const * StdBase::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10112770 ?Ref@StdBase@@QAEXXZ
void StdBase::Ref()
{
    m_RefCount++;
}

// 0x10112780 ?Unref@StdBase@@QAEXXZ
void StdBase::Unref()
{
    if (--m_RefCount <= 0)
        delete this;
}

// 0x101127a0 ?IsOfExactType@StdBase@@QBE_NPBUTypeInfo@1@@Z
bool StdBase::IsOfExactType(TypeInfo const *typeInfo) const
{
    return typeInfo == GetTypeInfo();
}

// 0x101127c0 ?IsOfType@StdBase@@QBE_NPBUTypeInfo@1@@Z
bool StdBase::IsOfType(TypeInfo const *typeInfo) const
{
    TypeInfo const *t = GetTypeInfo();
    while (t != typeInfo) {
        t = t->baseTypeInfo;
        if (t == 0)
            return false;
    }
    return true;
}

// the run-time type of StdBase (values from the original's data section)
StdBase::TypeInfo const StdBase::s_TypeInfo = { 0, 0 };
