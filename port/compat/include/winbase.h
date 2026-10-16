// winbase.h - kernel32 for the macOS port (written for the port): files, memory,
// processes and threads, synchronisation, time, modules and resources, locales.
// The functions are implemented in port/platform/.
#ifndef PORT_WINBASE_H
#define PORT_WINBASE_H

#include "windef.h"
#include <stdarg.h>
#include <string.h>

PORT_BEGIN_C

// Errors.
#define ERROR_SUCCESS 0
#define NO_ERROR 0
#define ERROR_INVALID_FUNCTION 1
#define ERROR_FILE_NOT_FOUND 2
#ifndef ERROR_RESOURCE_DATA_NOT_FOUND
#define ERROR_RESOURCE_DATA_NOT_FOUND 1812
#define ERROR_RESOURCE_TYPE_NOT_FOUND 1813
#define ERROR_RESOURCE_NAME_NOT_FOUND 1814
#define ERROR_RESOURCE_LANG_NOT_FOUND 1815
#endif
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_TOO_MANY_OPEN_FILES 4
#define ERROR_ACCESS_DENIED 5
#define ERROR_INVALID_HANDLE 6
#define ERROR_NOT_ENOUGH_MEMORY 8
#define ERROR_OUTOFMEMORY 14
#define ERROR_INVALID_DRIVE 15
#define ERROR_NO_MORE_FILES 18
#define ERROR_WRITE_PROTECT 19
#define ERROR_NOT_READY 21
#define ERROR_SHARING_VIOLATION 32
#define ERROR_HANDLE_EOF 38
#define ERROR_FILE_EXISTS 80
#define ERROR_INVALID_PARAMETER 87
#define ERROR_BROKEN_PIPE 109
#define ERROR_CALL_NOT_IMPLEMENTED 120
#define ERROR_INSUFFICIENT_BUFFER 122
#define ERROR_MOD_NOT_FOUND 126
#define ERROR_PROC_NOT_FOUND 127
#define ERROR_ALREADY_EXISTS 183
#define ERROR_MORE_DATA 234
#define ERROR_NO_MORE_ITEMS 259
#define ERROR_CLASS_ALREADY_EXISTS 1410
#define ERROR_CANCELLED 1223
#define WAIT_OBJECT_0 0x00000000
#define WAIT_ABANDONED 0x00000080
#define WAIT_TIMEOUT 0x00000102
#define WAIT_FAILED 0xFFFFFFFF
#define INFINITE 0xFFFFFFFF
#define STILL_ACTIVE 0x00000103

// Files.
#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_ALL 0x10000000
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define FILE_FLAG_OVERLAPPED 0x40000000
#define FILE_FLAG_NO_BUFFERING 0x20000000
#define FILE_FLAG_RANDOM_ACCESS 0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define FILE_TYPE_UNKNOWN 0x0000
#define FILE_TYPE_DISK 0x0001
#define FILE_TYPE_CHAR 0x0002
#define FILE_TYPE_PIPE 0x0003
#define STD_INPUT_HANDLE ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE ((DWORD)-12)
#define DRIVE_UNKNOWN 0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE 2
#define DRIVE_FIXED 3
#define DRIVE_REMOTE 4
#define DRIVE_CDROM 5
#define DRIVE_RAMDISK 6
#define OF_READ 0x00000000
#define OF_WRITE 0x00000001
#define OF_READWRITE 0x00000002
#define OF_SHARE_COMPAT 0x00000000
#define OF_SHARE_EXCLUSIVE 0x00000010
#define OF_SHARE_DENY_WRITE 0x00000020
#define OF_SHARE_DENY_READ 0x00000030
#define OF_SHARE_DENY_NONE 0x00000040
#define OF_CREATE 0x00001000
#define OF_EXIST 0x00004000
#define OFS_MAXPATHNAME 128
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_GUARD 0x100
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define MEM_RELEASE 0x8000
#define MEM_FREE 0x10000
#define MEM_PRIVATE 0x20000
#define MEM_TOP_DOWN 0x100000
#define SECTION_MAP_READ 0x0004
#define SECTION_MAP_WRITE 0x0002
#define FILE_MAP_COPY 0x0001
#define FILE_MAP_WRITE 0x0002
#define FILE_MAP_READ 0x0004
#define FILE_MAP_ALL_ACCESS 0x000f001f

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[MAX_PATH];
    CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
