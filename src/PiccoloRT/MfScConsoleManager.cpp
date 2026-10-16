// PiccoloRT.dll 0x100033a0-0x100044ff (vc5 /O2 /GX). No PDB; names from
// exports. 0x100033a0-0x10003c6f is the tail of the MfScClassInfo object,
// 0x10003c70- the start of the MfScConsoleManager object.
#include "MfScConsoleManager_unit.h"

// 0x100033a0 ??1MfScBytecodes@@QAE@XZ (original ?AddRef@MfScData@@UAEXXZ:) identical-COMDAT folded empty function
// (also ??1MfScBytecodes, ??1MfScLineInfo, ... and ??1Exemplar@MfScConsoleManager).
void MfScData::AddRef() {}

// 0x100033b0 ??1MfScClassInfo@@UAE@XZ
MfScClassInfo::~MfScClassInfo()
{
    if (m_items) {
        MfDblListItem *pos;
        for (MfScClassInfoItem *item = (MfScClassInfoItem *)m_items->GetFirst(pos); item;
             item = (MfScClassInfoItem *)m_items->GetNext(pos))
            delete item;
        delete m_items;
    }
}

// 0x10003470 ??HMfScClassInfo@@QAEAAV0@PAVMfScClassInfoItem@@@Z
MfScClassInfo &MfScClassInfo::operator+(MfScClassInfoItem *item)
{
    m_items->Add(item);
    return *this;
}

// 0x10003490 ?AddItems@MfScClassInfo@@QAEXPAV1@@Z
void MfScClassInfo::AddItems(MfScClassInfo *other)
{
    MfDblListItem *pos;
    for (MfScClassInfoItem *item = (MfScClassInfoItem *)other->m_items->GetFirst(pos); item;
         item = (MfScClassInfoItem *)other->m_items->GetNext(pos))
        *this + item;
    if (other->m_text != NULL_STRING)
        m_text << "\n" << other->m_text;
    other->m_items->Clear();
}

