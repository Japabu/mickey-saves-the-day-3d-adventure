#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "HCWinBlackBox.h"

// exception code the Microsoft C++ runtime raises for a throw ('msc' | 0xE0000000)
#define EXCEPTION_MSVC_CPP 0xE06D7363

const char *ExceptionStrings[21] = {
    "Access violation",
    "Array bounds exceeded",
    "Breakpoint hit",
    "Alignment error",
    "Floating-point error (EXCEPTION_FLT_DENORMAL_OPERAND)",
    "Floating-point divide-by-zero",
    "Floating-point inexact result",
    "Floating-point invalid operation",
    "Floating-point overflow",
    "Floating-point unit trashed the stack",
    "Floating-point underflow",
    "Illegal instruction",
    "Page not present (EXCEPTION_IN_PAGE_ERROR)",
    "Integer divide-by-zero",
    "Integer overflow",
    "EXCEPTION_INVALID_DISPOSITION",
    "Attempt to continue after non-continuable exception",
    "Privileged instruction",
    "Single-step",
    "Stack overflow",
    "Uncaught C++ exception",
};

// 0x004044e0 ?DebugLog@@YAXPBDZZ
void __cdecl DebugLog(const char *fmt, ...)
{
    char buffer[512];
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        int length = wvsprintf(buffer, fmt, args);
        FILE *file = fopen("log.txt", "a");
        fwrite(buffer, 1, length, file);
        fclose(file);
        OutputDebugString(buffer);
    }
}

// 0x00404550 ?GetNameFromPath@@YAPADPAD@Z
char *__cdecl GetNameFromPath(char *fullName)
{
    char *name = fullName;
    for (char *p = fullName; *p; ) {
        if (*p++ == '\\')
            name = p;
    }
    return name;
}

// 0x00404570 ?PrintFunctionFromAddress@@YAHPAXKAAPAU_IMAGEHLP_SYMBOL@@P6AXPBDZZ@Z
int __cdecl PrintFunctionFromAddress(void *symProcessProxy, unsigned long addr, _IMAGEHLP_SYMBOL *&imgSym, void (__cdecl *logFunc)(const char *, ...))
{
    IMAGEHLP_MODULE moduleInfo;
    moduleInfo.SizeOfStruct = sizeof(moduleInfo);
    if (!sMapper->SymGetModuleInfo(symProcessProxy, addr, &moduleInfo))
        lstrcpy(moduleInfo.ModuleName, "[unknown module]");
    int symbolFound = sMapper->SymGetSymFromAddr(symProcessProxy, addr, NULL, imgSym);
    if (symbolFound) {
        logFunc("%s", imgSym->Name);
        if (sMapper->SymGetLineFromAddr) {
            IMAGEHLP_LINE imgLine;
            unsigned long displacement;
            memset(&imgLine, 0, sizeof(imgLine));
            imgLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);
            imgLine.Address = addr;
            int lineFound = sMapper->SymGetLineFromAddr(symProcessProxy, addr, &displacement, &imgLine);
            if (lineFound)
                logFunc("  (%s, line %d) <%s>", GetNameFromPath(imgLine.FileName), imgLine.LineNumber, moduleInfo.ModuleName);
            return lineFound;
        }
    } else {
        logFunc("<%s>", moduleInfo.ModuleName);
    }
    return symbolFound;
}

// 0x00404680 ?InitProcess@@YAPAXXZ
void *__cdecl InitProcess(void)
{
    if (!sMapper) {
        SymbolFunctionMapper *mapper = (SymbolFunctionMapper *)GlobalAlloc(GMEM_FIXED, sizeof(SymbolFunctionMapper));
        if (mapper) {
            mapper->mOK = false;
            mapper->mDLLHandle = LoadLibrary("dbghelp.dll");
            if (!mapper->mDLLHandle)
                mapper->mDLLHandle = LoadLibrary("imagehlp.dll");
            if (mapper->mDLLHandle) {
                mapper->mOK = true;
                if ((mapper->SymInitialize = (SymInitializeProc)GetProcAddress(mapper->mDLLHandle, "SymInitialize")) != 0
                    && (mapper->SymLoadModule = (SymLoadModuleProc)GetProcAddress(mapper->mDLLHandle, "SymLoadModule")) != 0
                    && (mapper->SymGetSymFromAddr = (SymGetSymFromAddrProc)GetProcAddress(mapper->mDLLHandle, "SymGetSymFromAddr")) != 0
                    && (mapper->SymCleanup = (SymCleanupProc)GetProcAddress(mapper->mDLLHandle, "SymCleanup")) != 0
                    && (mapper->StackWalk = (StackWalkProc)GetProcAddress(mapper->mDLLHandle, "StackWalk")) != 0
                    && (mapper->SymFunctionTableAccess = (SymFunctionTableAccessProc)GetProcAddress(mapper->mDLLHandle, "SymFunctionTableAccess")) != 0
                    && (mapper->SymGetModuleBase = (SymGetModuleBaseProc)GetProcAddress(mapper->mDLLHandle, "SymGetModuleBase")) != 0
                    && (mapper->SymGetModuleInfo = (SymGetModuleInfoProc)GetProcAddress(mapper->mDLLHandle, "SymGetModuleInfo")) != 0
                    && (mapper->SymGetOptions = (SymGetOptionsProc)GetProcAddress(mapper->mDLLHandle, "SymGetOptions")) != 0
                    && (mapper->SymSetOptions = (SymSetOptionsProc)GetProcAddress(mapper->mDLLHandle, "SymSetOptions")) != 0)
                    mapper->mOK = true;
                // optional: older imagehlp.dll versions have no line information
                mapper->SymGetLineFromAddr = (SymGetLineFromAddrProc)GetProcAddress(mapper->mDLLHandle, "SymGetLineFromAddr");
            }
            if (!mapper->mOK)
                DebugLog("WARN: HCWinBlackBox: unable to find either imagehlp.dll, or the combo of dbghelp.dll & msdbi.dll.\n");
            sMapper = mapper;
        } else {
            sMapper = 0;
        }
    }
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
}

