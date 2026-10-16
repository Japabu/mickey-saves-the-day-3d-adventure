#include "MBRuntime.h"
#include <ctype.h>

extern "C" unsigned long __cdecl MBstrlen(const char *string);

// 0x100040a0 _MBHashString
// Case-insensitive hash, last character first: hash = hash * 38 + toupper(c).
extern "C" unsigned long __cdecl MBHashString(const char *name)
{
    const char *p = name + MBstrlen(name) - 1;
    unsigned long hash = 0;
    while (p >= name)
        hash = hash * 38 + toupper(*p--);
    return hash;
}


// Matched by this code:
// 0x100040e0 _$E20
// 0x10004100 _$E19