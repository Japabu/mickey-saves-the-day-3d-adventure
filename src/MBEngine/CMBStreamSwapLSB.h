// CMBStreamSwapLSB.h - reads little-endian values from an IMBStream.
#ifndef MBENGINE_CMBSTREAMSWAPLSB_H
#define MBENGINE_CMBSTREAMSWAPLSB_H

#include "IMBStream.h"

class CMBStreamSwapLSB {
public:
    CMBStreamSwapLSB(IMBStream *s) : mStream(s) {}
    operator IMBStream *(void) { return mStream; }
    IMBStream *operator->(void) { return mStream; }
    void Read(unsigned short &v) {
        unsigned long size = 2;
        mStream->Read(&v, size);
    }
    IMBStream *mStream;
};

#endif
