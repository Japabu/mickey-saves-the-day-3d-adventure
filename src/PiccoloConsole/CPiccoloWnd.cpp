// PiccoloConsole.dll 0x10009430-0x1000c15f: CPiccoloWnd, an editor window with
// a title bar, tool bars, the code editor and the output windows; the find
// tool's combo box and edit field. The exported inline members of the custom
// tool bar descriptions come first (this is the first unit that uses them).
#include "PiccoloConsole.h"

// 0x10009430 ??HMfPeMetaCustomToolbar@@QAEAAV0@PAVMfPeMetaCustomToolbarEntry@@@Z
// 0x10009450 ?ID@MfPeMetaCustomToolbar@@QBEHXZ
// 0x10009460 ?Name@MfPeMetaCustomToolbar@@QBEABVMfString@@XZ
// 0x10009470 ??0MfPeMetaCustomToolbar@@QAE@ABV0@@Z
// 0x100094b0 ?Clone@MfListItemInterface@@UAEPAXPAX@Z
// 0x100094c0 ?Accept@MfListItemInterface@@UAEHPAX@Z
// 0x100094e0 ?Compare@MfListItemInterface@@UAEHPAX0@Z
// 0x100094f0 ??4MfPeMetaCustomToolbar@@QAEAAV0@ABV0@@Z
// 0x10009520 ??_EMfPeMetaCustomToolbar@@UAEPAXI@Z
// 0x10009570 ??0MfPeMetaCustomToolbarEntry@@QAE@VLiiExemplar@@@Z
// 0x10009580 ??0MfPeMetaCustomToolbarEntry@@QAE@XZ
// 0x10009590 ??0MfPeMetaCustomToolbarEntry@@QAE@H@Z
// 0x100095b0 ?Type@MfPeMetaCustomToolbarEntry@@QBEHXZ
// 0x100095c0 ?ToItem@MfPeMetaCustomToolbarEntry@@QAEPAVMfPeMetaToolbarEntryItem@@XZ
// 0x100095d0 ??0MfPeMetaCustomToolbarEntry@@QAE@ABV0@@Z
// 0x100095f0 ??_EMfPeMetaCustomToolbarEntry@@UAEPAXI@Z
// 0x10009640 ?ToolTip@MfPeMetaToolbarEntryItem@@QBEABVMfString@@XZ
// 0x10009650 ?ID@MfPeMetaToolbarEntryItem@@QBEHXZ
// 0x10009660 ?HandleCommand@MfPeMetaToolbarEntryItem@@QAEHHPAVMfAiPiccoloEditor@@@Z
// 0x10009680 ?HandleUpdate@MfPeMetaToolbarEntryItem@@QAEHHPAVCCmdUI@@@Z
// 0x100096a0 ??0MfPeMetaToolbarEntryItem@@QAE@ABV0@@Z
// 0x10009710 ??4MfPeMetaToolbarEntryItem@@QAEAAV0@ABV0@@Z
// 0x10009740 ??_EMfPeMetaToolbarEntryItem@@UAEPAXI@Z
// 0x10009790 ??0MfPeMetaToolbarEntrySeparator@@QAE@XZ
// 0x100097a0 ??1MfPeMetaCustomToolbarEntry@@UAE@XZ
// 0x100097b0 ??0MfPeMetaToolbarEntrySeparator@@QAE@ABV0@@Z
// 0x100097d0 ??4MfPeMetaCustomToolbarEntry@@QAEAAV0@ABV0@@Z
// 0x100097e0 ??_EMfPeMetaToolbarEntrySeparator@@UAEPAXI@Z

// 0x10009830 _$E248
// 0x10009840 _$E244
// 0x10009860 _$E247
// 0x10009870 _$E245
MfDblList CPiccoloWnd::listMetaToolbars;
// 0x10009890 _$E253
// 0x100098a0 _$E250
// 0x100098c0 _$E252
// 0x100098d0 _$E251
MfDblList CPiccoloWnd::listWindowList;
// 0x100098f0 _$E258
// 0x10009900 _$E255
// 0x10009920 _$E257
// 0x10009930 _$E256
MfDblList CPiccoloWnd::listPiccoloWindows;

// EQUIV 0x10009950 ??0CPiccoloWnd@@QAE@HH@Z: reviewed: the order of independent member initialisations only
CPiccoloWnd::CPiccoloWnd(int flags, int paneSize)
    : mSplitter(0), mToolBar(0), mFindSelect(this), mCursorColumn(1), mCursorLine(1), mEditor(0), mOutput(0),
      mUpdatingCmdUI(0), mFlags(flags), mTitleHeight(0), mPaneSize(paneSize), mTitleFont(0), mTitleTruncated(0),
      mShowingTitle(0), mToolHelp(0), mNextCommand(ID_PE_CUSTOM_FIRST), mCustomToolbars(0)
{
    listPiccoloWindows.AddTail(this);
    mSplitter = new CPiccoloSplitter;
    mToolBar = new CPiccoloToolBar;
    mCustomToolbars = new MfDblList;
    InitTitleBarData();
}