// 0x10003500 ?InsertIntoTree@MfScClassInfo@@QAEXPAUHWND__@@PAU_TREEITEM@@@Z
void MfScClassInfo::InsertIntoTree(HWND hwnd, HTREEITEM parent)
{
    TV_INSERTSTRUCT tvis;
    tvis.hParent = parent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
    tvis.item.pszText = m_name;
    tvis.item.cchTextMax = m_name.Length();
    tvis.item.iImage = 4;
    tvis.item.iSelectedImage = 4;
    tvis.item.lParam = 1;
    HTREEITEM item = (HTREEITEM)SendMessage(hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    DisplayTextInfoInTree(hwnd, item);
    MfDblListItem *pos;
    for (MfScClassInfoItem *ci = (MfScClassInfoItem *)m_items->GetFirst(pos); ci;
         ci = (MfScClassInfoItem *)m_items->GetNext(pos))
        ci->DisplayInTree(hwnd, item);
}

// 0x100035b0 ?Matches@MfScClassInfo@@UAEHPAX0@Z
int MfScClassInfo::Matches(void *item, void *key)
{
    int result = 0;
    if (((MfScClassInfo *)item)->m_name == ((MfScClassInfo *)key)->m_name)
        result = 1;
    return result;
}

// EQUIV 0x100035e0 ?DisplayTextInfoInTree@MfScClassInfoText@@QAEPAU_TREEITEM@@PAUHWND__@@PAU2@@Z: reviewed: register allocation and the order of independent stores in the loop differ; same calls and arguments
HTREEITEM MfScClassInfoText::DisplayTextInfoInTree(HWND hwnd, HTREEITEM parent)
{
    if (m_text == NULL_STRING)
        return 0;
    MfStringList lines;
    m_text.ConvertToLines(&lines);
    MfDblListItem *pos;
    MfString *line = lines.First(pos);
    TV_INSERTSTRUCT tvis;
    tvis.hParent = parent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
    tvis.item.pszText = *line;
    tvis.item.cchTextMax = line->Length();
    tvis.item.iImage = 5;
    tvis.item.iSelectedImage = 5;
    tvis.item.lParam = 4;
    parent = (HTREEITEM)SendMessage(hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    for (line = lines.Next(pos); line; line = lines.Next(pos)) {
        tvis.hParent = parent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
        tvis.item.pszText = *line;
        tvis.item.cchTextMax = line->Length();
        tvis.item.iImage = -1;
        tvis.item.iSelectedImage = -1;
        tvis.item.lParam = 4;
        SendMessage(hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    }
    return 0;
}

// 0x100037a0 ??_EMfScCiProperty@@W3AEPAXI@Z (adjustor thunk, compiler-generated)
// 0x100037b0 ??_EMfScClassInfoItem@@W3AEPAXI@Z (adjustor thunk, compiler-generated)
// 0x100037c0 ??_EMfScClassInfo@@W7AEPAXI@Z (adjustor thunk, compiler-generated)
// 0x100037d0 ??_EMfScCiMethod@@W3AEPAXI@Z (adjustor thunk, compiler-generated)

// 0x100037e0 ?DisplayInTree@MfScClassInfoItem@@UAEPAU_TREEITEM@@PAUHWND__@@PAU2@@Z
HTREEITEM MfScClassInfoItem::DisplayInTree(HWND hwnd, HTREEITEM parent)
{
    TV_INSERTSTRUCT tvis;
    tvis.hParent = parent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
    tvis.item.pszText = m_name;
    tvis.item.cchTextMax = m_name.Length();
    tvis.item.iImage = GetTreeImage();
    tvis.item.iSelectedImage = GetTreeImage();
    tvis.item.lParam = 0;
    HTREEITEM item = (HTREEITEM)SendMessage(hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    DisplayTextInfoInTree(hwnd, item);
    return item;
}

// 0x10003870 ??0MfScMethodParameterInfo@@QAE@ABVMfString@@H@Z
MfScMethodParameterInfo::MfScMethodParameterInfo(const MfString &name, int optional)
    : m_name(name), m_optional(optional)
{
}

// 0x100038e0 ??1MfScMethodParameterInfo@@UAE@XZ
MfScMethodParameterInfo::~MfScMethodParameterInfo()
{
}

// 0x10003940 ?DisplayInTree@MfScMethodParameterInfo@@QAEPAU_TREEITEM@@PAUHWND__@@PAU2@H@Z
HTREEITEM MfScMethodParameterInfo::DisplayInTree(HWND hwnd, HTREEITEM parent, int index)
{
    int image;
    if (index > 8 || m_optional)
        image = 14;
    else
        image = index + 5;
    TV_INSERTSTRUCT tvis;
    tvis.hParent = parent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
    tvis.item.pszText = m_name;
    tvis.item.cchTextMax = m_name.Length();
    tvis.item.iImage = image;
    tvis.item.iSelectedImage = image;
    tvis.item.lParam = 5;
    HTREEITEM item = (HTREEITEM)SendMessage(hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    DisplayTextInfoInTree(hwnd, item);
    return item;
}

// 0x100039d0 ??0MfScCiMethod@@QAE@ABVMfString@@@Z
MfScCiMethod::MfScCiMethod(const MfString &name)
    : MfScClassInfoItem(name, 1)
{
    m_params = new MfDblList;
}

// 0x10003a80 ??_GMfDblList@@UAEPAXI@Z (compiler-generated)

// 0x10003aa0 ??1MfScCiMethod@@UAE@XZ
MfScCiMethod::~MfScCiMethod()
{
    MfDblListItem *pos;
    for (MfScMethodParameterInfo *p = (MfScMethodParameterInfo *)m_params->GetFirst(pos); p;
         p = (MfScMethodParameterInfo *)m_params->GetNext(pos)) {
        // the extra local shifts the unwind funclet's temporary to [ebp-0x20]
        MfScMethodParameterInfo *param = p;
        delete param;
    }
    delete m_params;
}

// 0x10003b60 ??HMfScCiMethod@@QAEAAV0@PAVMfScMethodParameterInfo@@@Z
MfScCiMethod &MfScCiMethod::operator+(MfScMethodParameterInfo *param)
{
    m_params->Add(param);
    return *this;
}

// 0x10003b80 ?DisplayInTree@MfScCiMethod@@UAEPAU_TREEITEM@@PAUHWND__@@PAU2@@Z
HTREEITEM MfScCiMethod::DisplayInTree(HWND hwnd, HTREEITEM parent)
{
    HTREEITEM item = MfScClassInfoItem::DisplayInTree(hwnd, parent);
    TV_ITEM tvi;
    tvi.mask = TVIF_PARAM | TVIF_HANDLE;
    tvi.hItem = item;
    tvi.lParam = 3;
    SendMessage(hwnd, TVM_SETITEM, 0, (LPARAM)&tvi);
    int index = 1;
    MfDblListItem *pos;
    for (MfScMethodParameterInfo *p = (MfScMethodParameterInfo *)m_params->GetFirst(pos); p;
         p = (MfScMethodParameterInfo *)m_params->GetNext(pos), index++)
        p->DisplayInTree(hwnd, item, index);
    return item;
}

// 0x10003c00 ?GetTreeImage@MfScCiMethod@@UAEHXZ
int MfScCiMethod::GetTreeImage()
{
    return 0;
}

// 0x10003c10 ?DisplayInTree@MfScCiProperty@@UAEPAU_TREEITEM@@PAUHWND__@@PAU2@@Z
HTREEITEM MfScCiProperty::DisplayInTree(HWND hwnd, HTREEITEM parent)
{
    HTREEITEM item = MfScClassInfoItem::DisplayInTree(hwnd, parent);
    TV_ITEM tvi;
    tvi.mask = TVIF_PARAM | TVIF_HANDLE;
    tvi.hItem = item;
    tvi.lParam = 2;
    SendMessage(hwnd, TVM_SETITEM, 0, (LPARAM)&tvi);
    return item;
}

// 0x10003c60 ?GetTreeImage@MfScCiProperty@@UAEHXZ
int MfScCiProperty::GetTreeImage()
{
    return 1;
}

// ---- MfScConsoleManager object ----

// 0x10003c70 ??0MfScConsoleManager@@QAE@XZ (inline, header)
// 0x10003c80 ?Manager@MfScConsoleManager@@SAPAV1@XZ (inline, header)
// 0x10003c90 ??0MfScConsoleManager@@QAE@ABV0@@Z (compiler-generated)
// 0x10003cb0 ??_EMfScConsoleManager@@UAEPAXI@Z (compiler-generated)
// 0x10003d00 ??0MfScHandleDict@@QAE@XZ (inline, header)
// 0x10003d20 ??1MfScHandleDict@@UAE@XZ (inline, header)
// 0x10003d70 ??0MfScHandleDict@@QAE@ABV0@@Z (compiler-generated)
// 0x10003da0 ??4MfScDataObject@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10003dd0 ??_EMfScHandleDict@@UAEPAXI@Z (compiler-generated)

MfScConsoleManager *MfScConsoleManager::pcmManager;

// 0x10003e20 ??0MfScConsoleManager@@QAE@VExemplar@0@@Z
// see .todo (the funclet's target ??1Exemplar is folded into 0x100033a0)
MfScConsoleManager::MfScConsoleManager(Exemplar)
{
    m_outputs = new MfDblList;
    pcmManager = this;

    MfScDataHandle *h = MfScDataHandle::CreateData(new MfScDataObject(0x200));
    h->SetReadOnly();
    MfScDataObject *obj = h ? dynamic_cast<MfScDataObject *>(h->GetData()) : 0;
    {
        MfString name("Console");
        MfScVmEnv::SetGlobalVariable(name, h, 1, 0);
    }
    obj->SetName("Console<instance>");

    MfScClassInfo *ci = new MfScClassInfo("Console");

    {
        MfString fullName("Console::Print");
        MfString name("Print");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(Console_Print, fullName, -1), 1);
    }
    *ci + &(*new MfScCiMethod("Print")
             + new MfScMethodParameterInfo("<var params>", 0));

    {
        MfString fullName("Console::PrintNl");
        MfString name("PrintNl");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(Console_PrintNl, fullName, -1), 1);
    }
    *ci + &(*new MfScCiMethod("PrintNl")
             + new MfScMethodParameterInfo("<var params>", 0));

    {
        MfString fullName("Console::Clear");
        MfString name("Clear");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(Console_Clear, fullName, -1), 1);
    }
    *ci + new MfScCiMethod("Clear");

    MfScDataObject::AddClassInfo(ci);
}

// 0x10004320 ??1MfScConsoleManager@@UAE@XZ
MfScConsoleManager::~MfScConsoleManager()
{
    if (this == pcmManager) {
        RemoveAllOutputs();
        delete m_outputs;
        pcmManager = 0;
    }
}

// 0x10004350 ?AddOutput@MfScConsoleManager@@QAEXPAVMfAiConsoleOutput@@@Z
void MfScConsoleManager::AddOutput(MfAiConsoleOutput *output)
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    if (!mgr->m_outputs->Find(output)) {
        mgr->m_outputs->Add(output);
        output->AddRef();
    }
}

// 0x10004390 ?RemoveOutput@MfScConsoleManager@@QAEXPAVMfAiConsoleOutput@@@Z
void MfScConsoleManager::RemoveOutput(MfAiConsoleOutput *output)
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    {
        MfDblListItem *pos;
        mgr->m_outputs->Delete(pos = mgr->m_outputs->Find(output));
        output->Release();
    }
}

