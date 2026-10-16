// PiccoloRT.dll 0x10009500-0x10009797: the list of diagnostic outputs.
#include "MfScDiagnostics.h"
#include "MfScDiagnosticCallBack.h"
#include "MfString.h"

int MfScDiagnostics::iLevel;
MfDblList MfScDiagnostics::plOutputList;

// 0x10009550 ?AddOutput@MfScDiagnostics@@SAXPAVMfScDiagnosticCallBack@@@Z
void __cdecl MfScDiagnostics::AddOutput(MfScDiagnosticCallBack *output)
{
    MfDblListItem *pos;
    for (MfScDiagnosticCallBack *o = (MfScDiagnosticCallBack *)plOutputList.GetFirst(pos); o;
         o = (MfScDiagnosticCallBack *)plOutputList.GetNext(pos)) {
        if (o == output)
            return;
    }
    MfDblList &list = plOutputList;     // a virtual call in the original
    list.Append(output);
}

// 0x10009590 ?RemoveOutput@MfScDiagnostics@@SAHPAVMfScDiagnosticCallBack@@@Z
int __cdecl MfScDiagnostics::RemoveOutput(MfScDiagnosticCallBack *output)
{
    MfDblListItem *pos = plOutputList.Find(output);
    MfScDiagnosticCallBack *o = (MfScDiagnosticCallBack *)plOutputList.Delete(pos);
    if (o) {
        delete o;
        return 1;
    }
    return 0;
}

// 0x100095d0 ?ClearAllOutput@MfScDiagnostics@@SAXXZ
void __cdecl MfScDiagnostics::ClearAllOutput()
{
    MfDblListItem *pos;
    for (MfScDiagnosticCallBack *o = (MfScDiagnosticCallBack *)plOutputList.GetFirst(pos); o;
         o = (MfScDiagnosticCallBack *)plOutputList.GetNext(pos))
        delete o;
    plOutputList.Clear();
}

// 0x10009610 ?Print@MfScDiagnostics@@SAXHABVMfString@@@Z
void __cdecl MfScDiagnostics::Print(int level, MfString const &message)
{
    if (plOutputList.GetCount() > 0 && (level == kAlways || (level != 0 && level <= iLevel))) {
        MfScDiagnosticInfo info;
        info.mLevel = level;
        info.mMessage = message;
        MfDblListItem *pos;
        for (MfScDiagnosticCallBack *o = (MfScDiagnosticCallBack *)plOutputList.GetFirst(pos); o;
             o = (MfScDiagnosticCallBack *)plOutputList.GetNext(pos))
            o->CallBack(info);
    }
}

// 0x100096d0 ?PrintNl@MfScDiagnostics@@SAXHABVMfString@@@Z
void __cdecl MfScDiagnostics::PrintNl(int level, MfString const &message)
{
    if (plOutputList.GetCount() > 0 && (level == kAlways || (level != 0 && level <= iLevel))) {
        MfScDiagnosticInfo info;
        info.mLevel = level;
        info.mMessage = message;
        info.mMessage << "\r\n";
        MfDblListItem *pos;
        for (MfScDiagnosticCallBack *o = (MfScDiagnosticCallBack *)plOutputList.GetFirst(pos); o;
             o = (MfScDiagnosticCallBack *)plOutputList.GetNext(pos))
            o->CallBack(info);
    }
}


// Matched by this code:
// 0x100094f0 _$E10
// 0x10009500 _$E6
// 0x10009520 _$E9
// 0x10009530 _$E7