// 0x10009b40 ??1MfPeCeDblClickHandler@@UAE@XZ
// 0x10009b50 ??_GMfPeCeDblClickHandler@@UAEPAXI@Z
// 0x10009b70 ??_GMfDblList@@UAEPAXI@Z
// 0x10009b90 ??_GMfPcsFindSelectEdit@@UAEPAXI@Z
// 0x10009bb0 ??1MfPcsFindSelectEdit@@UAE@XZ
// 0x10009c00 ??_ECPiccoloWnd@@UAEPAXI@Z
// 0x10009c60 ??_GCPiccoloSplitter@@UAEPAXI@Z
// 0x10009c80 ??_GCPiccoloToolBar@@UAEPAXI@Z
// 0x10009ca0 ??1CPiccoloSplitter@@UAE@XZ
// 0x10009cb0 ??1CPiccoloToolBar@@UAE@XZ

// 0x10009cc0 ??1CPiccoloWnd@@UAE@XZ
// 0x10009e70 ??1LiiExemplar@@QAE@XZ
CPiccoloWnd::~CPiccoloWnd()
{
    MfDblListItem *pli = listPiccoloWindows.Find(this);
    listPiccoloWindows.Delete(pli);
    pli = listWindowList.Find(this);
    listWindowList.Delete(pli);
    delete mTitleFont;
    if (mCustomToolbars) {
        LiiExemplar lii;
        MfPeCustomToolbar exemplar(lii);
        mCustomToolbars->Clear(exemplar);
        delete mCustomToolbars;
    }
    delete mSplitter;
    delete mToolBar;
    delete mToolHelp;
}

// 0x10009e80 ?InitTitleBarData@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::InitTitleBarData()
{
    OSVERSIONINFO version;
    NONCLIENTMETRICS metrics;

    delete mTitleFont;
    mTitleFont = new CFont;
    version.dwOSVersionInfoSize = sizeof(version);
    GetVersionEx(&version);
    if (version.dwMajorVersion >= 4) {
        metrics.cbSize = sizeof(metrics);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);
        mTitleHeight = metrics.iSmCaptionHeight + 3;
        mTitleFont->CreateFontIndirect(&metrics.lfSmCaptionFont);
    } else {
        mTitleHeight = 18;
        mTitleFont->CreateFont(8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "arial");
    }
}

// 0x10009f80 ?GetPaneSize@CPiccoloWnd@@QAEHXZ
int CPiccoloWnd::GetPaneSize()
{
    if (mFlags & 8) {
        CRect rect;
        mEditor->GetClientRect(&rect);
        return rect.Height();
    }
    int current, minimum;
    mSplitter->GetRowInfo(0, current, minimum);
    return current;
}

// 0x10009fd0 ?GetMessageMap@CPiccoloWnd@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CPiccoloWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_PICCOLO_COMPILE, OnPiccoloCompile)
    ON_WM_PAINT()
    ON_COMMAND(ID_PICCOLO_CLASSES, OnPiccoloClasses)
    ON_COMMAND(ID_PE_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_PE_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_PE_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_PE_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_PE_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_PE_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_PE_REDO, OnEditRedo)
    ON_UPDATE_COMMAND_UI(ID_PE_REDO, OnUpdateEditRedo)
    ON_COMMAND(ID_PE_UNDO, OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_PE_UNDO, OnUpdateEditUndo)
    ON_COMMAND(ID_PE_FIND_NEXT, OnFindNext)
    ON_COMMAND(ID_PE_FIND_PREVIOUS, OnFindPrevious)
    ON_WM_DESTROY()
    ON_COMMAND(ID_PE_TOGGLE_OVERWRITE, OnToggleOverwrite)
    ON_UPDATE_COMMAND_UI(ID_PE_TOGGLE_OVERWRITE, OnUpdateToggleOverwrite)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_SETTINGCHANGE, OnSettingChange)
    ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipNotify)
    ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipNotify)
    ON_COMMAND_RANGE(ID_PE_CUSTOM_FIRST, ID_PE_CUSTOM_LAST, OnCustomToolbarCommand)
    ON_UPDATE_COMMAND_UI_RANGE(ID_PE_CUSTOM_FIRST, ID_PE_CUSTOM_LAST, OnCustomToolbarUpdate)
END_MESSAGE_MAP()

// 0x10009fe0 ?SetModuleName@CPiccoloWnd@@QAEXABVMfString@@@Z
void CPiccoloWnd::SetModuleName(const MfString &module)
{
    mModuleName = module;
    mEditor->SetModuleName(mModuleName);
    if (mFlags & 1) {
        CRect rect;
        GetClientRect(&rect);
        rect.bottom = mTitleHeight - 1;
        InvalidateRect(&rect, TRUE);
    }
}

// EQUIV 0x1000a050 ?SetFlags@CPiccoloWnd@@QAEXH@Z: reviewed: the masked-bit merge uses other registers and stores the flags one instruction later; same calls
void CPiccoloWnd::SetFlags(int flags)
{
    if (flags != mFlags) {
        if ((~mFlags & 2) && (flags & 2))
            mToolBar->ShowWindow(SW_SHOW);
        if ((mFlags & 2) && (~flags & 2))
            mToolBar->ShowWindow(SW_HIDE);
        mFlags ^= (flags ^ mFlags) & 7;
        CRect rect;
        GetClientRect(&rect);
        AdjustWindows(rect.Width(), rect.Height());
        InvalidateRect(&rect, TRUE);
        if (mFlags & 8)
            mEditor->Invalidate(TRUE);
        else
            mSplitter->Invalidate(TRUE);
    }
}

