// PORT: gRefCount is a 32-bit LONG, the width of the Interlocked functions that update it.
#include "MBCom.h"
#include <algorithm>

// From the Runtime stdlib header: the by-value MBString parameter instantiates
// MBString, whose npos takes one $E number before gLoadedLibraryList's (the
// original numbers the list's initializers $E17..$E20 and ctype's $E26/$E27).
typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
extern "C" __declspec(dllimport) void __cdecl MBsystem(MBString command);

typedef long (__cdecl *MBComServerFunc)(void);

MBList<HINSTANCE> gLoadedLibraryList;   // server DLLs, in load order
LONG gRefCount;   // PORT: LONG, not unsigned long (64 bits here), for the 32-bit Interlocked calls; MBComInitialize calls minus MBComUninitialize calls

// 0x100010c0 _MBComLoadServer
extern "C" long __cdecl MBComLoadServer(const char *server)
{
    if (gRefCount <= 0) {
        MBDebugStr("Error in MBComLoadServer: MBComInitialize has not been called.");
        return -1;
    }
    HINSTANCE lib = LoadLibrary(server);
    MBComServerFunc registerServer = (MBComServerFunc)GetProcAddress(lib, "MBComRegisterServer");
    if (registerServer) {
        if (std::find(gLoadedLibraryList.begin(), gLoadedLibraryList.end(), lib) == gLoadedLibraryList.end()) {
            registerServer();
            gLoadedLibraryList.push_back(lib);
        }
        return 0;
    }
    FreeLibrary(lib);
    return -5;
}

// 0x10001170 _MBComInitialize
extern "C" long __cdecl MBComInitialize(void)
{
    if (gRefCount)
        InterlockedIncrement((LPLONG)&gRefCount);
    else
        InterlockedIncrement((LPLONG)&gRefCount);
    return 0;
}

// 0x10001190 _MBComUninitialize
extern "C" long __cdecl MBComUninitialize(void)
{
    InterlockedDecrement((LPLONG)&gRefCount);
    if (gRefCount == 0) {
        MBList<HINSTANCE>::reverse_iterator it;
        for (it = gLoadedLibraryList.rbegin(); it != gLoadedLibraryList.rend(); ++it) {
            MBComServerFunc unregisterServer = (MBComServerFunc)GetProcAddress(*it, "MBComUnregisterServer");
            if (unregisterServer)
                unregisterServer();
        }
        delete MBComFactory::GetFactory();
        for (it = gLoadedLibraryList.rbegin(); it != gLoadedLibraryList.rend(); ++it)
            FreeLibrary(*it);
    }
    return 0;
}

// 0x10001220 _MBComRegisterClassObject
extern "C" long __cdecl MBComRegisterClassObject(const MBGUID &guid, MBClassFactoryFunc func)
{
    MBComFactory::AddFactory(guid, func);
    return 0;
}

// 0x10001240 _MBComUnregisterClassObject
extern "C" long __cdecl MBComUnregisterClassObject(const MBGUID &guid, MBClassFactoryFunc func)
{
    MBComFactory::RemoveFactory(guid, func);
    return 0;
}

// 0x10001260 _MBComCreateInstance
extern "C" long __cdecl MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown)
{
    IMBUnknown *p = MBComFactory::Make(guid);
    *unknown = p;
    return p ? 0 : -3;
}

// 0x10001290 _MBComGetClassObject
extern "C" long __cdecl MBComGetClassObject(const MBGUID &guid, MBClassFactoryFunc *func)
{
    MBClassFactoryFunc p = MBComFactory::FindFactory(guid);
    *func = p;
    return p ? 0 : -3;
}

// 0x100012d0 _MBComUtilAddSingleton
extern "C" IMBUnknown *__cdecl MBComUtilAddSingleton(IMBUnknown *unk)
{
    if (unk)
        MBComFactory::AddSingleton(unk);
    return unk;
}


// Matched by this code:
// 0x10001000 _$E20
// 0x10001010 _$E17
// 0x10001040 _$E19
// 0x10001050 _$E18
// 0x10001280 ??2@YAPAXI@Z
// 0x100012b0 ??3@YAXPAX@Z
// 0x100012f0 ?find@std@@YA?AViterator@?$list@PAUHINSTANCE__@@V?$MBAllocator@PAUHINSTANCE__@@@@@1@V231@0ABQAUHINSTANCE__@@@Z
// 0x10001320 ??9std@@YA_NABV?$reverse_bidirectional_iterator@Viterator@?$list@PAUHINSTANCE__@@V?$MBAllocator@PAUHINSTANCE__@@@@@std@@PAUHINSTANCE__@@AAPAU4@PAPAU4@H@0@0@Z
// 0x10001340 _$E27
// 0x10001360 _$E26
// 0x10001370 ?id@?$ctype@G@std@@$E