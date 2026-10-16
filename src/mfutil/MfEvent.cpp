// MfEvent (mfutil.dll 0x10003a00-0x10003b50)
#include "MfEvent.h"

// Inline and compiler-generated members of MfEvent.h:
// 0x10003a00 ?Signal@MfEvent@@QAEHXZ
// 0x10003a10 ?Reset@MfEvent@@QAEHXZ
// 0x10003a20 ?Pulse@MfEvent@@QAEHXZ
// 0x10003a30 ?Wait@MfEvent@@QAEXK@Z
// 0x10003a60 ?IsWaiting@MfEvent@@QAEHXZ (folded with MfStringRef::GetLength)
// 0x10003a70 ??0MfEvent@@QAE@ABV0@@Z
// 0x10003a90 ??4MfEvent@@QAEAAV0@ABV0@@Z
// 0x10003ab0 ??_EMfEvent@@UAEPAXI@Z

// 0x10003b00 ??0MfEvent@@QAE@XZ
MfEvent::MfEvent()
    : waiting(0)
{
    handle = CreateEventA(0, 0, 0, 0);
}

// 0x10003b30 ??1MfEvent@@UAE@XZ
MfEvent::~MfEvent()
{
    if (handle) {
        CloseHandle(handle);
        handle = 0;
    }
}
