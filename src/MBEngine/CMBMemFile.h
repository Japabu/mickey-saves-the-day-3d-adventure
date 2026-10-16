// CMBMemFile.h - declarations for MBEngine.dll CMBMemFile.obj
// (core declarations copied from CMBWinFile.h; this object has no STL $E
// initializers, so no <string>/<windows.h> here)
#ifndef CMBMEMFILE_H
#define CMBMEMFILE_H

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) void *__cdecl MBmemcpy(void *dst, const void *src, unsigned int n);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
// the scalar deleting destructor tests the pointer before MBfree_release
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

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

// header-defined interface IDs (PDB ldata 0x10089f70..0x10089fc0)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
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

enum IMBStreamSeekValues {
    kIMBStreamSeekStart = 1,
    kIMBStreamSeekEnd = 2,
    kIMBStreamSeekCurrent = 3
};

enum IMBStreamError {
    kIMBStreamNoError = 0,
    kIMBStreamReadError = 1,
    kIMBStreamWriteError = 2,
    kIMBStreamOpenError = 3,
    kIMBStreamCloseError = 4,
    kIMBStreamSeekError = 5,
    kIMBStreamEOF = 6
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

class IMBMemoryBlock : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMemoryBlock; }
    virtual bool Allocate(unsigned long) = 0;
    virtual bool Grow(unsigned long) = 0;
    virtual unsigned char *GetData(void) = 0;
    virtual unsigned long GetSize(void) = 0;
};

// smart pointer (as in MBSoundEngine.h: Release inline, but the inline budget
// runs out inside an inlined CreateObject, giving out-of-line Release calls)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
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
    MBComPtr &operator=(const MBComPtr &p) { return *this = p.mInterface; }
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
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        Attach(0);
        return hr;
    }
    T *mInterface;
};

template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }

class IMBMemStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMemStream; }
    virtual MBComPtr<IMBMemoryBlock> GetMemoryBlock(void) = 0;
    virtual void SetMemoryBlock(const MBComPtr<IMBMemoryBlock> &) = 0;
};

class CMBMemStream : public IMBStream, private IMBMemStream {
public:
    MBComPtr<IMBMemoryBlock> mData;
    unsigned char *mOrigin;
    unsigned char *mBuffer;
    unsigned int mBytesAvail;
    unsigned int mSize;
    long mError;
    CMBMemStream(void) : mOrigin(0), mBuffer(0), mBytesAvail(0), mSize(0), mError(0) { _mRefCnt = 0; }
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual ~CMBMemStream(void) { Reset(); }
    void CopyBytes(void *dst, const void *src, unsigned long n);
    void Reset(void);
    virtual void SetMemoryBlock(const MBComPtr<IMBMemoryBlock> &data);
    virtual MBComPtr<IMBMemoryBlock> GetMemoryBlock(void);
    virtual void Clone(IMBStream **clone);
    virtual bool Flush(void);
    virtual bool Read(void *ptr, unsigned long &nBytes);
    bool Grow(unsigned long newSize);
    virtual bool Write(const void *ptr, unsigned long nBytes);
    virtual bool Seek(long offset, IMBStreamSeekValues origin);
    virtual unsigned long GetPosition(void);
    virtual bool SetLastError(IMBStreamError err);
    virtual IMBStreamError GetLastError(void);
    virtual unsigned long GetStreamSize(void);
};

inline void CMBMemStream::Reset(void)
{
    mData = 0;
    mOrigin = mBuffer = 0;
    mBytesAvail = mSize = 0;
    mError = 0;
}

inline void CMBMemStream::CopyBytes(void *dst, const void *src, unsigned long n)
{
    switch (n) {
    case 8:
        *(long *)dst = *(const long *)src;
        dst = (long *)dst + 1;
        src = (const long *)src + 1;
        // fall through
    case 4:
        *(long *)dst = *(const long *)src;
        break;
    case 2:
        *(short *)dst = *(const short *)src;
        break;
    case 1:
        *(char *)dst = *(const char *)src;
        break;
    default:
        MBmemcpy(dst, src, n);
        break;
    }
}

IMBUnknown *MakeCMBMemStream(void);

#endif
