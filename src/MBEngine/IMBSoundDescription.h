// IMBSoundDescription.h - what to play: stream locator, channel, loops, rate.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBSoundDescription.
#ifndef MBENGINE_IMBSOUNDDESCRIPTION_H
#define MBENGINE_IMBSOUNDDESCRIPTION_H

class IMBStreamLocator;

class IMBSoundDescription : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundDescription; }
    virtual void SetStreamLocator(IMBStreamLocator *) = 0;
    virtual void GetStreamLocator(IMBStreamLocator **) = 0;
    virtual unsigned long GetSoundID(void) const = 0;
    virtual void SetChannel(long) = 0;
    virtual long GetChannel(void) const = 0;
    virtual void SetLoopCount(long) = 0;
    virtual long GetLoopCount(void) const = 0;
    virtual void SetStreaming(bool) = 0;
    virtual bool IsStreaming(void) const = 0;
    virtual void SetRateMultiplier(float) = 0;
    virtual float GetRateMultiplier(void) const = 0;
};

#endif
