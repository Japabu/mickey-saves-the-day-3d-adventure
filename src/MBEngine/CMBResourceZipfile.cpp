// MBEngine.dll .\MBEngineRelease\CMBResourceZipfile.obj
#include "CMBResourceZipfile.h"

// the original reports the build machine's __FILE__
#define kThisFile "D:\\tugboat\\code\\sdk\\muffin\\src\\IO\\CMBResourceZipfile.cpp"

// 0x100128a0 ?MakeCMBResourceZipfile@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBResourceZipfile(void)
{
    IMBUnknown *p = (IMBResourceFile *)new CMBResourceZipfile;
    if (p)
        p->AddRef();
    return p;
}

// 0x100128d0 ??0CMBResourceZipfile@@QAE@XZ
CMBResourceZipfile::CMBResourceZipfile(void)
{
    _mRefCnt = 0;
}

// 0x10012980 ?QueryInterface@CMBResourceZipfile@@UAEJABUMBGUID@@PAPAX@Z
long CMBResourceZipfile::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBFile)
        p = (IMBFile *)this;
    if (iid == MBIID_IMBResourceFile)
        p = (IMBResourceFile *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBFile *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10012aa0 ?AddRef@CMBResourceZipfile@@UAEKXZ
unsigned long CMBResourceZipfile::AddRef()
{
    return ++_mRefCnt;
}

// 0x10012ab0 ?Release@CMBResourceZipfile@@UAEKXZ
unsigned long CMBResourceZipfile::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt && this)
    {
        this->~CMBResourceZipfile();
        MBfree_release(this);
    }
    return r;
}

// 0x10012ae0 ??1CMBResourceZipfile@@QAE@XZ
CMBResourceZipfile::~CMBResourceZipfile(void)
{
    Close();
}

// 0x10012bf0 ?Open@CMBResourceZipfile@@UAE_NPBVIMBFileSpec@@K@Z
bool CMBResourceZipfile::Open(const IMBFileSpec *fileSpec, unsigned long flags)
{
    IMBStream *tempStream;
    MBOpenFile(fileSpec, 5, &tempStream);
    mStream.Attach(tempStream);
    if (mStream) {
        IMBFileSpec *parent;
        fileSpec->GetParent(&parent);
        if (parent) {
            mSpec.CreateObject(MBIID_IMBFileSpec);
            mSpec->SetPath(parent, fileSpec->GetName().c_str());
            parent->Release();
        }
        if (ParseZip())
            return true;
    }
    return false;
}

// 0x10012d30 ?GetFileSpec@CMBResourceZipfile@@UAEXPAPAVIMBFileSpec@@@Z
void CMBResourceZipfile::GetFileSpec(IMBFileSpec **fileSpec)
{
    if (mSpec)
        mSpec->AddRef();
    *fileSpec = mSpec;
}

// 0x10012d60 ?Close@CMBResourceZipfile@@UAE_NXZ
bool CMBResourceZipfile::Close(void)
{
    mResMap.erase(mResMap.begin(), mResMap.end());
    mStream = 0;
    mSpec = 0;
    return true;
}

// 0x10012e30 ?LoadResource@CMBResourceZipfile@@UAEXKKPAPAVIMBMemoryBlock@@@Z
void CMBResourceZipfile::LoadResource(unsigned long resID, unsigned long type, IMBMemoryBlock **resourceData)
{
    *resourceData = 0;
    IMBStream *stream;
    GetStream(resID, type, &stream);
    if (stream) {
        unsigned long size = stream->GetStreamSize();
        IMBMemoryBlock *block;
        MBComCreateInstance(0, MBIID_CMBMemoryBlock, (IMBUnknown **)&block);
        if (block && !block->Allocate(size)) {
            MBDebugStr("ERROR: alloc failure in %s, line %d.\n", kThisFile, 301);
            block->Release();
            block = 0;
        }
        if (block) {
            unsigned long readSize = size;
            stream->Read(block->GetData(), readSize);
            if (size != readSize) {
                MBDebugStr("ERROR: read failure in %s, line %d.\n", kThisFile, 314);
                block->Release();
                block = 0;
            }
        }
        stream->Release();
        *resourceData = block;
    }
}

