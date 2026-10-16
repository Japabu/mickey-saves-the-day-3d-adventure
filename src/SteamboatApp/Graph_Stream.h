#pragma once

namespace Graph {
class Stream {
public:
    virtual ~Stream();
    virtual bool IsOpen() const = 0;
    virtual bool Read(unsigned char *, int) = 0;
    virtual bool Read(float *) = 0;
    virtual bool Read(int *) = 0;
    virtual bool Read(short *) = 0;
    virtual bool Read(double *) = 0;
    virtual bool Read(unsigned long *) = 0;
    virtual bool Write(unsigned char const *, int) = 0;
    virtual bool Write(float) = 0;
    virtual bool Write(int) = 0;
    virtual bool Write(short) = 0;
    virtual bool Write(double) = 0;
    virtual bool Write(unsigned long) = 0;
    Stream();
    static void __cdecl ByteSwap(double *);
    static void __cdecl ByteSwap(float *);
    static void __cdecl ByteSwap(int *);
    static void __cdecl ByteSwap(short *);
    static void __cdecl ByteSwap(unsigned char *, int);
    static void __cdecl ByteSwap(unsigned long *);
};
}