typedef WIN32_FIND_DATAA WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _OFSTRUCT {
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT, *POFSTRUCT;

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR szCSDVersion[128];
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;
typedef OSVERSIONINFOA OSVERSIONINFO, *POSVERSIONINFO, *LPOSVERSIONINFO;
#define VER_PLATFORM_WIN32s 0
#define VER_PLATFORM_WIN32_WINDOWS 1
#define VER_PLATFORM_WIN32_NT 2

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;
#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN 0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2

// Processes and threads.
typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;
typedef STARTUPINFOA STARTUPINFO, *LPSTARTUPINFO;
#define STARTF_USESHOWWINDOW 0x00000001
#define STARTF_USESTDHANDLES 0x00000100

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
#define CREATE_SUSPENDED 0x00000004
#define DETACHED_PROCESS 0x00000008
#define CREATE_NEW_CONSOLE 0x00000010
#define NORMAL_PRIORITY_CLASS 0x00000020
#define IDLE_PRIORITY_CLASS 0x00000040
#define HIGH_PRIORITY_CLASS 0x00000080
#define REALTIME_PRIORITY_CLASS 0x00000100
#define THREAD_PRIORITY_LOWEST (-2)
#define THREAD_PRIORITY_BELOW_NORMAL (-1)
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST 2
#define THREAD_PRIORITY_ABOVE_NORMAL 1
#define THREAD_PRIORITY_TIME_CRITICAL 15
#define THREAD_PRIORITY_IDLE (-15)
#define PROCESS_TERMINATE 0x0001
#define PROCESS_QUERY_INFORMATION 0x0400
#define PROCESS_ALL_ACCESS 0x001F0FFF
#define SYNCHRONIZE 0x00100000
#define DUPLICATE_CLOSE_SOURCE 0x00000001
#define DUPLICATE_SAME_ACCESS 0x00000002
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)