// 0x10012f10 ?GetStream@CMBResourceZipfile@@UAEXKKPAPAVIMBStream@@@Z
void CMBResourceZipfile::GetStream(unsigned long resID, unsigned long type, IMBStream **stream)
{
    *stream = 0;
    TypeMap::iterator it = mResMap.find(type);
    if (it != mResMap.end()) {
        EntryMap::iterator found = (*it).second.find(resID);
        if (found != (*it).second.end()) {
            CMBZipEntry *entry = new CMBZipEntry;
            if (entry) {
                if (entry->Open((*found).second, mStream))
                    entry->QueryInterface(MBIID_IMBStream, (void **)stream);
                else
                    delete entry;
            }
        }
    }
}

// zip records are read one field at a time
template <class T> inline bool ReadField(IMBStream *stream, T &field)
{
    unsigned long size = sizeof(T);
    return stream->Read(&field, size);
}

// EQUIV 0x10012ff0 ?ParseZip@CMBResourceZipfile@@IAE_NXZ: reviewed: the differences are inline choices of the same helpers: the original calls the first two MBComPtr::CreateObject (IMBMemoryBlock, IMBStream) and the map insert (_Tree::insert, the pair constructor) out of line where VC6 inlines them (CreateObject as Release, MBComCreateInstance and _QueryInterface, which does the same QueryInterface and Attach); the record reads, the name-length test (now written > sizeof(nameBuffer) - 1, the original constant), strings and calls agree. Was: inline budget: original keeps the first two CreateObject calls and every map::insert pair<iterator,bool> ctor out of line (only the third CreateObject inline); ours inlines more (40% similar)
bool CMBResourceZipfile::ParseZip(void)
{
    unsigned long readSize;
    long count;
    unsigned long hash;
    char *namePtr;
    char nameBuffer[1024];
    ZipCentDirEndRecord endRec;
    ZipEntryDirectoryRecord entryRec;
    ZipEntry entry;

    // the end of central directory record (no archive comment expected)
    mStream->Seek(-22, kIMBStreamSeekEnd);
    MBComPtr<IMBMemoryBlock> memBlock;
    memBlock.CreateObject(MBIID_CMBMemoryBlock);
    memBlock->Allocate(sizeof(ZipCentDirEndRecord));
    MBComPtr<IMBStream> memStreamSP;
    memStreamSP.CreateObject(MBIID_CMBMemStream);
    MBComPtr<IMBMemStream> memStreamMSP(memStreamSP);
    memStreamMSP->SetMemoryBlock(memBlock);
    readSize = memBlock->GetSize();
    mStream->Read(memBlock->GetData(), readSize);

    ReadField(memStreamSP, endRec.header);
    if (endRec.header != kZipCentDirEndSignature)
        return false;
    ReadField(memStreamSP, endRec.thisDisk);
    ReadField(memStreamSP, endRec.centDirStartDisk);
    ReadField(memStreamSP, endRec.entryCntThisDisk);
    ReadField(memStreamSP, endRec.entryCnt);
    ReadField(memStreamSP, endRec.centDirSize);
    ReadField(memStreamSP, endRec.centDirOffset);
    ReadField(memStreamSP, endRec.commentLen);

    // the central directory
    mStream->Seek(endRec.centDirOffset, kIMBStreamSeekStart);
    memStreamMSP->SetMemoryBlock(MBComPtr<IMBMemoryBlock>());
    memBlock.Release();
    memBlock.CreateObject(MBIID_CMBMemoryBlock);
    memBlock->Allocate(endRec.centDirSize);
    memStreamMSP->SetMemoryBlock(memBlock);
    mStream->Read(memBlock->GetData(), endRec.centDirSize);

    count = endRec.entryCnt;
    while (count--) {
        ReadField(memStreamSP, entryRec.header);
        if (entryRec.header != kZipEntryDirectorySignature)
            return false;
        ReadField(memStreamSP, entryRec.creatorVer);
        ReadField(memStreamSP, entryRec.verNeeded);
        ReadField(memStreamSP, entryRec.flags);
        ReadField(memStreamSP, entryRec.compMethod);
        ReadField(memStreamSP, entryRec.modTime);
        ReadField(memStreamSP, entryRec.modDate);
        ReadField(memStreamSP, entryRec.crc32);
        ReadField(memStreamSP, entryRec.compSize);
        ReadField(memStreamSP, entryRec.uncompSize);
        ReadField(memStreamSP, entryRec.filenameLen);
        ReadField(memStreamSP, entryRec.extraLen);
        ReadField(memStreamSP, entryRec.commentLen);
        ReadField(memStreamSP, entryRec.startDisk);
        ReadField(memStreamSP, entryRec.internalAttr);
        ReadField(memStreamSP, entryRec.externalAttr);
        ReadField(memStreamSP, entryRec.localHdrOffset);

        readSize = entryRec.filenameLen;
        if (readSize > sizeof(nameBuffer) - 1)
            return false;
        memStreamSP->Read(nameBuffer, readSize);
        nameBuffer[readSize] = 0;

        // resources are keyed by the hash of the bare name and the extension as a type
        namePtr = MBstrrchr(nameBuffer, '/');
        if (namePtr)
            namePtr++;
        else
            namePtr = nameBuffer;
        char *ext = MBstrrchr(namePtr, '.');
        if (ext) {
            *ext = 0;
            ext++;
        }
        hash = MBHashString(namePtr);
        if (MBstrlen(namePtr) > 0) {
            unsigned long type = Get4ByteType(ext);
            if (mResMap[type].find(hash) != mResMap[type].end()) {
                MBDebugStr("WARN: Hash collision in CMBResourceZipfile.\n");
                MBDebugStr("      File %s collides.\n", namePtr);
            } else {
                entry.localHdrOffset = entryRec.localHdrOffset;
                entry.fileDataOffset = 0;
                entry.compSize = entryRec.compSize;
                entry.uncompSize = entryRec.uncompSize;
                entry.compMethod = entryRec.compMethod;
                entry.flags = entryRec.flags;
                mResMap[type][hash] = entry;
            }
        }
    }
    return true;
}

