// MfHPTimer: high-resolution timer on QueryPerformanceCounter (mfutil.dll).
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names.
#ifndef MFUTIL_MFHPTIMER_H
#define MFUTIL_MFHPTIMER_H

#include <windows.h>
#include "mfutil_api.h"
#include "MfTime.h"

// size 0x10
class MFUTIL_API MfHPTimer {
public:
    MfHPTimer();
    ~MfHPTimer() {}
    void Start();
    void Stop();
    void AccumTimer(__int64 &acc);
    static __int64 GetFrequencyAsLong();
    static int Initialize();
    MfTime GetElapsedTimeAsMfTime();
    double GetElapsedTimeInMs();
    static double GetFrequency();

    LARGE_INTEGER m_start; // +0
    LARGE_INTEGER m_stop;  // +8

private:
    static LARGE_INTEGER liFrequency;
    static int bInitialized;
};

#endif
