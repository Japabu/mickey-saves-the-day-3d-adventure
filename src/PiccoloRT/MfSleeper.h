#pragma once

// mfutil: sleeps that can be stopped by a stopwatch.
class __declspec(dllimport) MfSleeper {
public:
    static void __cdecl Snooze(long ms);
};
