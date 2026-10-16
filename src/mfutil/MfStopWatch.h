#pragma once
// MfStopWatch: millisecond timer on GetTickCount. Exported names only; all
// members are inline (MfSleeper inlines them).
#include <windows.h>
#include "MfTime.h"

class __declspec(dllexport) MfStopWatch {
public:
    MfStopWatch() { dwStart = 0; dwStop = 0; dwEnd = 0; }
    ~MfStopWatch() {}
    void Start() { dwStart = GetTickCount(); }
    void Start(long ms)
    {
        dwEnd = ms;
        dwStart = GetTickCount();
        dwEnd += dwStart;
    }
    void Start(const MfTime &t)
    {
        dwEnd = t.Seconds() * 1000 + t.Milli();
        dwStart = GetTickCount();
        dwEnd += dwStart;
    }
    void Stop() { dwStop = GetTickCount(); }
    void MeasureElapsed() { dwStop = GetTickCount(); }
    MfTime GetElapsed() { return MfTime((long)(dwStop - dwStart)); }
    MfTime GetStartTime() const { return MfTime((long)dwStart); }
    MfTime GetStopTime() const { return MfTime((long)dwStop); }
    int HasExpired()
    {
        dwStop = GetTickCount();
        return dwStop > dwEnd;
    }
    // Restarts with the same duration.
    void Reset()
    {
        dwEnd -= dwStart;
        dwStart = GetTickCount();
        dwEnd += dwStart;
    }
private:
    DWORD dwStart;
    DWORD dwStop;
    DWORD dwEnd;
};
