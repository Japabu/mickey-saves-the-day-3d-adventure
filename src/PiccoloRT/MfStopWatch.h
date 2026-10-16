#pragma once
#include <windows.h>

// mfutil: a millisecond timer on GetTickCount (inline members only are used here).
class __declspec(dllimport) MfStopWatch {
public:
    MfStopWatch() { dwStart = 0; dwStop = 0; dwEnd = 0; }
    ~MfStopWatch() {}
    void Start(long ms)
    {
        dwEnd = ms;
        dwStart = GetTickCount();
        dwEnd += dwStart;
    }
    int HasExpired()
    {
        dwStop = GetTickCount();
        return dwStop > dwEnd;
    }
private:
    DWORD dwStart;
    DWORD dwStop;
    DWORD dwEnd;
};
