// MfTimer (mfutil.dll 0x10010610-0x10010650)
#include "MfTimer_unit.h"

// 0x10010610 ??0MfTimer@@QAE@XZ
MfTimer::MfTimer()
{
    reset();
}

// 0x10010620 ??4MfTimer@@QAEAAV0@ABV0@@Z (implicit, emitted for the exported class)

// 0x10010630 ?reset@MfTimer@@QAEXXZ
void MfTimer::reset()
{
    m_start = GetTickCount();
}

// 0x10010640 ?time@MfTimer@@QAEKXZ
DWORD MfTimer::time()
{
    return GetTickCount() - m_start;
}