// 0x100137f0 ?Get4ByteType@CMBResourceZipfile@@IAEKPBD@Z
unsigned long CMBResourceZipfile::Get4ByteType(const char *ext)
{
    unsigned long type = '    ';
    if (ext) {
        if (*ext == '.')
            ext++;
        long len = MBstrlen(ext);
        if (len > 4)
            len = 4;
        long i;
        long j = 3;
        for (i = 0; i < len; i++, j--)
            ((char *)&type)[j] = tolower(ext[i]);
        for (; i < 4; i++, j--)
            ((char *)&type)[j] = ' ';
    }
    return type;
}

// 0x10013870 ?ZlibMalloc@@YAPAXPAXII@Z
void *__cdecl ZlibMalloc(void *, unsigned int items, unsigned int size)
{
    return MBmalloc_release(items * size);
}

// 0x10013890 ?ZlibFree@@YAXPAX0@Z
void __cdecl ZlibFree(void *, void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}

// 0x100138b0 ??0CMBZipEntry@@QAE@XZ
CMBZipEntry::CMBZipEntry(void)
    : kMemoryBlockSize(0x2800), kCompMethodDeflate(Z_DEFLATED), mDataOffset(0), mCompSize(0), mUncompSize(0),
      mStreamPosition(0), mFileBytesToRead(0), mError(0), mCompMethod(0), mZStreamOpened(false)
{
    _mRefCnt = 0;
}