// 0x1000a130 ?PreCreateWindow@CPiccoloWnd@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL CPiccoloWnd::PreCreateWindow(CREATESTRUCT &cs)
{
    cs.style |= WS_CLIPCHILDREN;
    return CWnd::PreCreateWindow(cs);
}

// 0x1000a150 ?SubclassFindEdit@@YGHPAUHWND__@@J@Z
// The edit field of the find tool's combo box is subclassed as it is found.
static BOOL CALLBACK SubclassFindEdit(HWND hwnd, LPARAM lParam)
{
    CPiccoloWnd *wnd = (CPiccoloWnd *)lParam;
    wnd->mFindEdit.SubclassWindow(hwnd);
    wnd->mFindEdit.mOwner = wnd;
    return TRUE;
}

// 0x1000a180 ?OnCreate@CPiccoloWnd@@IAEHPAUtagCREATESTRUCTA@@@Z
int CPiccoloWnd::OnCreate(LPCREATESTRUCT cs)
{
    if (CWnd::OnCreate(cs) == -1)
        return -1;
    {
        PiccoloConsoleContext context;
        if (!mToolBar->Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP, AFX_IDW_TOOLBAR)
            || !mToolBar->LoadToolBar(103))
            return -1;
        mToolBar->SetBarStyle(mToolBar->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
        EnableToolTips(TRUE);
        mToolBar->SetButtonInfo(7, ID_PE_FIND_SELECT, TBBS_SEPARATOR, 112);
        mToolBar->SetButtonInfo(8, 0, TBBS_SEPARATOR, 8);
        CRect rect;
        mToolBar->GetItemRect(7, &rect);
        rect.top = 2;
        rect.bottom = rect.top + 100;
        if (!mFindSelect.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_HASSTRINGS,
                                rect, mToolBar, ID_PE_FIND_SELECT))
            return -1;
        LOGFONT logFont;
        memset(&logFont, 0, sizeof(logFont));
        if (!GetSystemMetrics(SM_DBCSENABLED)) {
            logFont.lfHeight = -12;
            logFont.lfWeight = FW_BOLD;
            logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
            lstrcpy(logFont.lfFaceName, "MS Sans Serif");
            mFont.CreateFontIndirect(&logFont);
            mFindSelect.SetFont(&mFont);
        } else {
            mFont.Attach(GetStockObject(SYSTEM_FONT));
            mFindSelect.SetFont(&mFont);
        }
        EnumChildWindows(mFindSelect.m_hWnd, SubclassFindEdit, (LPARAM)this);
        if (~mFlags & 2)
            mToolBar->ShowWindow(SW_HIDE);
        if (mPaneSize <= 0)
            mPaneSize = 200;
        if (mFlags & 8) {
            mEditor = new CConsoleEdit;
            mEditor->CreateEx(WS_EX_CLIENTEDGE, NULL, "Edit Window", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, GetSafeHwnd(), NULL);
        } else {
            mSplitter->CreateStatic(this, 2, 1);
            mSplitter->CreateView(0, 0, RUNTIME_CLASS(CConsoleEdit), CSize(0, mPaneSize), NULL);
            mSplitter->CreateView(1, 0, RUNTIME_CLASS(CConsoleOutput), CSize(0, 0), NULL);
            mEditor = (CConsoleEdit *)mSplitter->GetPane(0, 0);
            mOutput = (CConsoleOutput *)mSplitter->GetPane(1, 0);
            if (mFlags & 0x10)
                mOutput->InstallWnd(0);
            if (mFlags & 0x20)
                mOutput->InstallWnd(1);
            if (mFlags & 0x40)
                mOutput->InstallWnd(2);
            int window = -1;
            if (mFlags & 0x40)
                window = 2;
            if (mFlags & 0x20)
                window = 1;
            if (mFlags & 0x10)
                window = 0;
            if (window >= 0)
                mOutput->SwitchToWindow(window);
            mOutput->InstallBuildDblClickHandler(this);
        }
        mUpdatingCmdUI = 0;
        if (mFlags & 4)
            mEditor->SetUnixFormat();
        mEditor->SetPiccoloWindow(this);
        mEditor->RegisterCursorChangedCallback(this);
        mToolHelp = new MfToolHelp;
        mToolHelp->Create(NULL, "", 0, CRect(-100, -100, 0, 0), this, 0);
        mToolHelp->ShowWindow(SW_SHOW);
    }
    MfDblListItem *pli;
    for (MfPeMetaCustomToolbar *meta = (MfPeMetaCustomToolbar *)listMetaToolbars.First(pli); meta;
         meta = (MfPeMetaCustomToolbar *)MfDblList::Next(pli))
        AddCustomToolbar(meta);
    listWindowList.AddTail(this);
    return 0;
}

// 0x1000a5f0 ?OnDestroy@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnDestroy()
{
    CWnd::OnDestroy();
    MfDblListItem *pli = listWindowList.Find(this);
    listWindowList.Delete(pli);
}

// 0x1000a620 ?OnSize@CPiccoloWnd@@IAEXIHH@Z
void CPiccoloWnd::OnSize(UINT type, int cx, int cy)
{
    CWnd::OnSize(type, cx, cy);
    AdjustWindows(cx, cy);
}

