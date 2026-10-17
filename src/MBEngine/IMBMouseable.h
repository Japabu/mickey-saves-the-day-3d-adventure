// IMBMouseable.h - something that reacts to rollover and clicks.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBMouseable.
#ifndef MBENGINE_IMBMOUSEABLE_H
#define MBENGINE_IMBMOUSEABLE_H

class MBPoint;

class IMBMouseable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMouseable; }
    virtual bool IsRolledOver(const MBPoint &) const = 0;
    virtual void EnterRollover(const MBPoint &) = 0;
    virtual void WithinRollover(const MBPoint &) = 0;
    virtual void LeaveRollover(void) = 0;
    virtual bool IsClicked(const MBPoint &, short) const = 0;
    virtual bool IsTrackingClick(void) const = 0;
    virtual void StartTrackingClick(const MBPoint &, short) = 0;
    virtual void TrackClick(const MBPoint &) = 0;
    virtual void StopTrackingClick(const MBPoint &, short) = 0;
    virtual bool IsActive(void) const = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual void SetPixelTest(bool) = 0;
    virtual bool GetPixelTest(void) = 0;
};

#endif
