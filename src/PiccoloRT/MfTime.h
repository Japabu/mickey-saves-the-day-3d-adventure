#pragma once
#include <sys/timeb.h>

// mfutil.dll: seconds since 1970 and milliseconds (the inline part PiccoloRT uses).
class __declspec(dllimport) MfTime {
public:
    MfTime() { Update(); }
    MfTime(long s, long ms) { ttSec = s; ttMilli = ms % 1000; }
    ~MfTime() {}
    long Seconds() const { return ttSec; }
    long Milli() const { return ttMilli; }
    void Update()
    {
        struct timeb tb;
        ftime(&tb);
        ttSec = tb.time;
        ttMilli = tb.millitm;
    }
private:
    long ttSec;
    long ttMilli;
};
