// SquishFile.h - the file a Squish image streams from.
#ifndef MBENGINE_SQUISHFILE_H
#define MBENGINE_SQUISHFILE_H

#include "SquishFileOpenFlags.h"
#include "SquishFileSeekValues.h"

class SquishFile {
public:
    SquishFile(void);
    virtual ~SquishFile(void);
    virtual bool Open(const char *, SquishFileOpenFlags);
    virtual bool Close(void);
    virtual bool WriteBytes(const void *, unsigned long) = 0;
    virtual bool ReadBytes(void *, unsigned long) = 0;
    virtual bool Seek(long, SquishFileSeekValues);
};

#endif
