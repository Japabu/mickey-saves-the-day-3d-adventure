// IMBResourceStreamLocator.h - a stream locator for a resource (type and ID).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBResourceStreamLocator.
#ifndef MBENGINE_IMBRESOURCESTREAMLOCATOR_H
#define MBENGINE_IMBRESOURCESTREAMLOCATOR_H

#include "IMBStreamLocator.h"

class IMBResourceStreamLocator : public IMBStreamLocator {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceStreamLocator; }
    virtual long SetResource(unsigned long, unsigned long) = 0;
};

#endif
