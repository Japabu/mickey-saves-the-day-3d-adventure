// MBSoundTracker.h - declarations for MBEngine.dll MBSoundTracker.obj
#ifndef MBSOUNDTRACKER_H
#define MBSOUNDTRACKER_H

#include <string> // evidence: $E20/$E21 = <string> baseline 18/19 + 2 for the map's _Nil/_Nilrefs
#include <map>

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

// PDB ldata order, MBSoundTracker.obj 0x1008e638..0x1008e688
static const MBGUID MBIID_IMBUnknown = {0x59d231f0, 0x9c6c, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBResource = {0x89981b80, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventListener = {0x799755d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSoundDescription = {0x84481ea0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "IMBEventListener.h"

#include "IMBEventManager.h"

class IMBStreamLocator;

#include "IMBSoundDescription.h"

#include "SoundEndedEvent.h"

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
            if (hr)
                Attach(0);
            else
                Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

// No construct/destroy: the original instantiates std::_Construct for the map's pair
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

class MBSoundTracker : public IMBEventListener {
public:
    MBSoundTracker(void);
    ~MBSoundTracker(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual IMBSoundDescription *FindSound(unsigned long id);
protected:
    virtual void AddSound(IMBSoundDescription *sound);
    virtual void RemoveSound(IMBSoundDescription *sound);
    typedef MBMap<unsigned long, IMBSoundDescription *, std::less<unsigned long> > SoundList;
    SoundList mSoundList;
};

#endif
