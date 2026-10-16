#pragma once
// Graph_FileStream.h: the scene-graph library's file stream; it reads and writes through the
// engine's file objects (CMBWinFile, IMBFileSpec, IMBStream), swapping the byte order.
#include "Graph_Stream.h"
template <class T0> class Vec;
class Text;
class IMBFile;
class IMBFileSpec;
class IMBStream;
class IMBUnknown;

namespace Graph {
class FileStream : public Graph::Stream {
public:
    enum OpenMode { ReadMode, WriteMode };
    virtual ~FileStream();
    virtual bool IsOpen() const;
    virtual bool Read(unsigned char *, int);
    virtual bool Read(float *);
    virtual bool Read(int *);
    virtual bool Read(short *);
    virtual bool Read(double *);
    virtual bool Read(unsigned long *);
    virtual bool Write(unsigned char const *, int);
    virtual bool Write(float);
    virtual bool Write(int);
    virtual bool Write(short);
    virtual bool Write(double);
    virtual bool Write(unsigned long);
    FileStream();
    FileStream(char const *, OpenMode);
    bool Open(Text, Vec<Text>, OpenMode);
    bool Open(char const *, OpenMode);
    void Close();
    IMBFile *mFile;                                     // +0x04
    IMBFileSpec *mFileSpec;                             // +0x08
    IMBStream *mStream;                                 // +0x0c
    IMBUnknown *mFileObject;                            // +0x10 (a CMBWinFile)
    IMBUnknown *mFileSpecObject;                        // +0x14
};
}
