// CMBResourceZipfile.h - declarations for MBEngine.dll CMBResourceZipfile.obj: resources served from
// a zip archive (CMBResourceZipfile) and the inflating stream over one archive member (CMBZipEntry)
#ifndef CMBRESOURCEZIPFILE_H
#define CMBRESOURCEZIPFILE_H

#include "MBStl.h"
#include <string>
#include <map>
#include <ctype.h>

// zlib.dll is built with ZLIB_DLL/WIN32/_WINDOWS: its entry points are WINAPI
#define ZLIB_DLL
#define _WINDOWS
#include "../zlib/zlib.h"

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) void *MBmemcpy(void *dst, const void *src, unsigned int n);
extern "C" __declspec(dllimport) void MBmemclr(void *dst, unsigned int n);
extern "C" __declspec(dllimport) unsigned long MBstrlen(const char *s);
extern "C" __declspec(dllimport) char *MBstrrchr(const char *s, int c);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *s);
extern "C" __declspec(dllimport) void MBDebugStr(const char *, ...);

inline void *operator new(unsigned int size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

template <class T> inline const T &MBmin(const T &a, const T &b) { return (a < b) ? a : b; }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

// PDB ldata order in CMBResourceZipfile.obj (0x1008a248..0x1008a308)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBFile = { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinFile = { 0x89780490, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile = { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList = { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemStream = { 0x89bfd660, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemStream = { 0x89c5f320, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemoryBlock = { 0x7944d1b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemoryBlock = { 0x79496740, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// No construct/destroy: the original instantiates std::_Construct for its maps' pairs
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        long hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    template <class U> long _QueryInterface(const MBComPtr<U> &p) {
        long hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

#include "IMBStreamSeekValues.h"

#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBFileSpec.h"

#include "IMBFile.h"

#include "IMBWinFile.h"

#include "IMBMemoryBlock.h"

#include "IMBMemStream.h"

#include "IMBResourceFile.h"

extern "C" void MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);

// zip file records, read field by field (the file layout is packed)
struct ZipCentDirEndRecord {
    unsigned long header;
    unsigned short thisDisk;
    unsigned short centDirStartDisk;
    unsigned short entryCntThisDisk;
    unsigned short entryCnt;
    unsigned long centDirSize;
    unsigned long centDirOffset;
    unsigned short commentLen;
};

struct ZipEntryDirectoryRecord {
    unsigned long header;
    unsigned short creatorVer;
    unsigned short verNeeded;
    unsigned short flags;
    unsigned short compMethod;
    unsigned short modTime;
    unsigned short modDate;
    unsigned long crc32;
    unsigned long compSize;
    unsigned long uncompSize;
    unsigned short filenameLen;
    unsigned short extraLen;
    unsigned short commentLen;
    unsigned short startDisk;
    unsigned short internalAttr;
    unsigned long externalAttr;
    unsigned long localHdrOffset;
};

struct ZipEntryLocalHdrRecord {
    unsigned long header;
    unsigned short verNeeded;
    unsigned short flags;
    unsigned short compMethod;
    unsigned short modTime;
    unsigned short modDate;
    unsigned long crc32;
    unsigned long compSize;
    unsigned long uncompSize;
    unsigned short filenameLen;
    unsigned short extraLen;
};

enum {
    kZipCentDirEndSignature = 0x06054b50,
    kZipEntryDirectorySignature = 0x02014b50,
    kZipEntryLocalHdrSignature = 0x04034b50
};

// one archive member, as found in the central directory
struct ZipEntry {
    unsigned long localHdrOffset;
    unsigned long fileDataOffset;   // 0 until the local header has been skipped once
    unsigned long compSize;
    unsigned long uncompSize;
    unsigned short compMethod;
    unsigned short flags;
};

class CMBResourceZipfile : public IMBResourceFile, private IMBFile {
    typedef MBMap<unsigned long, ZipEntry, std::less<unsigned long> > EntryMap;   // name hash -> entry
    typedef MBMap<unsigned long, EntryMap, std::less<unsigned long> > TypeMap;    // file type -> entries
    TypeMap mResMap;
public:
    CMBResourceZipfile(void);
    ~CMBResourceZipfile(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool Open(const IMBFileSpec *fileSpec, unsigned long flags);
    virtual void GetFileSpec(IMBFileSpec **fileSpec);
    virtual bool Close(void);
    virtual bool SetSize(unsigned long size);
    virtual void LoadResource(unsigned long resID, unsigned long type, IMBMemoryBlock **resourceData);
    virtual void GetStream(unsigned long resID, unsigned long type, IMBStream **stream);
protected:
    bool ParseZip(void);
    unsigned long Get4ByteType(const char *ext);
    MBComPtr<IMBStream> mStream;
    MBComPtr<IMBFileSpec> mSpec;
};

class CMBZipEntry : public IMBStream, public IMBWinFile {
public:
    CMBZipEntry(void);
    ~CMBZipEntry(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    bool Open(ZipEntry &entry, IMBStream *zipStream);
    virtual bool Read(void *ptr, unsigned long &nBytes);
    virtual bool Write(const void *ptr, unsigned long nBytes);
    virtual bool Seek(long offset, IMBStreamSeekValues origin);
    virtual bool Flush(void);
    virtual bool SetLastError(IMBStreamError err);
    virtual IMBStreamError GetLastError(void);
    virtual unsigned long GetPosition(void);
    virtual unsigned long GetStreamSize(void);
    virtual void Clone(IMBStream **clone);
    virtual void *GetFileHandle(void);
    const unsigned long kMemoryBlockSize;
    const unsigned short kCompMethodDeflate;
protected:
    bool LocateStartOfZipEntry(ZipEntry &entry);
    bool OpenZipEntry(void);
    bool CloseZipEntry(void);
    // a byte FIFO over one memory block; reads and writes wrap at the block end
    class RingBuffer {
    public:
        RingBuffer(void);
        bool Init(unsigned long size);
        void RemoveBytes(IMBStream *dest, unsigned long count);
        void RemoveBytes(void *dest, unsigned long count);
        void InsertBytes(IMBStream *src, unsigned long count);
        void InsertBytes(const void *src, unsigned long count);
        const void *GetRemovePtr(void);
        void *GetInsertPtr(void);
        unsigned long GetBytesAvail(void);
        unsigned long GetSpaceAvail(void);
        void NotifyBytesRemoved(unsigned long count);
        void NotifyBytesInserted(unsigned long count);
        void Clear(void);
    protected:
        MBComPtr<IMBMemoryBlock> mBlock;
        unsigned long mBlockSize;
        unsigned long mReadOffset;
        unsigned long mWriteOffset;
    };
    MBComPtr<IMBStream> mStream;
    RingBuffer mInput;
    RingBuffer mOutput;
    z_stream mZStream;
    unsigned long mDataOffset;
    unsigned long mCompSize;
    unsigned long mUncompSize;
    unsigned long mStreamPosition;
    unsigned long mFileBytesToRead;
    unsigned long mError;
    unsigned short mCompMethod;
    bool mZStreamOpened;
};

IMBUnknown *MakeCMBResourceZipfile(void);

#endif