// Exceptions.
#define EXCEPTION_MAXIMUM_PARAMETERS 15
typedef struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;
// The register state of a 32-bit x86 thread as the crash handler reads it (Eip, Esp,
// Ebp). Nothing on arm64 fills it in; the port's crash handler reports its own way.
typedef struct _CONTEXT {
    DWORD ContextFlags;
    DWORD Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
    BYTE FloatSave[112];
    DWORD SegGs, SegFs, SegEs, SegDs;
    DWORD Edi, Esi, Ebx, Edx, Ecx, Eax;
    DWORD Ebp, Eip, SegCs, EFlags, Esp, SegSs;
    BYTE ExtendedRegisters[512];
} CONTEXT, *PCONTEXT, *LPCONTEXT;
#define CONTEXT_i386 0x00010000
#define CONTEXT_CONTROL (CONTEXT_i386 | 0x00000001L)
#define CONTEXT_INTEGER (CONTEXT_i386 | 0x00000002L)
#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_i386 | 0x00000004L)
BOOL WINAPI GetThreadContext(HANDLE hThread, LPCONTEXT lpContext);
typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS, *LPEXCEPTION_POINTERS;
typedef LONG (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;
#define EXCEPTION_EXECUTE_HANDLER 1
#define EXCEPTION_CONTINUE_SEARCH 0
#define EXCEPTION_CONTINUE_EXECUTION (-1)
#define EXCEPTION_NONCONTINUABLE 0x1
#define EXCEPTION_ACCESS_VIOLATION ((DWORD)0xC0000005)
#define EXCEPTION_BREAKPOINT ((DWORD)0x80000003)
#define EXCEPTION_INT_DIVIDE_BY_ZERO ((DWORD)0xC0000094)
#define EXCEPTION_STACK_OVERFLOW ((DWORD)0xC00000FD)
#define EXCEPTION_DATATYPE_MISALIGNMENT ((DWORD)0x80000002)
#define EXCEPTION_SINGLE_STEP ((DWORD)0x80000004)
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED ((DWORD)0xC000008C)
#define EXCEPTION_FLT_DENORMAL_OPERAND ((DWORD)0xC000008D)
#define EXCEPTION_FLT_DIVIDE_BY_ZERO ((DWORD)0xC000008E)
#define EXCEPTION_FLT_INEXACT_RESULT ((DWORD)0xC000008F)
#define EXCEPTION_FLT_INVALID_OPERATION ((DWORD)0xC0000090)
#define EXCEPTION_FLT_OVERFLOW ((DWORD)0xC0000091)
#define EXCEPTION_FLT_STACK_CHECK ((DWORD)0xC0000092)
#define EXCEPTION_FLT_UNDERFLOW ((DWORD)0xC0000093)
#define EXCEPTION_INT_OVERFLOW ((DWORD)0xC0000095)
#define EXCEPTION_PRIV_INSTRUCTION ((DWORD)0xC0000096)
#define EXCEPTION_IN_PAGE_ERROR ((DWORD)0xC0000006)
#define EXCEPTION_ILLEGAL_INSTRUCTION ((DWORD)0xC000001D)
#define EXCEPTION_NONCONTINUABLE_EXCEPTION ((DWORD)0xC0000025)
#define EXCEPTION_INVALID_DISPOSITION ((DWORD)0xC0000026)
#define EXCEPTION_GUARD_PAGE ((DWORD)0x80000001)
#define STATUS_GUARD_PAGE_VIOLATION ((DWORD)0x80000001)
#define STATUS_ACCESS_VIOLATION ((DWORD)0xC0000005)
#define STATUS_NO_MEMORY ((DWORD)0xC0000017)

// Synchronisation. The critical section is opaque to the game; the platform keeps its
// mutex behind DebugInfo.
typedef struct _RTL_CRITICAL_SECTION {
    PVOID DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;
} CRITICAL_SECTION, *PCRITICAL_SECTION, *LPCRITICAL_SECTION, RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

// Global and local memory.
#define GMEM_FIXED 0x0000
#define GMEM_MOVEABLE 0x0002
#define GMEM_NOCOMPACT 0x0010
#define GMEM_NODISCARD 0x0020
#define GMEM_ZEROINIT 0x0040
#define GMEM_MODIFY 0x0080
#define GMEM_DISCARDABLE 0x0100
#define GMEM_SHARE 0x2000
#define GMEM_DDESHARE 0x2000
#define GHND (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR (GMEM_FIXED | GMEM_ZEROINIT)
#define LMEM_FIXED 0x0000
#define LMEM_MOVEABLE 0x0002
#define LMEM_ZEROINIT 0x0040
#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)
#define HEAP_NO_SERIALIZE 0x00000001
#define HEAP_GENERATE_EXCEPTIONS 0x00000004
#define HEAP_ZERO_MEMORY 0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY 0x00000010

#define MoveMemory(d, s, n) memmove((d), (s), (n))
#define CopyMemory(d, s, n) memcpy((d), (s), (n))
#define FillMemory(d, n, f) memset((d), (f), (n))
#define ZeroMemory(d, n) memset((d), 0, (n))
#define RtlMoveMemory MoveMemory
#define RtlCopyMemory CopyMemory
#define RtlFillMemory FillMemory
#define RtlZeroMemory ZeroMemory

// Modules and resources.
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCE MAKEINTRESOURCEA
#define IS_INTRESOURCE(p) ((((ULONG_PTR)(p)) >> 16) == 0)
#define RT_CURSOR MAKEINTRESOURCE(1)
#define RT_BITMAP MAKEINTRESOURCE(2)
#define RT_ICON MAKEINTRESOURCE(3)
#define RT_MENU MAKEINTRESOURCE(4)
#define RT_DIALOG MAKEINTRESOURCE(5)
#define RT_STRING MAKEINTRESOURCE(6)
#define RT_FONTDIR MAKEINTRESOURCE(7)
#define RT_FONT MAKEINTRESOURCE(8)
#define RT_ACCELERATOR MAKEINTRESOURCE(9)
#define RT_RCDATA MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE MAKEINTRESOURCE(11)
#define RT_GROUP_CURSOR MAKEINTRESOURCE(12)
#define RT_GROUP_ICON MAKEINTRESOURCE(14)
#define RT_VERSION MAKEINTRESOURCE(16)
#define DLL_PROCESS_DETACH 0
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3

// Message formatting.
#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define FORMAT_MESSAGE_FROM_STRING 0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE 0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY 0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK 0x000000FF

// Consoles.
typedef BOOL (WINAPI *PHANDLER_ROUTINE)(DWORD CtrlType);
#define CTRL_C_EVENT 0
#define CTRL_BREAK_EVENT 1
#define CTRL_CLOSE_EVENT 2
typedef struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} SMALL_RECT, *PSMALL_RECT;

