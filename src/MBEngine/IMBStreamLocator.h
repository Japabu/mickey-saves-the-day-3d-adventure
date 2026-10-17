// IMBStreamLocator.h - finds the stream of a sound or other resource.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBStreamLocator.
#ifndef MBENGINE_IMBSTREAMLOCATOR_H
#define MBENGINE_IMBSTREAMLOCATOR_H

class IMBStream;

class IMBStreamLocator : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStreamLocator; }
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) = 0;
};

#endif
