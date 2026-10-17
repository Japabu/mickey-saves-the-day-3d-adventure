// MfImageHandler: handler registry and DllMain (MfImageLib.dll 0x10002200-0x10002440).
#include "MfImageHandler.h"
#include "MfBmpHandler.h"
#include "MfTgaHandler.h"
#include "MfJpgHandler.h"

// mfutil.dll
#include "../mfutil/MfDblListItem.h"

// own copy: MfImageLib's view of mfutil's MfDblList: the constructor is inline here
// (stores the imported vtable), the destructor/Insert/Clear are imported. DllMain calls
// Insert(p, LP_AFTER, pliLast) and walks the list inline: the inline helpers stand for
// whatever inline accessors the original header had, and mfutil/MfDblList.h has none
// that reach the protected pliLast.
class MFUTIL_API MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();                                                    // +0x00
    virtual void Prepend(void *const pData);                                 // +0x04
    virtual void Append(void *const pData);                                  // +0x08
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c
    virtual void Clear();

    void AppendItem(void *p) { Insert(p, LP_AFTER, pliLast); }
    void *First(MfDblListItem *&pli) const { pli = pliFirst; return pli ? pli->pData : 0; }
    void *Next(MfDblListItem *&pli) const
    {
        if (pli) pli = pli->pliNext;
        return pli ? pli->pData : 0;
    }

    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

// 0x10002200 _$E4 (static initializer: construct + atexit)
// 0x10002210 _$E1
// 0x10002230 _$E3 (atexit(_$E2))
static MfDblList s_handlers;

// 0x10002250 ??0MfImageParams@@QAE@XZ (inline, emitted here: an exported inline function
// is emitted by every object that sees its body, so only this one sees it)
inline MfImageParams::MfImageParams() : image(0), alignment(0), flags(0x1ffff), field_10(0) {}

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