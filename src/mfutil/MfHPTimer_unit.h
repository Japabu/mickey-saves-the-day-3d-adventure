// MfHPTimer: high-resolution timer on QueryPerformanceCounter (mfutil.dll).
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names. MfTime is duplicated (only what this unit needs).
#ifndef MFUTIL_MFHPTIMER_H
#define MFUTIL_MFHPTIMER_H

#include <windows.h>

#define MFUTIL_API __declspec(dllexport)

// size 8
class MFUTIL_API MfTime {
public:
    // inlined into MfHPTimer::GetElapsedTimeAsMfTime (out-of-line copy at 0x10002ff0)
    MfTime(long seconds, long milli) { m_seconds = seconds; m_milli = milli % 1000; }
    MfTime(const MfTime &t) { m_seconds = t.m_seconds; m_milli = t.m_milli; }
    ~MfTime() {}
    long m_seconds; // +0
    long m_milli;   // +4
};

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
