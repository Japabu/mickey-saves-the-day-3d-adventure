// PiccoloConsole.dll 0x10007b10-0x100089ef: CConsoleOutput, the output windows
// of an editor window on tabs, and the call backs that print into them.
#include "PiccoloConsole.h"

// The names of the tabs of the console, build and diagnostic windows.
static char *sTabNames[] = { "Output", "Build", "Diagnostic" };

// 0x10007b10 ??1MfPcErrorWindowCallBack@@UAE@XZ
MfPcErrorWindowCallBack::~MfPcErrorWindowCallBack()
{
    mOutput->ErrWndCBDestroyed(this);
}

// 0x10007b20 ??_GMfPcErrorWindowCallBack@@UAEPAXI@Z

// EQUIV 0x10007b40 ?CallBack@MfPcErrorWindowCallBack@@UAEXPAVMfIPcErrorInfo@@@Z: reviewed: stack slot of the this copy only
void MfPcErrorWindowCallBack::CallBack(MfIPcErrorInfo *info)
{
    if (mOutput) {
        mOutput->Print(1, info->GetModule(), 1);
        mOutput->Print(1, info->GetMessageA(), 1);
        MfStringList lines;
        info->FormatLine(lines);
        MfDblListItem *pli;
        for (MfString *line = (MfString *)lines.First(pli); line; line = (MfString *)MfDblList::Next(pli))
            mOutput->Print(1, *line, 1);
    }
}

// 0x10007c60 ??1MfDiagnosticOutputCallBack@@UAE@XZ
MfDiagnosticOutputCallBack::~MfDiagnosticOutputCallBack()
{
    mOutput->DiagOutputCBDestroyed(this);
}

// 0x10007cb0 ??_GMfDiagnosticOutputCallBack@@UAEPAXI@Z

// 0x10007cd0 ?CallBack@MfDiagnosticOutputCallBack@@UAEXABVMfScDiagnosticInfo@@@Z
void MfDiagnosticOutputCallBack::CallBack(const MfScDiagnosticInfo &info)
{
    if (mOutput)
        mOutput->Print(2, info.mMessage, 0);
}

// 0x10007cf0 ??1MfRuntimeErrorCallBack@@UAE@XZ
MfRuntimeErrorCallBack::~MfRuntimeErrorCallBack()
{
    mOutput->RtErrorCBDestroyed(this);
}

// 0x10007d40 ??_GMfRuntimeErrorCallBack@@UAEPAXI@Z

// 0x10007d60 ?CallBack@MfRuntimeErrorCallBack@@UAEXABVMfScDiagnosticInfo@@@Z
void MfRuntimeErrorCallBack::CallBack(const MfScDiagnosticInfo &info)
{
    if (mOutput && info.mLevel == 5)
        mOutput->Print(0, info.mMessage, 0);
}

// 0x10007d80 ?CreateObject@CConsoleOutput@@SGPAVCObject@@XZ
// 0x10007de0 ?_GetBaseClass@CConsoleOutput@@KGPAUCRuntimeClass@@XZ
// 0x10007df0 ?GetRuntimeClass@CConsoleOutput@@UBEPAUCRuntimeClass@@XZ
IMPLEMENT_DYNCREATE(CConsoleOutput, CTabCtrl)

// 0x10007e00 ??0CConsoleOutput@@QAE@XZ
CConsoleOutput::CConsoleOutput()
    : mConsole(0), mBuildWnd(0), mDiagWnd(0), mUserCount(0), mCurrentTab(-1), mTabCount(0), mDiagCallBack(0),
      mRuntimeCallBack(0)
{
    memset(mTabWindow, 0, sizeof(mTabWindow));
    memset(mWindowTab, 0, sizeof(mWindowTab));
    Initialize();
    mErrorHandler = new MfPcErrorWindowCallBack(this);
}

// 0x10007ec0 ??_ECConsoleOutput@@UAEPAXI@Z

// 0x10007f20 ??1CConsoleOutput@@UAE@XZ
CConsoleOutput::~CConsoleOutput()
{
    if (mErrorHandler) {
        delete mErrorHandler;
        mErrorHandler = 0;
    }
    if (mDiagCallBack && !MfScDiagnostics::RemoveOutput(mDiagCallBack)) {
        delete mDiagCallBack;
        mDiagCallBack = 0;
    }
    if (mRuntimeCallBack && !MfScDiagnostics::RemoveOutput(mRuntimeCallBack)) {
        delete mRuntimeCallBack;
        mRuntimeCallBack = 0;
    }
}

// 0x10008010 ?DiagOutputCBDestroyed@CConsoleOutput@@QAEXPAVMfDiagnosticOutputCallBack@@@Z
void CConsoleOutput::DiagOutputCBDestroyed(MfDiagnosticOutputCallBack *callBack)
{
    if (callBack == mDiagCallBack)
        mDiagCallBack = 0;
}

