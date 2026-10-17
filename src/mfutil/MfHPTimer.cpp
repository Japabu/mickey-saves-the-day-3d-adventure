// MfHPTimer (mfutil.dll 0x10004240-0x10004410)
#include "MfHPTimer.h"

LARGE_INTEGER MfHPTimer::liFrequency;
int MfHPTimer::bInitialized;

#define LI_TO_DOUBLE(li) ((double)(li).LowPart - (double)(li).HighPart * -4294967296.0)

// 0x10004240 ??0MfHPTimer@@QAE@XZ
MfHPTimer::MfHPTimer()
{
    Initialize();
}

// 0x10004250 ?Start@MfHPTimer@@QAEXXZ
void MfHPTimer::Start()
{
    QueryPerformanceCounter(&m_start);
}

// 0x10004260 ?Stop@MfHPTimer@@QAEXXZ
void MfHPTimer::Stop()
{
    QueryPerformanceCounter(&m_stop);
}

// 0x10004270 ?AccumTimer@MfHPTimer@@QAEXAA_J@Z
void MfHPTimer::AccumTimer(__int64 &acc)
{
    acc += m_stop.QuadPart - m_start.QuadPart;
}

// 0x100042a0 ?GetFrequencyAsLong@MfHPTimer@@SA_JXZ
__int64 MfHPTimer::GetFrequencyAsLong()
{
    return liFrequency.QuadPart;
}

// 0x100042b0 ??4MfHPTimer@@QAEAAV0@ABV0@@Z (implicit)

// 0x100042e0 ?Initialize@MfHPTimer@@SAHXZ
int MfHPTimer::Initialize()
{
    if (!bInitialized) {
        bInitialized = 1;
        return QueryPerformanceFrequency(&liFrequency);
    }
    return 1;
}

// 0x10004310 ?GetElapsedTimeAsMfTime@MfHPTimer@@QAE?AVMfTime@@XZ
MfTime MfHPTimer::GetElapsedTimeAsMfTime()
{
    // NONMATCHING: the original first stores 0 into sec's stack slot
    // (mov [esp+4], 0 before the call); not reproduced yet.
    double ms = GetElapsedTimeInMs();
    long sec = (long)(ms * 0.001);
    return MfTime(sec, (long)(ms - sec * 1000.0));
}

// 0x10004360 ?GetElapsedTimeInMs@MfHPTimer@@QAENXZ
double MfHPTimer::GetElapsedTimeInMs()
{
    long high = m_stop.HighPart - m_start.HighPart;
    DWORD low;
    if (m_stop.LowPart < m_start.LowPart) {
        low = m_stop.LowPart - m_start.LowPart;
        high--;
    } else {
        low = m_stop.LowPart - m_start.LowPart;
    }
    return ((double)low - (double)high * -4294967296.0) / LI_TO_DOUBLE(liFrequency) * 1000.0;
}

// 0x100043d0 ?GetFrequency@MfHPTimer@@SANXZ
double MfHPTimer::GetFrequency()
{
    if (!bInitialized)
        Initialize();
    return LI_TO_DOUBLE(liFrequency);
}
