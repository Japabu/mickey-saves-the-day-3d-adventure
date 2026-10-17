// CMBWinDebugConsole unit declarations (MBEngine.dll)
#ifndef CMBWINDEBUGCONSOLE_H
#define CMBWINDEBUGCONSOLE_H

#include <fstream> // evidence: original TU has $E32/$E33 for std::ctype<unsigned short>::id
#include <list>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) char *__cdecl MBstrcpy(char *, const char *);
extern "C" __declspec(dllimport) unsigned int __cdecl MBstrlen(const char *);

#include "_RTL_CRITICAL_SECTION.h"
struct _SMALL_RECT {
    short Left;
    short Top;
    short Right;
    short Bottom;
};
extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall DeleteCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall EnterCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall LeaveCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) int __stdcall AllocConsole(void);
extern "C" __declspec(dllimport) int __stdcall FreeConsole(void);
extern "C" __declspec(dllimport) void *__stdcall GetStdHandle(unsigned long);
extern "C" __declspec(dllimport) int __stdcall SetConsoleWindowInfo(void *, int, const _SMALL_RECT *);
extern "C" __declspec(dllimport) int __stdcall ReadConsoleA(void *, void *, unsigned long, unsigned long *, void *);
extern "C" __declspec(dllimport) int __stdcall WriteConsoleA(void *, const void *, unsigned long, unsigned long *, void *);
extern "C" unsigned long __cdecl _beginthread(void(__cdecl *)(void *), unsigned int, void *);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

// own copy: this unit declares its kernel32 imports itself and does not open <windows.h>, which ../mbruntime/CMBCriticalSection.h includes
class CMBCriticalSection {
public:
    CMBCriticalSection(void) { InitializeCriticalSection(&mSection); }
    ~CMBCriticalSection(void) { DeleteCriticalSection(&mSection); }
    void Enter(void) { EnterCriticalSection(&mSection); }
    void Leave(void) { LeaveCriticalSection(&mSection); }
private:
    _RTL_CRITICAL_SECTION mSection;
};

#include "../MBCom/MBGUID.h"

// .rdata 0x1008f328..0x1008f388
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBPeriodical = {0x79529260, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBPeriodicalManager = {0x7958af20, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventListener = {0x799755d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDebugConsole = {0x7a100da0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};


extern "C" IMBUnknown *__cdecl MBComUtilAddSingleton(IMBUnknown *unk);
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "../MBEngine/MBRect.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

class IMBDebugConsole : public IMBUnknown {
public:
    virtual void PositionWindow(MBRect) = 0;
};

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
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
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    T *mInterface;
};

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

class CMBWinDebugConsole : public IMBDebugConsole, public IMBEventListener, public IMBPeriodical {
public:
    CMBWinDebugConsole(void);
protected:
    ~CMBWinDebugConsole(void);
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void PositionWindow(MBRect pos);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle(void);
protected:
    static bool __cdecl Write(const char *output);
    static void __cdecl Read(void *context);
    static void *sStdInput;
    static void *sStdOutput;
    static void *sStdError;
    static MBList<char *> sInput;
    static bool sParentIsActive;
    static bool sInputThreadIsActive;
    // stand-in: the original's Make guard is $S21 (all $Es one higher than ours): 6 class statics
    // + Make static leave one number unexplained before Make; one static data member declaration
    // takes it (name unknown; never defined or used)
    static bool sUnknownConsoleStatic;
};

IMBUnknown *MakeCMBWinDebugConsole(void);

#endif
