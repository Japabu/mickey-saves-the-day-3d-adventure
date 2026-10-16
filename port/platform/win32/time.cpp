// time.cpp - clocks, system time, FILETIME conversions and the time zone.
#include "internal.h"

#include <mach/mach_time.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace win32 {

static uint64_t now_ns()
{
    static mach_timebase_info_data_t tb;
    if (!tb.denom)
        mach_timebase_info(&tb);
    return mach_absolute_time() * tb.numer / tb.denom;
}

static const uint64_t s_start_ns = now_ns();

DWORD ticks() { return (DWORD)((now_ns() - s_start_ns) / 1000000); }

// 100-ns intervals between 1601-01-01 and 1970-01-01.
static const ULONGLONG EPOCH_DIFF = 116444736000000000ULL;

static ULONGLONG ft_value(const FILETIME *ft)
{
    return ((ULONGLONG)ft->dwHighDateTime << 32) | ft->dwLowDateTime;
}

static void set_ft(FILETIME *ft, ULONGLONG v)
{
    ft->dwLowDateTime = (DWORD)v;
    ft->dwHighDateTime = (DWORD)(v >> 32);
}

void unix_to_filetime(time_t sec, long nsec, FILETIME *ft)
{
    set_ft(ft, (ULONGLONG)sec * 10000000ULL + (ULONGLONG)(nsec / 100) + EPOCH_DIFF);
}

static void tm_to_systemtime(const struct tm *tm, int ms, SYSTEMTIME *st)
{
    st->wYear = (WORD)(tm->tm_year + 1900);
    st->wMonth = (WORD)(tm->tm_mon + 1);
    st->wDayOfWeek = (WORD)tm->tm_wday;
    st->wDay = (WORD)tm->tm_mday;
    st->wHour = (WORD)tm->tm_hour;
    st->wMinute = (WORD)tm->tm_min;
    st->wSecond = (WORD)tm->tm_sec;
    st->wMilliseconds = (WORD)ms;
}

}  // namespace win32

using namespace win32;

extern "C" {

DWORD WINAPI GetTickCount(void) { return ticks(); }
DWORD WINAPI timeGetTime(void) { return ticks(); }
MMRESULT WINAPI timeBeginPeriod(UINT) { return TIMERR_NOERROR; }
MMRESULT WINAPI timeEndPeriod(UINT) { return TIMERR_NOERROR; }

MMRESULT WINAPI timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc)
{
    if (!ptc || cbtc < sizeof(TIMECAPS))
        return MMSYSERR_INVALPARAM;
    ptc->wPeriodMin = 1;
    ptc->wPeriodMax = 1000000;
    return MMSYSERR_NOERROR;
}

BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER *f)
{
    f->QuadPart = 1000000;
    return TRUE;
}

BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *c)
{
    c->QuadPart = (LONGLONG)((now_ns() - s_start_ns) / 1000);
    return TRUE;
}

void WINAPI Sleep(DWORD ms)
{
    if (ms == 0) {
        sched_yield();
        return;
    }
    struct timespec ts = {(time_t)(ms / 1000), (long)(ms % 1000) * 1000000L};
    while (nanosleep(&ts, &ts) != 0) {
    }
}

void WINAPI GetSystemTime(LPSYSTEMTIME st)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm tm;
    gmtime_r(&tv.tv_sec, &tm);
    tm_to_systemtime(&tm, (int)(tv.tv_usec / 1000), st);
}

void WINAPI GetLocalTime(LPSYSTEMTIME st)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    tm_to_systemtime(&tm, (int)(tv.tv_usec / 1000), st);
}

BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *st, LPFILETIME ft)
{
    struct tm tm = {};
    tm.tm_year = st->wYear - 1900;
    tm.tm_mon = st->wMonth - 1;
    tm.tm_mday = st->wDay;
    tm.tm_hour = st->wHour;
    tm.tm_min = st->wMinute;
    tm.tm_sec = st->wSecond;
    time_t t = timegm(&tm);
    unix_to_filetime(t, (long)st->wMilliseconds * 1000000L, ft);
    return TRUE;
}

BOOL WINAPI FileTimeToSystemTime(const FILETIME *ft, LPSYSTEMTIME st)
{
    ULONGLONG v = ft_value(ft);
    if (v < EPOCH_DIFF) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    v -= EPOCH_DIFF;
    time_t t = (time_t)(v / 10000000ULL);
    struct tm tm;
    gmtime_r(&t, &tm);
    tm_to_systemtime(&tm, (int)((v % 10000000ULL) / 10000), st);
    return TRUE;
}

static LONGLONG utc_offset_100ns(time_t at)
{
    struct tm tm;
    localtime_r(&at, &tm);
    return (LONGLONG)tm.tm_gmtoff * 10000000LL;
}

BOOL WINAPI FileTimeToLocalFileTime(const FILETIME *ft, LPFILETIME local)
{
    ULONGLONG v = ft_value(ft);
    time_t t = (time_t)((v - EPOCH_DIFF) / 10000000ULL);
    set_ft(local, (ULONGLONG)((LONGLONG)v + utc_offset_100ns(t)));
    return TRUE;
}

BOOL WINAPI LocalFileTimeToFileTime(const FILETIME *local, LPFILETIME ft)
{
    ULONGLONG v = ft_value(local);
    time_t t = (time_t)((v - EPOCH_DIFF) / 10000000ULL);
    set_ft(ft, (ULONGLONG)((LONGLONG)v - utc_offset_100ns(t)));
    return TRUE;
}

LONG WINAPI CompareFileTime(const FILETIME *a, const FILETIME *b)
{
    ULONGLONG x = ft_value(a), y = ft_value(b);
    return x < y ? -1 : x > y ? 1 : 0;
}

DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION tzi)
{
    memset(tzi, 0, sizeof(*tzi));
    time_t now = time(0);
    struct tm tm;
    localtime_r(&now, &tm);
    // Bias is UTC minus local time, in minutes. The daylight rules are not reported.
    tzi->Bias = (LONG)(-tm.tm_gmtoff / 60) + (tm.tm_isdst > 0 ? 60 : 0);
    tzi->DaylightBias = -60;
    const char *std_name = tzname[0], *dst_name = tzname[1];
    for (int i = 0; i < 31 && std_name && std_name[i]; i++)
        tzi->StandardName[i] = (WCHAR)(unsigned char)std_name[i];
    for (int i = 0; i < 31 && dst_name && dst_name[i]; i++)
        tzi->DaylightName[i] = (WCHAR)(unsigned char)dst_name[i];
    return tm.tm_isdst > 0 ? TIME_ZONE_ID_DAYLIGHT : TIME_ZONE_ID_STANDARD;
}

}  // extern "C"
