// PscClassInfo.dll 0x10001000-0x1000209c: CClassInfo frame window and the
// CClassTree tree control (see the header).
#include "PscClassInfo.h"

// 0x10001000 ?CreateObject@CClassInfo@@SGPAVCObject@@XZ (original CClassInfo::CreateObject)
// 0x10001060 ?_GetBaseClass@CClassInfo@@KGPAUCRuntimeClass@@XZ
// 0x10001070 ?GetRuntimeClass@CClassInfo@@UBEPAUCRuntimeClass@@XZ (original CClassInfo::GetRuntimeClass)
IMPLEMENT_DYNCREATE(CClassInfo, CFrameWnd)

// 0x10001080 ??0CClassInfo@@QAE@XZ
CClassInfo::CClassInfo()
{
    m_pTree = NULL;
    m_pfnClose = NULL;
}

// 0x100010c0 ??0CClassInfo@@QAE@PAVCWnd@@P6AXPAV0@@Z@Z
CClassInfo::CClassInfo(CWnd *pParent, CloseCallback pfnClose)
    : m_pTree(NULL), m_pfnClose(pfnClose)
{
    CreateEx(0, NULL, "Piccolo Class Information", WS_OVERLAPPEDWINDOW,
             CW_USEDEFAULT, CW_USEDEFAULT, 230, 300,
             pParent->GetSafeHwnd(), NULL, NULL);
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

// 0x10001160 ??1CClassInfo@@UAE@XZ
CClassInfo::~CClassInfo()
{
    if (m_pfnClose)
        m_pfnClose(this);
}

// 0x100011d0 ?_GetBaseMessageMap@CClassInfo@@KGPBUAFX_MSGMAP@@XZ
// 0x100011e0 ?GetMessageMap@CClassInfo@@MBEPBUAFX_MSGMAP@@XZ (original CClassInfo::GetMessageMap)
BEGIN_MESSAGE_MAP(CClassInfo, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// 0x10001200 ?OnCreate@CClassInfo@@IAEHPAUtagCREATESTRUCTA@@@Z
int CClassInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) != -1) {
        CWnd *pWnd = MfScDataObject::CreateClassTree(this, 0);
        if (pWnd) {
            m_pTree = dynamic_cast<CClassTree *>(pWnd);
            return 0;
        }
    }
    return -1;
}

// 0x10001250 ?OnSize@CClassInfo@@IAEXIHH@Z
void CClassInfo::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    CRect rect;
    GetClientRect(&rect);
    m_pTree->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

// 0x100012c0 ??0CClassTree@@QAE@XZ
CClassTree::CClassTree()
    : m_hRoot(NULL), m_pImageList(NULL)
{
}

// 0x10001380 ??1CClassTree@@UAE@XZ
CClassTree::~CClassTree()
{
    delete m_pImageList;
}

// 0x10001440 ?GetInterfaceName@CClassTree@@UAEXPADHPAH1@Z
void CClassTree::GetInterfaceName(char *name, int size, int *major, int *minor)
{
    strncpy(name, "PscClassTree", size);
    *major = 1;
    *minor = 0;
}

// 0x10001470 ?AddRef@CClassTree@@UAEHXZ
int CClassTree::AddRef()
{
    return 1;
}

// The same code as AddRef.
int CClassTree::Release()
{
    return 1;
}

// 0x10001480 ?InsertClass@CClassTree@@UAEXPAVMfScClassInfo@@@Z
void CClassTree::InsertClass(MfScClassInfo *info)
{
    info->InsertIntoTree(m_hWnd, m_hRoot);
}

// 0x100014a0 ?GetWindow@CClassTree@@UAEPAVCWnd@@XZ
CWnd *CClassTree::GetWindow()
{
    return this;
}

// 0x100014b0 ?Show@CClassTree@@UAEXXZ
void CClassTree::Show()
{
    ShowWindow(SW_SHOW);
}

// 0x100014c0 ?Hide@CClassTree@@UAEXXZ
void CClassTree::Hide()
{
    ShowWindow(SW_HIDE);
}

// 0x100014d0 ?_GetBaseMessageMap@CClassTree@@KGPBUAFX_MSGMAP@@XZ
// 0x100014e0 ?GetMessageMap@CClassTree@@MBEPBUAFX_MSGMAP@@XZ (original CClassTree::GetMessageMap)
BEGIN_MESSAGE_MAP(CClassTree, CTreeCtrl)
    ON_WM_CREATE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_KEYDOWN()
    ON_COMMAND_RANGE(0xd000, 0xd400, OnMenuClass)
    ON_COMMAND(0xd401, OnCopyToClipboard)
    ON_COMMAND(0xd402, OnCopyToClipboard)
    ON_COMMAND(0xd403, OnCopyToClipboard)
