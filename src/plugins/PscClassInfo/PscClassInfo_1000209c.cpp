// PscClassInfo.dll 0x1000209c-0x10002870 (see the header). 0x1000209c-0x100021ff
// are the tail and jump table of CClassTree::OnRButtonUp (unit
// PscClassInfo_10001000).
#include "PscClassInfo.h"

// The MFC inlines in this range (??_GCObject at 0x10002220, ??_GCMenu, ??1CMenu,
// CWnd::BeginModalState/EndModalState, ??_GCTreeCtrl at 0x100023d0-0x10002460)
// are emitted by the object of PscClassInfo_10001000.cpp and claimed there.

// EQUIV 0x10002240 ?OnMenuClass@CClassTree@@IAEXI@Z: reviewed: register choice only
void CClassTree::OnMenuClass(UINT nID)
{
    int n = nID - 0xd000;
    MfDblListItem *pos;
    MfString *p = m_list.First(pos);
    while (p && n > 0) {
        n--;
        p = m_list.Next(pos);
    }
    CopyToClipboard(*p);
}

// 0x10002280 ?OnCopyToClipboard@CClassTree@@IAEXXZ
void CClassTree::OnCopyToClipboard()
{
    CopyToClipboard(m_strCopy);
}

// 0x10002290 ?OnKeyDown@CClassTree@@IAEXIII@Z
void CClassTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL bControl = GetKeyState(VK_CONTROL) < 0;
    HTREEITEM hItem = GetSelectedItem();
    if (bControl && nChar == 'C' && hItem) {
        if (GetItemData(hItem) == 3)
            m_strCopy = GetClassText(hItem);
        else if (GetItemData(hItem) == 2)
            m_strCopy = GetMemberText(hItem);
        CopyToClipboard(m_strCopy);
    } else {
        CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

// 0x100023a0 ?PreTranslateMessage@CClassTree@@UAEHPAUtagMSG@@@Z
BOOL CClassTree::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        DispatchMessage(pMsg);
        return TRUE;
    }
    return CTreeCtrl::PreTranslateMessage(pMsg);
}

MfClassInfo *MfClassInfo::pInstance;

// 0x10002480 ??0MfClassInfo@@QAE@XZ
MfClassInfo::MfClassInfo()
    : m_nRefs(1)
{
    pInstance = this;
}

// 0x100024a0 ??_GMfClassInfo@@UAEPAXI@Z
// 0x100024c0 ??1MfClassInfo@@UAE@XZ
MfClassInfo::~MfClassInfo()
{
    pInstance = NULL;
}

// 0x100024e0 ?GetInterfaceName@MfClassInfo@@UAEXPADHPAH1@Z
void MfClassInfo::GetInterfaceName(char *name, int size, int *major, int *minor)
{
    strncpy(name, "PscClassInfo", size);
    *major = 1;
    *minor = 0;
}

// 0x10002510 ?AddRef@MfClassInfo@@UAEHXZ (original ?AddRef@MfClassInfo@@UAEHXZ,) 0x10002520 ?Release@MfClassInfo@@UAEHXZ (inline)

static CClassInfo *pClassInfoWindow;

// 0x10002690 ?OnClassInfoClose@@YAXPAVCClassInfo@@@Z (original (called) when the window closes)
static void OnClassInfoClose(CClassInfo *)
{
    pClassInfoWindow = NULL;
}

// 0x10002540 ?ShowClassInfo@MfClassInfo@@UAEXPAVCWnd@@@Z
void MfClassInfo::ShowClassInfo(CWnd *pParent)
{
    if (pClassInfoWindow == NULL)
        pClassInfoWindow = new CClassInfo(pParent, OnClassInfoClose);
    else
        pClassInfoWindow->ShowWindow(SW_SHOW);
}

// 0x100025e0 ?CreateClassTree@MfClassInfo@@UAEHPAVCWnd@@KPAPAVMfAiClassTree@@@Z
int MfClassInfo::CreateClassTree(CWnd *pParent, DWORD dwStyle, MfAiClassTree **ppTree)
{
    CClassTree *pTree = new CClassTree;
    CRect rect(0, 0, 10, 10);
    pTree->Create(dwStyle | TVS_HASBUTTONS | TVS_HASLINES, rect, pParent, 0);
    *ppTree = pTree;
    return 1;
}

// 0x100026a0 _ExtRawDllMain@12
#include <afxdllx.h>

AFX_EXTENSION_MODULE PscClassInfoDLL = { NULL, NULL };

// 0x100026d0 _DllMain@12
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        AfxInitExtensionModule(PscClassInfoDLL, hInstance);
        new CDynLinkLibrary(PscClassInfoDLL);
    }
    return 1;
}

// 0x10002740 _PscLoad@0
extern "C" __declspec(dllexport) int __stdcall PscLoad()
{
    MfAi *pApp;
    MfInterfaceRegistry::GetInterface("ApplicationI", &pApp);
    int result = 0;
    if (pApp) {
        if (((MfAiApplication *)pApp)->GetApplicationType() == 2)
            result = 1;
        pApp->Release();
    }
    return result;
}

// 0x100027d0 _PscInitialize@8
extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *err)
{
    *err = NULL_STRING;
    MfScDataObject::paiciCiInterface = new MfClassInfo;
    return 1;
}

// 0x10002840 _PscShutdown@4
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err)
{
    *err = NULL_STRING;
    MfScDataObject::paiciCiInterface = NULL;
    MfClassInfo::pInstance->Release();
    return 1;
}


// Matched by this code:
// piccoloconsole.dll 0x1000cdd0 _ExtRawDllMain@12
// piccoloconsole.dll 0x1000ce00 _DllMain@12
// piccolort.dll 0x100098e0 ?AddRef@MfClassInfo@@UAEHXZ
// piccolort.dll 0x1000c0f0 ?OnClassInfoClose@@YAXPAVCClassInfo@@@Z
// piccolort.dll 0x1000c5d0 ?OnClassInfoClose@@YAXPAVCClassInfo@@@Z
// 0x10002520 ?Release@MfClassInfo@@UAEHXZ
// plugins/pscdebugger.dll 0x10001350 ?AddRef@MfClassInfo@@UAEHXZ
// plugins/pscdebugger.dll 0x1000b940 _ExtRawDllMain@12
// plugins/pscdebugger.dll 0x1000ba50 _PscLoad@0
// runtimelib.dll 0x100044b0 ?AddRef@MfClassInfo@@UAEHXZ