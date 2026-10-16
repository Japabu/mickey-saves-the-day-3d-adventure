#include "MBWinStdlib.h"

// 0x10001b90 _MBsleep
extern "C" void __cdecl MBsleep(unsigned long ms)
{
    Sleep(ms);
}

// 0x10001ba0 _MBatexit
extern "C" void __cdecl MBatexit(void (__cdecl *func)(void))
{
    atexit(func);
}

// 0x10001bb0 _MBexit
extern "C" void __cdecl MBexit(long status)
{
    exit(status);
}

// 0x10001bc0 _MBsystem
extern "C" void __cdecl MBsystem(MBString command)
{
    system(command.c_str());
}

// 0x10001c00 _MBatos
extern "C" short __cdecl MBatos(const char *nptr)
{
    return (short)atoi(nptr);
}

// 0x10001c10 _MBatol
extern "C" long __cdecl MBatol(const char *nptr)
{
    return atol(nptr);
}

// 0x10001c20 _MBatof
extern "C" float __cdecl MBatof(const char *nptr)
{
    double d = atof(nptr);
    return (float)d;
}

// 0x10001c30 _MBatod
extern "C" double __cdecl MBatod(const char *nptr)
{
    double d = atof(nptr);
    return d;
}

// 0x10001c40 _MBrand
extern "C" long __cdecl MBrand(void)
{
    return rand();
}

// 0x10001c50 _MBrandom
extern "C" long __cdecl MBrandom(long first, long last)
{
    if (first >= last)
        return first;
    return rand() % (last - first + 1) + first;
}

// 0x10001c80 _MBsrand
extern "C" void __cdecl MBsrand(unsigned long seed)
{
    srand(seed);
}


// Matched by this code:
// 0x10001c90 _$E20
// 0x10001cb0 _$E19