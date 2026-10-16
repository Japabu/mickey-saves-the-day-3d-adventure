#include <windows.h>
#include "MfLock.h"

// 0x10005c60 ??0MfLock@@QAE@XZ
MfLock::MfLock()
{
    lwLockWord = 0;
    InterlockedExchange(&lwLockWord, 0);
}

// 0x10005ce0 ?Lock@MfLock@@QAEHJ@Z
// Waits in steps of 10 ms; a timeout of -1 waits for ever.
int MfLock::Lock(long timeout)
{
    long remaining = timeout;
    while (InterlockedExchange(&lwLockWord, 1) != 0) {
        if (timeout != -1 && remaining <= 0)
            return 0;
        Sleep(10);
        remaining -= 10;
    }
    return 1;
}

// 0x10005d40 ?Unlock@MfLock@@QAEXXZ
void MfLock::Unlock()
{
    InterlockedExchange(&lwLockWord, 0);
}

// 0x10005d50 ?Test@MfLock@@QAEHXZ
int MfLock::Test()
{
    return InterlockedExchange(&lwLockWord, lwLockWord) == 0;
}