// 0x10013920 ?QueryInterface@CMBZipEntry@@UAEJABUMBGUID@@PAPAX@Z
long CMBZipEntry::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBWinFile)
        p = (IMBWinFile *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBStream *)this;
    if (iid == MBIID_IMBStream)
        p = (IMBStream *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10013a30 ?AddRef@CMBZipEntry@@UAEKXZ
unsigned long CMBZipEntry::AddRef()
{
    return ++_mRefCnt;
}

// 0x10013a40 ?Release@CMBZipEntry@@UAEKXZ
unsigned long CMBZipEntry::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt) {
        if (this) {
            this->~CMBZipEntry();
            MBfree_release(this);
        }
    }
    return r;
}

// 0x10013a70 ??1CMBZipEntry@@QAE@XZ
CMBZipEntry::~CMBZipEntry(void)
{
    if (mZStreamOpened)
        inflateEnd(&mZStream);
    mStream.Release();
}

// 0x10013ae0 ?Clone@CMBZipEntry@@UAEXPAPAVIMBStream@@@Z
void CMBZipEntry::Clone(IMBStream **clone)
{
    CMBZipEntry *entry = new CMBZipEntry;
    IMBStream *tempStream;
    mStream->Clone(&tempStream);
    entry->mStream.Attach(tempStream);
    entry->mDataOffset = mDataOffset;
    entry->mCompSize = mCompSize;
    entry->mUncompSize = mUncompSize;
    entry->QueryInterface(MBIID_IMBStream, (void **)clone);
}

// 0x10013b50 ?Open@CMBZipEntry@@QAE_NAAUZipEntry@@PAVIMBStream@@@Z
bool CMBZipEntry::Open(ZipEntry &entry, IMBStream *zipStream)
{
    mStream = zipStream;
    if (!mStream) {
        SetLastError(kIMBStreamOpenError);
        return false;
    }
    if (!LocateStartOfZipEntry(entry))
        return false;
    return OpenZipEntry();
}

// 0x10013bb0 ?LocateStartOfZipEntry@CMBZipEntry@@IAE_NAAUZipEntry@@@Z
bool CMBZipEntry::LocateStartOfZipEntry(ZipEntry &entry)
{
    mCompSize = entry.compSize;
    mUncompSize = entry.uncompSize;
    mFileBytesToRead = mCompSize;
    mCompMethod = entry.compMethod;
    if (mCompMethod != 0 && mCompMethod != kCompMethodDeflate) {
        SetLastError(kIMBStreamOpenError);
        return false;
    }
    if (!entry.fileDataOffset) {
        // skip the local header once; its offset is remembered in the entry
        mStream->Seek(entry.localHdrOffset, kIMBStreamSeekStart);
        IMBStream *in = mStream;
        ZipEntryLocalHdrRecord localHdrRec;
        { unsigned long size = 4; in->Read(&localHdrRec.header, size); }
        if (localHdrRec.header != kZipEntryLocalHdrSignature) {
            SetLastError(kIMBStreamReadError);
            return false;
        }
        { unsigned long size = 2; in->Read(&localHdrRec.verNeeded, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.flags, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.compMethod, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.modTime, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.modDate, size); }
        { unsigned long size = 4; in->Read(&localHdrRec.crc32, size); }
        { unsigned long size = 4; in->Read(&localHdrRec.compSize, size); }
        { unsigned long size = 4; in->Read(&localHdrRec.uncompSize, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.filenameLen, size); }
        { unsigned long size = 2; in->Read(&localHdrRec.extraLen, size); }
        if (!mStream->Seek(localHdrRec.filenameLen + localHdrRec.extraLen, kIMBStreamSeekCurrent)) {
            SetLastError(kIMBStreamSeekError);
            return false;
        }
        mDataOffset = entry.fileDataOffset = mStream->GetPosition();
        return true;
    }
    mDataOffset = entry.fileDataOffset;
    return true;
}

// 0x10013d90 ?CloseZipEntry@CMBZipEntry@@IAE_NXZ
bool CMBZipEntry::CloseZipEntry(void)
{
    if (mZStreamOpened)
        inflateEnd(&mZStream);
    mZStreamOpened = false;
    return true;
}

