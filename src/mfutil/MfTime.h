// MfTime: seconds since 1970 and milliseconds. Declaration from the SteamboatApp
// PDB; mfutil defines everything but the arithmetic inline.
#ifndef MFUTIL_MFTIME_H
#define MFUTIL_MFTIME_H

#include "mfutil_api.h"
#include <time.h>
#include <sys/timeb.h>
class ostream;

class MFUTIL_API MfTime {
public:
    MfTime() { Update(); }
    MfTime(long s, long ms) { ttSec = s; ttMilli = ms % 1000; }
    MfTime(long ms) { ttSec = ms / 1000; ttMilli = ms % 1000; }
    MfTime(const MfTime &t) { ttSec = t.ttSec; ttMilli = t.ttMilli; }
    ~MfTime() {}
    MfTime &operator=(long ms) { ttSec = ms / 1000; ttMilli = ms % 1000; return *this; }
    MfTime &operator=(const MfTime &t) { ttSec = t.ttSec; ttMilli = t.ttMilli; return *this; }
    MfTime operator+(const MfTime &) const;
    MfTime operator-(const MfTime &) const;
    MfTime &operator+=(const MfTime &);
    MfTime &operator-=(const MfTime &);
    int operator==(const MfTime &t) const
    {
        return ttSec == t.ttSec && ttMilli == t.ttMilli;
    }
    int operator!=(const MfTime &t) const
    {
        return ttSec != t.ttSec || ttMilli != t.ttMilli;
    }
    int operator<(const MfTime &t) const
    {
        return ttSec < t.ttSec || (ttSec == t.ttSec && ttMilli < t.ttMilli);
    }
    int operator>(const MfTime &t) const
    {
        return ttSec > t.ttSec || (ttSec == t.ttSec && ttMilli > t.ttMilli);
    }
    int operator<=(const MfTime &t) const
    {
        return ttSec < t.ttSec || (ttSec == t.ttSec && ttMilli <= t.ttMilli);
    }
    int operator>=(const MfTime &t) const
    {
        return ttSec > t.ttSec || (ttSec == t.ttSec && ttMilli >= t.ttMilli);
    }
    long Seconds() const { return ttSec; }
    long Milli() const { return ttMilli; }
    void Update()
    {
        struct timeb tb;
        ftime(&tb);
        ttSec = tb.time;
        ttMilli = tb.millitm;
    }
    struct tm *LocalTime() const { return localtime(&ttSec); }
    struct tm *GreenwichMeanTime() const { return gmtime(&ttSec); }

private:
    long ttSec;
    long ttMilli;
};

MFUTIL_API ostream &operator<<(ostream &os, const MfTime &t);

#endif
