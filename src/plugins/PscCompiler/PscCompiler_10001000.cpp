// PscCompiler.dll 0x10001000-0x10001050
#include "PscCompiler_10001000_unit.h"

// 0x10001000 _DllMain@12 (unnamed; entry point called by the CRT startup)
int __stdcall DllMain(void *hinst, unsigned long reason, void *reserved)
{
    return 1;
}

// 0x10001010 _PscInitialize@8
extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *err)
{
    *err = NULL_STRING;
    MfPcCompiler::Initialize();
    return 1;
}

// 0x10001030 _PscShutdown@4
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err)
{
    *err = NULL_STRING;
    MfPcCompiler::Shutdown();
    return 1;
}
