#pragma once
// The logger's user interface, run by its own thread (MfLoggerFrameUI): the
// log window (MfLoggerFrame) with a report list view (MfLoggerWindow), the
// message details dialog and the options dialog. Not exported; class names
// from the RTTI type descriptors, layouts from the code.
#include "MfLogger.h"
#include "MfThread.h"
#include <commctrl.h>
#include <shellapi.h>

// Profile keys; every unit including this header has its own copies.
static char *szLoggerSection = "Logger Options";
static char *szLoggerX = "x";
static char *szLoggerY = "y";
static char *szLoggerCX = "cx";
static char *szLoggerCY = "cy";
static char *szLoggerFlags = "Flags";

// Resources of mfutil.dll
#define IDB_LOGGER_TYPES        102     // image list: one 16x16 image per type
#define IDD_LOG_MESSAGE         103
#define IDI_LOGGER              104
#define IDR_LOGGER_TRAY         106
#define IDD_LOGGER_OPTIONS      108

// Controls of the message dialog
#define IDC_LOG_EXTRA           1000
#define IDC_LOG_DATE            1001
#define IDC_LOG_TIME            1002
#define IDC_LOG_SOURCE          1003
#define IDC_LOG_MESSAGE         1004
#define IDC_LOG_NEXT            1005
#define IDC_LOG_PREV            1006
#define IDC_LOG_TYPE            1007
#define IDC_LOG_ICON            1026

// Controls of the options dialog
#define IDC_OPT_APPLY           1013
#define IDC_OPT_TITLE           1015
#define IDC_OPT_SOURCE          1017
#define IDC_OPT_MESSAGE         1018
#define IDC_OPT_EXTRA           1019
#define IDC_OPT_SOURCE_ON       1020
#define IDC_OPT_MESSAGE_ON      1021
#define IDC_OPT_EXTRA_ON        1022
#define IDC_OPT_QUIET           1023
#define IDC_OPT_NOWINDOW        1024
#define IDC_OPT_ERRORS          1025
#define IDC_OPT_WARNINGS        1026
#define IDC_OPT_INFORMATION     1027

// Commands of the tray menu and the system menu
#define ID_LOGGER_ONTOP         40001
#define ID_LOGGER_SHOW          40021
#define ID_LOGGER_HIDE          40022
#define ID_LOGGER_CLEAR         40023
#define ID_LOGGER_OPTIONS       40024
#define IDM_LOGGER_ONTOP        0x100
#define IDM_LOGGER_CLEAR        0x200
#define IDM_LOGGER_OPTIONS      0x400

#define WM_LOGGER_TRAY          (WM_USER + 100)

class MfLoggerOptions;

// The report list view: one row per message, the text comes from the message.
class MfLoggerWindow {
public:
    MfLoggerWindow();
    virtual ~MfLoggerWindow();
    HWND GetHwnd() { return hWnd; }
    HIMAGELIST GetImageList() { return hImageList; }
    int InsertColumn(int iCol, const MfString &fsText, int iFormat, int iWidth, int iSubItem);
    MfLogger::MfLogMessage *GetMessage(int iItem);
    void AddMessage(MfLogger::MfLogMessage *pMessage);
    MfLogger::MfLogMessage *NextMessage(int iItem);
    MfLogger::MfLogMessage *PrevMessage(int iItem);
    void Attach(HWND hwnd);
    void OnGetDispInfo(NMHDR *pnmh, LRESULT *pResult);
    void OnDblClick(NMHDR *pnmh, LRESULT *pResult);

    HWND hWnd;                  // +4
    HIMAGELIST hImageList;      // +8
};

// The log window, with a tray icon while window output is enabled.
class MfLoggerFrame {
public:
    MfLoggerFrame();
    virtual ~MfLoggerFrame();
    HWND__ *GetLoggerHwnd() { return hWnd; }
    void AddTrayIcon();
    void RemoveTrayIcon();
    int OnCreate(CREATESTRUCT *pcs);
    void OnSize(UINT uiType, int cx, int cy);
    void OnMove(int x, int y);
    void Hide();
    void OnTimer(UINT uiTimer);
    LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    void OptionsClosed();
    void DisplayLogMessageWindow();
    void Close();
    void ToggleOnTop();
    void OnSysCommand(WPARAM wParam, LPARAM lParam);
    void OnShow();
    void OnHide();
    LRESULT OnNotify(NMHDR *pnmh);
    void OnDestroy();
    void OnOptions();
    void OnClear();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    HWND hWnd;                  // +0x04
    int iWidth;                 // +0x08
    int iHeight;                // +0x0c
    int iX;                     // +0x10
    int iY;                     // +0x14
    MfLoggerWindow *pList;      // +0x18
    int bOnTop;                 // +0x1c
    MfLoggerOptions *pOptions;  // +0x20
    int bTrayIcon;              // +0x24
    UINT uiMsg;                 // +0x28 the message being handled
    WPARAM wParam;              // +0x2c
    LPARAM lParam;              // +0x30

    static const char *pszClassName;
};

// Modal dialog showing one message; Next and Previous walk the list view.
class MfLogMessageDialog {
public:
    MfLogMessageDialog(MfLogger::MfLogMessage *pMessage, HIMAGELIST hImageList,
                       MfLoggerWindow *pList, HWND hParent);
    int DoModal();
    virtual BOOL OnInitDialog();
    void OnNext();
    void OnPrev();
    static BOOL CALLBACK DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    HWND hExtra;                // +0x04
    HWND hDate;                 // +0x08
    HWND hTime;                 // +0x0c
    HWND hMessage;              // +0x10
    HWND hSource;               // +0x14
    HWND hType;                 // +0x18
    MfLogger::MfLogMessage *pMessage;   // +0x1c
    HWND hDlg;                  // +0x20
    MfLoggerWindow *pList;      // +0x24
    HWND hParent;               // +0x28
    HIMAGELIST hImageList;      // +0x2c
};

// Modeless options dialog: quiet mode, window output and the six filters.
class MfLoggerOptions {
public:
    MfLoggerOptions(HWND hParent, MfLoggerFrame *pFrame)
        : bChanged(0), bInitializing(0), hDlg(0), hParent(hParent), pFrame(pFrame) {}
    HWND Create();
    BOOL IsDialogMessage(MSG *pMsg);
    void InitFilter(int iFilter, int idCheck, int idEdit);
    virtual BOOL OnInitDialog();
    virtual void OnClose();
    void ApplyFilter(int iFilter, int idCheck, int idEdit);
    virtual void OnApply();
    void SetChanged();
    void ClearChanged();
    static BOOL CALLBACK DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    int bChanged;               // +0x04 enables Apply
    int bInitializing;          // +0x08
    HWND hDlg;                  // +0x0c
    HWND hParent;               // +0x10
    MfLoggerFrame *pFrame;      // +0x14
};

// The logger thread: runs the window's message loop.
class MfLoggerFrameUI : public MfThread, public MfThreadWork {
public:
    MfLoggerFrameUI() : MfThread(MF_THREAD_AUTODELETE) {}
    virtual ~MfLoggerFrameUI() {}
    virtual int InitInstance();
    virtual int ExitInstance();
    virtual void DoWork();

    MfLoggerFrame *pFrame;      // +0x24
};
