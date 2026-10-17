// MfSleeper: sleeps that can be stopped by a stopwatch. Exported names only.
#ifndef MFUTIL_MFSLEEPER_H
#define MFUTIL_MFSLEEPER_H

#include "mfutil_api.h"

class MfTime;

class MFUTIL_API MfSleeper {
public:
    MfSleeper() {}
    static void Snooze(const MfTime &t);
    static void Snooze(long ms);
private:
    static void SystemSleep(long ms);
};

#endif
