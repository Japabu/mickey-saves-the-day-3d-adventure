// IMBFileStreamLocator.h - a stream locator for a file.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBFileStreamLocator.
#ifndef MBENGINE_IMBFILESTREAMLOCATOR_H
#define MBENGINE_IMBFILESTREAMLOCATOR_H

#include "IMBStreamLocator.h"

class IMBFileSpec;

class IMBFileStreamLocator : public IMBStreamLocator {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileStreamLocator; }
    virtual long SetFile(IMBFileSpec *) = 0;
};

#endif