// 0x10008030 ?RtErrorCBDestroyed@CConsoleOutput@@QAEXPAVMfRuntimeErrorCallBack@@@Z
void CConsoleOutput::RtErrorCBDestroyed(MfRuntimeErrorCallBack *callBack)
{
    if (callBack == mRuntimeCallBack)
        mRuntimeCallBack = 0;
}

// 0x10008050 ?ErrWndCBDestroyed@CConsoleOutput@@QAEXPAVMfPcErrorWindowCallBack@@@Z
void CConsoleOutput::ErrWndCBDestroyed(MfPcErrorWindowCallBack *callBack)
{
    if (callBack == mErrorHandler)
        mErrorHandler = 0;
}

// 0x10008070 ?GetWindow@CConsoleOutput@@IAEPAVCWnd@@H@Z
CWnd *CConsoleOutput::GetWindow(int window)
{
    CWnd *wnd = 0;
    switch (window) {
    case 0:
        wnd = mConsole;
        break;
    case 1:
        wnd = mBuildWnd;
        break;
    case 2:
        wnd = mDiagWnd;
        break;
    default:
        int user = window - 3;
        if (user >= 0 && user < mUserCount)
            wnd = mUserWnds[user];
        break;
    }
    return wnd;
}

// 0x100080b0 ?SwitchToWindow@CConsoleOutput@@QAEXH@Z
void CConsoleOutput::SwitchToWindow(int window)
{
    if (mCurrentTab == -1 || window != mTabWindow[mCurrentTab]) {
        if (mCurrentTab != -1) {
            CWnd *current = GetWindow(mTabWindow[mCurrentTab]);
            if (current)
                current->ShowWindow(SW_HIDE);
        }
        CWnd *wnd = GetWindow(window);
        if (wnd)
            wnd->ShowWindow(SW_SHOW);
        SetCurSel(mWindowTab[window]);
        mCurrentTab = mWindowTab[window];
    }
}

// 0x10008130 ?Print@CConsoleOutput@@QAEXHABVMfString@@H@Z
void CConsoleOutput::Print(int window, const MfString &text, int newLine)
{
    if (window >= 0 && window < 3) {
        MfPeCodeEditor *editor = dynamic_cast<MfPeCodeEditor *>(GetWindow(window));
        if (editor) {
            MfString line(text);
            if (newLine)
                line << "\r\n";
            editor->AppendText(line);
            editor->PostMessage(WM_PE_UPDATE_CARET);
        }
    }
}

// 0x100081f0 ?Clear@CConsoleOutput@@QAEXH@Z
void CConsoleOutput::Clear(int window)
{
    if (window >= 0 && window < 3) {
        MfPeCodeEditor *editor = dynamic_cast<MfPeCodeEditor *>(GetWindow(window));
        if (editor)
            editor->Clear();
    }
}

// 0x10008230 ?InstallBuildDblClickHandler@CConsoleOutput@@QAEXPAVMfPeCeDblClickHandler@@@Z
void CConsoleOutput::InstallBuildDblClickHandler(MfPeCeDblClickHandler *handler)
{
    if (mBuildWnd)
        mBuildWnd->InstallDblClickHandler(handler);
}

// 0x10008250 ?_GetBaseMessageMap@CConsoleOutput@@KGPBUAFX_MSGMAP@@XZ
// 0x10008260 ?GetMessageMap@CConsoleOutput@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CConsoleOutput, CTabCtrl)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
    ON_WM_NCDESTROY()
END_MESSAGE_MAP()

// 0x10008270 ??_GCGdiObject@@UAEPAXI@Z

// 0x10008290 ?PreCreateWindow@CConsoleOutput@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL CConsoleOutput::PreCreateWindow(CREATESTRUCT &cs)
{
    InitCommonControls();
    cs.lpszClass = WC_TABCONTROL;
    return CWnd::PreCreateWindow(cs);
}

// 0x100082b0 ??1CGdiObject@@UAE@XZ

// 0x10008300 ?OnCreate@CConsoleOutput@@IAEHPAUtagCREATESTRUCTA@@@Z
int CConsoleOutput::OnCreate(LPCREATESTRUCT cs)
{
    if (CTabCtrl::OnCreate(cs) != -1) {
        CDC *dc = GetDC();
        if (mFont.CreatePointFont(80, "MS Sans Serif", dc)) {
            SetFont(&mFont, FALSE);
            ReleaseDC(dc);
            mDiagCallBack = new MfDiagnosticOutputCallBack(this);
            MfScDiagnostics::AddOutput(mDiagCallBack);
            mRuntimeCallBack = new MfRuntimeErrorCallBack(this);
            MfScDiagnostics::AddOutput(mRuntimeCallBack);
            return 0;
        }
        ReleaseDC(dc);
    }
    return -1;
}

// 0x100083f0 ?SetupOutput@CConsoleOutput@@QAEXXZ
void CConsoleOutput::SetupOutput()
{
    mDiagCallBack = new MfDiagnosticOutputCallBack(this);
    MfScDiagnostics::AddOutput(mDiagCallBack);
    mRuntimeCallBack = new MfRuntimeErrorCallBack(this);
    MfScDiagnostics::AddOutput(mRuntimeCallBack);
    mConsole->Enable();
}