// Locales and code pages.
#define CP_ACP 0
#define CP_OEMCP 1
#define CP_MACCP 2
#define CP_THREAD_ACP 3
#define CP_UTF7 65000
#define CP_UTF8 65001
#define MB_PRECOMPOSED 0x00000001
#define MB_COMPOSITE 0x00000002
#define MB_USEGLYPHCHARS 0x00000004
#define MB_ERR_INVALID_CHARS 0x00000008
#define WC_COMPOSITECHECK 0x00000200
#define WC_DISCARDNS 0x00000010
#define WC_SEPCHARS 0x00000020
#define WC_DEFAULTCHAR 0x00000040
#define MAX_LEADBYTES 12
#define MAX_DEFAULTCHAR 2
typedef struct _cpinfo {
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
} CPINFO, *LPCPINFO;
#define CT_CTYPE1 0x00000001
#define CT_CTYPE2 0x00000002
#define CT_CTYPE3 0x00000004
#define C1_UPPER 0x0001
#define C1_LOWER 0x0002
#define C1_DIGIT 0x0004
#define C1_SPACE 0x0008
#define C1_PUNCT 0x0010
#define C1_CNTRL 0x0020
#define C1_BLANK 0x0040
#define C1_XDIGIT 0x0080
#define C1_ALPHA 0x0100
#define LCMAP_LOWERCASE 0x00000100
#define LCMAP_UPPERCASE 0x00000200
#define LCMAP_SORTKEY 0x00000400
#define LCMAP_BYTEREV 0x00000800
#define NORM_IGNORECASE 0x00000001
#define NORM_IGNORENONSPACE 0x00000002
#define NORM_IGNORESYMBOLS 0x00000004
#define SORT_STRINGSORT 0x00001000
#define CSTR_LESS_THAN 1
#define CSTR_EQUAL 2
#define CSTR_GREATER_THAN 3
#define LCID_INSTALLED 0x00000001
#define LCID_SUPPORTED 0x00000002
#define LOCALE_NOUSEROVERRIDE 0x80000000
#define LOCALE_ILANGUAGE 0x00000001
#define LOCALE_SLANGUAGE 0x00000002
#define LOCALE_SENGLANGUAGE 0x00001001
#define LOCALE_SABBREVLANGNAME 0x00000003
#define LOCALE_ICOUNTRY 0x00000005
#define LOCALE_SCOUNTRY 0x00000006
#define LOCALE_SENGCOUNTRY 0x00001002
#define LOCALE_SABBREVCTRYNAME 0x00000007
#define LOCALE_IDEFAULTLANGUAGE 0x00000009
#define LOCALE_IDEFAULTCOUNTRY 0x0000000A
#define LOCALE_IDEFAULTCODEPAGE 0x0000000B
#define LOCALE_IDEFAULTANSICODEPAGE 0x00001004
#define LOCALE_SDECIMAL 0x0000000E
#define LOCALE_STHOUSAND 0x0000000F
#define LOCALE_SDATE 0x0000001D
#define LOCALE_STIME 0x0000001E
#define LOCALE_SSHORTDATE 0x0000001F
#define LOCALE_SLONGDATE 0x00000020
typedef BOOL (CALLBACK *LOCALE_ENUMPROCA)(LPSTR);
typedef LOCALE_ENUMPROCA LOCALE_ENUMPROC;

// Every library's entry point (the port's load stub calls it, see port/flags.py).
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

// kernel32 functions the game imports.
BOOL WINAPI AllocConsole(void);
BOOL WINAPI Beep(DWORD dwFreq, DWORD dwDuration);
BOOL WINAPI CloseHandle(HANDLE hObject);
LONG WINAPI CompareFileTime(const FILETIME *lpFileTime1, const FILETIME *lpFileTime2);
int WINAPI CompareStringA(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2);
int WINAPI CompareStringW(LCID Locale, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2);
BOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
BOOL WINAPI CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect,
                                 DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
