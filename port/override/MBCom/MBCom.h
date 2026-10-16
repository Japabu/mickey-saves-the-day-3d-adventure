// PORT: MBGUID is the 16-byte GUID layout (32-bit data1), and operator< compares data4 as two 32-bit words.
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
__declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
__declspec(dllimport) void __cdecl MBfree_release(void *ptr);
__declspec(dllimport) void __cdecl MBDebugStr(const char *fmt, ...);
}

// Runtime/MBNew.h: the global operators route to the release heap.
// 0x10001280 ??2@YAPAXI@Z (MBCom.obj)
inline void *__cdecl operator new(size_t size)
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
    port_u32 data1;   // PORT: 32 bits, as in the GUID layout (C long is 64 here)
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &guid) const;
    bool operator!=(const MBGUID &guid) const;
};
static_assert(sizeof(MBGUID) == 16, "MBGUID is the 16-byte GUID layout");   // PORT

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
    // PORT: data4 as two 32-bit words, as the original read them (unsigned long is 8 bytes here)
    if (*(port_u32 *)&guid1.data4[0] > *(port_u32 *)&guid2.data4[0])
        return false;
    if (*(port_u32 *)&guid1.data4[0] < *(port_u32 *)&guid2.data4[0])
        return true;
    return *(port_u32 *)&guid1.data4[4] < *(port_u32 *)&guid2.data4[4];
}

class IMBUnknown {
public:
    virtual int QueryInterface(const MBGUID &iid, void **obj) = 0;
    virtual unsigned int AddRef(void) = 0;
    virtual unsigned int Release(void) = 0;
};

typedef IMBUnknown *(__cdecl *MBClassFactoryFunc)(void);

// No construct/destroy: the original instantiates std::_Construct for the map's pair.
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned int n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned int n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned int) { if (p) MBfree_release(p); }
    MBAllocator() {} template <class U> MBAllocator(const MBAllocator<U> &) {} template <class U> struct rebind { typedef MBAllocator<U> other; }; T *allocate(size_t n) { return (T *)MBmalloc_release(n * sizeof(T)); } };

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<std::pair<const K, V> > > {
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
