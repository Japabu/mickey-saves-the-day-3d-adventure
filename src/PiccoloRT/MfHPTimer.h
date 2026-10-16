#pragma once
#include <windows.h>

// mfutil.dll: high-resolution timer on QueryPerformanceCounter.
class __declspec(dllimport) MfHPTimer {
public:
    MfHPTimer() { Initialize(); }
    ~MfHPTimer() {}
    void Start() { QueryPerformanceCounter(&m_start); }
    void Stop() { QueryPerformanceCounter(&m_stop); }
    static int Initialize();
    double GetElapsedTimeInMs();

    LARGE_INTEGER m_start;          // +0x00
    LARGE_INTEGER m_stop;           // +0x08
};
