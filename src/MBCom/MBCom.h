// MBCom.dll: the muffin component object model (class factories, singletons,
// server DLLs). Shared declarations of MBCom.cpp and MBComFactory.cpp.
#ifndef MBCOM_H
#define MBCOM_H
#pragma warning(disable: 4786)
#include <windows.h>
#include <string>
#include <list>
#include <map>

extern "C" {
__declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
__declspec(dllimport) void __cdecl MBfree_release(void *ptr);
__declspec(dllimport) void __cdecl MBDebugStr(const char *fmt, ...);
}

// Runtime/MBNew.h: the global operators route to the release heap.
// 0x10001280 ??2@YAPAXI@Z (MBCom.obj)
inline void *__cdecl operator new(unsigned int size)
{
    return MBmalloc_release(size);
}

// 0x100012b0 ??3@YAXPAX@Z (MBCom.obj)
inline void __cdecl operator delete(void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &guid) const;
    bool operator!=(const MBGUID &guid) const;
};

// 0x100022d0 ??M@YA_NABUMBGUID@@0@Z (MBComFactory.obj)
inline bool operator<(const MBGUID &guid1, const MBGUID &guid2)
{
    if (guid1.data1 > guid2.data1)
        return false;
    if (guid1.data1 < guid2.data1)
        return true;
    if (guid1.data2 > guid2.data2)
        return false;
    if (guid1.data2 < guid2.data2)
        return true;
    if (guid1.data3 > guid2.data3)
        return false;
    if (guid1.data3 < guid2.data3)
        return true;
    if (*(unsigned long *)&guid1.data4[0] > *(unsigned long *)&guid2.data4[0])
        return false;
    if (*(unsigned long *)&guid1.data4[0] < *(unsigned long *)&guid2.data4[0])
        return true;
    return *(unsigned long *)&guid1.data4[4] < *(unsigned long *)&guid2.data4[4];
}

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

typedef IMBUnknown *(__cdecl *MBClassFactoryFunc)(void);

// No construct/destroy: the original instantiates std::_Construct for the map's pair.
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

// Registry of class factories (by class GUID) and of the singletons to release
// at shutdown. One instance, created by the first AddFactory.
class MBComFactory {
public:
    static IMBUnknown *__cdecl Make(const MBGUID &guid);
    static void __cdecl AddFactory(const MBGUID &guid, MBClassFactoryFunc func);
    static void __cdecl RemoveFactory(const MBGUID &guid, MBClassFactoryFunc func);
    static MBComFactory *__cdecl GetFactory(void);
    static MBClassFactoryFunc __cdecl FindFactory(const MBGUID &guid);
    static IMBUnknown *__cdecl AddSingleton(IMBUnknown *unk);
    ~MBComFactory(void);
protected:
    MBComFactory(const MBComFactory &);
    MBComFactory(void);
    MBMap<MBGUID, MBClassFactoryFunc, std::less<MBGUID> > mFactoryMap;
    MBList<IMBUnknown *> mSingletonList;
};

#endif
