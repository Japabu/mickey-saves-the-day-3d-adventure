// CMBWinFile.h - declarations for MBEngine.dll CMBWinFile.obj
// (also included by CMBMemFile and MBFileSystemWin)
#ifndef CMBWINFILE_H
#define CMBWINFILE_H

#include <windows.h>
#include <string>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &g) const {
        return data1 == g.data1 && data2 == g.data2 && data3 == g.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1];
    }
};

// header-defined interface IDs (PDB ldata, values read from the DLL)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBFile = { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinFile = { 0x89780490, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinFileSpec = { 0x89843e10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinFile = { 0x89b39ce0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemStream = { 0x89bfd660, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
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

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

#include "IMBStreamSeekValues.h"

#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBFileSpec.h"

#include "IMBFile.h"

#include "IMBWinFile.h"

#include "IMBMemoryBlock.h"

// smart pointer (as in MBSoundEngine.h: Release inline, but the inline budget
// runs out inside an inlined CreateObject, giving out-of-line Release calls)
// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(T *p, bool addRef) : mInterface(p) { if (addRef && p) p->AddRef(); }
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
    T *mInterface;
};

class CMBWinFile : public IMBStream, private IMBFile, private IMBWinFile {
public:
    CMBWinFile(void);
    ~CMBWinFile(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void *GetFileHandle(void);
    virtual bool Open(const IMBFileSpec *spec, unsigned long flags);
    virtual void GetFileSpec(IMBFileSpec **fileSpec);
    virtual bool Close(void);
    virtual bool SetSize(unsigned long size);
    virtual bool Write(const void *ptr, unsigned long nBytes);
    virtual bool Read(void *ptr, unsigned long &nBytes);
    virtual bool Seek(long offset, IMBStreamSeekValues origin);
    virtual bool Flush(void);
    virtual bool SetLastError(IMBStreamError err);
    virtual IMBStreamError GetLastError(void);
    virtual unsigned long GetPosition(void);
    virtual unsigned long GetStreamSize(void);
    virtual void Clone(IMBStream **target);
protected:
    void *mFile;
    MBComPtr<IMBFileSpec> mSpec;
    long mError;
};

IMBUnknown *MakeCMBWinFile(void);

#endif
