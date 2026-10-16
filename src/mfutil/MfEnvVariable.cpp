// MfEnvVariable (mfutil.dll 0x10003740-0x10003a00)
#include <stdlib.h>
#include "MfEnvVariable.h"

// 0x10003740 ??0MfEnvVariable@@QAE@ABVMfString@@@Z (inline, header)
// 0x100037a0 ??1MfEnvVariable@@UAE@XZ (inline, header)
// 0x100037f0 ?Set@MfEnvVariable@@QAEXABVMfString@@@Z (inline, header)

// 0x10003810 ??BMfEnvVariable@@QAE?AVMfString@@XZ (identical to Get, folded by the linker)
MfEnvVariable::operator MfString()
{
    return m_value;
}

// EQUIV 0x10003810 ?Get@MfEnvVariable@@QBE?AVMfString@@XZ: proven (renaming-equivalent); bytes differ: (100% similar) identical code, folded by the linker into operator MfString (claimed above)
MfString MfEnvVariable::Get() const
{
    return m_value;
}

// 0x10003830 ?SetName@MfEnvVariable@@QAEXABVMfString@@@Z
void MfEnvVariable::SetName(const MfString &name)
{
    m_name = name;
    ReadVariable();
}

// 0x10003850 ?GetName@MfEnvVariable@@QBE?AVMfString@@XZ
MfString MfEnvVariable::GetName() const
{
    return m_name;
}

// 0x10003870 ?SetValueDirect@MfEnvVariable@@IAEXABVMfString@@@Z (inline, header)
// 0x10003880 ??0MfEnvVariable@@QAE@ABV0@@Z (implicit copy constructor)
// 0x100038e0 ??4MfEnvVariable@@QAEAAV0@ABV0@@Z (implicit assignment; folded with MfPathSearch's)
// 0x10003910 ??_EMfEnvVariable@@UAEPAXI@Z (vector deleting destructor)

// 0x10003960 ?ReadVariable@MfEnvVariable@@MAEXXZ
void MfEnvVariable::ReadVariable()
{
    m_value = getenv(m_name);
}

// 0x10003980 ?WriteVariable@MfEnvVariable@@MAEXXZ
void MfEnvVariable::WriteVariable()
{
    MfString s(m_name);
    s << "=";
    s << m_value;
    putenv(s);  // the original imports MSVCRT _putenv via the oldnames alias putenv
}


// Matched by this code:
// 0x10010f20 ??BMfEnvVariable@@QAE?AVMfString@@XZ