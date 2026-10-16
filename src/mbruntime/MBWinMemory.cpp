#include "MBWinMemory.h"

// 0x10001a60 _MBmalloc_release
extern "C" void *__cdecl MBmalloc_release(unsigned long size)
{
    if (size == 0)
        size = 4;
    void *p = GlobalAlloc(GMEM_FIXED, size);
    while (p == NULL && InvokeLowMemHandlers(size) == true)
        p = GlobalAlloc(GMEM_FIXED, size);
    return p;
}

// 0x10001aa0 _MBfree_release
extern "C" void __cdecl MBfree_release(void *ptr)
{
    GlobalFree(ptr);
}

// 0x10001ab0 _MBrealloc_release
extern "C" void *__cdecl MBrealloc_release(void *ptr, unsigned long size)
{
    if (size == 0)
        size = 4;
    void *p = GlobalReAlloc(ptr, size, 0);
    while (p == NULL && InvokeLowMemHandlers(size) == true)
        p = GlobalReAlloc(ptr, size, 0);
    return p;
}

// 0x10001b00 _MBmemcpy
extern "C" void __cdecl MBmemcpy(void *dest, const void *src, unsigned long cnt)
{
    memcpy(dest, src, cnt);
}

// 0x10001b20 _MBmemmove
extern "C" void __cdecl MBmemmove(void *dest, const void *src, unsigned long cnt)
{
    memmove(dest, src, cnt);
}

// 0x10001b40 _MBmemset
extern "C" void __cdecl MBmemset(void *dest, unsigned char fill, unsigned long cnt)
{
    memset(dest, fill, cnt);
}

// 0x10001b70 _MBmemclr
extern "C" void __cdecl MBmemclr(void *dest, unsigned long cnt)
{
    memset(dest, 0, cnt);
}