// EQUIV 0x10013dc0 ?OpenZipEntry@CMBZipEntry@@IAE_NXZ: reviewed: register choice; the second min picks its operand with the inverse jump (same result)
bool CMBZipEntry::OpenZipEntry(void)
{
    if (!CloseZipEntry())
        return false;
    mFileBytesToRead = mCompSize;
    mStreamPosition = 0;
    if (!mStream->Seek(mDataOffset, kIMBStreamSeekStart)) {
        SetLastError(kIMBStreamSeekError);
        return false;
    }
    if (mCompMethod == kCompMethodDeflate) {
        mInput.Clear();
        mOutput.Clear();
        if (!mInput.GetSpaceAvail() && !mInput.Init(MBmin(kMemoryBlockSize, mCompSize))) {
            SetLastError(kIMBStreamOpenError);
            return false;
        }
        if (!mOutput.GetSpaceAvail() && !mOutput.Init(MBmin(kMemoryBlockSize, mUncompSize))) {
            SetLastError(kIMBStreamOpenError);
            return false;
        }
        MBmemclr(&mZStream, sizeof(mZStream));
        mZStream.zalloc = ZlibMalloc;
        mZStream.zfree = ZlibFree;
        mZStream.opaque = 0;
        // raw deflate data: no zlib header
        if (inflateInit2(&mZStream, -MAX_WBITS) != Z_OK) {
            SetLastError(kIMBStreamOpenError);
            return false;
        }
        mZStreamOpened = true;
    }
    return true;
}

// EQUIV 0x10013f00 ?Read@CMBZipEntry@@UAE_NPAXAAK@Z: reviewed: block layout only (the loop's back edge is inverted and the error block placed after the exit); the inserted terminator literal is now the original's "\0"
bool CMBZipEntry::Read(void *ptr, unsigned long &nBytes)
{
    if (mCompMethod == 0) {
        // stored
        bool result = mStream->Read(ptr, nBytes);
        mStreamPosition += nBytes;
        SetLastError(mStream->GetLastError());
        return result;
    }
    unsigned long bytesRead = 0;
    bool endOfFile = false;
    while (bytesRead < nBytes && GetLastError() == kIMBStreamNoError) {
        unsigned long bytesWeGot = mOutput.GetBytesAvail();
        if (bytesWeGot) {
            unsigned long bytesWeWant = nBytes - bytesRead;
            unsigned long count = MBmin(bytesWeGot, bytesWeWant);
            mOutput.RemoveBytes(ptr, count);
            bytesRead += count;
            mStreamPosition += count;
            ptr = (char *)ptr + count;
            if (mStreamPosition == mUncompSize) {
                SetLastError(kIMBStreamEOF);
                endOfFile = true;
                break;
            }
            if (bytesRead == nBytes)
                break;
        }
        if (!mInput.GetBytesAvail()) {
            unsigned long bytesToRead = MBmin(mInput.GetSpaceAvail(), mFileBytesToRead);
            if (!bytesToRead) {
                // inflate wants one extra byte after the end of the data
                mInput.InsertBytes("\0", 1);
            } else {
                mInput.InsertBytes(mStream, bytesToRead);
                mFileBytesToRead -= bytesToRead;
            }
        }
        mZStream.next_in = (Bytef *)mInput.GetRemovePtr();
        mZStream.avail_in = mInput.GetBytesAvail();
        mZStream.next_out = (Bytef *)mOutput.GetInsertPtr();
        mZStream.avail_out = mOutput.GetSpaceAvail();
        int err = inflate(&mZStream, Z_SYNC_FLUSH);
        if (err < Z_OK || err > Z_STREAM_END) {
            SetLastError(kIMBStreamReadError);
            return false;
        }
        mOutput.NotifyBytesInserted(mOutput.GetSpaceAvail() - mZStream.avail_out);
        mInput.NotifyBytesRemoved(mInput.GetBytesAvail() - mZStream.avail_in);
    }
    nBytes = bytesRead;
    return !endOfFile;
}

