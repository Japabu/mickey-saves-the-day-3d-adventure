// MBEngine.dll .\MBEngineRelease\CMBWinClock.obj
#include "CMBWinClock.h"

// 0x1003f5e0 ?MakeCMBWinClock@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinClock(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CMBWinClock);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1003f650 ??0CMBWinClock@@QAE@XZ
CMBWinClock::CMBWinClock(void)
{
    _mRefCnt = 0;
    mLastTime = 0;
    mCurrentGameTime = 0;
    mCurrentRealTime = 0;
    mMSperSec = 1000;
    mPauseCount = 0;
}

// 0x1003f680 ?QueryInterface@CMBWinClock@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinClock::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBGameClock)
        p = (IMBGameClock *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1003f750 ?AddRef@CMBWinClock@@UAEKXZ
unsigned long CMBWinClock::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1003f760 ?Release@CMBWinClock@@UAEKXZ
unsigned long CMBWinClock::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1003f790 ??1CMBWinClock@@QAE@XZ
CMBWinClock::~CMBWinClock(void)
{
}

// 0x1003f7a0 ?GetGameTime@CMBWinClock@@UAEKXZ
unsigned long CMBWinClock::GetGameTime(void)
{
    UpdateTime();
    return mCurrentGameTime;
}

// 0x1003f7b0 ?GetRealTime@CMBWinClock@@UAEKXZ
unsigned long CMBWinClock::GetRealTime(void)
{
    UpdateTime();
    return mCurrentRealTime;
}

// 0x1003f7c0 ?Pause@CMBWinClock@@UAEJXZ
long CMBWinClock::Pause(void)
{
    return ++mPauseCount;
}

// 0x1003f7d0 ?Unpause@CMBWinClock@@UAEJXZ
long CMBWinClock::Unpause(void)
{
    if (mPauseCount > 0)
        mPauseCount--;
    return mPauseCount;
}

// 0x1003f7e0 ?AddTime@CMBWinClock@@UAEXK@Z
void CMBWinClock::AddTime(unsigned long addtime)
{
    mCurrentGameTime += addtime;
}

// 0x1003f7f0 ?SetTime@CMBWinClock@@UAEXK@Z
void CMBWinClock::SetTime(unsigned long newtime)
{
    mCurrentGameTime = newtime;
}

// 0x1003f800 ?SetMSPerSecond@CMBWinClock@@UAEXJ@Z
void CMBWinClock::SetMSPerSecond(long mspersec)
{
    mMSperSec = mspersec;
}

// 0x1003f810 ?UpdateTime@CMBWinClock@@IAEXXZ
void CMBWinClock::UpdateTime(void)
{
    unsigned long now = timeGetTime();
    unsigned long elapsed = now - mLastTime;
    mCurrentRealTime = now;
    mLastTime = now;
    if (mPauseCount == 0 && mMSperSec != 0 && elapsed != now) {
        unsigned long scaled = mMSperSec * elapsed;
        mCurrentGameTime += scaled / 1000;
        mLastTime = now - (scaled % 1000) / mMSperSec;
    }
}