END_MESSAGE_MAP()

// 0x100014f0 ?GetClassText@CClassTree@@QAE?AVMfString@@PAU_TREEITEM@@@Z
MfString CClassTree::GetClassText(HTREEITEM hItem)
{
    MfString s;
    if (hItem && GetItemData(hItem) == 3) {
        s = GetItemText(hItem);
        HTREEITEM hChild = GetChildItem(hItem);
        s << "(";
        if (hChild)
            s << " ";
        int n;
        n = 0;
        while (hChild) {
            if (n > 0)
                s << ", ";
            s << GetItemText(hChild);
            n++;
            hChild = GetNextSiblingItem(hChild);
        }
        if (n > 0)
            s << " ";
        s << ")";
    }
    return s;
}

// 0x10001660 ?GetMemberText@CClassTree@@QAE?AVMfString@@PAU_TREEITEM@@@Z
MfString CClassTree::GetMemberText(HTREEITEM hItem)
{
    MfString s;
    if (hItem && GetItemData(hItem) == 2)
        s = GetItemText(hItem);
    return s;
}

// 0x10001720 ?GetOtherText@CClassTree@@QAE?AVMfString@@PAU_TREEITEM@@@Z
MfString CClassTree::GetOtherText(HTREEITEM hItem)
{
    MfString s;
    if (hItem && GetItemData(hItem) == 4)
        s = GetItemText(hItem);
    return s;
}

// EQUIV 0x100017e0 ?CopyToClipboard@CClassTree@@QAEXABVMfString@@@Z: reviewed: callee-saved registers are pushed later (after the first two calls); same calls and arguments
void CClassTree::CopyToClipboard(const MfString &s)
{
    if (!OpenClipboard())
        return;
    if (EmptyClipboard() && s.Length() > 0) {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, s.Length() + 1);
        char *p = (char *)GlobalLock(hMem);
        memcpy(p, (char *)s, s.Length() + 1);
        GlobalUnlock(hMem);
        SetClipboardData(CF_TEXT, hMem);
    }
    CloseClipboard();
}

// EQUIV 0x10001860 ?BuildAlphabeticMenu@CClassTree@@QAEXPAVCMenu@@@Z: reviewed: register choice, stack slots (pos/nID swapped) and the order of independent instructions (the original finishes the rep stosd before loading m_list's head); same calls, arguments and branches
void CClassTree::BuildAlphabeticMenu(CMenu *pMenu)
{
    CMenu *menus[26];
    memset(menus, 0, sizeof(menus));
    UINT nID;
    MfDblListItem *pos;
    MfString *p = m_list.First(pos);
    if (p) {
        nID = 0xd000;
        do {
            MfString name(*p);
            char c = name.GetCharacter(0);
            if (islower(c))
                c = toupper(c);
            if (isalpha(c)) {
                CMenu *&pSub = menus[c - 'A'];
                if (pSub == NULL) {
                    pSub = new CMenu;
                    pSub->CreatePopupMenu();
                }
                if (name.Length() > 60) {
                    name.Truncate(59);
                    name << "...";
                }
                pSub->AppendMenu(MF_STRING, nID, (const char *)name);
                nID++;
            }
            p = m_list.Next(pos);
        } while (p);
    }
    for (int i = 0; i < 26; i++) {
        if (menus[i]) {
            char title[2];
            title[0] = 'A' + i;
            title[1] = 0;
            pMenu->AppendMenu(MF_POPUP, (UINT)menus[i]->m_hMenu, title);
            menus[i]->Detach();
            delete menus[i];
        }
    }
}

// 0x10001ad0 ?BuildMenu@CClassTree@@QAEXPAVCMenu@@@Z
void CClassTree::BuildMenu(CMenu *pMenu)
{
    MfDblListItem *pos;
    MfString *p = m_list.First(pos);
    if (p) {
        UINT nID = 0xd000;
        do {
            MfString name(*p);
            if (name.Length() > 60) {
                name.Truncate(59);
                name << "...";
            }
            pMenu->AppendMenu(MF_STRING, nID, (const char *)name);
            nID++;
            p = m_list.Next(pos);
        } while (p);
    }
}

// 0x10001c30 ?OnCreate@CClassTree@@IAEHPAUtagCREATESTRUCTA@@@Z
int CClassTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;
    PscClassInfoContext context;
    m_pImageList = new CImageList;
    m_pImageList->Create(0x50a4, 16, 0, RGB(128, 0, 128));
    SetImageList(m_pImageList, TVSIL_NORMAL);
    m_hRoot = InsertItem(MfString("Piccolo Classes"), 2, 3, TVI_ROOT, TVI_LAST);
    return 0;
}