// 0x10014100 ?Write@CMBZipEntry@@UAE_NPBXK@Z
bool CMBZipEntry::Write(const void *ptr, unsigned long nBytes)
{
    SetLastError(kIMBStreamWriteError);
    return false;
}

// 0x10014110 ?Seek@CMBZipEntry@@UAE_NJW4IMBStreamSeekValues@@@Z
bool CMBZipEntry::Seek(long offset, IMBStreamSeekValues origin)
{
    unsigned long target = 0;
    unsigned long position = GetPosition();
    switch (origin) {
    case kIMBStreamSeekCurrent:
        if (!offset)
            return true;
        target = position + offset;
        break;
    case kIMBStreamSeekEnd:
        target = GetStreamSize() - offset;
        break;
    case kIMBStreamSeekStart:
        target = offset;
        break;
    }
    if (target == position)
        return true;
    if (target > position) {
        // forward: inflate and drop
        unsigned long distanceToGo = target - position;
        char temp[1024];
        while (distanceToGo) {
            unsigned long readSize = MBmin(distanceToGo, (unsigned long)sizeof(temp));
            Read(temp, readSize);
            distanceToGo -= readSize;
            if (!readSize) {
                SetLastError(kIMBStreamSeekError);
                return false;
            }
        }
        return true;
    }
    if (target < position) {
        // backward: start over
        if (!CloseZipEntry() || !OpenZipEntry() || !Seek(offset, origin)) {
            SetLastError(kIMBStreamSeekError);
            return false;
        }
    }
    return true;
}

// 0x10014200 ?Flush@CMBZipEntry@@UAE_NXZ
bool CMBZipEntry::Flush()
{
    return true;
}

// 0x10014210 ?SetLastError@CMBZipEntry@@UAE_NW4IMBStreamError@@@Z
bool CMBZipEntry::SetLastError(IMBStreamError err)
{
    mError = err;
    return true;
}

// 0x10014220 ?GetLastError@CMBZipEntry@@UAE?AW4IMBStreamError@@XZ
IMBStreamError CMBZipEntry::GetLastError()
{
    IMBStreamError err = (IMBStreamError)mError;
    SetLastError(kIMBStreamNoError);
    return err;
}

// 0x10014240 ?GetPosition@CMBZipEntry@@UAEKXZ
unsigned long CMBZipEntry::GetPosition()
{
    return mStreamPosition;
}

// 0x10014250 ?GetStreamSize@CMBZipEntry@@UAEKXZ
unsigned long CMBZipEntry::GetStreamSize()
{
    return mUncompSize;
}

// 0x10014260 ?GetFileHandle@CMBZipEntry@@UAEPAXXZ
void *CMBZipEntry::GetFileHandle(void)
{
    MBComPtr<IMBWinFile> winFile(mStream);
    if (winFile)
        return winFile->GetFileHandle();
    return 0;
}

// 0x100142e0 ?SetSize@CMBResourceZipfile@@UAE_NK@Z
bool CMBResourceZipfile::SetSize(unsigned long size)
{
    return false;
}

// 0x100142f0 ??0RingBuffer@CMBZipEntry@@QAE@XZ
CMBZipEntry::RingBuffer::RingBuffer()
{
    mBlock = 0;
    mBlockSize = 0;
    mReadOffset = 0;
    mWriteOffset = 0;
}

// 0x10014300 ?Init@RingBuffer@CMBZipEntry@@QAE_NK@Z
bool CMBZipEntry::RingBuffer::Init(unsigned long size)
{
    mWriteOffset = 0;
    mReadOffset = 0;
    mBlockSize = size;
    mBlock.CreateObject(MBIID_CMBMemoryBlock);
    if (mBlock && mBlock->Allocate(size))
        return true;
    return false;
}

// 0x100143b0 ?Clear@RingBuffer@CMBZipEntry@@QAEXXZ
void CMBZipEntry::RingBuffer::Clear()
{
    mWriteOffset = 0;
    mReadOffset = 0;
}

