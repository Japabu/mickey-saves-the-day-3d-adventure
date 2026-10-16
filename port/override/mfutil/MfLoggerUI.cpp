// PORT: the window keeps its C++ object in GWLP_USERDATA through Set/GetWindowLongPtr (a LONG truncates the pointer).
#include "MfLoggerUI.h"
#include "UtilityApp.h"

static const char szFrameClass[] = "MfLoggerUIFrame";
const char *MfLoggerFrame::pszClassName;

// 0x10007fc0 ??0MfLoggerFrame@@QAE@XZ
// Registers the window class on first use and creates the window where the
// profile says it was last.
MfLoggerFrame::MfLoggerFrame()
    : hWnd(0), pList(0), bOnTop(0), pOptions(0), bTrayIcon(0)
{
    if (!pszClassName) {
        WNDCLASS wc;
        pszClassName = szFrameClass;
        wc.style = CS_OWNDC | CS_DBLCLKS;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = UtilityApp::LibInstance();
        wc.hIcon = LoadIcon(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDI_LOGGER));
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName = 0;
        wc.lpszClassName = pszClassName;
        RegisterClass(&wc);
    }
    UtilityApp app;
    iX = app.GetProfileInt(szLoggerSection, szLoggerX, 50);
    iY = app.GetProfileInt(szLoggerSection, szLoggerY, 50);
    iWidth = app.GetProfileInt(szLoggerSection, szLoggerCX, 300);
    iHeight = app.GetProfileInt(szLoggerSection, szLoggerCY, 100);
    CreateWindowEx(WS_EX_CLIENTEDGE, pszClassName, "", WS_OVERLAPPEDWINDOW,
                   iX, iY, iWidth, iHeight, 0, 0, UtilityApp::LibInstance(), this);
    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
    AppendMenu(hMenu, MF_STRING, IDM_LOGGER_ONTOP, "Always on &Top");
    AppendMenu(hMenu, MF_STRING, IDM_LOGGER_CLEAR, "Clea&r");
    AppendMenu(hMenu, MF_STRING, IDM_LOGGER_OPTIONS, "&Options...");
}

// 0x10008280 ??_GMfLoggerFrame@@UAEPAXI@Z

// 0x100082a0 ??1MfLoggerFrame@@UAE@XZ
MfLoggerFrame::~MfLoggerFrame()
{
}

// 0x100082b0 ?WndProc@MfLoggerFrame@@SGJPAUHWND__@@IIJ@Z
LRESULT CALLBACK MfLoggerFrame::WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    MfLoggerFrame *pFrame;
    if (uiMsg == WM_CREATE) {
        CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        pFrame = (MfLoggerFrame *)pcs->lpCreateParams;
        pFrame->hWnd = hwnd;
        lResult = pFrame->OnCreate(pcs);
    } else {
        pFrame = (MfLoggerFrame *)GetWindowLongPtr(hwnd, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        if (pFrame) {
            pFrame->uiMsg = uiMsg;
            pFrame->wParam = wParam;
            pFrame->lParam = lParam;
        }
        if (!pFrame)
            return DefWindowProc(hwnd, uiMsg, wParam, lParam);
        switch (uiMsg) {
        case WM_MOVE:
            pFrame->OnMove(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_SIZE:
            pFrame->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_CLOSE:
            pFrame->Hide();
            break;
        case WM_NOTIFY:
            lResult = pFrame->OnNotify((NMHDR *)lParam);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case ID_LOGGER_ONTOP:
                pFrame->ToggleOnTop();
                break;
            case ID_LOGGER_CLEAR:
                pFrame->OnClear();
                break;
            case ID_LOGGER_OPTIONS:
                pFrame->OnOptions();
                break;
            case ID_LOGGER_SHOW:
                pFrame->OnShow();
                break;
            case ID_LOGGER_HIDE:
                pFrame->OnHide();
                break;
            }
            break;
        case WM_SYSCOMMAND:
            pFrame->OnSysCommand(wParam, lParam);
            break;
        case WM_TIMER:
            pFrame->OnTimer(wParam);
            break;
        case WM_LOGGER_TRAY:
            lResult = pFrame->OnTrayNotify(wParam, lParam);
            break;
        case WM_DESTROY:
            pFrame->OnDestroy();
            // and the default processing
        default:
            lResult = DefWindowProc(hwnd, uiMsg, wParam, lParam);
            break;
        }
    }
    return lResult;
}

