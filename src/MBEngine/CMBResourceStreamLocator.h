// CMBResourceStreamLocator.h - declarations for MBEngine.dll CMBResourceStreamLocator.obj
#ifndef CMBRESOURCESTREAMLOCATOR_H
#define CMBRESOURCESTREAMLOCATOR_H

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

static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList = { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr = { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStreamLocator = { 0x89cc0fe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceStreamLocator = { 0x89d22ca0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBMemoryBlock;
class IMBStream;

class IMBResourceFile : public IMBUnknown {
public:
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

class IMBResourceFileList : public IMBResourceFile {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFileList; }
};

class IMBStreamLocator : public IMBUnknown {
public:
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) = 0;
};

class IMBResourceStreamLocator : public IMBStreamLocator {
public:
    virtual long SetResource(unsigned long, unsigned long) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Attach(T *p) {
        if (mInterface) mInterface->Release();
        mInterface = p;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
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
    long _QueryInterface(IMBUnknown *const &p);
    T *mInterface;
};

template <class T>
long MBComPtr<T>::_QueryInterface(IMBUnknown *const &p)
{
    long hr = -3;
    if (p) {
        T *pInterface;
        hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
        if (hr) {
            Release();
            return hr;
        }
        Attach(pInterface);
        return hr;
    }
    if (mInterface) {
        T *old = mInterface;
        mInterface = 0;
        old->Release();
    }
    return hr;
}

class CMBResourceStreamLocator : public IMBResourceStreamLocator {
public:
    CMBResourceStreamLocator(void);
    ~CMBResourceStreamLocator(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long GetStream(IMBStream **stream);
    virtual unsigned long GetID(void);
    virtual long SetResource(unsigned long resID, unsigned long type);
protected:
    unsigned long mResID;
    unsigned long mType;
};

IMBUnknown *MakeCMBResourceStreamLocator(void);

#endif
