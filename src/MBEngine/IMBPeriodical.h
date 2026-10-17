// IMBPeriodical.h - something that gets idle time from the periodical manager.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBPeriodical.
#ifndef MBENGINE_IMBPERIODICAL_H
#define MBENGINE_IMBPERIODICAL_H

class IMBPeriodical : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodical; }
    virtual void Idle(void) = 0;
};

#endif