// 0x100084f0 ?AddTrayIcon@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::AddTrayIcon()
{
    if (!bTrayIcon) {
        NOTIFYICONDATA nid;
        bTrayIcon = 1;
        HICON hIcon = (HICON)LoadImage(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDI_LOGGER),
                                       IMAGE_ICON, 16, 16, 0);
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = (UINT)hWnd;
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        nid.uCallbackMessage = WM_LOGGER_TRAY;
        nid.hIcon = hIcon;
        lstrcpy(nid.szTip, "Motion Factory Logger");
        Shell_NotifyIcon(NIM_ADD, &nid);
    }
}

// 0x10008570 ?RemoveTrayIcon@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::RemoveTrayIcon()
{
    if (bTrayIcon == 1) {
        NOTIFYICONDATA nid;
        bTrayIcon = 0;
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = (UINT)hWnd;
        nid.uFlags = 0;
        nid.hIcon = 0;
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
}

// 0x100085b0 ?OnCreate@MfLoggerFrame@@QAEHPAUtagCREATESTRUCTA@@@Z
int MfLoggerFrame::OnCreate(CREATESTRUCT *pcs)
{
    pList = new MfLoggerWindow;
    pList->Attach(CreateWindowEx(0, "SysListView32", "", WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                 0, 0, 0, 0, hWnd, 0, UtilityApp::LibInstance(), 0));
    SetWindowText(hWnd, MfLogger::fsLogWindowTitle);
    MfLogger::pwndLogMessageWindow = this;
    if (!SetTimer(hWnd, 1, 100, 0))
        return -1;
    if (MfLogger::bWindowEnabled)
        AddTrayIcon();
    return 0;
}

// 0x100086a0 ?OnSize@MfLoggerFrame@@QAEXIHH@Z
void MfLoggerFrame::OnSize(UINT uiType, int cx, int cy)
{
    RECT rc;
    if (!IsIconic(hWnd)) {
        GetWindowRect(hWnd, &rc);
        iWidth = rc.right - rc.left;
        iHeight = rc.bottom - rc.top;
    }
    GetClientRect(hWnd, &rc);
    MoveWindow(pList->GetHwnd(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    if (uiType == SIZE_MINIMIZED)
        Hide();
}

// 0x10008730 ?OnMove@MfLoggerFrame@@QAEXHH@Z
void MfLoggerFrame::OnMove(int x, int y)
{
    if (!IsIconic(hWnd)) {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        iX = rc.left;
        iY = rc.top;
    }
}

// 0x10008770 ?Hide@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::Hide()
{
    ShowWindow(hWnd, SW_HIDE);
    if (pOptions)
        ShowWindow(pOptions->hDlg, SW_HIDE);
}

// 0x100087a0 ?OnTimer@MfLoggerFrame@@QAEXI@Z
void MfLoggerFrame::OnTimer(UINT uiTimer)
{
    MfLogger::DoWork();
}

// 0x100087b0 ?OnTrayNotify@MfLoggerFrame@@QAEJIJ@Z
LRESULT MfLoggerFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    switch (lParam) {
    case WM_RBUTTONUP: {
        HMENU hMenu = LoadMenu(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDR_LOGGER_TRAY));
        HMENU hPopup = GetSubMenu(hMenu, 0);
        POINT pt;
        GetCursorPos(&pt);
        SetForegroundWindow(hWnd);
        SetMenuDefaultItem(hPopup, ID_LOGGER_SHOW, FALSE);
        CheckMenuItem(hPopup, ID_LOGGER_ONTOP, bOnTop ? MF_CHECKED : MF_UNCHECKED);
        TrackPopupMenu(hPopup, 0, pt.x, pt.y, 0, hWnd, 0);
        DestroyMenu(hMenu);
        break;
    }
    case WM_LBUTTONDBLCLK:
        DisplayLogMessageWindow();
        break;
    }
    return 0;
}

// 0x10008870 ?OptionsClosed@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OptionsClosed()
{
    if (pOptions)
        pOptions = 0;
}

// 0x10008880 ?DisplayLogMessageWindow@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::DisplayLogMessageWindow()
{
    if (IsIconic(hWnd))
        ShowWindow(hWnd, SW_RESTORE);
    else
        ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    if (pOptions)
        ShowWindow(pOptions->hDlg, SW_SHOW);
}

// 0x100088d0 ?Close@MfLoggerFrame@@QAEXXZ
// Removes the tray icon and remembers the window position.
void MfLoggerFrame::Close()
{
    RemoveTrayIcon();
    UtilityApp app;
    app.WriteProfileInt(szLoggerSection, szLoggerX, iX);
    app.WriteProfileInt(szLoggerSection, szLoggerY, iY);
    app.WriteProfileInt(szLoggerSection, szLoggerCX, iWidth);
    app.WriteProfileInt(szLoggerSection, szLoggerCY, iHeight);
}

// 0x10008a80 ?ToggleOnTop@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::ToggleOnTop()
{
    bOnTop = !bOnTop;
    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    CheckMenuItem(hMenu, IDM_LOGGER_ONTOP, bOnTop ? MF_CHECKED : MF_UNCHECKED);
    if (bOnTop)
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    else
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

// 0x10008ae0 ?OnSysCommand@MfLoggerFrame@@QAEXIJ@Z
void MfLoggerFrame::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
    switch (wParam & 0xfff0) {
    case IDM_LOGGER_ONTOP:
        ToggleOnTop();
        break;
    case IDM_LOGGER_CLEAR:
        OnClear();
        break;
    case IDM_LOGGER_OPTIONS:
        OnOptions();
        break;
    default:
        DefWindowProc(hWnd, uiMsg, this->wParam, this->lParam);
        break;
    }
}

// 0x10008b30 ?OnShow@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OnShow()
{
    DisplayLogMessageWindow();
}

// 0x10008b40 ?OnHide@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OnHide()
{
    Hide();
}

// 0x10008b50 ?OnNotify@MfLoggerFrame@@QAEJPAUtagNMHDR@@@Z
LRESULT MfLoggerFrame::OnNotify(NMHDR *pnmh)
{
    LRESULT lResult = 0;
    if (pList && pnmh->hwndFrom == pList->GetHwnd()) {
        switch (pnmh->code) {
        case LVN_GETDISPINFO:
            pList->OnGetDispInfo(pnmh, &lResult);
            break;
        case NM_DBLCLK:
            pList->OnDblClick(pnmh, &lResult);
            break;
        }
    }
    return lResult;
}

// 0x10008bb0 ?OnDestroy@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OnDestroy()
{
    delete pList;
    pList = 0;
}

// 0x10008bd0 ?OnOptions@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OnOptions()
{
    if (!pOptions && pList) {
        pOptions = new MfLoggerOptions(pList->GetHwnd(), this);
        pOptions->Create();
    } else if (pOptions && IsIconic(hWnd))
        DisplayLogMessageWindow();
}

// 0x10008c50 ?OnClear@MfLoggerFrame@@QAEXXZ
void MfLoggerFrame::OnClear()
{
    MfLogger::ClearWindow();
}

// 0x10008c60 ??0MfLoggerWindow@@QAE@XZ
MfLoggerWindow::MfLoggerWindow()
    : hWnd(0), hImageList(0)
{
}

// 0x10008c80 ??_GMfLoggerWindow@@UAEPAXI@Z

// 0x10008ca0 ??1MfLoggerWindow@@UAE@XZ
MfLoggerWindow::~MfLoggerWindow()
{
    ImageList_Destroy(hImageList);
}

// 0x10008cc0 ?InsertColumn@MfLoggerWindow@@QAEHHABVMfString@@HHH@Z
int MfLoggerWindow::InsertColumn(int iCol, const MfString &fsText, int iFormat, int iWidth, int iSubItem)
{
    LV_COLUMN col;
    col.fmt = iFormat;
    col.cx = iWidth;
    col.pszText = fsText;
    col.cchTextMax = fsText.Length();
    col.iSubItem = iSubItem;
    col.mask = LVCF_FMT | LVCF_TEXT;
    if (iWidth >= 0)
        col.mask |= LVCF_WIDTH;
    if (iSubItem >= 0)
        col.mask |= LVCF_SUBITEM;
    return SendMessage(hWnd, LVM_INSERTCOLUMN, iCol, (LPARAM)&col);
}

// 0x10008d40 ?GetMessageA@MfLoggerWindow@@QAEPAVMfLogMessage@MfLogger@@H@Z
MfLogger::MfLogMessage *MfLoggerWindow::GetMessage(int iItem)
{
    LV_ITEM item;
    item.mask = LVIF_PARAM;
    item.iItem = iItem;
    MfLogger::MfLogMessage *pMessage = 0;
    if (SendMessage(hWnd, LVM_GETITEM, 0, (LPARAM)&item))
        pMessage = (MfLogger::MfLogMessage *)item.lParam;
    return pMessage;
}

// 0x10008d80 ?AddMessage@MfLoggerWindow@@QAEXPAVMfLogMessage@MfLogger@@@Z
void MfLoggerWindow::AddMessage(MfLogger::MfLogMessage *pMessage)
{
    LV_ITEM item;
    item.iItem = SendMessage(hWnd, LVM_GETITEMCOUNT, 0, 0);
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iSubItem = 0;
    item.iImage = I_IMAGECALLBACK;
    item.pszText = LPSTR_TEXTCALLBACK;
    item.lParam = (LPARAM)pMessage;
    pMessage->iItem = SendMessage(hWnd, LVM_INSERTITEM, 0, (LPARAM)&item);
    SendMessage(hWnd, LVM_ENSUREVISIBLE, pMessage->iItem, FALSE);
}

// 0x10008df0 ?NextMessage@MfLoggerWindow@@QAEPAVMfLogMessage@MfLogger@@H@Z
MfLogger::MfLogMessage *MfLoggerWindow::NextMessage(int iItem)
{
    int iNext = SendMessage(hWnd, LVM_GETNEXTITEM, iItem, LVNI_ALL);
    if (iNext >= 0)
        return GetMessage(iNext);
    return 0;
}

// 0x10008e20 ?PrevMessage@MfLoggerWindow@@QAEPAVMfLogMessage@MfLogger@@H@Z
MfLogger::MfLogMessage *MfLoggerWindow::PrevMessage(int iItem)
{
    int iPrev = iItem - 1;
    if (iPrev >= 0)
        return GetMessage(iPrev);
    return 0;
}

// 0x10008e40 ?Attach@MfLoggerWindow@@QAEXPAUHWND__@@@Z
void MfLoggerWindow::Attach(HWND hwnd)
{
    hWnd = hwnd;
    hImageList = ImageList_LoadImage(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDB_LOGGER_TYPES),
                                     16, 1, RGB(255, 0, 255), IMAGE_BITMAP, 0);
    SendMessage(hWnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hImageList);
    InsertColumn(0, "Date", LVCFMT_LEFT, 100, -1);
    InsertColumn(1, "Time", LVCFMT_LEFT, 100, -1);
    InsertColumn(2, "Source", LVCFMT_LEFT, 150, -1);
    InsertColumn(3, "Category", LVCFMT_LEFT, 150, -1);
    InsertColumn(4, "Message", LVCFMT_LEFT, 300, -1);
}

