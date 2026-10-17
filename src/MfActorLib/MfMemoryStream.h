#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfMemoryStream: a stream in a memory buffer, its own (growing) or the caller's.
#include "MfStream.h"

// size 0x2c
class MFACTOR_API MfMemoryStream : public MfStream {
public:
    MfMemoryStream();
    MfMemoryStream(void *buffer, int size);
    ~MfMemoryStream();
    virtual MfStreamError ReadBytes(unsigned long n, void *p);
    virtual MfStreamError WriteBytes(unsigned long n, const void *p);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int n);
    // The buffer and the number of bytes written.
    char *GetBuffer(int &size);
protected:
    char *buffer;           // +0x1c
    unsigned long size;     // +0x20
    unsigned long position; // +0x24
    bool ownsBuffer;        // +0x28 (grows on demand)
};
