// MfSleeper (mfutil.dll 0x1000bba0-0x1000bc90), no PDB.
#include <windows.h>
#include "MfTime.h"
#include "MfStopWatch.h"
#include "MfSleeper.h"

// 0x1000bba0 ??0MfSleeper@@QAE@XZ (inline, header)
// 0x1000bbb0 ??4MfSleeper@@QAEAAV0@ABV0@@Z (implicit)

// 0x1000bbc0 ?Snooze@MfSleeper@@SAXABVMfTime@@@Z
void MfSleeper::Snooze(const MfTime &t)
{
    MfStopWatch sw;
    sw.Start(t);
    for (long i = 0; i < t.Seconds(); i++) {
        if (sw.HasExpired())
            return;
        SystemSleep(1000);
    }
    if (sw.HasExpired())
        return;
    SystemSleep(t.Milli());
}

// 0x1000bc70 ?Snooze@MfSleeper@@SAXJ@Z
void MfSleeper::Snooze(long ms)
{
    SystemSleep(ms);
}

// 0x1000bc80 ?SystemSleep@MfSleeper@@CAXJ@Z
void MfSleeper::SystemSleep(long ms)
{
    Sleep(ms);
}