HANDLE WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
BOOL WINAPI CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                           LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
                           LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo,
                           LPPROCESS_INFORMATION lpProcessInformation);
HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize,
                           LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
                           LPDWORD lpThreadId);
void WINAPI DebugBreak(void);
void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL WINAPI DeleteFileA(LPCSTR lpFileName);
BOOL WINAPI DisableThreadLibraryCalls(HMODULE hLibModule);
BOOL WINAPI DuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle,
                            LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions);
void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL WINAPI EnumSystemLocalesA(LOCALE_ENUMPROCA lpLocaleEnumProc, DWORD dwFlags);
DECLSPEC_NORETURN void WINAPI ExitProcess(UINT uExitCode);
DECLSPEC_NORETURN void WINAPI ExitThread(DWORD dwExitCode);
void WINAPI FatalAppExitA(UINT uAction, LPCSTR lpMessageText);
BOOL WINAPI FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime);
BOOL WINAPI FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime);
BOOL WINAPI FindClose(HANDLE hFindFile);
HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
HRSRC WINAPI FindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType);
BOOL WINAPI FlushFileBuffers(HANDLE hFile);
DWORD WINAPI FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId,
                            LPSTR lpBuffer, DWORD nSize, va_list *Arguments);
BOOL WINAPI FreeConsole(void);
BOOL WINAPI FreeEnvironmentStringsA(LPSTR penv);
BOOL WINAPI FreeEnvironmentStringsW(LPWSTR penv);
BOOL WINAPI FreeLibrary(HMODULE hLibModule);
UINT WINAPI GetACP(void);
BOOL WINAPI GetCPInfo(UINT CodePage, LPCPINFO lpCPInfo);
LPSTR WINAPI GetCommandLineA(void);
DWORD WINAPI GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
HANDLE WINAPI GetCurrentProcess(void);
DWORD WINAPI GetCurrentProcessId(void);
HANDLE WINAPI GetCurrentThread(void);
DWORD WINAPI GetCurrentThreadId(void);
UINT WINAPI GetDriveTypeA(LPCSTR lpRootPathName);
LPSTR WINAPI GetEnvironmentStrings(void);
LPWSTR WINAPI GetEnvironmentStringsW(void);
BOOL WINAPI GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode);
DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName);
BOOL WINAPI GetFileInformationByHandle(HANDLE hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation);
BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
DWORD WINAPI GetFileType(HANDLE hFile);
DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
DWORD WINAPI GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart);
DWORD WINAPI GetLastError(void);
void WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime);
int WINAPI GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData);
int WINAPI GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData);
DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
UINT WINAPI GetOEMCP(void);
FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
DWORD WINAPI GetProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize);
void WINAPI GetStartupInfoA(LPSTARTUPINFOA lpStartupInfo);
HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
BOOL WINAPI GetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType);
BOOL WINAPI GetStringTypeW(DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType);
void WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
void WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime);
DWORD WINAPI GetTickCount(void);
DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation);
LCID WINAPI GetUserDefaultLCID(void);
DWORD WINAPI GetVersion(void);
BOOL WINAPI GetVersionExA(LPOSVERSIONINFOA lpVersionInformation);
HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes);
HGLOBAL WINAPI GlobalFree(HGLOBAL hMem);
LPVOID WINAPI GlobalLock(HGLOBAL hMem);
BOOL WINAPI GlobalUnlock(HGLOBAL hMem);
void WINAPI GlobalMemoryStatus(LPMEMORYSTATUS lpBuffer);
HGLOBAL WINAPI GlobalReAlloc(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags);
SIZE_T WINAPI GlobalSize(HGLOBAL hMem);
LPVOID WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
HANDLE WINAPI HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
BOOL WINAPI HeapDestroy(HANDLE hHeap);
BOOL WINAPI HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
LPVOID WINAPI HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
SIZE_T WINAPI HeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
HANDLE WINAPI GetProcessHeap(void);
void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
static inline LONG InterlockedIncrement(LONG volatile *p) { return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedDecrement(LONG volatile *p) { return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedExchange(LONG volatile *p, LONG v) { return __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedExchangeAdd(LONG volatile *p, LONG v) { return __atomic_fetch_add(p, v, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedCompareExchange(LONG volatile *p, LONG v, LONG cmp)
{
    __atomic_compare_exchange_n(p, &cmp, v, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return cmp;
}
BOOL WINAPI IsBadCodePtr(FARPROC lpfn);
BOOL WINAPI IsBadReadPtr(const void *lp, UINT_PTR ucb);
BOOL WINAPI IsBadWritePtr(LPVOID lp, UINT_PTR ucb);
BOOL WINAPI IsValidCodePage(UINT CodePage);
BOOL WINAPI IsValidLocale(LCID Locale, DWORD dwFlags);
int WINAPI LCMapStringA(LCID Locale, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI LCMapStringW(LCID Locale, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
HGLOBAL WINAPI LoadResource(HMODULE hModule, HRSRC hResInfo);
BOOL WINAPI LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime);
HLOCAL WINAPI LocalAlloc(UINT uFlags, SIZE_T uBytes);
HLOCAL WINAPI LocalFree(HLOCAL hMem);
LPVOID WINAPI LockResource(HGLOBAL hResData);
LPVOID WINAPI MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh,
                            DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
int WINAPI MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte,
                               LPWSTR lpWideCharStr, int cchWideChar);
HFILE WINAPI OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle);
HANDLE WINAPI OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
void WINAPI OutputDebugStringA(LPCSTR lpOutputString);
BOOL WINAPI PulseEvent(HANDLE hEvent);
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);
BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);
void WINAPI RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                           const ULONG_PTR *lpArguments);
BOOL WINAPI ReadConsoleA(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead,
                         LPDWORD lpNumberOfCharsRead, LPVOID lpReserved);
BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                     LPOVERLAPPED lpOverlapped);
BOOL WINAPI ReleaseMutex(HANDLE hMutex);
HANDLE WINAPI CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName);
BOOL WINAPI ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
BOOL WINAPI RemoveDirectoryA(LPCSTR lpPathName);
BOOL WINAPI ResetEvent(HANDLE hEvent);
DWORD WINAPI ResumeThread(HANDLE hThread);
void WINAPI RtlUnwind(PVOID TargetFrame, PVOID TargetIp, PEXCEPTION_RECORD ExceptionRecord, PVOID ReturnValue);
BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE HandlerRoutine, BOOL Add);
BOOL WINAPI SetConsoleWindowInfo(HANDLE hConsoleOutput, BOOL bAbsolute, const SMALL_RECT *lpConsoleWindow);
BOOL WINAPI SetCurrentDirectoryA(LPCSTR lpPathName);
BOOL WINAPI SetEndOfFile(HANDLE hFile);
BOOL WINAPI SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue);
DWORD WINAPI GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer, DWORD nSize);
BOOL WINAPI SetEvent(HANDLE hEvent);
BOOL WINAPI SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes);
DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
UINT WINAPI SetHandleCount(UINT uNumber);
void WINAPI SetLastError(DWORD dwErrCode);
BOOL WINAPI SetStdHandle(DWORD nStdHandle, HANDLE hHandle);
BOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
DWORD WINAPI SizeofResource(HMODULE hModule, HRSRC hResInfo);
void WINAPI Sleep(DWORD dwMilliseconds);
DWORD WINAPI SuspendThread(HANDLE hThread);
BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime);
BOOL WINAPI TerminateProcess(HANDLE hProcess, UINT uExitCode);
DWORD WINAPI TlsAlloc(void);
BOOL WINAPI TlsFree(DWORD dwTlsIndex);
LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex);
BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);
BOOL WINAPI UnmapViewOfFile(LPCVOID lpBaseAddress);
LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL WINAPI VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
DWORD WINAPI WaitForMultipleObjects(DWORD nCount, const HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);
int WINAPI WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar,
                               LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
