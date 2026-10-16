// MfSystemTime (mfutil.dll 0x1000ff80-0x100101e0)
#include "MfSystemTime.h"

// 0x1000ff80 ??0MfSystemTime@@QAE@H@Z
MfSystemTime::MfSystemTime(int mode)
{
    iMode = mode;
}

// 0x1000ff90 ??0MfSystemTime@@QAE@ABV0@H@Z
MfSystemTime::MfSystemTime(const MfSystemTime &t, int mode)
{
    iMode = mode;
    *this = t;
}

// 0x1000ffb0 ??0MfSystemTime@@QAE@ABU_FILETIME@@H@Z
MfSystemTime::MfSystemTime(const FILETIME &ft, int mode)
{
    iMode = mode;
    *this = ft;
}

// 0x1000ffd0 ??0MfSystemTime@@QAE@ABUtm@@H@Z
MfSystemTime::MfSystemTime(const tm &t, int mode)
{
    iMode = mode;
    *this = t;
}

// 0x1000fff0 ??0MfSystemTime@@QAE@ABVMfTime@@H@Z
MfSystemTime::MfSystemTime(const MfTime &t, int mode)
{
    iMode = mode;
    *this = t;
}

// 0x10010010 ??BMfSystemTime@@QBE?AVMfTime@@XZ
MfSystemTime::operator MfTime() const
{
    tm t;
    Convert(&t);
    return MfTime(mktime(&t), 0);
}

// 0x10010050 ?Convert@MfSystemTime@@QBEXPAU_FILETIME@@@Z
void MfSystemTime::Convert(FILETIME *ft) const
{
    if (iMode == STM_LOCAL) {
        FILETIME local;
        SystemTimeToFileTime(this, &local);
        LocalFileTimeToFileTime(&local, ft);
    } else {
        SystemTimeToFileTime(this, ft);
    }
}

// 0x10010090 ?Convert@MfSystemTime@@QBEXPAUtm@@@Z
void MfSystemTime::Convert(tm *t) const
{
    t->tm_sec = wSecond;
    t->tm_min = wMinute;
    t->tm_hour = wHour;
    t->tm_mday = wDay;
    t->tm_mon = wMonth - 1;
    t->tm_year = wYear - 1900;
    t->tm_wday = wDayOfWeek;
    t->tm_yday = 0;
    t->tm_isdst = -1;
}

// 0x100100f0 ??4MfSystemTime@@QAEAAV0@ABV0@@Z
MfSystemTime &MfSystemTime::operator=(const MfSystemTime &t)
{
    SYSTEMTIME::operator=(t);
    iMode = t.iMode;
    return *this;
}

// 0x10010120 ??4MfSystemTime@@QAEAAV0@ABU_FILETIME@@@Z
MfSystemTime &MfSystemTime::operator=(const FILETIME &ft)
{
    if (iMode == STM_LOCAL) {
        FILETIME local;
        FileTimeToLocalFileTime(&ft, &local);
        FileTimeToSystemTime(&local, this);
    } else {
        FileTimeToSystemTime(&ft, this);
    }
    return *this;
}

// 0x10010170 ??4MfSystemTime@@QAEAAV0@ABUtm@@@Z
MfSystemTime &MfSystemTime::operator=(const tm &t)
{
    wYear = t.tm_year + 1900;
    wMonth = t.tm_mon + 1;
    wDayOfWeek = t.tm_wday;
    wDay = t.tm_mday;
    wHour = t.tm_hour;
    wMinute = t.tm_min;
    wSecond = t.tm_sec;
    wMilliseconds = 0;
    return *this;
}

// 0x100101c0 ??4MfSystemTime@@QAEAAV0@ABVMfTime@@@Z
MfSystemTime &MfSystemTime::operator=(const MfTime &t)
{
    return *this = *t.LocalTime();
}
