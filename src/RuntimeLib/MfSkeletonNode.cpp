// RuntimeLib.dll 10001000-10001aa0 (first object file of the DLL).
// Besides the directory scan below, this range holds the COMDAT copies of
// inline members of the exported classes whose headers it includes.
#include <windows.h>
#include "MfColNode.h"
#include "MfLiveActorList.h"
#include "MfRenderAxisInfo.h"
#include "MfRenderFogInfo.h"
#include "MfRenderGridInfo.h"
#include "MfRuntime.h"
#include "MfSkeletonNode.h"
#include "MfSpaceList.h"
#include "MfString.h"
#include "MfStringList.h"

// 0x10001000 ?FindFilesWithExtension@@YAHPAVMfStringList@@ABVMfString@@1@Z (original (unnamed;) called from MfLiveActor::LoadBehaviors)
// Adds to 'list' every file of 'dir' whose name ends in / contains 'ext'
// (as given or upper-cased), skipping "." and "..".
int FindFilesWithExtension(MfStringList *list, const MfString &dir, const MfString &ext)
{
    WIN32_FIND_DATA fd;
    MfString path(dir);
    path << "\\*" << ext;
    HANDLE h = FindFirstFile(path, &fd);
    MfString upper(ext);
    MfString name;
    upper.ConvertToUpper();
    if (h != INVALID_HANDLE_VALUE) {
        do {
            name = fd.cFileName;
            bool ok = name != MfString("..") && name != MfString(".");
            if (ok) {
                if (name.ContainsSubString(ext, 0) > -1 || name.ContainsSubString(upper, 0) > -1)
                    list->Add(*new MfString(name));
            }
        } while (FindNextFile(h, &fd) == TRUE);
        FindClose(h);
        return 1;
    }
    return 0;
}

// Inline members emitted here (defined in the headers above), in address order:
// 0x10001310 ??_GMfStringRef@@UAEPAXI@Z (local vtable copy of a dllimport class)
// 0x10001340 ??0MfRuntime@@QAE@ABV0@@Z
// 0x10001370 ??4MfRuntime@@QAEAAV0@ABV0@@Z
// 0x10001390 ??_EMfRuntime@@UAEPAXI@Z
// 0x100013e0 ?Prepend@MfSpaceList@@QAEXPBVMfSpace@@@Z (original ?Prepend@MfLiveActorList@@QAEXPBVMfLiveActor@@@Z) = ?Prepend@MfSpaceList@@QAEXPBVMfSpace@@@Z
// 0x10001400 ??1MfSpaceList@@UAE@XZ
// 0x10001410 ?Add@MfLiveActorList@@QAEXPBVMfLiveActor@@@Z
// 0x10001420 ?Append@MfLiveActorList@@QAEXPBVMfLiveActor@@@Z
// 0x10001440 ??0MfRenderAxisInfo@@QAE@XZ
// 0x100014a0 ??0MfRenderAxisInfo@@QAE@ABU0@@Z
// 0x10001520 ??4MfRenderAxisInfo@@QAEAAU0@ABU0@@Z
// 0x10001570 ??1MfRenderAxisInfo@@QAE@XZ
// 0x10001580 ??0MfRenderGridInfo@@QAE@XZ
// 0x100015b0 ??0MfRenderGridInfo@@QAE@ABU0@@Z
// 0x10001610 ??4MfRenderGridInfo@@QAEAAU0@ABU0@@Z
// 0x10001670 ??1MfRenderGridInfo@@QAE@XZ
// 0x10001680 ??0MfRenderFogInfo@@QAE@XZ
// 0x10001690 ??0MfRenderFogInfo@@QAE@ABU0@@Z
// 0x100016d0 ??4MfRenderFogInfo@@QAEAAU0@ABU0@@Z
// 0x10001710 ??1MfRenderFogInfo@@QAE@XZ
// 0x10001720 ?Transform@MfColNode@@UAEAAVMfTransform3@@XZ
// 0x10001730 ?ColModel@MfColNode@@UAEAAPAVMfColModel@@XZ
// 0x10001740 ?OwnColModel@MfColNode@@QAEXPAVMfColModel@@@Z
// 0x10001750 ?SetUserData@MfColNode@@UAE_NPAVMfAi@@@Z
// 0x10001760 ?GetUserData@MfColNode@@UAEPAVMfAi@@XZ
// 0x10001770 ??4MfColNode@@QAEAAV0@ABV0@@Z
// 0x10001810 ??_FMfColNode@@QAEXXZ
// 0x10001820 ?SetColFlag@MfColNode@@QAEHABH@Z
// 0x10001840 ?ClearColFlag@MfColNode@@QAEHABH@Z
// 0x10001860 ?TestColFlag@MfColNode@@QBEHABH@Z
// 0x10001880 ?createCopy@MfSkeletonNode@@UAEPAV1@XZ
// 0x100018e0 ?Type@MfSkeletonNode@@UAEHXZ
// 0x100018f0 ?IsType@MfSkeletonNode@@UAEHH@Z
// 0x10001920 ?name@MfSkeletonNode@@QAEABVMfString@@XZ
// 0x10001940 ?SetLocalName@MfSkeletonNode@@QAEXVMfString@@@Z
// 0x100019a0 ?homeTransform@MfSkeletonNode@@QAE?AVMfTransform3@@XZ
// 0x10001a30 ?SetLocalHomeTransform@MfSkeletonNode@@QAEXABVMfTransform3@@@Z
// 0x10001a80 ?jointLimit@MfSkeletonNode@@QAEPAVMfJLimit@@XZ
// 0x10001a90 ?colSpheres@MfSkeletonNode@@QAEPAVMfDblList@@XZ

// force emission of the implicit members of the exported classes
static MfRuntime &(MfRuntime::*s_runtimeAssign)(const MfRuntime &) = &MfRuntime::operator=;


// Matched by this code:
// mfactorlib.dll 0x100127a0 ??_GMfStringRef@@UAEPAXI@Z
// piccolocomp.dll 0x10006b00 ??_GMfStringRef@@UAEPAXI@Z
// piccolort.dll 0x10009c00 ??_GMfStringRef@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x100043c0 ??_GMfStringRef@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x1000b100 ??_GMfStringRef@@UAEPAXI@Z
// 0x10087e80 ?Add@MfLiveActorList@@QAEXPBVMfLiveActor@@@Z