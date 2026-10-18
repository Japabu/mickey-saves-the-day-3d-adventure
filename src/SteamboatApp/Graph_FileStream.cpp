// Graph::FileStream (FileStream.obj): the scene-graph library's file stream on the engine's
// file objects. The library swaps every value to its file byte order.
#include "Graph_FileStream.h"
#include "MBInterfaces.h"
#include "Text.h"
#include "Vec.h"
// The scene-graph library is not no-alias code: Close and Open reload the members after every
// call; MBContainers.h (through MBInterfaces.h) turns "a" on for the game code.
#pragma optimize("a", off)

static const MBGUID MBIID_CMBWinFile = { 0x89b39ce0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// the library's array delete (the only ??_V of the binary is this object's)
inline void operator delete[](void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}

// 0x100e16b0 ??0FileStream@Graph@@QAE@XZ
Graph::FileStream::FileStream()
{
    mFile = 0;
    MBComCreateInstance(0, MBIID_CMBWinFile, &mFileObject);
}

// 0x100e1710 ??0FileStream@Graph@@QAE@PBDW4OpenMode@01@@Z
Graph::FileStream::FileStream(const char *fileName, OpenMode mode)
{
    mFile = 0;
    MBComCreateInstance(0, MBIID_CMBWinFile, &mFileObject);
    mFileObject->QueryInterface(MBIID_IMBFile, (void **)&mFile);
    Open(fileName, mode);
}

// 0x100e17a0 ?IsOpen@FileStream@Graph@@UBE_NXZ
bool Graph::FileStream::IsOpen() const
{
    return (mFile != 0) ? true : false;
}

// 0x100e17b0 ?Open@FileStream@Graph@@QAE_NPBDW4OpenMode@12@@Z
bool Graph::FileStream::Open(const char *fileName, OpenMode mode)
{
    if (!mFile) {
        mFileObject->QueryInterface(MBIID_IMBFile, (void **)&mFile);
        MBComCreateInstance(0, MBIID_IMBFileSpec, &mFileSpecObject);
        mFileSpecObject->QueryInterface(MBIID_IMBFileSpec, (void **)&mFileSpec);
        mFileSpec->SetPath(fileName);
        if (mode == ReadMode)
            mFile->Open(mFileSpec, 1);
        else
            mFile->Open(mFileSpec, 2);
        mFile->QueryInterface(MBIID_IMBStream, (void **)&mStream);
        return true;
    }
    return false;
}

// TODO 0x100e1830 ?Open@FileStream@Graph@@QAE_NVText@@V?$Vec@VText@@@@W4OpenMode@12@@Z: (66% similar) the original calls Vec<Text>::append(const T *, int) out of line, VC6 inlines it (as in Gfx_LODManager; a definition after the use inlines too)
bool Graph::FileStream::Open(Text fileName, Vec<Text> searchPath, OpenMode mode)
{
    if (searchPath.length() == 0)
        searchPath.append(&Text("."), 1);
    for (int i = 0; i < searchPath.length(); i++) {
        Text path = searchPath[i];
        path += "/";
        path += fileName;
        if (Open(path, mode))
            return true;
    }
    return false;
}

// 0x100e19e0 ?Close@FileStream@Graph@@QAEXXZ
void Graph::FileStream::Close()
{
    if (mFile) {
        mFile->Close();
        mStream->Release();
        mFile->Release();
        mFileSpec->Release();
        mFileObject->Release();
        mFileSpecObject->Release();
        mFile = 0;
        mFileSpec = 0;
        mStream = 0;
        mFileObject = 0;
        mFileSpecObject = 0;
    }
}

// 0x100e1a30 ?Read@FileStream@Graph@@UAE_NPAEH@Z
bool Graph::FileStream::Read(unsigned char *buffer, int count)
{
    mStream->Read(buffer, (unsigned long &)count);
    return true;
}

// 0x100e1a50 ?Read@FileStream@Graph@@UAE_NPAM@Z
bool Graph::FileStream::Read(float *value)
{
    unsigned long size = 4;
    mStream->Read(value, size);
    ByteSwap(value);
    return true;
}

// 0x100e1a80 ?Read@FileStream@Graph@@UAE_NPAH@Z
bool Graph::FileStream::Read(int *value)
{
    unsigned long size = 4;
    mStream->Read(value, size);
    ByteSwap(value);
    return true;
}

// 0x100e1ab0 ?Read@FileStream@Graph@@UAE_NPAF@Z
bool Graph::FileStream::Read(short *value)
{
    unsigned long size = 2;
    mStream->Read(value, size);
    ByteSwap(value);
    return true;
}

// 0x100e1ae0 ?Read@FileStream@Graph@@UAE_NPAN@Z
bool Graph::FileStream::Read(double *value)
{
    unsigned long size = 8;
    mStream->Read(value, size);
    ByteSwap(value);
    return true;
}

// 0x100e1b10 ?Read@FileStream@Graph@@UAE_NPAK@Z
bool Graph::FileStream::Read(unsigned long *value)
{
    unsigned long size = 4;
    mStream->Read(value, size);
    ByteSwap(value);
    return true;
}

// 0x100e1b40 ?Write@FileStream@Graph@@UAE_NPBEH@Z
bool Graph::FileStream::Write(const unsigned char *buffer, int count)
{
    mStream->Write(buffer, count);
    return true;
}

// 0x100e1b60 ?Write@FileStream@Graph@@UAE_NM@Z
bool Graph::FileStream::Write(float value)
{
    ByteSwap(&value);
    mStream->Write(&value, 4);
    return true;
}

// 0x100e1b90 ?Write@FileStream@Graph@@UAE_NH@Z
bool Graph::FileStream::Write(int value)
{
    ByteSwap(&value);
    mStream->Write(&value, 4);
    return true;
}

// 0x100e1bc0 ?Write@FileStream@Graph@@UAE_NF@Z
bool Graph::FileStream::Write(short value)
{
    ByteSwap(&value);
    mStream->Write(&value, 2);
    return true;
}

// 0x100e1bf0 ?Write@FileStream@Graph@@UAE_NN@Z
bool Graph::FileStream::Write(double value)
{
    ByteSwap(&value);
    mStream->Write(&value, 8);
    return true;
}

// 0x100e1c20 ?Write@FileStream@Graph@@UAE_NK@Z
bool Graph::FileStream::Write(unsigned long value)
{
    ByteSwap(&value);
    mStream->Write(&value, 4);
    return true;
}

// 0x100e1c50 ??1FileStream@Graph@@UAE@XZ
Graph::FileStream::~FileStream()
{
    if (mFile)
        Close();
}


// Matched by this code (template and inline instances):
// 0x100e16e0 ??_GFileStream@Graph@@UAEPAXI@Z
// 0x100e1cb0 ??1?$Vec@VText@@@@UAE@XZ
// 0x100e1db0 ??_G?$Vec@VText@@@@UAEPAXI@Z
// 0x100e1de0 ??_V@YAXPAX@Z

// Not claimed (static initializers and header instances of this object):
// TODO 0x100e1d00 ?append@?$Vec@VText@@@@QAEXPBVText@@H@Z: header/template instance that our object does not emit (VC6 inlines it or another unit provides it)
// TODO 0x100e1e00 (unnamed): the ctype guard initializer followed by its atexit stub; our _$E pair has the same code but the function list merges the two (tool)
