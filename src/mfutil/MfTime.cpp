// MfTime (mfutil.dll 0x100104b0-0x10010610)
#include <iostream.h>
#include "MfTime_unit.h"

// 0x100104b0 ??HMfTime@@QBE?AV0@ABV0@@Z
MfTime MfTime::operator+(const MfTime &t) const
{
    long ms = ttMilli + t.ttMilli;
    return MfTime(ttSec + t.ttSec + ms / 1000, ms);
}

// 0x10010500 ??GMfTime@@QBE?AV0@ABV0@@Z
MfTime MfTime::operator-(const MfTime &t) const
{
    long s = ttSec;
    long ms = ttMilli - t.ttMilli;
    if (ms < 0) {
        ms += 1000;
        s--;
    }
    s = s - t.ttSec - ms / 1000;
    if (s < 0)
        s = -s;
    return MfTime(s, ms);
}

// 0x10010560 ??YMfTime@@QAEAAV0@ABV0@@Z
MfTime &MfTime::operator+=(const MfTime &t)
{
    *this = *this + t;
    return *this;
}

// 0x10010590 ??ZMfTime@@QAEAAV0@ABV0@@Z
MfTime &MfTime::operator-=(const MfTime &t)
{
    *this = *this - t;
    return *this;
}

// 0x100105c0 ??6@YAAAVostream@@AAV0@ABVMfTime@@@Z
ostream &operator<<(ostream &os, const MfTime &t)
{
    long milli = t.Milli();
    if (milli < 0)
        milli = -milli;
    os << t.Seconds() << '.';
    os.fill('0');
    os.width(3);
    os << milli;
    return os;
}
