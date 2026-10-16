// PiccoloConsole.dll 0x1000c980-0x1000ca4f: MfPiccoloParametersDlg, the
// parameters of a script run.
#include "PiccoloConsole.h"

// 0x1000c980 ??0MfPiccoloParametersDlg@@QAE@PAVCWnd@@@Z
MfPiccoloParametersDlg::MfPiccoloParametersDlg(CWnd *parent)
    : CDialog(117, parent)
{
    mParameters = "";
}

// 0x1000c9f0 ??_GMfPiccoloParametersDlg@@UAEPAXI@Z

// 0x1000ca10 ?DoDataExchange@MfPiccoloParametersDlg@@MAEXPAVCDataExchange@@@Z
void MfPiccoloParametersDlg::DoDataExchange(CDataExchange *dx)
{
    DDX_Text(dx, 1018, mParameters);
}

// 0x1000ca30 ?_GetBaseMessageMap@MfPiccoloParametersDlg@@KGPBUAFX_MSGMAP@@XZ
// 0x1000ca40 ?GetMessageMap@MfPiccoloParametersDlg@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPiccoloParametersDlg, CDialog)
END_MESSAGE_MAP()