// 0x100143c0 ?RemoveBytes@RingBuffer@CMBZipEntry@@QAEXPAXK@Z
void CMBZipEntry::RingBuffer::RemoveBytes(void *dest, unsigned long count)
{
    MBmemcpy(dest, GetRemovePtr(), count);
    NotifyBytesRemoved(count);
}

// 0x100143f0 ?InsertBytes@RingBuffer@CMBZipEntry@@QAEXPBXK@Z
void CMBZipEntry::RingBuffer::InsertBytes(const void *src, unsigned long count)
{
    MBmemcpy(GetInsertPtr(), src, count);
    NotifyBytesInserted(count);
}

// 0x10014420 ?RemoveBytes@RingBuffer@CMBZipEntry@@QAEXPAVIMBStream@@K@Z
void CMBZipEntry::RingBuffer::RemoveBytes(IMBStream *dest, unsigned long count)
{
    dest->Write(GetRemovePtr(), count);
    NotifyBytesRemoved(count);
}

// 0x10014450 ?InsertBytes@RingBuffer@CMBZipEntry@@QAEXPAVIMBStream@@K@Z
void CMBZipEntry::RingBuffer::InsertBytes(IMBStream *src, unsigned long count)
{
    src->Read(GetInsertPtr(), count);
    NotifyBytesInserted(count);
}

// 0x10014480 ?GetRemovePtr@RingBuffer@CMBZipEntry@@QAEPBXXZ
const void *CMBZipEntry::RingBuffer::GetRemovePtr()
{
    if (GetBytesAvail())
        return mBlock->GetData() + mReadOffset;
    return 0;
}

// 0x100144a0 ?GetInsertPtr@RingBuffer@CMBZipEntry@@QAEPAXXZ
void *CMBZipEntry::RingBuffer::GetInsertPtr()
{
    if (GetSpaceAvail())
        return mBlock->GetData() + mWriteOffset;
    return 0;
}

// 0x100144c0 ?GetBytesAvail@RingBuffer@CMBZipEntry@@QAEKXZ
unsigned long CMBZipEntry::RingBuffer::GetBytesAvail()
{
    if (mReadOffset <= mWriteOffset)
        return mWriteOffset - mReadOffset;
    return mBlockSize - mReadOffset;
}

// 0x100144d0 ?GetSpaceAvail@RingBuffer@CMBZipEntry@@QAEKXZ
unsigned long CMBZipEntry::RingBuffer::GetSpaceAvail()
{
    if (mWriteOffset < mReadOffset)
        return mReadOffset - mWriteOffset;
    return mBlockSize - mWriteOffset;
}

// 0x100144e0 ?NotifyBytesRemoved@RingBuffer@CMBZipEntry@@QAEXK@Z
void CMBZipEntry::RingBuffer::NotifyBytesRemoved(unsigned long count)
{
    mReadOffset += count;
    if (mWriteOffset == mBlockSize)
        mWriteOffset = 0;
    if (mReadOffset == mBlockSize)
        if (mWriteOffset > 0)
            mReadOffset = 0;
}

// 0x10014520 ?NotifyBytesInserted@RingBuffer@CMBZipEntry@@QAEXK@Z
void CMBZipEntry::RingBuffer::NotifyBytesInserted(unsigned long count)
{
    mWriteOffset += count;
    if (mReadOffset == mBlockSize)
        mReadOffset = 0;
    if (mWriteOffset == mBlockSize)
        if (mReadOffset > 0)
            mWriteOffset = 0;
}

