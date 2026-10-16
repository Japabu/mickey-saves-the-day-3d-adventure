#pragma once
// MfSleeper: sleeps that can be stopped by a stopwatch. Exported names only.
class MfTime;

class __declspec(dllexport) MfSleeper {
public:
    MfSleeper() {}
    static void Snooze(const MfTime &t);
    static void Snooze(long ms);
private:
    static void SystemSleep(long ms);
};