BOOL WINAPI WriteConsoleA(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite,
                          LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                      LPOVERLAPPED lpOverlapped);
int WINAPI _lclose(HFILE hFile);
HFILE WINAPI _lopen(LPCSTR lpPathName, int iReadWrite);
UINT WINAPI _lread(HFILE hFile, LPVOID lpBuffer, UINT uBytes);
LPSTR WINAPI lstrcatA(LPSTR lpString1, LPCSTR lpString2);
int WINAPI lstrcmpA(LPCSTR lpString1, LPCSTR lpString2);
int WINAPI lstrcmpiA(LPCSTR lpString1, LPCSTR lpString2);
LPSTR WINAPI lstrcpyA(LPSTR lpString1, LPCSTR lpString2);
LPSTR WINAPI lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength);
int WINAPI lstrlenA(LPCSTR lpString);

// The ANSI names.
#define CompareString CompareStringA
#define CopyFile CopyFileA
#define CreateDirectory CreateDirectoryA
#define CreateEvent CreateEventA
#define CreateFile CreateFileA
#define CreateFileMapping CreateFileMappingA
#define CreateMutex CreateMutexA
#define CreateSemaphore CreateSemaphoreA
#define CreateProcess CreateProcessA
#define DeleteFile DeleteFileA
#define EnumSystemLocales EnumSystemLocalesA
#define FatalAppExit FatalAppExitA
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA
#define FindResource FindResourceA
#define FormatMessage FormatMessageA
#define FreeEnvironmentStrings FreeEnvironmentStringsA
#define GetCommandLine GetCommandLineA
#define GetCurrentDirectory GetCurrentDirectoryA
#define GetDriveType GetDriveTypeA
#define GetFileAttributes GetFileAttributesA
#define GetFullPathName GetFullPathNameA
#define GetLocaleInfo GetLocaleInfoA
#define GetModuleFileName GetModuleFileNameA
#define GetModuleHandle GetModuleHandleA
#define GetProfileString GetProfileStringA
#define GetStartupInfo GetStartupInfoA
#define GetStringType GetStringTypeA
#define GetVersionEx GetVersionExA
#define LCMapString LCMapStringA
#define LoadLibrary LoadLibraryA
#define OutputDebugString OutputDebugStringA
#define ReadConsole ReadConsoleA
#define RemoveDirectory RemoveDirectoryA
#define SetCurrentDirectory SetCurrentDirectoryA
#define SetEnvironmentVariable SetEnvironmentVariableA
#define GetEnvironmentVariable GetEnvironmentVariableA
#define SetFileAttributes SetFileAttributesA
#define WriteConsole WriteConsoleA
#define lstrcat lstrcatA
#define lstrcmp lstrcmpA
#define lstrcmpi lstrcmpiA
#define lstrcpy lstrcpyA
#define lstrcpyn lstrcpynA
#define lstrlen lstrlenA

