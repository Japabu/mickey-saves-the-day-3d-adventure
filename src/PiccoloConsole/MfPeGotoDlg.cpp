// PiccoloConsole.dll 0x10019f60-0x1001a16f: MfPeGotoDlg, the dialog that asks
// for the line to go to.
#include "PiccoloConsole.h"

// 0x10019f60 ??0MfPeGotoDlg@@QAE@HHPAVCWnd@@@Z
MfPeGotoDlg::MfPeGotoDlg(int line, int lines, CWnd *parent)
    : CDialog(108, parent), mCurrentLine(line), mLineCount(lines)
{
    MfString current(mCurrentLine);
    MfString count(mLineCount);
    mCurrentText = (const char *)current;
    mLineText = "";
    mCountText = (const char *)count;
}

// 0x1001a060 ??_GMfPeGotoDlg@@UAEPAXI@Z

// 0x1001a080 ?DoDataExchange@MfPeGotoDlg@@MAEXPAVCDataExchange@@@Z
void MfPeGotoDlg::DoDataExchange(CDataExchange *dx)
{
    DDX_Text(dx, 1006, mCurrentText);
    DDX_Text(dx, 1009, mLineText);
    DDX_Text(dx, 1007, mCountText);
}

// 0x1001a0c0 ?OnOK@MfPeGotoDlg@@MAEXXZ
void MfPeGotoDlg::OnOK()
{
    UpdateData(TRUE);
    MfString text(mLineText);
    mLine = atoi(text.GetData());
    if (mLine >= 1 && mLine <= mLineCount)
        CDialog::OnOK();
    else
        MessageBox("Invalid Line Number", "Error", MB_ICONEXCLAMATION);
}

// 0x1001a160 ?GetMessageMap@MfPeGotoDlg@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPeGotoDlg, CDialog)
END_MESSAGE_MAP()
