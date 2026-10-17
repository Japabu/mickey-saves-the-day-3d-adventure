// IMBGameClock.h - the game clock (real and pausable game time).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBGameClock.
#ifndef MBENGINE_IMBGAMECLOCK_H
#define MBENGINE_IMBGAMECLOCK_H

class IMBGameClock : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBGameClock; }
    virtual unsigned long GetRealTime(void) = 0;
    virtual unsigned long GetGameTime(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Unpause(void) = 0;
    virtual void SetTime(unsigned long) = 0;
    virtual void AddTime(unsigned long) = 0;
    virtual void SetMSPerSecond(long) = 0;
};

#endif