// 0x10014560 ?Release@?$MBComPtr@VIMBStream@@@@QAEXXZ
// TODO 0x10014580 ?CreateObject@?$MBComPtr@VIMBStream@@@@QAEJABUMBGUID@@@Z: only emitted out of line once ParseZip keeps memStreamSP.CreateObject out of line
// 0x10014620 ?begin@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?ba65767c
// 0x10014630 ?find@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$2e1dcd07
// 0x10014680 ??0?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@std@@QAE@ABU?$less@K@1@ABV?$MBAllocator@UZipEntry@@@@@Z
// 0x10014720 ??1?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE@XZ
// 0x10014850 ?Release@?$MBComPtr@VIMBMemStream@@@@QAEXXZ
// 0x10014870 ?Release@?$MBComPtr@VIMBWinFile@@@@QAEXXZ
// 0x10014890 ??0?$MBMap@KUZipEntry@@U?$less@K@std@@@@QAE@ABV0@@Z
// 0x10014a40 ?insert@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@Uccecc2ac
// 0x10014ce0 ?erase@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?36106f22
// 0x10015250 ?_Erase@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@Uf5ddf605
// 0x10015340 ?begin@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE?AViterator@12@XZ
// 0x10015350 ?insert@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry4e737b89
// 0x100155f0 ?erase@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100156e0 ?lower_bound@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10015730 ??0iterator@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE@PAU_Node@12@@Z
// 0x10015740 ??Eiterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntrec0ee538
// 0x100157d0 ??0?$pair@Viterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@K4b168ee9
// 0x100157f0 ?_Insert@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@6196e8f8
// 0x10015ad0 ?_Lbound@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@@3b92b5ac
// 0x10015b20 ?_Lrotate@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@fffcc9e1
// 0x10015b80 ?_Rrotate@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@806e6b10
// 0x10015be0 ?_Buynode@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@KUZipEntry@cba4bc78
// 0x10015c00 ?deallocate@?$MBAllocator@UZipEntry@@@@QAEXPAXK@Z
// 0x10015c20 ?erase@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10016110 ?_Erase@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10016170 ?_Insert@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKUZipEntry@@@2@@Z
// 0x10016450 ?_Lbound@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x100164a0 ?_Lmost@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEAAPAU_Node@12@XZ
// 0x100164b0 ?_Copy@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x10016550 ?_Lrotate@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100165b0 ?_Rrotate@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10016610 ?_Buynode@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10016630 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAEXXZ
// 0x100166d0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@b90dbe91
// 0x10016770 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KUZipEntry@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KUZipEntry@@U?$less@K@std@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBMap@53464630
// 0x10016800 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@QAEXXZ
// 0x10016890 ?_QueryInterface@?$MBComPtr@VIMBMemoryBlock@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10016910 ?_Construct@std@@YAXPAU?$pair@$$CBKV?$MBMap@KUZipEntry@@U?$less@K@std@@@@@1@ABU21@@Z
// 0x10016ac0 ?_Max@?$_Tree@KU?$pair@$$CBKUZipEntry@@@std@@U_Kfn@?$map@KUZipEntry@@U?$less@K@std@@V?$MBAllocator@UZipEntry@@@@@2@U?$less@K@2@V?$MBAllocator@UZipEntry@@@@@std@@KAPAU_Node@12@PAU312@@Z
// 0x10016b00 ?_Construct@std@@YAXPAU?$pair@$$CBKUZipEntry@@@1@ABU21@@Z
// 0x10016b20 ?_QueryInterface@?$MBComPtr@VIMBMemStream@@@@AAEJABV?$MBComPtr@VIMBStream@@@@@Z
// 0x10016ba0 $E24
// 0x10016bc0 $E23
// 0x10016bd0 ??4?$MBComPtr@VIMBWinFile@@@@QAEAAV0@PAVIMBWinFile@@@Z
// 0x10016c00 ?QueryInterface@CMBZipEntry@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10016c10 ?AddRef@CMBZipEntry@@W3AEKXZ
// 0x10016c20 ?Release@CMBZipEntry@@W3AEKXZ
// 0x10016c30 ?QueryInterface@CMBResourceZipfile@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10016c40 ?AddRef@CMBResourceZipfile@@W3AEKXZ
// 0x10016c50 ?Release@CMBResourceZipfile@@W3AEKXZ
