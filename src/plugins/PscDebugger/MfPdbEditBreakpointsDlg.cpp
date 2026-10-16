// PscDebugger.dll 0x100069a0-0x10006cae: the breakpoint dialog.
#include "PscDebugger.h"

// Controls of the dialog.
enum {
    IDC_BREAKPOINTS = 0x3ea,
    IDC_REMOVE = 0x3ec,
    IDC_LOCATION = 0x3ee
};

// 0x100069a0 ??0MfPdbEditBreakpointsDlg@@QAE@PAVCWnd@@@Z
MfPdbEditBreakpointsDlg::MfPdbEditBreakpointsDlg(CWnd *parent)
    : CDialog(IDD, parent)
{
}

// 0x10006a30 ??_GMfPdbEditBreakpointsDlg@@UAEPAXI@Z

// 0x10006a50 ?DoDataExchange@MfPdbEditBreakpointsDlg@@MAEXPAVCDataExchange@@@Z
void MfPdbEditBreakpointsDlg::DoDataExchange(CDataExchange *dx)
{
    DDX_Control(dx, IDC_REMOVE, mRemove);
    DDX_Control(dx, IDC_LOCATION, mLocation);
    DDX_Control(dx, IDC_BREAKPOINTS, mBreakpoints);
}

// 0x10006a90 ?GetMessageMap@MfPdbEditBreakpointsDlg@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbEditBreakpointsDlg, CDialog)
    ON_BN_CLICKED(IDC_REMOVE, OnRemove)
    ON_LBN_SELCHANGE(IDC_BREAKPOINTS, OnSelChangeBreakpoints)
END_MESSAGE_MAP()

// 0x10006aa0 ?OnInitDialog@MfPdbEditBreakpointsDlg@@MAEHXZ
BOOL MfPdbEditBreakpointsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    pdbDebugger->mBreakpoints->FillListBox(&mBreakpoints);
    return TRUE;
}

// 0x10006ad0 ?OnRemove@MfPdbEditBreakpointsDlg@@IAEXXZ
void MfPdbEditBreakpointsDlg::OnRemove()
{
    int sel = mBreakpoints.GetCurSel();
    if (sel != LB_ERR
        && pdbDebugger->mBreakpoints->DeleteSetBreakpoint((MfPdbSetBreakpoint *)mBreakpoints.GetItemDataPtr(sel))) {
        mBreakpoints.DeleteString(sel);
        if (mBreakpoints.GetCurSel() == LB_ERR)
            mRemove.EnableWindow(FALSE);
    }
}

// 0x10006b50 ?OnSelChangeBreakpoints@MfPdbEditBreakpointsDlg@@IAEXXZ
void MfPdbEditBreakpointsDlg::OnSelChangeBreakpoints()
{
    int sel = mBreakpoints.GetCurSel();
    if (sel != LB_ERR) {
        MfPdbSetBreakpoint *bp = (MfPdbSetBreakpoint *)mBreakpoints.GetItemDataPtr(sel);
        MfString text;
        text << bp->mLine << ",";
        CString item;
        mBreakpoints.GetText(sel, item);
        text << item;
        mLocation.SetWindowText(text);
        mRemove.EnableWindow(TRUE);
    } else {
        mRemove.EnableWindow(FALSE);
    }
}

// 0x10006c70 ??_GCButton@@UAEPAXI@Z
// 0x10006c90 ??_GCEdit@@UAEPAXI@Z