// 0x10008fc0 ?OnGetDispInfo@MfLoggerWindow@@QAEXPAUtagNMHDR@@PAJ@Z
void MfLoggerWindow::OnGetDispInfo(NMHDR *pnmh, LRESULT *pResult)
{
    LV_DISPINFO *pdi = (LV_DISPINFO *)pnmh;
    ((MfLogger::MfLogMessage *)pdi->item.lParam)->GetDispInfo(pdi);
    *pResult = 0;
}

// 0x10008fe0 ?OnDblClick@MfLoggerWindow@@QAEXPAUtagNMHDR@@PAJ@Z
void MfLoggerWindow::OnDblClick(NMHDR *pnmh, LRESULT *pResult)
{
    int iItem = SendMessage(hWnd, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (iItem >= 0)
        MfLogger::DisplayLogMessageDialog(GetMessage(iItem));
    *pResult = 0;
}

// 0x10009020 ??0MfLogMessageDialog@@QAE@PAVMfLogMessage@MfLogger@@PAU_IMAGELIST@@PAVMfLoggerWindow@@PAUHWND__@@@Z
MfLogMessageDialog::MfLogMessageDialog(MfLogger::MfLogMessage *pMessage, HIMAGELIST hImageList,
                                       MfLoggerWindow *pList, HWND hParent)
    : pMessage(pMessage), pList(pList), hParent(hParent), hImageList(hImageList)
{
}

// 0x10009050 ?DoModal@MfLogMessageDialog@@QAEHXZ
int MfLogMessageDialog::DoModal()
{
    return DialogBoxParam(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDD_LOG_MESSAGE), hParent,
                          DialogProc, (LPARAM)this);
}