// 0x1000a640 ?AdjustWindows@CPiccoloWnd@@IAEXHH@Z
void CPiccoloWnd::AdjustWindows(int cx, int cy)
{
    int top = 0;
    int toolBarHeight;

    if (mFlags & 1)
        top = mTitleHeight;
    if (mFlags & 2) {
        CSize size = mToolBar->CalcFixedLayout(FALSE, TRUE);
        int width = cx;
        if (mCustomToolbars && mCustomToolbars->GetCount() > 0)
            width = size.cx;
        mToolBar->MoveWindow(0, top, width, 30);
        toolBarHeight = ArrangeCustomToolbars(mToolBar, width, top, cx, cy);
    } else {
        toolBarHeight = ArrangeCustomToolbars(NULL, 0, top, cx, cy);
    }
    if (mFlags & 8)
        mEditor->MoveWindow(0, top + toolBarHeight, cx, cy - top - toolBarHeight);
    else
        mSplitter->MoveWindow(0, top + toolBarHeight, cx, cy - top - toolBarHeight);
    mToolBarHeight = toolBarHeight;
}

// Puts the custom tool bars after the tool bar, in rows of 30 pixels;
// returns the height of the rows.
// EQUIV 0x1000a720 ?ArrangeCustomToolbars@CPiccoloWnd@@IAEHPAVCToolBar@@HHHH@Z: reviewed: register choice and one member load scheduled earlier (no store in between)
int CPiccoloWnd::ArrangeCustomToolbars(CToolBar *after, int x, int y, int cx, int cy)
{
    int height = (mFlags & 2) ? 30 : 0;
    if (mCustomToolbars->GetCount() > 0) {
        height = 30;
        int index = 1;
        MfDblListItem *item;
        MfPeCustomToolbar *toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetFirst(item);
        while (toolbar) {
            CSize size = toolbar->mToolBar->CalcFixedLayout(FALSE, TRUE);
            int width = size.cx;
            int right = x + width;
            if (right > cx && right - cx > width / 10) {
                // a new row
                if (after) {
                    CRect rect;
                    CRect frame;
                    ::GetWindowRect(after->m_hWnd, &rect);
                    ::GetWindowRect(m_hWnd, &frame);
                    ::OffsetRect(&rect, -frame.left, -frame.top);
                    rect.right = rect.left + cx;
                    after->MoveWindow(&rect);
                }
                y += 30;
                height += 30;
                x = 0;
            }
            if (index == mCustomToolbars->GetCount())
                width = cx - x;
            after = toolbar->mToolBar;
            toolbar->mToolBar->MoveWindow(x, y, width, 30);
            x += width;
            toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetNext(item);
            index++;
        }
    }
    return height;
}

