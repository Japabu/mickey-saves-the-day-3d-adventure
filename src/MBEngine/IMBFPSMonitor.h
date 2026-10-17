// IMBFPSMonitor.h - measures and shows frames per second.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBFPSMonitor.
#ifndef MBENGINE_IMBFPSMONITOR_H
#define MBENGINE_IMBFPSMONITOR_H

class IMBFPSMonitor : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFPSMonitor; }
    virtual void SetEventLoopTime(unsigned long) = 0;
    virtual void SetDrawLoopTime(unsigned long) = 0;
    virtual unsigned long GetEventLoopFPS(void) const = 0;
    virtual unsigned long GetDrawLoopFPS(void) const = 0;
    virtual void Enable(bool) = 0;
};

#endif