// 0x10009070 ?DialogProc@MfLogMessageDialog@@SGHPAUHWND__@@IIJ@Z
BOOL CALLBACK MfLogMessageDialog::DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bResult = FALSE;
    MfLogMessageDialog *pDialog;
    if (uiMsg == WM_INITDIALOG) {
        pDialog = (MfLogMessageDialog *)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        pDialog->hDlg = hDlg;
        bResult = pDialog->OnInitDialog();
        RECT rc;
        GetWindowRect(hDlg, &rc);
        MoveWindow(hDlg, (GetSystemMetrics(SM_CXMAXIMIZED) - (rc.right - rc.left)) / 2,
                   (GetSystemMetrics(SM_CYMAXIMIZED) - (rc.bottom - rc.top)) / 2,
                   rc.right - rc.left, rc.bottom - rc.top, TRUE);
    } else {
        pDialog = (MfLogMessageDialog *)GetWindowLongPtr(hDlg, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        if (uiMsg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {
            EndDialog(hDlg, 0);
            bResult = TRUE;
        } else if (uiMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED) {
            switch (LOWORD(wParam)) {
            case IDC_LOG_NEXT:
                pDialog->OnNext();
                bResult = TRUE;
                break;
            case IDC_LOG_PREV:
                pDialog->OnPrev();
                bResult = TRUE;
                break;
            }
        } else if (uiMsg == WM_DRAWITEM)
            ImageList_Draw(pDialog->hImageList, pDialog->pMessage->iType,
                           ((DRAWITEMSTRUCT *)lParam)->hDC, 0, 0, ILD_TRANSPARENT);
    }
    return bResult;
}

// 0x100091d0 ?OnInitDialog@MfLogMessageDialog@@UAEHXZ
BOOL MfLogMessageDialog::OnInitDialog()
{
    hExtra = GetDlgItem(hDlg, IDC_LOG_EXTRA);
    hDate = GetDlgItem(hDlg, IDC_LOG_DATE);
    hTime = GetDlgItem(hDlg, IDC_LOG_TIME);
    hMessage = GetDlgItem(hDlg, IDC_LOG_MESSAGE);
    hSource = GetDlgItem(hDlg, IDC_LOG_SOURCE);
    hType = GetDlgItem(hDlg, IDC_LOG_TYPE);
    pMessage->Display(this);
    return TRUE;
}

// 0x10009240 ?OnNext@MfLogMessageDialog@@QAEXXZ
void MfLogMessageDialog::OnNext()
{
    MfLogger::MfLogMessage *pNext = pList->NextMessage(pMessage->iItem);
    if (pNext)
        pNext->Display(this);
    HWND hIcon = GetDlgItem(hDlg, IDC_LOG_ICON);
    InvalidateRect(hIcon, 0, TRUE);
}

// 0x10009280 ?OnPrev@MfLogMessageDialog@@QAEXXZ
void MfLogMessageDialog::OnPrev()
{
    MfLogger::MfLogMessage *pPrev = pList->PrevMessage(pMessage->iItem);
    if (pPrev)
        pPrev->Display(this);
    HWND hIcon = GetDlgItem(hDlg, IDC_LOG_ICON);
    InvalidateRect(hIcon, 0, TRUE);
}

// 0x100092c0 ?Create@MfLoggerOptions@@QAEPAUHWND__@@XZ
HWND MfLoggerOptions::Create()
{
    return CreateDialogParam(UtilityApp::LibInstance(), MAKEINTRESOURCE(IDD_LOGGER_OPTIONS), hParent,
                             DialogProc, (LPARAM)this);
}

// 0x100092e0 ?IsDialogMessageA@MfLoggerOptions@@QAEHPAUtagMSG@@@Z
BOOL MfLoggerOptions::IsDialogMessage(MSG *pMsg)
{
    return ::IsDialogMessage(hDlg, pMsg);
}

// 0x10009300 ?DialogProc@MfLoggerOptions@@SGHPAUHWND__@@IIJ@Z
BOOL CALLBACK MfLoggerOptions::DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bResult = FALSE;
    MfLoggerOptions *pOptions;
    if (uiMsg == WM_INITDIALOG) {
        pOptions = (MfLoggerOptions *)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        pOptions->bInitializing = 1;
        pOptions->hDlg = hDlg;
        bResult = pOptions->OnInitDialog();
        pOptions->bInitializing = 0;
    } else {
        pOptions = (MfLoggerOptions *)GetWindowLongPtr(hDlg, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        switch (uiMsg) {
        case WM_CLOSE:
            pOptions->OnClose();
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDOK:
                pOptions->OnApply();
                pOptions->OnClose();
                break;
            case IDCANCEL:
                pOptions->OnClose();
                break;
            case IDC_OPT_APPLY:
                pOptions->OnApply();
                break;
            case IDC_OPT_SOURCE:
                if (HIWORD(wParam) == EN_CHANGE && !pOptions->bInitializing) {
                    HWND hCheck = GetDlgItem(hDlg, IDC_OPT_SOURCE_ON);
                    SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
                    pOptions->SetChanged();
                }
                break;
            case IDC_OPT_MESSAGE:
                if (HIWORD(wParam) == EN_CHANGE && !pOptions->bInitializing) {
                    HWND hCheck = GetDlgItem(hDlg, IDC_OPT_MESSAGE_ON);
                    SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
                    pOptions->SetChanged();
                }
                break;
            case IDC_OPT_EXTRA:
                if (HIWORD(wParam) == EN_CHANGE && !pOptions->bInitializing) {
                    HWND hCheck = GetDlgItem(hDlg, IDC_OPT_EXTRA_ON);
                    SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
                    pOptions->SetChanged();
                }
                break;
            case IDC_OPT_SOURCE_ON:
            case IDC_OPT_MESSAGE_ON:
            case IDC_OPT_EXTRA_ON:
            case IDC_OPT_QUIET:
            case IDC_OPT_NOWINDOW:
            case IDC_OPT_ERRORS:
            case IDC_OPT_WARNINGS:
            case IDC_OPT_INFORMATION:
                if (HIWORD(wParam) == BN_CLICKED)
                    pOptions->SetChanged();
                break;
            }
            break;
        }
    }
    return bResult;
}

