// IMBMouseMonitor.h - tracks the mouse over the registered mouseables.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBMouseMonitor.
#ifndef MBENGINE_IMBMOUSEMONITOR_H
#define MBENGINE_IMBMOUSEMONITOR_H

class IMBMouseable;

class IMBMouseMonitor : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMouseMonitor; }
    virtual void AddMouseable(IMBMouseable *) = 0;
    virtual void RemoveMouseable(IMBMouseable *) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual bool IsActive(void) = 0;
};

#endif
