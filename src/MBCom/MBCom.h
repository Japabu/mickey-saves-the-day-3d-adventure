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

#include "MBGUID.h"
#include "IMBUnknown.h"

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