// 0x100094a0 ?InitFilter@MfLoggerOptions@@QAEXHHH@Z
void MfLoggerOptions::InitFilter(int iFilter, int idCheck, int idEdit)
{
    const MfLogger::LoggerFilter *pFilter = MfLogger::QueryFilter(iFilter);
    HWND hCheck = GetDlgItem(hDlg, idCheck);
    if (pFilter->iLevel)
        SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
    if (idEdit) {
        HWND hEdit = GetDlgItem(hDlg, idEdit);
        SetWindowText(hEdit, pFilter->fsName);
    }
}

// 0x10009510 ?OnInitDialog@MfLoggerOptions@@UAEHXZ
BOOL MfLoggerOptions::OnInitDialog()
{
    HWND hTitle = GetDlgItem(hDlg, IDC_OPT_TITLE);
    HFONT hFont = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
    if (hFont)
        SendMessage(hTitle, WM_SETFONT, (WPARAM)hFont, 0);
    HWND hQuiet = GetDlgItem(hDlg, IDC_OPT_QUIET);
    if (MfLogger::iLoggerFlags & MF_LOG_QUIET)
        SendMessage(hQuiet, BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(hQuiet, BM_SETCHECK, BST_UNCHECKED, 0);
    InitFilter(0, IDC_OPT_INFORMATION, 0);
    InitFilter(1, IDC_OPT_WARNINGS, 0);
    InitFilter(2, IDC_OPT_ERRORS, 0);
    InitFilter(3, IDC_OPT_SOURCE_ON, IDC_OPT_SOURCE);
    InitFilter(4, IDC_OPT_MESSAGE_ON, IDC_OPT_MESSAGE);
    InitFilter(5, IDC_OPT_EXTRA_ON, IDC_OPT_EXTRA);
    ClearChanged();
    return TRUE;
}

// 0x10009610 ?OnClose@MfLoggerOptions@@UAEXXZ
void MfLoggerOptions::OnClose()
{
    DestroyWindow(hDlg);
    pFrame->OptionsClosed();
    delete this;
}

// 0x10009630 ?ApplyFilter@MfLoggerOptions@@QAEXHHH@Z
void MfLoggerOptions::ApplyFilter(int iFilter, int idCheck, int idEdit)
{
    int iLevel = 0;
    MfString fsName(NULL_STRING);
    char szName[256];
    HWND hCheck = GetDlgItem(hDlg, idCheck);
    if (SendMessage(hCheck, BM_GETCHECK, 0, 0) == BST_CHECKED)
        iLevel = 1;
    if (idEdit) {
        HWND hEdit = GetDlgItem(hDlg, idEdit);
        GetWindowText(hEdit, szName, 255);
        fsName = szName;
    }
    MfLogger::Filter(iFilter, iLevel, fsName);
}

// 0x10009710 ?OnApply@MfLoggerOptions@@UAEXXZ
void MfLoggerOptions::OnApply()
{
    if (bChanged) {
        HWND hQuiet = GetDlgItem(hDlg, IDC_OPT_QUIET);
        if (SendMessage(hQuiet, BM_GETCHECK, 0, 0) == BST_CHECKED)
            MfLogger::iLoggerFlags |= MF_LOG_QUIET;
        else
            MfLogger::iLoggerFlags &= ~MF_LOG_QUIET;
        HWND hNoWindow = GetDlgItem(hDlg, IDC_OPT_NOWINDOW);
        if (SendMessage(hNoWindow, BM_GETCHECK, 0, 0) == BST_CHECKED)
            MfLogger::EnableWindowOutput(0);
        else
            MfLogger::EnableWindowOutput(1);
        ApplyFilter(0, IDC_OPT_INFORMATION, 0);
        ApplyFilter(1, IDC_OPT_WARNINGS, 0);
        ApplyFilter(2, IDC_OPT_ERRORS, 0);
        ApplyFilter(3, IDC_OPT_SOURCE_ON, IDC_OPT_SOURCE);
        ApplyFilter(4, IDC_OPT_MESSAGE_ON, IDC_OPT_MESSAGE);
        ApplyFilter(5, IDC_OPT_EXTRA_ON, IDC_OPT_EXTRA);
        ClearChanged();
    }
}

// 0x10009800 ?SetChanged@MfLoggerOptions@@QAEXXZ
void MfLoggerOptions::SetChanged()
{
    bChanged = 1;
    HWND hApply = GetDlgItem(hDlg, IDC_OPT_APPLY);
    EnableWindow(hApply, bChanged);
}

// 0x10009830 ?ClearChanged@MfLoggerOptions@@QAEXXZ
void MfLoggerOptions::ClearChanged()
{
    bChanged = 0;
    HWND hApply = GetDlgItem(hDlg, IDC_OPT_APPLY);
    EnableWindow(hApply, bChanged);
}
