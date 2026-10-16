// PiccoloConsole.dll 0x1001a840-0x1001aaaf: MfPePwPreferencesDlg, the
// preferences of the editor windows.
#include "PiccoloConsole.h"

// 0x1001a840 ??0MfPePwPreferencesDlg@@QAE@HHHHHHPAVCWnd@@@Z
MfPePwPreferencesDlg::MfPePwPreferencesDlg(int tabSize, int defaultTabSize, int syntaxHighlight,
                                           int defaultSyntaxHighlight, int autoIndent, int defaultAutoIndent,
                                           CWnd *parent)
    : CDialog(109, parent)
{
    MfString tabSizeText(tabSize);
    MfString defaultTabSizeText(defaultTabSize);
    mAutoIndent = autoIndent;
    mSyntaxHighlight = syntaxHighlight;
    mDefaultAutoIndent = defaultAutoIndent;
    mDefaultSyntaxHighlight = defaultSyntaxHighlight;
    mTabSizeText = (const char *)tabSizeText;
    mDefaultTabSizeText = (const char *)defaultTabSizeText;
}

// 0x1001a940 ??_GMfPePwPreferencesDlg@@UAEPAXI@Z

// 0x1001a960 ?DoDataExchange@MfPePwPreferencesDlg@@MAEXPAVCDataExchange@@@Z
void MfPePwPreferencesDlg::DoDataExchange(CDataExchange *dx)
{
    DDX_Check(dx, 1018, mAutoIndent);
    DDX_Check(dx, 1016, mSyntaxHighlight);
    DDX_Check(dx, 1019, mDefaultAutoIndent);
    DDX_Check(dx, 1017, mDefaultSyntaxHighlight);
    DDX_Text(dx, 1010, mTabSizeText);
    DDX_Text(dx, 1011, mDefaultTabSizeText);
}

// 0x1001a9d0 ?OnOK@MfPePwPreferencesDlg@@MAEXXZ
void MfPePwPreferencesDlg::OnOK()
{
    UpdateData(TRUE);
    MfString tabSizeText(mTabSizeText);
    MfString defaultTabSizeText(mDefaultTabSizeText);
    mTabSize = atoi(tabSizeText.GetData());
    mDefaultTabSize = atoi(defaultTabSizeText.GetData());
    if (mTabSize >= 1 && mDefaultTabSize >= 1)
        CDialog::OnOK();
    else
        MessageBox("Invalid Tab Size entered", "Error", MB_ICONEXCLAMATION);
}

// 0x1001aaa0 ?GetMessageMap@MfPePwPreferencesDlg@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPePwPreferencesDlg, CDialog)
END_MESSAGE_MAP()