// EQUIV 0x1000a8a0 ?OnPaint@CPiccoloWnd@@IAEXXZ: reviewed: an argument of the later SendMessage call is pushed before the intervening argument-less string conversion; same arguments
void CPiccoloWnd::OnPaint()
{
    CPaintDC dc(this);

    if (mFlags & 1) {
        CRect client;
        GetClientRect(&client);
        dc.FillRect(&dc.m_ps.rcPaint, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
        CFont *oldFont = dc.SelectObject(mTitleFont);
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(RGB(0, 0, 0));
        int available = client.right - DrawCursorPosition(&dc, 1) - 25;
        if (available < 0)
            available = 25;
        MfString title(MfStringSize(mModuleName.Length() + 64));
        title = "Piccolo Editor";
        if (mModuleName.Length() > 0)
            title << " - " << mModuleName;
        CSize size = dc.GetTextExtent(title, title.Length());
        MfString shown(title);
        mTitleTruncated = 0;
        int cut = 3;
        while (size.cx > available && cut < title.Length()) {
            mTitleTruncated = 1;
            shown = title.LeftString(title.Length() - cut);
            shown << "...";
            size = dc.GetTextExtent(shown, shown.Length());
            cut++;
        }
        title = shown;
        TextOut(dc.m_hDC, 5, 3, title, title.Length());
        dc.SelectObject(oldFont);
    }
}

// 0x1000ab40 ?OnMouseMove@CPiccoloWnd@@IAEXIVCPoint@@@Z
void CPiccoloWnd::OnMouseMove(UINT flags, CPoint point)
{
    if ((mFlags & 1) && mTitleTruncated) {
        CRect rect;
        GetClientRect(&rect);
        rect.bottom = mTitleHeight;
        if (rect.PtInRect(point)) {
            if (!mShowingTitle) {
                SetCapture();
                mShowingTitle = 1;
                ClientToScreen(&rect);
                MfString title(MfStringSize(mModuleName.Length() + 64));
                title = "Piccolo Editor";
                if (mModuleName.Length() > 0)
                    title << " - " << mModuleName;
                mToolHelp->mText = title;
                mToolHelp->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
                mToolHelp->FitText();
                mToolHelp->ShowWindow(SW_SHOW);
                mEditor->SetFocus();
            }
        } else if (mShowingTitle) {
            mShowingTitle = 0;
            ReleaseCapture();
            mToolHelp->ShowWindow(SW_HIDE);
        }
    }
}

// 0x1000ad10 ?EraseCursorPosition@CPiccoloWnd@@QAEXPAVCDC@@@Z
void CPiccoloWnd::EraseCursorPosition(CDC *dc)
{
    if (mFlags & 1) {
        CRect client;
        GetClientRect(&client);
        MfString text(MfStringSize(32));
        text.Format("Ln %d, Col %d", mCursorLine, mCursorColumn);
        CFont *oldFont = dc->SelectObject(mTitleFont);
        dc->SetBkMode(TRANSPARENT);
        dc->SetTextColor(RGB(0, 0, 0));
        CSize size = dc->GetTextExtent(text, text.Length());
        client.right -= 4;
        client.left = client.right - size.cx - 1;
        client.bottom = mTitleHeight;
        dc->FillRect(&client, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
        dc->SelectObject(oldFont);
    }
}

// 0x1000ae50 ?DrawCursorPosition@CPiccoloWnd@@QAEHPAVCDC@@H@Z
int CPiccoloWnd::DrawCursorPosition(CDC *dc, int measure)
{
    int width = 0;

    if (mFlags & 1) {
        CRect client;
        GetClientRect(&client);
        MfString text(MfStringSize(32));
        text.Format("Ln %d, Col %d", mCursorLine, mCursorColumn);
        UINT oldAlign = dc->SetTextAlign(TA_RIGHT);
        CFont *oldFont = dc->SelectObject(mTitleFont);
        dc->SetBkMode(TRANSPARENT);
        dc->SetTextColor(RGB(0, 0, 0));
        dc->TextOut(client.right - 5, 3, text, text.Length());
        dc->SetTextAlign(oldAlign);
        if (measure)
            width = dc->GetTextExtent(text, text.Length()).cx;
        dc->SelectObject(oldFont);
    }
    return width;
}

// 0x1000afa0 ?Compile@CPiccoloWnd@@QAEHHH@Z
int CPiccoloWnd::Compile(int debug, int flags)
{
    MfAiCompiler *compiler;

    mEditor->ClearGlyphs(3);
    if (mOutput) {
        mOutput->SwitchToWindow(1);
        mOutput->Clear(1);
    }
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        MfString source = mEditor->GetEntireContents();
        if (mOutput) {
            mCompileResult = compiler->Compile(mModuleName, "FILE", source, &mOutput->ErrorHandler(),
                                               (debug ? 8 : 0) | flags | 2);
            MfString message;
            message.Format("Compilation Finished %d Error%s, %d Warning%s", compiler->NumberOfErrors(),
                           compiler->NumberOfErrors() == 1 ? "" : "s", compiler->NumberOfWarnings(),
                           compiler->NumberOfWarnings() == 1 ? "" : "s");
            mOutput->Print(1, message, 1);
            mEditor->SetFocus();
        } else {
            mCompileResult = compiler->Compile(mModuleName, "FILE", source, NULL, 0);
        }
        compiler->Release();
    }
    return mCompileResult;
}

// 0x1000b1c0 ?OnPiccoloCompile@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnPiccoloCompile()
{
    Compile(0, 0);
}

// 0x1000b1d0 ?HandleDblClick@CPiccoloWnd@@UAEXABVMfString@@@Z
void CPiccoloWnd::HandleDblClick(const MfString &line)
{
    MfAiCompiler *compiler;

    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        MfString module = compiler->ParseModuleNameFromErrorInfo(line);
        int lineNumber = compiler->ParseLineNumberFromErrorInfo(line);
        if (lineNumber >= 0 && module.CaseCompare(mModuleName) == 0)
            mEditor->GotoLine(lineNumber, 1);
        compiler->Release();
    }
}

// 0x1000b2b0 ?GetInterfaceName@CPiccoloWnd@@UAEXPADHAAH1@Z
void CPiccoloWnd::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "EditorWindowInterface", size);
    major = 1;
    minor = 0;
}

// 0x1000b2e0 ?QueryMainWindow@CPiccoloWnd@@UAEHPAPAVCWnd@@@Z
int CPiccoloWnd::QueryMainWindow(CWnd **wnd)
{
    *wnd = this;
    return 1;
}

// 0x1000b300 ?QueryEditorWindow@CPiccoloWnd@@UAEHPAPAVCWnd@@@Z
int CPiccoloWnd::QueryEditorWindow(CWnd **wnd)
{
    *wnd = mEditor;
    return 1;
}

// 0x1000b320 ?LookupIWindows@CPiccoloWnd@@SAPAVMfAiPeWindows@@PAVMfPeEditorData@@@Z
MfAiPeWindows *CPiccoloWnd::LookupIWindows(MfPeEditorData *data)
{
    MfDblListItem *pli;
    for (CPiccoloWnd *wnd = (CPiccoloWnd *)listPiccoloWindows.First(pli); wnd; wnd = (CPiccoloWnd *)MfDblList::Next(pli)) {
        if (wnd->mEditor->IsAttached(data))
            return wnd;
    }
    return 0;
}

// 0x1000b370 ?OnSettingChange@CPiccoloWnd@@IAEJIJ@Z
LRESULT CPiccoloWnd::OnSettingChange(WPARAM wParam, LPARAM lParam)
{
    InitTitleBarData();
    if (mFlags & 1) {
        CRect rect;
        GetClientRect(&rect);
        rect.bottom = mTitleHeight - 1;
        InvalidateRect(&rect, TRUE);
    }
    return 0;
}

// 0x1000b3c0 ?OnPiccoloClasses@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnPiccoloClasses()
{
    MfScDataObject::DisplayClassInfo(this);
}

// 0x1000b3d0 ?OnEditCopy@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnEditCopy()
{
    mEditor->OnEditCopy();
}

// 0x1000b3e0 ?OnUpdateEditCopy@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnUpdateEditCopy(CCmdUI *cmdUI)
{
    mEditor->OnUpdateEditCopy(cmdUI);
}