// 0x004047e0 ?InitSymbols@@YA_NPBDPAX@Z
bool __cdecl InitSymbols(const char *gameDLLName, void *symProcessProxy)
{
    if (!symProcessProxy)
        return false;
    if (!sMapper->mOK) {
        DebugLog("             Imagehlp.dll (or dbghelp.dll AND msdbi.dll) not installed, unable to load symbol info.\n");
        return false;
    }
    sMapper->SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST);
    char name[261];
    GetModuleFileName(0, name, MAX_PATH);
    *GetNameFromPath(name) = 0;
    lstrcat(name, ";.;");
    if (gameDLLName) {
        *GetNameFromPath((char *)gameDLLName) = 0;
        lstrcat(name, gameDLLName);
    }
    if (sMapper->SymInitialize(symProcessProxy, name, TRUE))
        return true;
    DebugLog("             SymInitialize failed (GLE=0x%08X).\n", GetLastError());
    return false;
}

// 0x004048c0 ?CloseSymbols@@YAXPAX@Z
void __cdecl CloseSymbols(void *symProcessProxy)
{
    if (symProcessProxy)
        sMapper->SymCleanup(symProcessProxy);
}

// 0x004048e0 ?CloseProcess@@YAXPAX@Z
void __cdecl CloseProcess(void *process)
{
    if (process)
        CloseHandle(process);
    if (sMapper) {
        SymbolFunctionMapper *mapper = sMapper;
        if (mapper->mDLLHandle) {
            FreeLibrary(mapper->mDLLHandle);
            mapper->mDLLHandle = 0;
        }
        GlobalFree(mapper);
        sMapper = 0;
    }
}

// 0x00404930 ?FillCallStack@@YAHPAX0PAU_CONTEXT@@PAKH@Z
int __cdecl FillCallStack(void *process, void *thread, _CONTEXT *threadContext, unsigned long *entries, int nEntries)
{
    if (!process || !entries || !nEntries)
        return 0;
    if (!sMapper->mOK) {
        DebugLog("             Imagehlp.dll (or dbghelp.dll AND msdbi.dll) not installed, unable to load symbol info.\n");
        return 0;
    }
    STACKFRAME stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));
    if (threadContext) {
        stackFrame.AddrPC.Offset = threadContext->Eip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = threadContext->Esp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = threadContext->Ebp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
    }
    int i;
    for (i = 0; i < nEntries; i++) {
        if (!sMapper->StackWalk(IMAGE_FILE_MACHINE_I386, process, thread, &stackFrame, 0, 0,
                                sMapper->SymFunctionTableAccess, sMapper->SymGetModuleBase, 0))
            break;
        entries[i] = stackFrame.AddrPC.Offset;
    }
    return i;
}

