// PscStdlib.dll 0x10001000-0x10001230: the plugin entry points.
#include "PscStdlib.h"

void InitializeClasses();
void ShutdownClasses();

// 0x10001000 _PscInitialize@8
int __stdcall PscInitialize(MfConstInitFunction constInit, MfString *errors)
{
    int ok = 1;
    *errors = NULL_STRING;
    InitializeClasses();
    if (constInit) {
        if (!constInit("<List Constants>", MfPlList::GetConstants(), errors)
            || !constInit("<Math Constants>", MfPlMath::GetConstants(), errors)
            || !constInit("<Logger Constants>", MfPlLogger::GetConstants(), errors)
            || !constInit("<Dictionary Constants>", MfPlDictionary::GetConstants(), errors))
            ok = 0;
    }
    return ok;
}

// 0x100011d0 _PscShutdown@4
int __stdcall PscShutdown(MfString *errors)
{
    *errors = NULL_STRING;
    ShutdownClasses();
    return 1;
}

// 0x100011f0 _PscIsStandardLibrary@0
int __stdcall PscIsStandardLibrary()
{
    return 1;
}

// 0x10001200 ?InitializeClasses@@YAXXZ
void InitializeClasses()
{
    MfPlMath::Initialize();
    MfPlList::Initialize();
    MfPlDictionary::Initialize();
    MfPlLogger::Initialize();
    MfPlTime::Initialize();
}

// 0x10001220 ?ShutdownClasses@@YAXXZ
void ShutdownClasses()
{
    MfPlMath::Shutdown();
    MfPlList::Shutdown();
    MfPlDictionary::Shutdown();
    MfPlLogger::Shutdown();
    MfPlTime::Shutdown();
}