// 0x10001dc0 ?PostNcDestroy@CClassTree@@MAEXXZ
void CClassTree::PostNcDestroy()
{
    CTreeCtrl::PostNcDestroy();
    delete this;
}

// 0x10001de0 ?OnNotify@CClassTree@@MAEHIJPAJ@Z
BOOL CClassTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
    *pResult = 0;
    return TRUE;
}

// 0x10001e00 ?OnRButtonDown@CClassTree@@IAEXIVCPoint@@@Z
void CClassTree::OnRButtonDown(UINT nFlags, CPoint point)
{
    UINT flags = 0;
    HTREEITEM hItem = HitTest(point, &flags);
    if (flags & TVHT_ONITEM)
        SelectItem(hItem);
}

// 0x10001e40 ?OnRButtonUp@CClassTree@@IAEXIVCPoint@@@Z (runs to 0x100021ff)
void CClassTree::OnRButtonUp(UINT nFlags, CPoint point)
{
    UINT flags;
    HTREEITEM hItem = HitTest(point, &flags);
    if (flags & TVHT_ONITEM) {
        switch (GetItemData(hItem)) {
        case 1: {
            m_list.Clear();
            CMenu menu;
            menu.CreatePopupMenu();
            HTREEITEM hChild = GetChildItem(hItem);
            while (hChild) {
                MfString s;
                if (GetItemData(hChild) == 3)
                    s = GetClassText(hChild);
                else if (GetItemData(hChild) == 2)
                    s = GetMemberText(hChild);
                m_list.Add(s);
                hChild = GetNextSiblingItem(hChild);
            }
            if (m_list.GetCount() > 25)
                BuildAlphabeticMenu(&menu);
            else
                BuildMenu(&menu);
            ClientToScreen(&point);
            menu.TrackPopupMenu(0, point.x, point.y, this);
            break;
        }
        case 3: {
            CMenu menu;
            menu.CreatePopupMenu();
            m_strCopy = GetClassText(hItem);
            menu.AppendMenu(MF_STRING, 0xd401, "Copy to Clipboard");
            ClientToScreen(&point);
            menu.TrackPopupMenu(0, point.x, point.y, this);
            break;
        }
        case 2: {
            CMenu menu;
            menu.CreatePopupMenu();
            m_strCopy = GetMemberText(hItem);
            menu.AppendMenu(MF_STRING, 0xd402, "Copy to Clipboard");
            ClientToScreen(&point);
            menu.TrackPopupMenu(0, point.x, point.y, this);
            break;
        }
        case 4: {
            CMenu menu;
            menu.CreatePopupMenu();
            m_strCopy = GetOtherText(hItem);
            menu.AppendMenu(MF_STRING, 0xd403, "Copy to Clipboard");
            ClientToScreen(&point);
            menu.TrackPopupMenu(0, point.x, point.y, this);
            break;
        }
        }
    }
}


// Matched by this code:
// 0x100010a0 ??_GCClassInfo@@UAEPAXI@Z
// 0x100011c0 ??3CObject@@SGXPAX@Z
// 0x100011f0 ?AssertValid@CObject@@UBEXXZ
// 0x100012b0 ?Serialize@CObject@@UAEXAAVCArchive@@@Z
// 0x10001360 ??_GCClassTree@@UAEPAXI@Z
// 0x10001aa0 ??_GMfStringRef@@UAEPAXI@Z
// 0x10001d90 ??_GPscClassInfoContext@@UAEPAXI@Z
// 0x10002220 ??_GCObject@@UAEPAXI@Z
// 0x100023d0 ??_GCMenu@@UAEPAXI@Z
// 0x100023f0 ??1CMenu@@UAE@XZ
// 0x10002440 ?BeginModalState@CWnd@@UAEXXZ
// 0x10002450 ?EndModalState@CWnd@@UAEXXZ
// 0x10002460 ??_GCTreeCtrl@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007b80 ??1CMenu@@UAE@XZ
// plugins/pscdebugger.dll 0x10007bd0 ??1CMenu@@UAE@XZ
// plugins/pscdebugger.dll 0x10007c40 ??1CMenu@@UAE@XZ
// plugins/pscdebugger.dll 0x100097d0 ??1CMenu@@UAE@XZ
// plugins/pscdebugger.dll 0x10009840 ??1CMenu@@UAE@XZ