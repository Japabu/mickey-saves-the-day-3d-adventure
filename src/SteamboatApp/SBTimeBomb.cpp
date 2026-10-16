#include "SBTimeBomb.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x1009b580 ??0SBTimeBomb@@QAE@XZ
SBTimeBomb::SBTimeBomb()
    : mTime(0), mStartTime(0), mChannel(0), mEvent(0), mData(0), mArmed(false)
{
}

// 0x1009b5b0 ?QueryInterface@SBTimeBomb@@UAEJABUMBGUID@@PAPAX@Z
long SBTimeBomb::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1009b650 ?AddRef@SBTimeBomb@@UAEKXZ
unsigned long SBTimeBomb::AddRef()
{
    return ++_mRefCnt;
}

// 0x1009b670 ?Release@SBTimeBomb@@UAEKXZ
unsigned long SBTimeBomb::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1009b6b0 ??1SBTimeBomb@@QAE@XZ
SBTimeBomb::~SBTimeBomb()
{
}

// 0x1009b6d0 ?SetBroadcastChannel@SBTimeBomb@@QAEXK@Z
void SBTimeBomb::SetBroadcastChannel(unsigned long channel)
{
    mChannel = channel;
}

// 0x1009b6f0 ?SetBroadcastEvent@SBTimeBomb@@QAEXK@Z
void SBTimeBomb::SetBroadcastEvent(unsigned long event)
{
    mEvent = event;
}

// 0x1009b710 ?SetBroadcastData@SBTimeBomb@@QAEXPAX@Z
void SBTimeBomb::SetBroadcastData(void *data)
{
    mData = data;
}

// 0x1009b730 ?Arm@SBTimeBomb@@QAEXK@Z
void SBTimeBomb::Arm(unsigned long timeInMilliseconds)
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    mStartTime = clock->GetRealTime();
    MBComPtr<IMBPeriodicalManager> periodicalManager(MBIID_IMBPeriodicalManager);
    if (periodicalManager)
        periodicalManager->AddPeriodical(this);
    mTime = timeInMilliseconds;
    mArmed = true;
}

// 0x1009b870 ?IsArmed@SBTimeBomb@@QAE_NXZ
bool SBTimeBomb::IsArmed()
{
    return mArmed;
}

// 0x1009b890 ?Defuse@SBTimeBomb@@QAEXXZ
void SBTimeBomb::Defuse()
{
    MBComPtr<IMBPeriodicalManager> periodicalManager(MBIID_IMBPeriodicalManager);
    if (periodicalManager)
        periodicalManager->RemovePeriodical(this);
    mArmed = false;
}

// 0x1009b960 ?Idle@SBTimeBomb@@UAEXXZ
void SBTimeBomb::Idle()
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    unsigned long now = clock->GetRealTime();
    if (mArmed && now - mStartTime > mTime) {
        MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
        eventManager->BroadcastEvent(mChannel, mEvent, mData);
        Defuse();
    }
}

// 0x1009bac0 _$E24
// 0x1009bb00 _$E20
// 0x1009bb20 _$E23
