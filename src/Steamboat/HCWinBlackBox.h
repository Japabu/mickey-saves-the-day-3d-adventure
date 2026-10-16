#pragma once
#include <windows.h>
#include <imagehlp.h>

// The imagehlp.dll entry points, loaded at run time so that the game still
// starts on machines without the DLL.
typedef int (__stdcall *SymInitializeProc)(void *, char *, int);
typedef int (__stdcall *SymLoadModuleProc)(void *, void *, char *, char *, unsigned long, unsigned long);
typedef int (__stdcall *SymGetSymFromAddrProc)(void *, unsigned long, unsigned long *, _IMAGEHLP_SYMBOL *);
typedef int (__stdcall *SymGetLineFromAddrProc)(void *, unsigned long, unsigned long *, _IMAGEHLP_LINE *);
typedef int (__stdcall *SymCleanupProc)(void *);
typedef int (__stdcall *StackWalkProc)(unsigned long, void *, void *, _tagSTACKFRAME *, void *, int (__stdcall *)(void *, const void *, void *, unsigned long, unsigned long *), void *(__stdcall *)(void *, unsigned long), unsigned long (__stdcall *)(void *, unsigned long), unsigned long (__stdcall *)(void *, void *, _tagADDRESS *));
typedef void *(__stdcall *SymFunctionTableAccessProc)(void *, unsigned long);
typedef unsigned long (__stdcall *SymGetModuleBaseProc)(void *, unsigned long);
typedef int (__stdcall *SymGetModuleInfoProc)(void *, unsigned long, _IMAGEHLP_MODULE *);
typedef unsigned long (__stdcall *SymGetOptionsProc)(void);
typedef unsigned long (__stdcall *SymSetOptionsProc)(unsigned long);

struct SymbolFunctionMapper {
    HINSTANCE__ *mDLLHandle;
    bool mOK;
    SymInitializeProc SymInitialize;
    SymLoadModuleProc SymLoadModule;
    SymGetSymFromAddrProc SymGetSymFromAddr;
    SymGetLineFromAddrProc SymGetLineFromAddr;
    SymCleanupProc SymCleanup;
    StackWalkProc StackWalk;
    SymFunctionTableAccessProc SymFunctionTableAccess;
    SymGetModuleBaseProc SymGetModuleBase;
    SymGetModuleInfoProc SymGetModuleInfo;
    SymGetOptionsProc SymGetOptions;
    SymSetOptionsProc SymSetOptions;
};

static SymbolFunctionMapper *sMapper;

