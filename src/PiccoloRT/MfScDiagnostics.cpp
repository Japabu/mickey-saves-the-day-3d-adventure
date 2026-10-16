// PiccoloRT.dll 0x10004f80-0x10004fc2
#include "MfScDiagnostics.h"

// 0x10004f80 ?SetLevel@MfScDiagnostics@@SAXH@Z
void __cdecl MfScDiagnostics::SetLevel(int i)
{
    iLevel = i;
}

// 0x10004f90 ?GetLevel@MfScDiagnostics@@SAHXZ
int __cdecl MfScDiagnostics::GetLevel()
{
    return iLevel;
}

// 0x10004fa0 ?Active@MfScDiagnostics@@SAHH@Z
int __cdecl MfScDiagnostics::Active(int level)
{
    if (plOutputList.GetCount() > 0 && (level == kAlways || level <= iLevel))
        return 1;
    return 0;
}
