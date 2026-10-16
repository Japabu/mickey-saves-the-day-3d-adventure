#pragma once
// SBTimeBomb.h: a one-shot timer: armed with a delay, it broadcasts its channel/event/data once
// the delay has passed (SBTimeBomb.obj, declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class SBTimeBomb : public IMBPeriodical {
public:
    SBTimeBomb();
    ~SBTimeBomb();
protected:
    unsigned long _mRefCnt;                             // +0x0004
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    void SetBroadcastChannel(unsigned long channel);
    void SetBroadcastEvent(unsigned long event);
    void SetBroadcastData(void *data);
    void Arm(unsigned long timeInMilliseconds);
    bool IsArmed();
    void Defuse();
    virtual void Idle();
private:
    unsigned long mTime;                                // +0x0008
    unsigned long mStartTime;                           // +0x000c
    unsigned long mChannel;                             // +0x0010
    unsigned long mEvent;                               // +0x0014
    void *mData;                                        // +0x0018
    bool mArmed;                                        // +0x001c
};
