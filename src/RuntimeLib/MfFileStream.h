// MfFileStream (MfActorLib.dll): a stream on a Win32 file (see
// src/MfActorLib/MfFileStream.h).
#ifndef RUNTIMELIB_MFFILESTREAM_H
#define RUNTIMELIB_MFFILESTREAM_H
#include "MfStream.h"
class MfString;

// size 0x34
class __declspec(dllimport) MfFileStream : public MfStream {
public:
    MfFileStream(const MfString &name, MfFileMode mode, bool buffered);
    MfFileStream(void *handle, MfFileMode mode, bool buffered);
    ~MfFileStream();
    virtual MfStreamError ReadBytes(unsigned long n, void *p);
    virtual MfStreamError WriteBytes(unsigned long n, const void *p);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int n);

    void *file;                                             // 0x1c
    bool closeFile;                                         // 0x20
    char *buffer;                                           // 0x24
    unsigned long bufferFill;                               // 0x28
    unsigned long bufferPos;                                // 0x2c
    MfFileMode mode;                                        // 0x30
};

#endif
