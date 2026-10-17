// SquishStream.h - a SquishFile over an IMBStream.
#ifndef MBENGINE_SQUISHSTREAM_H
#define MBENGINE_SQUISHSTREAM_H

#include "SquishFile.h"
#include "CMBStreamSwapLSB.h"

#include "IMBStreamSeekValues.h"
#include "SquishFileOpenFlags.h"
#include "SquishFileSeekValues.h"

class SquishStream : public SquishFile {
public:
    SquishStream(const CMBStreamSwapLSB &stream) : mStream(stream) {} // PDB: by value; by value is not inlined (CMBFont)
    virtual ~SquishStream(void) {}
    virtual bool Open(const char *filename, SquishFileOpenFlags flags) { return true; }
    virtual bool Close(void) { return true; }
    virtual bool WriteBytes(const void *ptr, unsigned long nBytes) {
        if (!mStream)
            return false;
        return mStream->Write(ptr, nBytes);
    }
    virtual bool ReadBytes(void *ptr, unsigned long nBytes) {
        if (!mStream)
            return false;
        if (!mStream->Read(ptr, nBytes) && mStream->GetLastError() != 6)
            return false;
        return true;
    }
    virtual bool Seek(long offset, SquishFileSeekValues origin) {
        if (!mStream)
            return false;
        IMBStreamSeekValues streamOrigin;
        switch (origin) {
        case kSquishFileSeekStart:
            streamOrigin = kIMBStreamSeekStart;
            break;
        case kSquishFileSeekEnd:
            streamOrigin = kIMBStreamSeekEnd;
            break;
        case kSquishFileSeekCurrent:
            streamOrigin = kIMBStreamSeekCurrent;
            break;
        }
        return mStream->Seek(offset, streamOrigin);
    }
    CMBStreamSwapLSB mStream;
    unsigned char *mOrigin;
    unsigned char *mBuffer;
    unsigned int mBytesAvail;
    unsigned int mSize;
};

#endif
