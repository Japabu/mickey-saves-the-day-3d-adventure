// PORT: MBDebugStr formats through va_start/vsnprintf instead of a hand-built va_list (invalid on arm64).
#include "MBWinDebug.h"

// 0x10001000 _MBDebugStr
extern "C" void __cdecl MBDebugStr(const char *fmt, ...)
{
    static CMBCriticalSection criticalSection;
    // Truncates log.txt once per run, before the first message is logged.
    static struct LogFileDeleter {
        // 0x100010e0 ??0LogFileDeleter@?1??MBDebugStr@@9@QAE@XZ
        LogFileDeleter(void) { fclose(fopen("log.txt", "w")); }
    } logDeleter;

    char buffer[512];

    if (fmt) {
        criticalSection.Enter();
        // PORT: a real va_list (the original built one from &fmt + 1, which is not where
        // arm64 puts variadic arguments); len is clamped to what fits in buffer
        va_list ap;
        va_start(ap, fmt);
        long len = vsnprintf(buffer, sizeof buffer, fmt, ap);
        va_end(ap);
        if (len < 0)
            len = 0;
        if (len > (long)sizeof buffer - 1)
            len = sizeof buffer - 1;
        OutputDebugString(buffer);
        FILE *f = fopen("log.txt", "a");
        fwrite(buffer, 1, len, f);
        fclose(f);
        criticalSection.Leave();
    }
}

// 0x10001120 _MBDebugBreak
extern "C" void __cdecl MBDebugBreak(void)
{
    DebugBreak();
}

// 0x10001130 _MBAssertBox
extern "C" void __cdecl MBAssertBox(const char *failedCond, const char *msg, const char *file, int line)
{
    char temp[16];
    MBString str("Assert failed!\n\nMessage: ");
    str += msg;
    str += "\nCondition: ";
    str += failedCond;
    str += "\nFile: ";
    str += file;
    str += ", line ";
    _itoa(line, temp, 10);
    str += temp;
    str += "\nPress Abort to quit, Retry to debug, Ignore to continue.";

    switch (MessageBox(NULL, str.c_str(), "Assertion failed!", MB_ABORTRETRYIGNORE)) {
    case IDABORT:
        exit(-1);
    case IDRETRY:
        MBDebugBreak();
        break;
    }
}

// 0x10001470 ??2@YAPAXI@Z
void *__cdecl operator new(size_t size)
{
    return MBmalloc_release(size);
}

// 0x10001480 ??3@YAXPAX@Z
void __cdecl operator delete(void *ptr)
{
    if (ptr)
        MBfree_release(ptr);
}


// Matched by this code:
// 0x100010e0 ??0LogFileDeleter@?1??MBDebugStr@@9@QAE@XZ
// 0x10001100 _$E17
// 0x10001110 _$E16
// 0x10001490 ?append@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x10001550 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEX_N@Z
// 0x100015a0 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x100015c0 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAE_NI_N@Z
// 0x10001770 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x10001810 _$E23
// 0x10001830 _$E22
// 0x10001840 ?id@?$ctype@G@std@@$E