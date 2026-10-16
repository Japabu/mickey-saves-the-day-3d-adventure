// CConsoleOptions: the options dialog of the console.
#include "PiccoloConsole.h"

// 0x10007a20 ??0CConsoleOptions@@QAE@HHHHHHPAVCWnd@@@Z
// 0x10007a70 ??_GCConsoleOptions@@UAEPAXI@Z
CConsoleOptions::CConsoleOptions(int clearAfterStore, int storeAfterCompile, int autoSwitchToOutput,
                                 int writeSeparateDebug, int pwTitleBar, int pwToolBar, CWnd *parent)
    : CDialog(0x86, parent)
{
    //{{AFX_DATA_INIT(CConsoleOptions)
    mClearAfterStore = clearAfterStore;
    mStoreAfterCompile = storeAfterCompile;
    mAutoSwitchToOutput = autoSwitchToOutput;
    mWriteSeparateDebug = writeSeparateDebug;
    mPwTitleBar = pwTitleBar;
    mPwToolBar = pwToolBar;
    //}}AFX_DATA_INIT
}

// 0x10007a90 ?DoDataExchange@CConsoleOptions@@MAEXPAVCDataExchange@@@Z
void CConsoleOptions::DoDataExchange(CDataExchange *dx)
{
    CDialog::DoDataExchange(dx);
    //{{AFX_DATA_MAP(CConsoleOptions)
    DDX_Check(dx, 0x3eb, mClearAfterStore);
    DDX_Check(dx, 0x3ea, mStoreAfterCompile);
    DDX_Check(dx, 0x3e8, mAutoSwitchToOutput);
    DDX_Check(dx, 0x3ed, mWriteSeparateDebug);
    DDX_Check(dx, 0x3e9, mPwTitleBar);
    DDX_Check(dx, 0x3ec, mPwToolBar);
    //}}AFX_DATA_MAP
}

// 0x10007b00 ?GetMessageMap@CConsoleOptions@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CConsoleOptions, CDialog)
    //{{AFX_MSG_MAP(CConsoleOptions)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
