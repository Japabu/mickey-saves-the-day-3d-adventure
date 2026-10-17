// MBSoundUtils.h - declarations for MBEngine.dll MBSoundUtils.obj
#ifndef MBSOUNDUTILS_H
#define MBSOUNDUTILS_H

#include <string> // instantiates std::ctype<unsigned short>::id ($E19/$E20)

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// header-defined interface IDs (local data of every object that includes them)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStreamLocator = { 0x89cc0fe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceStreamLocator = { 0x89d22ca0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceStreamLocator = { 0x89d84960, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileStreamLocator = { 0x89de6620, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFileStreamLocator = { 0x89e482e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription = { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBSoundDescription = { 0x844fc290, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager = { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBStream;
class IMBFileSpec;

// own copy: carries the sUnknownStreamLocatorStatic $E stand-in (see NOTES.md)
class IMBStreamLocator : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStreamLocator; }
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) const = 0;
    // stand-in: the original's ctype id $E20/$E19 are one past the <string> baseline ($E19/$E18);
    // the obj's PDB data has no guard/static, and every STL unit including the IMBStreamLocator
    // header has the +1, so one static data member declaration there takes it (name unknown;
    // never defined or used)
    static const unsigned long sUnknownStreamLocatorStatic;
};

// own copy: derives from this unit's own IMBStreamLocator
class IMBResourceStreamLocator : public IMBStreamLocator {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceStreamLocator; }
    virtual long SetResource(unsigned long, unsigned long) = 0;
};

// own copy: derives from this unit's own IMBStreamLocator
class IMBFileStreamLocator : public IMBStreamLocator {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileStreamLocator; }
    virtual long SetFile(IMBFileSpec *) = 0;
};

#include "IMBSoundDescription.h"

#include "IMBSoundManager.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    operator bool(void) const { return mInterface != 0; }
    void Attach(T *p) {
        if (mInterface) mInterface->Release();
        mInterface = p;
    }
    void Release(void);
    long CreateObject(const MBGUID &clsid);
private:
    long _QueryInterface(IMBUnknown *const &p);
    T *mInterface;
};

// MBComPtr template members (emitted out of line where VC6 does not inline them).

// 0x10048140 ?CreateObject@?$MBComPtr@VIMBResourceStreamLocator@@@@QAEJABUMBGUID@@@Z
// 0x10048200 ?CreateObject@?$MBComPtr@VIMBSoundDescription@@@@QAEJABUMBGUID@@@Z
// 0x100482a0 ?CreateObject@?$MBComPtr@VIMBFileStreamLocator@@@@QAEJABUMBGUID@@@Z
template <class T>
long MBComPtr<T>::CreateObject(const MBGUID &clsid)
{
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

// 0x100481e0 ?Release@?$MBComPtr@VIMBSoundDescription@@@@QAEXXZ
template <class T>
void MBComPtr<T>::Release(void)
{
    if (mInterface) mInterface->Release();
    mInterface = 0;
}

// 0x10048340 ?_QueryInterface@?$MBComPtr@VIMBSoundDescription@@@@AAEJABQAVIMBUnknown@@@Z
template <class T>
long MBComPtr<T>::_QueryInterface(IMBUnknown *const &p)
{
    long hr;
    if (p) {
        T *pInterface;
        hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
        if (hr) {
            Release();
            return hr;
        }
        Attach(pInterface);
    } else {
        if (mInterface) {
            T *old = mInterface;
            mInterface = 0;
            old->Release();
        }
        hr = -3;
    }
    return hr;
}

extern "C" {
void MBPlaySound(const char *soundName, long loopCount);
void MBForceSound(const char *soundName, long channelID, long loopCount);
void MBQueueSound(const char *soundName, long channelID, long loopCount);
void MBPlaySoundID(unsigned long id, long loopCount);
void MBForceSoundID(unsigned long id, long channelID, long loopCount);
void MBQueueSoundID(unsigned long id, long channelID, long loopCount);
void MBPlaySoundFile(IMBFileSpec *fileSpec, long loopCount);
void MBForceSoundFile(IMBFileSpec *fileSpec, long channelID, long loopCount);
void MBQueueSoundFile(IMBFileSpec *fileSpec, long channelID, long loopCount);
}

#endif