// 0x00404a20 ?BlackBoxExceptionFilter@@YGJPBDPAU_EXCEPTION_POINTERS@@@Z
long __stdcall BlackBoxExceptionFilter(const char *gameDLLName, _EXCEPTION_POINTERS *info)
{
    static bool sBeenHere;
    if (!sBeenHere) {
        sBeenHere = true;
        DebugLog("FATAL ERROR: Unhandled exception.\n");
        DebugLog("             Program address: 0x%08X.\n", info->ExceptionRecord->ExceptionAddress);
        DebugLog("             Offending thread: 0x%X.\n", GetCurrentThreadId());
        switch (info->ExceptionRecord->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            DebugLog("             Exception: Access violation. Attempted to %s data at 0x%08X.\n",
                     info->ExceptionRecord->ExceptionInformation[0] ? "write" : "read",
                     info->ExceptionRecord->ExceptionInformation[1]);
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            DebugLog("             Exception: %s.\n", ExceptionStrings[1]);
            break;
        case EXCEPTION_BREAKPOINT:
            DebugLog("             Exception: %s.\n", ExceptionStrings[2]);
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            DebugLog("             Exception: %s.\n", ExceptionStrings[3]);
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            DebugLog("             Exception: %s.\n", ExceptionStrings[4]);
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            DebugLog("             Exception: %s.\n", ExceptionStrings[5]);
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            DebugLog("             Exception: %s.\n", ExceptionStrings[6]);
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            DebugLog("             Exception: %s.\n", ExceptionStrings[7]);
            break;
        case EXCEPTION_FLT_OVERFLOW:
            DebugLog("             Exception: %s.\n", ExceptionStrings[8]);
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            DebugLog("             Exception: %s.\n", ExceptionStrings[9]);
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            DebugLog("             Exception: %s.\n", ExceptionStrings[10]);
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            DebugLog("             Exception: %s.\n", ExceptionStrings[11]);
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            DebugLog("             Exception: %s.\n", ExceptionStrings[12]);
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            DebugLog("             Exception: %s.\n", ExceptionStrings[13]);
            break;
        case EXCEPTION_INT_OVERFLOW:
            DebugLog("             Exception: %s.\n", ExceptionStrings[14]);
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            DebugLog("             Exception: %s.\n", ExceptionStrings[15]);
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            DebugLog("             Exception: %s.\n", ExceptionStrings[16]);
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            DebugLog("             Exception: %s.\n", ExceptionStrings[17]);
            break;
        case EXCEPTION_SINGLE_STEP:
            DebugLog("             Exception: %s.\n", ExceptionStrings[18]);
            break;
        case EXCEPTION_STACK_OVERFLOW:
            // no stack left to walk
            DebugLog("             Exception: %s.\n", ExceptionStrings[19]);
            return 0;
        case EXCEPTION_MSVC_CPP:
            DebugLog("             Exception: %s.\n", ExceptionStrings[20]);
            break;
        default:
            DebugLog("             Exception type unknown.\n");
            break;
        }

        bool isNT = false;
        {
            OSVERSIONINFO info;
            info.dwOSVersionInfoSize = sizeof(info);
            if (GetVersionEx(&info)) {
                switch (info.dwPlatformId) {
                case VER_PLATFORM_WIN32_NT:
                    DebugLog("             OS info: Windows NT %d.%d (build %d),", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);
                    isNT = true;
                    break;
                case VER_PLATFORM_WIN32_WINDOWS:
                    if (info.dwMinorVersion > 0)
                        DebugLog("             OS info: Windows 98 %d.%d (build %d).", HIWORD(info.dwBuildNumber) >> 8, HIWORD(info.dwBuildNumber) & 0xff, LOWORD(info.dwBuildNumber));
                    else
                        DebugLog("             OS info: Windows 95 %d.%d (build %d).", HIWORD(info.dwBuildNumber) >> 8, HIWORD(info.dwBuildNumber) & 0xff, LOWORD(info.dwBuildNumber));
                    break;
                case VER_PLATFORM_WIN32s:
                    DebugLog("             OS info: Windows 3.1 with Win32s (?!) build=%d major=%d minor=%d.", info.dwBuildNumber, info.dwMajorVersion, info.dwMinorVersion);
                    break;
                default:
                    DebugLog("             OS info: Unknown.  build=%d major=%d minor=%d.", info.dwBuildNumber, info.dwMajorVersion, info.dwMinorVersion);
                    break;
                }
                if (info.szCSDVersion[0])
                    DebugLog("  %s", info.szCSDVersion);
                DebugLog("\n");
            }
        }

        void *process = InitProcess();
        if (process) {
            // imagehlp wants the process handle on NT and the process id on Windows 9x
            void *symProcessProxy = isNT ? process : (void *)GetCurrentProcessId();
            if (InitSymbols(gameDLLName, symProcessProxy)) {
                unsigned long stack[64];
                int n = FillCallStack(process, GetCurrentThread(), info->ContextRecord, stack, 64);
                if (n > 0) {
                    char buf[1048];
                    _IMAGEHLP_SYMBOL *imgSym = (_IMAGEHLP_SYMBOL *)buf;
                    memset(buf, 0, sizeof(buf));
                    imgSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
                    imgSym->MaxNameLength = 1023;
                    for (int i = 0; i < n; i++) {
                        DebugLog("%-15c0x%08X  ", ' ', stack[i]);
                        PrintFunctionFromAddress(symProcessProxy, stack[i], imgSym, DebugLog);
                        DebugLog("\n");
                    }
                }
                CloseSymbols(symProcessProxy);
            }
            CloseProcess(process);
        }
    }
    return 0;
}
