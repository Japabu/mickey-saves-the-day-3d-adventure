// MfActorLib.dll 0x1001a5a0-0x1001a7b0: MfMemoryStream.
#include <string.h>
#include "MfMemoryStream.h"
#include "MfTriangleSet.h"

// 0x1001a5a0 ??0MfMemoryStream@@QAE@XZ
MfMemoryStream::MfMemoryStream()
{
    ownsBuffer = true;
    size = 0x800;
    buffer = new char[0x800];
    position = 0;
    userFormat = 0;
}

// 0x1001a610 ??0MfMemoryStream@@QAE@PAXH@Z
MfMemoryStream::MfMemoryStream(void *p, int n)
{
    ownsBuffer = false;
    buffer = (char *)p;
    size = n;
    position = 0;
    userFormat = 0;
}

// 0x1001a640 ??1MfMemoryStream@@QAE@XZ
MfMemoryStream::~MfMemoryStream()
{
}

// 0x1001a650 ?GetBuffer@MfMemoryStream@@QAEPADAAH@Z
char *MfMemoryStream::GetBuffer(int &n)
{
    n = position;
    return buffer;
}

// 0x1001a660 ?ReadBytes@MfMemoryStream@@UAE?AW4MfStreamError@@KPAX@Z
MfStreamError MfMemoryStream::ReadBytes(unsigned long n, void *p)
{
    if (size - position < n) {
        status = MF_STREAM_READ_FAILED;  // end of stream
    } else {
        memcpy(p, buffer + position, n);
        position += n;
        status = MF_STREAM_OK;
    }
    return status;
}

// 0x1001a6c0 ?WriteBytes@MfMemoryStream@@UAE?AW4MfStreamError@@KPBX@Z
MfStreamError MfMemoryStream::WriteBytes(unsigned long n, const void *p)
{
    if (size - position < n) {
        if (ownsBuffer) {
            size *= 2;
            if (size - position < n)
                size = n + position;
            char *newBuffer = new char[size];
            memcpy(newBuffer, buffer, position);
            delete buffer;
            buffer = newBuffer;
        } else {
            status = MF_STREAM_WRITE_FAILED;  // buffer full
        }
    }
    if (status == 0) {
        memcpy(buffer + position, p, n);
        position += n;
        status = MF_STREAM_OK;
    }
    return status;
}

// 0x1001a770 ?Flush@MfMemoryStream@@UAE?AW4MfStreamError@@XZ
MfStreamError MfMemoryStream::Flush()
{
    return status;
}

// 0x1001a770 ?GetNumVertices@MfTriangleSet@@QAEHXZ (folded with Flush: the member at +0x14;
// not in MfTriangleSet.cpp, where the original calls it out of line)
int MfTriangleSet::GetNumVertices()
{
    return mNumVertices;
}

// 0x1001a780 ?Seek@MfMemoryStream@@UAE?AW4MfStreamError@@H@Z
MfStreamError MfMemoryStream::Seek(int n)
{
    unsigned long p = position + n;
    if (p < size)
        position = p;
    else
        status = MF_STREAM_SEEK_FAILED; // outside the buffer
    return status;
}
