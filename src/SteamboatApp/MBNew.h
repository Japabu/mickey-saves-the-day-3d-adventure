#pragma once
// muffin/include/Runtime/MBNew.h: the engine's allocator behind new and delete.
extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);

inline void *operator new(unsigned int size)
{
    return MBmalloc_release(size);
}

inline void operator delete(void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}
