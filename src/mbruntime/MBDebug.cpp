#include "MBRuntime.h"

extern "C" void __cdecl MBDebugStr(const char *fmt, ...);
extern "C" void __cdecl MBAssertBox(const char *failedCond, const char *msg, const char *file, long line);

// What a failed assertion does (MBSetAssertBehavior).
enum {
    kMBAssertIgnore = 0,
    kMBAssertLog = 1,
    kMBAssertBox = 2,
    kMBAssertBreak = 3,
    kMBAssertExit = 4
};

static unsigned long gBehavior = kMBAssertBox;    // 2 in the binary

// 0x10001f50 _MBSetAssertBehavior
extern "C" unsigned long __cdecl MBSetAssertBehavior(unsigned long behavior)
{
    unsigned long old = gBehavior;
    gBehavior = behavior;
    return old;
}

// 0x10001f60 _MBHandleAssert
extern "C" void __cdecl MBHandleAssert(const char *failedCond, const char *msg, const char *file, long line)
{
    if (gBehavior != kMBAssertIgnore) {
        MBDebugStr("WARN: Assertion failed!\n      Msg : %s\n      Cond: %s\n      File: %s, line %d\n",
                   msg, failedCond, file, line);
        switch (gBehavior) {
        case kMBAssertExit:
            exit(-1);
        case kMBAssertBreak:
            MBDebugBreak();
            break;
        case kMBAssertBox:
            MBAssertBox(failedCond, msg, file, line);
            break;
        }
    }
}
