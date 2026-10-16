// MfImageHandler: handler registry and DllMain (MfImageLib.dll 0x10002200-0x10002440).
#include "MfImageHandler_unit.h"

// 0x10002200 _$E4 (static initializer: construct + atexit)
// 0x10002210 _$E1
// 0x10002230 _$E3 (atexit(_$E2))
static MfDblList s_handlers;

// 0x10002250 ??0MfImageParams@@QAE@XZ (inline, emitted here)

// 0x10002280 ?MfFindImageHandler@@YAPAVMfImageHandler@@W4MfImageOp@@ABVMfString@@@Z
MfImageHandler *MfFindImageHandler(MfImageOp op, const MfString &path)
{
    MfString ext;
    path.SplitPath(0, 0, 0, &ext);
    MfDblListItem *pli;
    MfImageHandler *handler;
    for (handler = (MfImageHandler *)s_handlers.First(pli); handler;
         handler = (MfImageHandler *)s_handlers.Next(pli)) {
        if (handler->GetExtensions().CaseContainsSubString(ext, 0) != -1 &&
            handler->CanHandle(op))
            break;
    }
    return handler;
}

// 0x10002340 _DllMain@12 (original DllMain)
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    MfDblListItem *pli;
    MfImageHandler *handler;
    if (reason == DLL_PROCESS_ATTACH) {
        s_handlers.AppendItem(new MfBmpHandler);
        s_handlers.AppendItem(new MfTgaHandler);
        s_handlers.AppendItem(new MfJpgHandler);
    } else if (reason == DLL_PROCESS_DETACH) {
        for (handler = (MfImageHandler *)s_handlers.First(pli); handler;
             handler = (MfImageHandler *)s_handlers.Next(pli))
            delete handler;
        s_handlers.Clear();
    }
    return TRUE;
}


// Matched by this code:
// 0x10002240 _$E2