// PscDebugger.dll 0x10006cb0-0x10006ec9: the flat list of the modules in the
// module selector.
#include "PscDebugger.h"

// 0x10006cb0 ??0MfPdbFlatModuleList@@QAE@XZ
MfPdbFlatModuleList::MfPdbFlatModuleList()
{
}

// 0x10006cd0 ??_GMfPdbFlatModuleList@@UAEPAXI@Z

// 0x10006cf0 ??1MfPdbFlatModuleList@@UAE@XZ
MfPdbFlatModuleList::~MfPdbFlatModuleList()
{
}

// 0x10006d10 ?Enumerate@MfPdbFlatModuleList@@UAEHABVMfString@@0PAX@Z
int MfPdbFlatModuleList::Enumerate(const MfString &module, const MfString &moduleType, void *)
{
    HTREEITEM item = InsertItem(module, TVI_ROOT, TVI_SORT);
    MfPdbModuleItem *data = new MfPdbModuleItem(module, moduleType);
    SetItemData(item, (DWORD)data);
    return 1;
}

// 0x10006dd0 ?_GetBaseMessageMap@MfPdbFlatModuleList@@KGPBUAFX_MSGMAP@@XZ
// 0x10006de0 ?GetMessageMap@MfPdbFlatModuleList@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbFlatModuleList, CTreeCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// 0x10006df0 ?PreCreateWindow@MfPdbFlatModuleList@@UAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPdbFlatModuleList::PreCreateWindow(CREATESTRUCT &cs)
{
    return CWnd::PreCreateWindow(cs);
}

// FillTree and OnCreate compile to the same bytes as MfPdbModuleNameTree's and
// were folded into them by the linker (0x10007f90, 0x10007ff0).
void MfPdbFlatModuleList::FillTree()
{
    MfScFunctionRegistry *registry = pdbDebugger->mRegistry;
    registry->EnumModules(this, 0);
}

int MfPdbFlatModuleList::OnCreate(LPCREATESTRUCT cs)
{
    if (CTreeCtrl::OnCreate(cs) == -1)
        return -1;
    FillTree();
    return 0;
}

// 0x10006e00 ?OnDestroy@MfPdbFlatModuleList@@IAEXXZ
void MfPdbFlatModuleList::OnDestroy()
{
    HTREEITEM item = GetRootItem();
    while (item) {
        MfPdbModuleItem *data = (MfPdbModuleItem *)GetItemData(item);
        if (data) {
            delete data;
            SetItemData(item, 0);
        }
        item = GetNextSiblingItem(item);
    }
    CTreeCtrl::OnDestroy();
}

// 0x10006e80 ??1MfPdbModuleItem@@QAE@XZ
