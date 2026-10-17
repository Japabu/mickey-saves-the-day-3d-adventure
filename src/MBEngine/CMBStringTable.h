// CMBStringTable.h - declarations for MBEngine.dll CMBStringTable.obj
#ifndef CMBSTRINGTABLE_H
#define CMBSTRINGTABLE_H

#include "MBStl.h"
#include <stdio.h>
#include <string> // evidence: ctype $E20/$E21 = <string> baseline + MBString/MBString16 npos

extern "C" __declspec(dllimport) void *MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

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

// PDB ldata order in CMBStringTable.obj (0x1008f1b8..0x1008f258)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec =
    { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile =
    { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList =
    { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStringTable =
    { 0x8f1468d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBPref =
    { 0x79fc3030, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPref =
    { 0x7a00c5c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
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
            T *p;
            long hr = unk->QueryInterface(T::GetGUID(), (void **)&p);
            if (hr)
                Attach(0);
            else
                Attach(p);
            unk->Release();
            return hr;
        }
        return -2;
    }
    T *mInterface;
};

#include "IMBStream.h"

#include "IMBFileSpec.h"

extern "C" void MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);

// own copy: default arguments for the last parameter of GetString, which this unit's calls leave out; in IMBPref.h they would instantiate MBString16 in every unit (NOTES.md)
class IMBPref : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPref; }
    // overloads in reverse declaration order (see IMBFileSpec)
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString16 &,
                           const MBString16 & = MBString16()) = 0;                     // vftable+0x10
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString &,
                           const MBString & = MBString()) = 0;                         // vftable+0x0c
    virtual long GetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString16 &) = 0; // vftable+0x1c
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString &) = 0;   // vftable+0x18
    virtual long SetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
};

#include "../MBEngine/MBLocale.h"

#include "IMBStringTable.h"

class CMBStringTable : public IMBStringTable {
public:
    CMBStringTable(void);
    ~CMBStringTable(void);
protected:
    unsigned long _mRefCnt;              // +0x04
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool Load(const char *filename);
    virtual bool Load(unsigned long id, IMBStream *in);
    virtual bool GetNamedString(const MBString &name, MBString16 &ustring);
    virtual bool GetString(unsigned long id, MBString16 &ustring);
    virtual bool SetLocale(const MBLocale &locale);
protected:
    bool mNeedsReloading;                // +0x08
    MBComPtr<IMBStream> mStream;         // +0x0c
    MBComPtr<IMBPref> mPref;             // +0x10
    MBLocale mLocale;                    // +0x14
};

IMBUnknown *MakeCMBStringTable(void);

#endif
