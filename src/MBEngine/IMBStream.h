// IMBStream.h - a byte stream (file, memory, resource).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBStream.
#ifndef MBENGINE_IMBSTREAM_H
#define MBENGINE_IMBSTREAM_H

#include "IMBStreamError.h"
#include "IMBStreamSeekValues.h"

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStream; }
    virtual bool Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual bool Flush(void) = 0;
    virtual bool SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

#endif
