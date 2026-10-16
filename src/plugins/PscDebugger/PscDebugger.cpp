// PscDebugger.dll 0x1000b940-0x1000bbc7: the DLL and plugin entry points.
#include "PscDebugger.h"
#include <afxdllx.h>

// _ExtRawDllMain@12 at 0x1000b940 comes from afxdllx.h; it is claimed in
// ../PscClassInfo/PscClassInfo_1000209c.cpp (the same bytes).

// 0x10016f94
MfEvent *pdbUIReady;
// 0x10016f98
AFX_EXTENSION_MODULE PscDebuggerDLL = { NULL, NULL };
// 0x10016fac
HINSTANCE hPscDebugger;

// 0x1000b970 _DllMain@12
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        AfxInitExtensionModule(PscDebuggerDLL, hInstance);
        new CDynLinkLibrary(PscDebuggerDLL);
        pdbUIReady = new MfEvent;
        hPscDebugger = hInstance;
    } else if (dwReason == DLL_PROCESS_DETACH) {
        if (pdbUIReady)
            delete pdbUIReady;
    }
    return 1;
}

// _PscLoad@0 at 0x1000ba50 compiles to the same bytes as PscClassInfo's and is
// claimed there (../PscClassInfo/PscClassInfo_1000209c.cpp).
extern "C" __declspec(dllexport) int __stdcall PscLoad()
{
    MfAi *app;
    MfInterfaceRegistry::GetInterface("ApplicationI", &app);
    int result = 0;
    if (app) {
        if (((MfAiApplication *)app)->GetApplicationType() == 2)
            result = 1;
        app->Release();
    }
    return result;
}

// 0x1000bae0 _PscUnload@0
extern "C" __declspec(dllexport) int __stdcall PscUnload()
{
    return 1;
}

// 0x1000baf0 _PscInitialize@8
extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *errors)
{
    *errors = NULL_STRING;
    MfPdbDebugger *debugger = new MfPdbDebugger;
    debugger->Initialize();
    MfScDebugger::pdbgTheDebugger = debugger;
    return 1;
}

// 0x1000bb80 _PscShutdown@4
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *errors)
{
    *errors = NULL_STRING;
    MfAiPdbDebugger *debugger = pdbDebugger;
    if (debugger) {
        debugger->Shutdown();
        MfScDebugger::pdbgTheDebugger = 0;
        debugger->Release();
    }
    return 1;
}
