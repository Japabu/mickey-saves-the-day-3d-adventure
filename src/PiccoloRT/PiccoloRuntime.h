#pragma once
#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

// The DLL's module handle (set by DllMain).
class __declspec(dllexport) PiccoloRuntime {
public:
    static HINSTANCE InstHandle();
private:
    friend BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
    friend class MfScFileProvider;
    static HINSTANCE hInstHandle;
};