// 0x1000b400 ?OnEditCut@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnEditCut()
{
    mEditor->OnEditCut();
}

// 0x1000b410 ?OnUpdateEditCut@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnUpdateEditCut(CCmdUI *cmdUI)
{
    mEditor->OnUpdateEditCut(cmdUI);
}

// 0x1000b430 ?OnEditPaste@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnEditPaste()
{
    mEditor->OnEditPaste();
}

// The same code as OnUpdateEditCopy, which is kept (MfPeCodeEditor's
// OnUpdateEditPaste is folded into OnUpdateEditCopy too).
void CPiccoloWnd::OnUpdateEditPaste(CCmdUI *cmdUI)
{
    mEditor->OnUpdateEditCopy(cmdUI);
}

// 0x1000b440 ?OnEditRedo@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnEditRedo()
{
    mEditor->OnEditRedo();
}

// 0x1000b450 ?OnUpdateEditRedo@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnUpdateEditRedo(CCmdUI *cmdUI)
{
    mEditor->OnUpdateEditRedo(cmdUI);
}

// 0x1000b470 ?OnEditUndo@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnEditUndo()
{
    mEditor->OnEditUndo();
}

// 0x1000b480 ?OnUpdateEditUndo@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnUpdateEditUndo(CCmdUI *cmdUI)
{
    mEditor->OnUpdateEditUndo(cmdUI);
}

// 0x1000b4a0 ?OnFindNext@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnFindNext()
{
    CString text;
    mFindEdit.GetWindowText(text);
    MfString search(text);
    mEditor->SetSearchString(search);
    mEditor->OnFindNext();
}

// 0x1000b540 ?OnFindPrevious@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnFindPrevious()
{
    CString text;
    mFindEdit.GetWindowText(text);
    MfString search(text);
    mEditor->SetSearchString(search);
    mEditor->OnFindPrevious();
}

// 0x1000b5e0 ?SetFocusToEdit@CPiccoloWnd@@QAEXXZ
void CPiccoloWnd::SetFocusToEdit()
{
    mEditor->SetFocus();
}

// 0x1000b5f0 ?SetFocusToFindSelect@CPiccoloWnd@@QAEXXZ
void CPiccoloWnd::SetFocusToFindSelect()
{
    mFindEdit.SetFocus();
}

// 0x1000b600 ?SetSearchString@CPiccoloWnd@@QAEXXZ
void CPiccoloWnd::SetSearchString()
{
    CString text;
    mFindEdit.GetWindowText(text);
    MfString search(text);
    mEditor->SetSearchString(search);
}

// 0x1000b6a0 ?OnToggleOverwrite@CPiccoloWnd@@IAEXXZ
void CPiccoloWnd::OnToggleOverwrite()
{
    mEditor->OnToggleOverwrite();
}

// 0x1000b6b0 ?OnUpdateToggleOverwrite@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnUpdateToggleOverwrite(CCmdUI *cmdUI)
{
    mEditor->OnUpdateToggleOverwrite(cmdUI);
}

// 0x1000b6d0 ?IsFrameWnd@CPiccoloWnd@@MBEHXZ
BOOL CPiccoloWnd::IsFrameWnd() const
{
    return mUpdatingCmdUI;
}

