// CMBResourceTree.h - declarations for MBEngine.dll CMBResourceTree.obj: resources served from a
// directory tree, indexed by file type and name hash
#ifndef CMBRESOURCETREE_H
#define CMBRESOURCETREE_H

#include "MBStl.h"
#include <string>
#include <vector>
#include <map>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) void MBDebugStr(const char *, ...);

inline void *operator new(unsigned int size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

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

// PDB ldata order in CMBResourceTree.obj (0x1008a160..0x1008a1f0)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBFile = { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile = { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList = { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
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

// No construct/destroy: the original instantiates std::_Construct and the pairs' ??_G for its maps
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

enum IMBStreamSeekValues {
    kIMBStreamSeekStart = 1,
    kIMBStreamSeekEnd = 2,
    kIMBStreamSeekCurrent = 3
};

enum IMBStreamError {
    kIMBStreamNoError = 0
};

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStream; }
    virtual bool Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual bool Flush(void) = 0;
    virtual bool SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

class IMBFileSpec : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileSpec; }
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void SetPath(const char *) = 0;                       // vftable+0x10
    virtual void SetPath(const IMBFileSpec *, const char *) = 0;  // vftable+0x0c
    virtual MBString GetName(void) const = 0;
    virtual MBString GetExtension(void) const = 0;
    virtual MBString GetPath(void) const = 0;
    virtual MBString GetTitle(void) const = 0;
    virtual unsigned long GetType(void) const = 0;
    virtual void GetParent(IMBFileSpec **) const = 0;
    virtual unsigned long Hash(void) const = 0;
};

class IMBFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFile; }
    virtual bool Open(const IMBFileSpec *, unsigned long) = 0;
    virtual void GetFileSpec(IMBFileSpec **) = 0;
    virtual bool Close(void) = 0;
    virtual bool SetSize(unsigned long) = 0;
};

class IMBMemoryBlock : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMemoryBlock; }
    virtual bool Allocate(unsigned long) = 0;
    virtual bool Grow(unsigned long) = 0;
    virtual unsigned char *GetData(void) = 0;
    virtual unsigned long GetSize(void) = 0;
};

class IMBResourceFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFile; }
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
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
private:
    T *mInterface;
};

extern "C" void MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);
extern "C" void MBGetFilesInDirectory(const IMBFileSpec *directory, MBVector<MBComPtr<IMBFileSpec> > &kids,
                                      unsigned long flags, long recursionDepth);

class CMBResourceTree : public IMBResourceFile, private IMBFile {
    typedef MBMap<unsigned long, MBComPtr<IMBFileSpec>, std::less<unsigned long> > FileMap;   // name hash -> file
    typedef MBMap<unsigned long, FileMap, std::less<unsigned long> > TypeMap;                // file type -> files
    TypeMap mResMap;
public:
    CMBResourceTree(void);
    ~CMBResourceTree(void);
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
    void Dump(void);
protected:
    bool AddDirectory(const IMBFileSpec *dir);
    MBComPtr<IMBFileSpec> mSpec;
};

IMBUnknown *MakeCMBResourceTree(void);

#endif