// 0x100043d0 ?RemoveAllOutputs@MfScConsoleManager@@QAEXXZ
void MfScConsoleManager::RemoveAllOutputs()
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    MfDblListItem *pos;
    for (MfAiConsoleOutput *o = (MfAiConsoleOutput *)mgr->m_outputs->GetFirst(pos); o;
         o = (MfAiConsoleOutput *)mgr->m_outputs->GetNext(pos))
        o->Release();
    mgr->m_outputs->Clear();
}

// 0x10004420 ?PrintString@MfScConsoleManager@@QAEXABVMfString@@@Z
void MfScConsoleManager::PrintString(const MfString &s)
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    MfDblListItem *pos;
    for (MfAiConsoleOutput *o = (MfAiConsoleOutput *)mgr->m_outputs->GetFirst(pos); o;
         o = (MfAiConsoleOutput *)mgr->m_outputs->GetNext(pos))
        o->Print(s);
}

// 0x10004470 ?PrintStringNl@MfScConsoleManager@@QAEXABVMfString@@@Z
void MfScConsoleManager::PrintStringNl(const MfString &s)
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    MfDblListItem *pos;
    for (MfAiConsoleOutput *o = (MfAiConsoleOutput *)mgr->m_outputs->GetFirst(pos); o;
         o = (MfAiConsoleOutput *)mgr->m_outputs->GetNext(pos))
        o->PrintNl(s);
}

