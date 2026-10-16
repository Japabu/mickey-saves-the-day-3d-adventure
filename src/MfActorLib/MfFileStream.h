#pragma once
// MfFileStream: a stream on a Win32 file, optionally buffered (2 KB buffers kept
// in a free list). A MF_HOOK_FILE_HANDLE hook can supply the file handle.
#include "MfStream.h"
class MfString;

// size 0x34
class __declspec(dllexport) MfFileStream : public MfStream {
public:
    MfFileStream(const MfString &name, MfFileMode mode, bool buffered);
    MfFileStream(void *handle, MfFileMode mode, bool buffered);
    ~MfFileStream();
    virtual MfStreamError ReadBytes(unsigned long n, void *p);
    virtual MfStreamError WriteBytes(unsigned long n, const void *p);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int n);
protected:
    void *file;                 // +0x1c HANDLE
    bool closeFile;             // +0x20 the stream opened the file
    char *buffer;               // +0x24 0 when unbuffered
    unsigned long bufferFill;   // +0x28 bytes in the buffer
    unsigned long bufferPos;    // +0x2c next byte to read
    MfFileMode mode;            // +0x30 0 = read
};
