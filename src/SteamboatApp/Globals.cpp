#include "Globals.h"
#include "ios.h"
#include "ostream.h"

// 0x100dd210 ?flush@@YAAAVostream@@AAV1@@Z
ostream & __cdecl flush(ostream & r)
{
    return r.flush();
}

// 0x10109ab0 ?__setprecision@@YAAAVios@@AAV1@H@Z
ios & __cdecl __setprecision(ios & r, int p)
{
    *(int *)((char *)&r + 0x28) = p;
    return r;
}

// 0x10109ac0 ?__setw@@YAAAVios@@AAV1@H@Z
ios & __cdecl __setw(ios & r, int w)
{
    *(int *)((char *)&r + 0x30) = w;
    return r;
}