// 0x1000b6e0 ?OnToolTipNotify@CPiccoloWnd@@IAEHIPAUtagNMHDR@@PAJ@Z
BOOL CPiccoloWnd::OnToolTipNotify(UINT id, NMHDR *nmhdr, LRESULT *result)
{
    TOOLTIPTEXTA *textA = (TOOLTIPTEXTA *)nmhdr;
    TOOLTIPTEXTW *textW = (TOOLTIPTEXTW *)nmhdr;
    CString tip;
    UINT from = nmhdr->idFrom;

    if (from) {
        if (from >= ID_PE_CUSTOM_FIRST && from <= ID_PE_CUSTOM_LAST) {
            if (mCustomToolbars) {
                MfString text;
                MfDblListItem *pli;
                for (MfPeCustomToolbar *toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetFirst(pli); toolbar;
                     toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetNext(pli)) {
                    if (toolbar->GetToolTip(from, text))
                        break;
                }
                tip = (const char *)text;
            }
        } else {
            PiccoloConsoleContext context;
            if (from == (UINT)mFindSelect.m_hWnd)
                from = ID_PE_FIND_SELECT;
            TCHAR text[256];
            AfxLoadString(from, text, 256);
            AfxExtractSubString(tip, text, 1, '\n');
        }
    }
    if (nmhdr->code == TTN_NEEDTEXTA)
        lstrcpyn(textA->szText, tip, 80);
    else
        _mbstowcsz(textW->szText, tip, 80);
    *result = 0;
    ::SetWindowPos(nmhdr->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
    return TRUE;
}

// 0x1000b8c0 ?OnCustomToolbarCommand@CPiccoloWnd@@IAEXI@Z
void CPiccoloWnd::OnCustomToolbarCommand(UINT id)
{
    if (mCustomToolbars) {
        MfDblListItem *pli;
        for (MfPeCustomToolbar *toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetFirst(pli); toolbar;
             toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetNext(pli)) {
            if (toolbar->HandleCommand(id, mEditor->GetIEditor()))
                return;
        }
    }
}

// 0x1000b920 ?OnCustomToolbarUpdate@CPiccoloWnd@@IAEXPAVCCmdUI@@@Z
void CPiccoloWnd::OnCustomToolbarUpdate(CCmdUI *cmdUI)
{
    if (mCustomToolbars) {
        MfDblListItem *pli;
        for (MfPeCustomToolbar *toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetFirst(pli); toolbar;
             toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetNext(pli)) {
            if (toolbar->HandleUpdate(cmdUI->m_nID, cmdUI))
                return;
        }
    }
}

// 0x1000b970 ?AddMetaToolbar@CPiccoloWnd@@SAXPAVMfPeMetaCustomToolbar@@@Z
void CPiccoloWnd::AddMetaToolbar(MfPeMetaCustomToolbar *meta)
{
    MfDblListItem *position;

    if (RemoveMetaToolbar(meta->mName, &position)) {
        if (position)
            listMetaToolbars.Insert(meta, MfDblList::LP_BEFORE, position);
        else
            listMetaToolbars.AddLast(meta);
    } else {
        listMetaToolbars.AddLast(meta);
    }
    MfDblListItem *pli;
    for (CPiccoloWnd *wnd = (CPiccoloWnd *)listPiccoloWindows.First(pli); wnd; wnd = (CPiccoloWnd *)MfDblList::Next(pli))
        wnd->AddCustomToolbar(meta);
}

// 0x1000ba10 ?RemoveMetaToolbar@CPiccoloWnd@@SAHABVMfString@@PAPAVMfDblListItem@@@Z
int CPiccoloWnd::RemoveMetaToolbar(const MfString &name, MfDblListItem **position)
{
    MfDblListItem *pli;
    for (MfPeMetaCustomToolbar *meta = (MfPeMetaCustomToolbar *)listMetaToolbars.First(pli); meta;
         meta = (MfPeMetaCustomToolbar *)MfDblList::Next(pli)) {
        if (meta->mName == name) {
            listMetaToolbars.Delete(pli);
            MfDblListItem *wndItem;
            for (CPiccoloWnd *wnd = (CPiccoloWnd *)listPiccoloWindows.First(wndItem); wnd;
                 wnd = (CPiccoloWnd *)MfDblList::Next(wndItem))
                wnd->RemoveCustomToolbar(meta);
            delete meta;
            if (position)
                *position = pli;
            return 1;
        }
    }
    return 0;
}

// 0x1000bae0 ?ClearAllMetaToolbars@CPiccoloWnd@@SAXXZ
void CPiccoloWnd::ClearAllMetaToolbars()
{
    LiiExemplar lii;
    MfPeMetaCustomToolbar exemplar(lii);
    listMetaToolbars.Clear(exemplar);
}

// 0x1000bb40 ?AddCustomToolbar@CPiccoloWnd@@QAEXPAVMfPeMetaCustomToolbar@@@Z
void CPiccoloWnd::AddCustomToolbar(MfPeMetaCustomToolbar *meta)
{
    MfPeCustomToolbar *toolbar = meta->CreateInstance(this, mNextCommand);
    mCustomToolbars->Add(toolbar);
    CRect rect;
    GetClientRect(&rect);
    AdjustWindows(rect.Width(), rect.Height());
    rect.top = mTitleHeight;
    rect.bottom = mToolBarHeight + mTitleHeight;
    InvalidateRect(&rect, TRUE);
}

// 0x1000bbc0 ?RemoveCustomToolbar@CPiccoloWnd@@QAEXPAVMfPeMetaCustomToolbar@@@Z
void CPiccoloWnd::RemoveCustomToolbar(MfPeMetaCustomToolbar *meta)
{
    MfDblListItem *pli;
    for (MfPeCustomToolbar *toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetFirst(pli); toolbar;
         toolbar = (MfPeCustomToolbar *)mCustomToolbars->GetNext(pli)) {
        if (toolbar->mMeta == meta) {
            mCustomToolbars->Delete(pli);
            delete toolbar;
            CRect rect;
            GetClientRect(&rect);
            AdjustWindows(rect.Width(), rect.Height());
            rect.top = mTitleHeight;
            rect.bottom = mToolBarHeight + mTitleHeight;
            InvalidateRect(&rect, TRUE);
            return;
        }
    }
}

// 0x1000bc80 ?SetGlyphOnLineForModule@CPiccoloWnd@@SAXABVMfString@@HH@Z
void CPiccoloWnd::SetGlyphOnLineForModule(const MfString &module, int line, int glyph)
{
    MfDblListItem *pli;
    for (CPiccoloWnd *wnd = (CPiccoloWnd *)listWindowList.First(pli); wnd; wnd = (CPiccoloWnd *)MfDblList::Next(pli)) {
        if (wnd->GetModuleName() == module)
            wnd->mEditor->SetGlyphOnLine(line, glyph);
    }
}

// 0x1000bd40 ?UnsetGlyphOnLineForModule@CPiccoloWnd@@SAXABVMfString@@HH@Z
void CPiccoloWnd::UnsetGlyphOnLineForModule(const MfString &module, int line, int glyph)
{
    MfDblListItem *pli;
    for (CPiccoloWnd *wnd = (CPiccoloWnd *)listWindowList.First(pli); wnd; wnd = (CPiccoloWnd *)MfDblList::Next(pli)) {
        if (wnd->GetModuleName() == module)
            wnd->mEditor->UnsetGlyphOnLine(line, glyph);
    }
}

// EQUIV 0x1000be00 ?ClearDisplayedGlyphs@CPiccoloWnd@@SAXH@Z: reviewed: one branch goes straight to the shared target instead of through a second test of the same flag
void CPiccoloWnd::ClearDisplayedGlyphs(int glyph)
{
    MfDblListItem *pli;
    for (CPiccoloWnd *wnd = (CPiccoloWnd *)listWindowList.First(pli); wnd; wnd = (CPiccoloWnd *)MfDblList::Next(pli))
        wnd->mEditor->ClearGlyphs(glyph);
}

// 0x1000be40 ?CursorChanged@CPiccoloWnd@@UAEXHH@Z
void CPiccoloWnd::CursorChanged(int column, int line)
{
    if (column != mCursorColumn || line != mCursorLine) {
        CDC *dc = GetDC();
        EraseCursorPosition(dc);
        mCursorColumn = column;
        mCursorLine = line;
        DrawCursorPosition(dc, 0);
        ReleaseDC(dc);
    }
}

// 0x1000beb0 ?GetMessageMap@MfPcsFindSelectEdit@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPcsFindSelectEdit, CEdit)
    ON_WM_KEYDOWN()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// 0x1000bec0 ?GetSuperWndProcAddr@MfPcsFindSelectEdit@@MAEPAP6GJPAUHWND__@@IIJ@ZXZ
WNDPROC *MfPcsFindSelectEdit::GetSuperWndProcAddr()
{
    return &mSuperWndProc;
}

// 0x1000bed0 ?PreTranslateMessage@MfPcsFindSelectEdit@@UAEHPAUtagMSG@@@Z
BOOL MfPcsFindSelectEdit::PreTranslateMessage(MSG *msg)
{
    if (msg->message == WM_KEYDOWN
        && (msg->wParam == VK_RETURN || msg->wParam == VK_ESCAPE
            || ((GetKeyState(VK_CONTROL) & 0xfff0) && msg->wParam == 'F'))) {
        DispatchMessage(msg);
        return TRUE;
    }
    return CWnd::PreTranslateMessage(msg);
}

// TODO 0x1000bf30 ?OnKeyDown@MfPcsFindSelectEdit@@IAEXIII@Z: 94% - the original branches to push ID_PE_FIND_PREVIOUS or ID_PE_FIND_NEXT, VC5 here computes the id with setl/add (also with an if/else on a local); same calls, arguments and paths (Return/F3 and plain F share the final Default())
void MfPcsFindSelectEdit::OnKeyDown(UINT key, UINT repeat, UINT flags)
{
    if ((key == VK_DOWN || key == VK_UP) && !mOwner->mFindSelect.GetDroppedState()) {
        mOwner->mFindSelect.ShowDropDown(TRUE);
        return;
    }
    if (key == VK_RETURN || key == VK_F3) {
        if (mOwner->mFindSelect.GetDroppedState())
            mOwner->mFindSelect.ShowDropDown(FALSE);
        GetWindowText(mLastText);
        if (mLastText.GetLength()) {
            int index = mOwner->mFindSelect.FindString(-1, mLastText);
            if (index >= 0)
                mOwner->mFindSelect.DeleteString(index);
            mOwner->mFindSelect.InsertString(0, mLastText);
            SetWindowText(mLastText);
            SendMessage(EM_SETSEL, 0, -1);
            SendMessage(EM_SCROLLCARET);
            UpdateWindow();
            short shift = GetKeyState(VK_SHIFT);
            mOwner->SendMessage(WM_COMMAND, shift < 0 ? ID_PE_FIND_PREVIOUS : ID_PE_FIND_NEXT, (LPARAM)m_hWnd);
        }
        SetFocus();
    } else if (key == VK_ESCAPE) {
        SetWindowText(mLastText);
        mOwner->SetFocusToEdit();
        Default();
        return;
    } else if (key == 'F' && GetKeyState(VK_CONTROL) < 0) {
        return;
    }
    Default();
}

// 0x1000c0b0 ?OnSetFocus@MfPcsFindSelectEdit@@IAEXPAVCWnd@@@Z
void MfPcsFindSelectEdit::OnSetFocus(CWnd *oldWnd)
{
    GetWindowText(mLastText);
    CEdit::OnSetFocus(oldWnd);
}

// 0x1000c0d0 ??0CFindSelectComb@@QAE@PAVCPiccoloWnd@@@Z
CFindSelectComb::CFindSelectComb(CPiccoloWnd *owner)
{
    mOwner = owner;
}

// 0x1000c0f0 ??_GCFindSelectComb@@UAEPAXI@Z

// 0x1000c110 ??1CFindSelectComb@@UAE@XZ
CFindSelectComb::~CFindSelectComb()
{
}

// 0x1000c120 ?GetMessageMap@CFindSelectComb@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CFindSelectComb, CComboBox)
END_MESSAGE_MAP()

// 0x1000c3a0 ?AddRef@CPiccoloWnd@@UAEHXZ
int CPiccoloWnd::AddRef()
{
    return 1;
}

// 0x1000c3b0 ?Release@CPiccoloWnd@@UAEHXZ
int CPiccoloWnd::Release()
{
    return 0;
}

// 0x1000c130 ??_ECPiccoloWnd@@WEA@AEPAXI@Z
// 0x1000c140 ??_GCEdit@@UAEPAXI@Z
