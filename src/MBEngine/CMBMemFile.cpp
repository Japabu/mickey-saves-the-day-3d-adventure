// MBEngine.dll .\MBEngineRelease\CMBMemFile.obj
#include "CMBMemFile.h"

static const unsigned long kDefaultGrowSize = 1024;

// 0x1000e5d0 ?MakeCMBMemStream@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBMemStream(void)
{
    IMBUnknown *p = (IMBStream *)new CMBMemStream;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000e620 ?QueryInterface@CMBMemStream@@UAEJABUMBGUID@@PAPAX@Z
long CMBMemStream::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBStream *)this;
    if (iid == MBIID_IMBStream)
        p = (IMBStream *)this;
    if (iid == MBIID_IMBMemStream)
        p = (IMBMemStream *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000e730 ?AddRef@CMBMemStream@@UAEKXZ
unsigned long CMBMemStream::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000e740 ?Release@CMBMemStream@@UAEKXZ
unsigned long CMBMemStream::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1000e760 ?SetMemoryBlock@CMBMemStream@@UAEXABV?$MBComPtr@VIMBMemoryBlock@@@@@Z
void CMBMemStream::SetMemoryBlock(const MBComPtr<IMBMemoryBlock> &data)
{
    Reset();
    mData = data;
    if (mData) {
        mOrigin = mBuffer = mData->GetData();
        mBytesAvail = mSize = mData->GetSize();
    }
}

// 0x1000e7d0 ?GetMemoryBlock@CMBMemStream@@UAE?AV?$MBComPtr@VIMBMemoryBlock@@@@XZ
MBComPtr<IMBMemoryBlock> CMBMemStream::GetMemoryBlock(void)
{
    return mData;
}

// 0x1000e7f0 ?Clone@CMBMemStream@@UAEXPAPAVIMBStream@@@Z
void CMBMemStream::Clone(IMBStream **clone)
{
    *clone = 0;
    CMBMemStream *stream = new CMBMemStream;
    if (stream) {
        stream->SetMemoryBlock(GetMemoryBlock());
        if (stream->Seek(GetPosition(), kIMBStreamSeekStart))
            stream->QueryInterface(MBIID_IMBStream, (void **)clone);
        else
            delete stream;
    }
}

// 0x1000e8b0 ?Flush@CMBMemStream@@UAE_NXZ
bool CMBMemStream::Flush(void)
{
    return true;
}

// 0x1000e8c0 ?Read@CMBMemStream@@UAE_NPAXAAK@Z
bool CMBMemStream::Read(void *ptr, unsigned long &nBytes)
{
    if (ptr && mBuffer && mBytesAvail) {
        if (nBytes > mBytesAvail) {
            CopyBytes(ptr, mBuffer, mBytesAvail);
            mBuffer += mBytesAvail;
            nBytes = mBytesAvail;
            mBytesAvail = 0;
            SetLastError(kIMBStreamEOF);
            return false;
        }
        CopyBytes(ptr, mBuffer, nBytes);
        mBuffer += nBytes;
        mBytesAvail -= nBytes;
        return true;
    }
    nBytes = 0;
    return false;
}

inline bool CMBMemStream::Grow(unsigned long growSize)
{
    unsigned long newSize = growSize + mSize;
    if (!mData) {
        MBComPtr<IMBMemoryBlock> data(MBIID_CMBMemoryBlock);
        if (data && data->Allocate(newSize)) {
            SetMemoryBlock(data);
            return true;
        }
    } else if (mData->Grow(newSize)) {
        unsigned long offset = mBuffer - mOrigin;
        mOrigin = mData->GetData();
        mBuffer = mOrigin + offset;
        mBytesAvail += newSize - mSize;
        mSize = newSize;
        return true;
    }
    SetLastError(kIMBStreamWriteError);
    return false;
}

// EQUIV 0x1000ea00 ?Write@CMBMemStream@@UAE_NPBXK@Z: reviewed: only the order of two independent loads in the inlined Grow (mSize before or after the MBmax result) differs. Was: Write: inlined Grow loads mSize before the MBmax result (original: max first, then mSize); tried max+mSize, mSize+max, a local, and the add inside Grow (83% similar)
bool CMBMemStream::Write(const void *ptr, unsigned long nBytes)
{
    if (!ptr)
        return false;
    if (mBuffer && mBytesAvail) {
        if (nBytes > mBytesAvail) {
            unsigned long growSize = (nBytes - mBytesAvail) * 2;
            Grow(MBmax(growSize, kDefaultGrowSize));
        }
    } else {
        unsigned long growSize = 0;
        Grow(MBmax(growSize, kDefaultGrowSize));
    }
    if (nBytes > mBytesAvail) {
        CopyBytes(mBuffer, ptr, mBytesAvail);
        mBuffer += mBytesAvail;
        mBytesAvail = 0;
        SetLastError(kIMBStreamWriteError);
        return false;
    }
    CopyBytes(mBuffer, ptr, nBytes);
    mBuffer += nBytes;
    mBytesAvail -= nBytes;
    return true;
}

// EQUIV 0x1000eca0 ?Seek@CMBMemStream@@UAE_NJW4IMBStreamSeekValues@@@Z: reviewed: the same switch, tests and stores; the original saves edi in the prologue and shares the null-buffer error tail with the End case where VC6 duplicates it. Was: Seek: original saves edi in the prologue and shares the null-buffer error tail with the End case; every switch shape tried (5) shrink-wraps edi and merges tails differently (67% similar)
bool CMBMemStream::Seek(long offset, IMBStreamSeekValues origin)
{
    unsigned char *end;
    if (!mBuffer) {
        SetLastError(kIMBStreamSeekError);
        return false;
    }
    switch (origin) {
    case kIMBStreamSeekStart:
        if (offset < 0 || (unsigned long)offset > mSize) {
            SetLastError(kIMBStreamSeekError);
            return false;
        }
        mBuffer = mOrigin + offset;
        mBytesAvail = mSize - offset;
        return true;
    case kIMBStreamSeekEnd:
        if (offset > 0 || (unsigned long)-offset > mSize) {
            SetLastError(kIMBStreamSeekError);
            return false;
        }
        mBytesAvail = -offset;
        end = mOrigin + mSize;
        mBuffer = end + offset;
        return true;
    case kIMBStreamSeekCurrent:
        end = mOrigin + mSize;
        if (mBuffer + offset > end || mBuffer + offset < mOrigin) {
            SetLastError(kIMBStreamSeekError);
            return false;
        }
        mBuffer += offset;
        mBytesAvail -= offset;
        return true;
    }
    return true;
}

// 0x1000ed50 ?GetPosition@CMBMemStream@@UAEKXZ
unsigned long CMBMemStream::GetPosition(void)
{
    if (!mBuffer)
        return 0xffffffff;
    return mBuffer - mOrigin;
}

// 0x1000ed60 ?SetLastError@CMBMemStream@@UAE_NW4IMBStreamError@@@Z
bool CMBMemStream::SetLastError(IMBStreamError err)
{
    mError = err;
    return true;
}

// 0x1000ed70 ?GetLastError@CMBMemStream@@UAE?AW4IMBStreamError@@XZ
IMBStreamError CMBMemStream::GetLastError(void)
{
    IMBStreamError err = (IMBStreamError)mError;
    SetLastError(kIMBStreamNoError);
    return err;
}

// 0x1000ed80 ?GetStreamSize@CMBMemStream@@UAEKXZ
unsigned long CMBMemStream::GetStreamSize(void)
{
    if (mBuffer)
        return mSize;
    return 0xffffffff;
}

// 0x1000ed90 ??_GCMBMemStream@@UAEPAXI@Z (compiler-generated)
// 0x1000edf0 ?Release@?$MBComPtr@VIMBMemoryBlock@@@@QAEXXZ (MBComPtr template, CMBMemFile.h)
// 0x1000ee10 ?CreateObject@?$MBComPtr@VIMBMemoryBlock@@@@QAEJABUMBGUID@@@Z (MBComPtr template, CMBMemFile.h)


// Matched by this code:
// 0x1000eeb0 ?QueryInterface@CMBMemStream@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1000eec0 ?AddRef@CMBMemStream@@W3AEKXZ
// 0x1000eed0 ?Release@CMBMemStream@@W3AEKXZ