PORT_END_C

#ifdef __cplusplus
// The source keeps many of these values in C `long`s, which are 64 bits on macOS.
// These overloads take them as they are.
inline LONG InterlockedIncrement(long volatile *p) { return (LONG)__atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST); }
inline LONG InterlockedDecrement(long volatile *p) { return (LONG)__atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST); }
inline LONG InterlockedExchange(long volatile *p, long v) { return (LONG)__atomic_exchange_n(p, v, __ATOMIC_SEQ_CST); }
inline LONG InterlockedIncrement(unsigned long volatile *p) { return (LONG)__atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST); }
inline LONG InterlockedDecrement(unsigned long volatile *p) { return (LONG)__atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST); }
inline BOOL ReadFile(HANDLE h, LPVOID buf, DWORD n, unsigned long *done, LPOVERLAPPED ov)
{
    DWORD d = 0;
    BOOL ok = ReadFile(h, buf, n, &d, ov);
    if (done)
        *done = d;
    return ok;
}
inline BOOL WriteFile(HANDLE h, LPCVOID buf, DWORD n, unsigned long *done, LPOVERLAPPED ov)
{
    DWORD d = 0;
    BOOL ok = WriteFile(h, buf, n, &d, ov);
    if (done)
        *done = d;
    return ok;
}
inline DWORD SetFilePointer(HANDLE h, long dist, long *high, DWORD method)
{
    LONG hi = high ? (LONG)*high : 0;
    DWORD r = SetFilePointer(h, (LONG)dist, high ? &hi : 0, method);
    if (high)
        *high = hi;
    return r;
}
#endif

#endif
