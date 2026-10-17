// IMBPeriodicalManager.h - hands idle time to the registered periodicals.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBPeriodicalManager.
#ifndef MBENGINE_IMBPERIODICALMANAGER_H
#define MBENGINE_IMBPERIODICALMANAGER_H

class IMBPeriodical;

class IMBPeriodicalManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodicalManager; }
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

#endif
