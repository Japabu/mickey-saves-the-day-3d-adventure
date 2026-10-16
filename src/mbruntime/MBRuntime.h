// mbruntime.dll: the muffin runtime (memory, strings, stdlib, debug) that every
// other MB binary imports. Shared declarations of the Runtime units.
#ifndef MBRUNTIME_H
#define MBRUNTIME_H
#pragma warning(disable: 4786)
#include <windows.h>
// The STL is used without exceptions: try/catch in the library templates
// compile to plain blocks. Evidence: basic_string::_Copy has no EH frame and is
// inlined into _Grow (a function with try is never inlined by VC6), and the
// binary is built without /GX.
#include <xstddef>
#undef _TRY_BEGIN
#undef _CATCH
#undef _CATCH_ALL
#undef _CATCH_END
#define _TRY_BEGIN {
#define _CATCH(x) } if (0) {
#define _CATCH_ALL } if (0) {
#define _CATCH_END }
#include <string>

extern "C" {
void *__cdecl MBmalloc_release(unsigned long size);
void __cdecl MBfree_release(void *ptr);
void *__cdecl MBrealloc_release(void *ptr, unsigned long size);
}

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

extern "C" void __cdecl MBsystem(MBString command);

extern "C" void __cdecl MBDebugBreak(void);

// MBMemory.cpp
bool InvokeLowMemHandlers(unsigned long size);

// Runtime/Win: a Win32 critical section.
class CMBCriticalSection {
public:
    CMBCriticalSection(void) { InitializeCriticalSection(&mSection); }
    ~CMBCriticalSection(void) { DeleteCriticalSection(&mSection); }
    void Enter(void) { EnterCriticalSection(&mSection); }
    void Leave(void) { LeaveCriticalSection(&mSection); }
private:
    CRITICAL_SECTION mSection;
};

class CMBCriticalSectionLock {
    CMBCriticalSection &mSection;
public:
    CMBCriticalSectionLock(CMBCriticalSection &section) : mSection(section) { mSection.Enter(); }
    ~CMBCriticalSectionLock(void) { mSection.Leave(); }
};

#endif
