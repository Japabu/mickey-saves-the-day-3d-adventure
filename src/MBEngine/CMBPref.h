// CMBPref.h - declarations for MBEngine.dll CMBPref.obj (the .ini style preference file)
#ifndef CMBPREF_H
#define CMBPREF_H

#include "MBStl.h"
#include <stdio.h>
#include <string> // evidence: ctype $E24/$E25 = <string> baseline + MBString/MBString16 npos + two maps
#include <map>

extern "C" __declspec(dllimport) void *MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) long MBatol(const char *s);

inline void *operator new(unsigned int size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// PDB ldata order in CMBPref.obj (0x1008f628..0x1008f678)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBPref =
    { 0x7a00c5c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFile =
    { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec =
    { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// No construct/destroy: the original instantiates std::_Construct for the maps' pairs
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

extern "C" void MB_utf8_to_ucs2(const MBString &utf8, MBString16 &ucs2);
extern "C" void MB_ucs2_to_utf8(const MBString16 &ucs2, MBString &utf8);


#include "IMBStreamSeekValues.h"

#include "IMBStreamError.h"

#include "IMBStream.h"

class IMBFileSpec;

#include "IMBFile.h"

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
            AddRef();
            do { // inline budget: same code, more nodes (SetString)
                if (old != 0)
                    old->Release();
            } while (0);
        }
        return *this;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
private:
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

#include "IMBPref.h"

class CMBPref : public IMBPref {
public:
    CMBPref(void);
protected:
    ~CMBPref(void);
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long GetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString &dest, const MBString &deflt);
    virtual long GetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString16 &dest, const MBString16 &deflt);
    virtual long GetLong(IMBStream *stream, const MBString &section, const MBString &keyName, long deflt);
    virtual long SetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString16 &value);
    virtual long SetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString &value);
    virtual long SetLong(IMBStream *stream, const MBString &section, const MBString &keyName, long value);
protected:
    virtual bool ApplyChanges(void);
    typedef MBMap<MBString, MBString *, std::less<MBString> > MBPrefSectionList;
    typedef MBMap<MBString, MBPrefSectionList *, std::less<MBString> > MBPrefTree;
    void SetListLong(MBPrefSectionList *, MBString &, long);
    void SetListString(MBPrefSectionList *, MBString &, MBString &);
    void ReadStreamIntoTree(void);
    bool ReadTokenToEOL(MBString &buffer, unsigned long &startPos, char stopChar, MBString &dest);
    void StripChar(char vchar, MBString &buffer);
    void TrimLeadingWhiteSpace(MBString &buffer);
    void TrimTrailingWhiteSpace(MBString &buffer);
    void CleanUpTree(void);
    MBComPtr<IMBStream> mStream;
    MBPrefTree mPrefTree;
    bool mHasChanged;
};

IMBUnknown *MakeCMBPref(void);

#endif
