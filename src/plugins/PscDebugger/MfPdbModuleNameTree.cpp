// PscDebugger.dll 0x10007c90-0x100080d6: the modules of the function registry
// as a tree in the module selector; the source type of a module splits its
// name into the path of the tree.
#include "PscDebugger.h"

// 0x10007c90 ??0MfPdbModuleNameTree@@QAE@XZ
MfPdbModuleNameTree::MfPdbModuleNameTree()
{
}

// 0x10007cb0 ??_GMfPdbModuleNameTree@@UAEPAXI@Z

// 0x10007cd0 ??1MfPdbModuleNameTree@@UAE@XZ
MfPdbModuleNameTree::~MfPdbModuleNameTree()
{
}

// EQUIV 0x10007cf0 ?Enumerate@MfPdbModuleNameTree@@UAEHABVMfString@@0PAX@Z: reviewed: register choice and stack slots only (this in ebp and part in ebx in the original, swapped here; this/source spill slots swapped; the original reloads this once more after constructing the MfPdbModuleItem); same calls, arguments and branches
int MfPdbModuleNameTree::Enumerate(const MfString &module, const MfString &moduleType, void *)
{
    MfAiSourceResolver *resolver = MfScPiccolo::GetSourceResolver();
    MfAiSourceType *source = 0;
    if (resolver->Find(moduleType, &source) && source) {
        MfStringList parts;
        source->SplitModuleName(module, &parts);
        source->Release();
        if (parts.GetCount() > 0) {
            HTREEITEM parent = TVI_ROOT;
            HTREEITEM item = GetRootItem();
            MfDblListItem *pos;
            MfString *part = parts.First(pos);
            while (part) {
                int found = 0;
                while (item) {
                    if (*part == MfString(GetItemText(item))) {
                        found = 1;
                        break;
                    }
                    item = GetNextSiblingItem(item);
                }
                if (found)
                    parent = item;
                else
                    parent = InsertItem(*part, parent, TVI_SORT);
                part = parts.Next(pos);
                if (!found && !part) {
                    MfPdbModuleItem *data = new MfPdbModuleItem(module, moduleType);
                    SetItemData(parent, (DWORD)data);
                }
                item = GetChildItem(parent);
            }
        }
    }
    return 1;
}

// 0x10007f90 ?FillTree@MfPdbModuleNameTree@@QAEXXZ
void MfPdbModuleNameTree::FillTree()
{
    MfScFunctionRegistry *registry = pdbDebugger->mRegistry;
    registry->EnumModules(this, 0);
}

// 0x10007fc0 ?GetMessageMap@MfPdbModuleNameTree@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbModuleNameTree, CTreeCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// 0x10007fd0 ?PreCreateWindow@MfPdbModuleNameTree@@UAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPdbModuleNameTree::PreCreateWindow(CREATESTRUCT &cs)
{
    cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    return CWnd::PreCreateWindow(cs);
}

// 0x10007ff0 ?OnCreate@MfPdbModuleNameTree@@IAEHPAUtagCREATESTRUCTA@@@Z
int MfPdbModuleNameTree::OnCreate(LPCREATESTRUCT cs)
{
    if (CTreeCtrl::OnCreate(cs) == -1)
        return -1;
    FillTree();
    return 0;
}

// 0x10008010 ?DeleteItemData@MfPdbModuleNameTree@@QAEXPAU_TREEITEM@@@Z
void MfPdbModuleNameTree::DeleteItemData(HTREEITEM item)
{
    for (HTREEITEM child = GetChildItem(item); child; child = GetNextSiblingItem(child))
        DeleteItemData(child);
    MfPdbModuleItem *data = (MfPdbModuleItem *)GetItemData(item);
    if (data) {
        delete data;
        SetItemData(item, 0);
    }
}

// 0x10008090 ?OnDestroy@MfPdbModuleNameTree@@IAEXXZ
void MfPdbModuleNameTree::OnDestroy()
{
    for (HTREEITEM item = GetRootItem(); item; item = GetNextSiblingItem(item))
        DeleteItemData(item);
    CTreeCtrl::OnDestroy();
}
