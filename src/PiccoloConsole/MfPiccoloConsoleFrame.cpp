// PiccoloConsole.dll 0x1000c4a0-0x1000c97f: MfPiccoloConsoleFrame, the frame
// window of the script console with its tool bar.
#include "PiccoloConsole.h"

// 0x1000c4a0 ??0MfPiccoloConsoleFrame@@QAE@PAVCWnd@@PAVMfConsoleFrameEventCallback@@@Z
MfPiccoloConsoleFrame::MfPiccoloConsoleFrame(CWnd *parent, MfConsoleFrameEventCallback *callback)
    : mEmpty(1), mLogging(1), mCallback(callback)
{
    CreateEx(0, NULL, "Piccolo Console", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
             parent->GetSafeHwnd(), NULL);
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

// 0x1000c580 ??_EMfPiccoloConsoleFrame@@UAEPAXI@Z

// 0x1000c5e0 ??1MfPiccoloConsoleFrame@@UAE@XZ
MfPiccoloConsoleFrame::~MfPiccoloConsoleFrame()
{
}

// 0x1000c640 ?BmcbNotify@MfPiccoloConsoleFrame@@MAEXH@Z
void MfPiccoloConsoleFrame::BmcbNotify(int empty)
{
    if (mEmpty != empty) {
        mEmpty = empty;
        BOOL enable = !empty;
        mToolBar.GetToolBarCtrl().EnableButton(ID_PCW_CLEAR, enable);
    }
}

// 0x1000c680 ?_GetBaseMessageMap@MfPiccoloConsoleFrame@@KGPBUAFX_MSGMAP@@XZ
// 0x1000c690 ?GetMessageMap@MfPiccoloConsoleFrame@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPiccoloConsoleFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_NCDESTROY()
    ON_COMMAND(ID_PCW_CLEAR, OnPcwClear)
    ON_UPDATE_COMMAND_UI(ID_PCW_CLEAR, OnUpdatePcwClear)
    ON_COMMAND(ID_PCW_TOGGLE_LOGGING, OnPcwToggleLogging)
END_MESSAGE_MAP()

// 0x1000c6a0 ?PreCreateWindow@MfPiccoloConsoleFrame@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPiccoloConsoleFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    cs.lpszClass = AfxRegisterWndClass(CS_OWNDC, NULL, NULL,
                                       LoadIcon(hPiccoloConsoleResources, MAKEINTRESOURCE(132)));
    return CFrameWnd::PreCreateWindow(cs);
}

// 0x1000c6e0 ?OnCreate@MfPiccoloConsoleFrame@@IAEHPAUtagCREATESTRUCTA@@@Z
int MfPiccoloConsoleFrame::OnCreate(LPCREATESTRUCT cs)
{
    if (CFrameWnd::OnCreate(cs) == -1)
        return -1;
    if (mToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP, AFX_IDW_TOOLBAR)) {
        PiccoloConsoleContext context;
        mToolBar.LoadBitmap(116);
        UINT buttons[2];
        buttons[0] = ID_PCW_CLEAR;
        buttons[1] = ID_PCW_TOGGLE_LOGGING;
        mToolBar.SetButtons(buttons, 2);
        mToolBar.SetButtonInfo(0, ID_PCW_CLEAR, TBBS_BUTTON, 0);
        mToolBar.SetButtonInfo(1, ID_PCW_TOGGLE_LOGGING, TBBS_BUTTON, 1);
    }
    mToolBar.SetBarStyle((mToolBar.GetBarStyle() & ~CBRS_ALIGN_TOP) | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY);
    mConsole = new MfPiccoloConsoleWnd;
    mConsole->Create(CRect(0, 0, 0, 0), this, 0, WS_CHILD, WS_EX_CLIENTEDGE);
    mConsole->ShowWindow(SW_SHOW);
    mConsole->InstallBufferModifiedCallBack(this);
    return 0;
}

// 0x1000c880 ?OnSize@MfPiccoloConsoleFrame@@IAEXIHH@Z
void MfPiccoloConsoleFrame::OnSize(UINT type, int cx, int cy)
{
    CRect rect;

    CFrameWnd::OnSize(type, cx, cy);
    mToolBar.GetWindowRect(&rect);
    mConsole->MoveWindow(rect.Width(), 0, cx - rect.Width(), cy);
}

// 0x1000c8e0 ?OnNcDestroy@MfPiccoloConsoleFrame@@IAEXXZ
void MfPiccoloConsoleFrame::OnNcDestroy()
{
    if (mCallback)
        mCallback->FrameDestroyed(this);
    CWnd::OnNcDestroy();
}

// 0x1000c900 ?OnPcwClear@MfPiccoloConsoleFrame@@IAEXXZ
void MfPiccoloConsoleFrame::OnPcwClear()
{
    mConsole->Clear();
}

// 0x1000c910 ?OnUpdatePcwClear@MfPiccoloConsoleFrame@@IAEXPAVCCmdUI@@@Z
void MfPiccoloConsoleFrame::OnUpdatePcwClear(CCmdUI *cmdUI)
{
    cmdUI->Enable(mEmpty == 0);
}

// 0x1000c930 ?OnPcwToggleLogging@MfPiccoloConsoleFrame@@IAEXXZ
void MfPiccoloConsoleFrame::OnPcwToggleLogging()
{
    mLogging = !mLogging;
    if (mLogging) {
        mConsole->Enable();
        mToolBar.SetButtonInfo(1, ID_PCW_TOGGLE_LOGGING, TBBS_BUTTON, 1);
    } else {
        mConsole->Disable();
        mToolBar.SetButtonInfo(1, ID_PCW_TOGGLE_LOGGING, TBBS_BUTTON, 2);
    }
}
