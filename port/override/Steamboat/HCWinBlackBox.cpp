// port override: the original crash reporter walks the x86 stack (_CONTEXT Eip/Ebp) through imagehlp.dll and writes a log; on macOS the system's crash report does that, so the filter only lets the exception pass on
#include "GameApp.h"

int __stdcall BlackBoxExceptionFilter(const char *gameDLLName, _EXCEPTION_POINTERS *info)
{
    (void)gameDLLName;
    (void)info;
    return 0; // EXCEPTION_CONTINUE_SEARCH
}