// EQUIV 0x10008460 ?InstallWnd@CConsoleOutput@@QAEXH@Z: reviewed: two zero-initialised locals swap slots
void CConsoleOutput::InstallWnd(int window)
{
    CWnd *wnd = 0;
    CRect rect(0, 0, 0, 0);

    switch (window) {
    case 0:
        mConsole = new MfPiccoloConsoleWnd;
        wnd = mConsole;
        mConsole->Create(rect, this, 0, WS_CHILD | WS_BORDER, 0);
        break;
    case 1:
        mBuildWnd = new MfPeCodeEditor;
        wnd = mBuildWnd;
        mBuildWnd->Create(NULL, "", WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL, rect, this, 0);
        mBuildWnd->DoSetReadOnly();
        mBuildWnd->DisableSyntaxHighlight();
        mBuildWnd->SetDblClickSelectsLine();
        break;
    case 2:
        mDiagWnd = new MfPeCodeEditor;
        wnd = mDiagWnd;
        mDiagWnd->Create(NULL, "", WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL, rect, this, 0);
        mDiagWnd->DoSetReadOnly();
        mDiagWnd->DisableSyntaxHighlight();
        break;
    }
    if (wnd) {
        TC_ITEM item;
        mTabWindow[mTabCount] = window;
        mWindowTab[window] = mTabCount;
        item.mask = TCIF_TEXT | TCIF_PARAM;
        MfString title;
        title << "Output window #" << window << "\r\n";
        wnd->SetWindowText(title);
        item.pszText = sTabNames[window];
        item.lParam = (LPARAM)wnd;
        InsertItem(mTabCount, &item);
        mTabCount++;
    }
}

// 0x10008690 ?InstallUserWnd@CConsoleOutput@@QAEHABVMfString@@PAVCWnd@@@Z
int CConsoleOutput::InstallUserWnd(const MfString &name, CWnd *wnd)
{
    int window = -1;
    if (mUserCount < 10) {
        TC_ITEM item;
        mUserWnds[mUserCount] = wnd;
        window = mUserCount + 3;
        mTabWindow[mTabCount] = window;
        mWindowTab[window] = mTabCount;
        item.mask = TCIF_TEXT | TCIF_PARAM;
        MfString title;
        title << "Output window #" << window << "\r\n";
        wnd->SetWindowText(title);
        wnd->ShowWindow(SW_HIDE);
        item.pszText = (char *)name;
        item.lParam = (LPARAM)wnd;
        InsertItem(mTabCount, &item);
        mTabCount++;
        mUserCount++;
    }
    return window;
}

// 0x100087a0 ?OnSize@CConsoleOutput@@IAEXIHH@Z
void CConsoleOutput::OnSize(UINT type, int cx, int cy)
{
    CRect rect;
    int i;

    CTabCtrl::OnSize(type, cx, cy);
    GetClientRect(&rect);
    AdjustRect(FALSE, &rect);
    for (i = 0; i < 3; i++) {
        CWnd *wnd = GetWindow(i);
        if (wnd)
            wnd->MoveWindow(&rect);
    }
    for (i = 0; i < mUserCount; i++)
        mUserWnds[i]->MoveWindow(&rect);
}

// 0x10008850 ??_GCFont@@UAEPAXI@Z
// 0x10008870 ??1CFont@@UAE@XZ

// 0x100088c0 ?OnSelchange@CConsoleOutput@@IAEXPAUtagNMHDR@@PAJ@Z
void CConsoleOutput::OnSelchange(NMHDR *nmhdr, LRESULT *result)
{
    if (mCurrentTab != GetCurSel()) {
        if (mCurrentTab != -1) {
            CWnd *current = GetWindow(mTabWindow[mCurrentTab]);
            if (current)
                current->ShowWindow(SW_HIDE);
        }
        mCurrentTab = GetCurSel();
        CWnd *wnd = GetWindow(mTabWindow[mCurrentTab]);
        if (wnd)
            wnd->ShowWindow(SW_SHOW);
    }
    *result = 0;
}

// 0x10008940 ?OnNcDestroy@CConsoleOutput@@IAEXXZ
void CConsoleOutput::OnNcDestroy()
{
    CWnd::OnNcDestroy();
    delete this;
}

// 0x10008960 ?OnNotify@CConsoleOutput@@MAEHIJPAJ@Z
BOOL CConsoleOutput::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    NMHDR *nmhdr = (NMHDR *)lParam;
    *result = 0;
    for (int i = 0; i < mUserCount; i++) {
        if (mUserWnds[i]->GetSafeHwnd() == nmhdr->hwndFrom)
            *result = mUserWnds[i]->SendMessage(WM_NOTIFY, wParam, lParam);
    }
    return TRUE;
}
