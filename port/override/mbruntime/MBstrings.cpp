// PORT: MBHashString computes and returns 32 bits, so hashes match the original values and the int members that hold them.
#include "MBRuntime.h"
#include <ctype.h>

extern "C" unsigned long __cdecl MBstrlen(const char *string);

// 0x100040a0 _MBHashString
// Case-insensitive hash, last character first: hash = hash * 38 + toupper(c).
extern "C" port_u32 __cdecl MBHashString(const char *name)   // PORT: a 32-bit hash, so int members holding it compare equal
{
    const char *p = name + MBstrlen(name) - 1;
    port_u32 hash = 0;   // PORT: the original wraps at 32 bits (C long is 64 here)
    while (p >= name)
        hash = hash * 38 + toupper(*p--);
    return hash;
}


// Matched by this code:
// 0x100040e0 _$E20
// 0x10004100 _$E19