// 0x100044c0 ?ClearConsole@MfScConsoleManager@@QAEXXZ
void MfScConsoleManager::ClearConsole()
{
    MfScConsoleManager *mgr = this;
    while (mgr != pcmManager)
        mgr = pcmManager;
    MfDblListItem *pos;
    for (MfAiConsoleOutput *o = (MfAiConsoleOutput *)mgr->m_outputs->GetFirst(pos); o;
         o = (MfAiConsoleOutput *)mgr->m_outputs->GetNext(pos))
        o->Clear();
}


// Also matched by this code:
// mfpolibrary.dll 0x100055b0 ??_EMfScCiProperty@@W3AEPAXI@Z
// mfutil.dll 0x10009d90 ?GetTreeImage@MfScCiProperty@@UAEHXZ
// piccolocomp.dll 0x100119a0 ?GetTreeImage@MfScCiProperty@@UAEHXZ
// plugins/pscdebugger.dll 0x10005930 ??_EMfScCiProperty@@W3AEPAXI@Z
// plugins/pscdebugger.dll 0x10005940 ??_EMfScCiProperty@@W3AEPAXI@Z
// plugins/pscdebugger.dll 0x10005950 ??_EMfScCiProperty@@W3AEPAXI@Z
// runtimelib.dll 0x100069a0 ??0MfScHandleDict@@QAE@ABV0@@Z
// runtimelib.dll 0x1006ccc0 ??_EMfScCiProperty@@W3AEPAXI@Z
// runtimelib.dll 0x1006ccd0 ??_EMfScCiProperty@@W3AEPAXI@Z