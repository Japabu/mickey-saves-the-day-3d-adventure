// CMBPeriodicalManager unit declarations (MBEngine.dll)
#ifndef CMBPERIODICALMANAGER_H
#define CMBPERIODICALMANAGER_H

#include <fstream> // evidence: original TU has $E19/$E20 for std::ctype<unsigned short>::id, which <fstream> produces with matching numbering
#include <list>

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
    bool operator!=(const MBGUID &g) const { return !(*this == g); }
};

static const MBGUID MBIID_IMBUnknown = {0x59d231f0, 0x9c6c, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBPeriodical = {0x79529260, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBPeriodicalManager = {0x7958af20, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBPeriodical : public IMBUnknown {
public:
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

extern "C" IMBUnknown *__cdecl MBComUtilAddSingleton(IMBUnknown *unk);

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
    void construct(T *p, const T &v) { new ((void *)p) T(v); }
    void destroy(T *p) { p->~T(); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

class CMBPeriodicalManager : public IMBPeriodicalManager {
public:
    CMBPeriodicalManager(void);
    ~CMBPeriodicalManager(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void AddPeriodical(IMBPeriodical *periodical);
    virtual void RemovePeriodical(IMBPeriodical *periodical);
    virtual void DevoteTimeToPeriodicals(void);
protected:
    MBList<IMBPeriodical *> mPeriodicals;
};

IMBUnknown *MakeCMBPeriodicalManager(void);

#endif
