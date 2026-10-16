// PscDebugger.dll 0x100080e0-0x10008364: the dialog that selects the module
// the debugger shows, as a tree or as a flat list.
#include "PscDebugger.h"

// Controls of the dialog.
enum {
    IDC_MODULE_TREE = 0x3eb,
    IDC_MODULE_LIST = 0x3ef,
    IDC_SHOW_TREE = 0x3f0
};

// 0x100080e0 ??0MfPdbModuleSelector@@QAE@PAVCWnd@@@Z
MfPdbModuleSelector::MfPdbModuleSelector(CWnd *parent)
    : CDialog(IDD, parent)
{
}

// 0x10008180 ??_GMfPdbModuleSelector@@UAEPAXI@Z

// 0x100081a0 ?DoDataExchange@MfPdbModuleSelector@@MAEXPAVCDataExchange@@@Z
void MfPdbModuleSelector::DoDataExchange(CDataExchange *dx)
{
    DDX_Control(dx, IDC_MODULE_LIST, mList);
    DDX_Control(dx, IDC_SHOW_TREE, mShowTree);
    DDX_Control(dx, IDC_MODULE_TREE, mTree);
}

// 0x100081e0 ?GetMessageMap@MfPdbModuleSelector@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbModuleSelector, CDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_MODULE_TREE, OnDblClkTree)
    ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnDblClkList)
    ON_BN_CLICKED(IDC_SHOW_TREE, OnShowTree)
END_MESSAGE_MAP()

// 0x100081f0 ?OnInitDialog@MfPdbModuleSelector@@MAEHXZ
BOOL MfPdbModuleSelector::OnInitDialog()
{
    CDialog::OnInitDialog();
    mTree.FillTree();
    mList.FillTree();
    mShowTree.SetCheck(1);
    return TRUE;
}

// 0x10008230 ?OnDblClkTree@MfPdbModuleSelector@@IAEXPAUtagNMHDR@@PAJ@Z
void MfPdbModuleSelector::OnDblClkTree(NMHDR *, LRESULT *result)
{
    MfPdbModuleItem *data = (MfPdbModuleItem *)mTree.GetItemData(mTree.GetSelectedItem());
    if (data) {
        mModule = data->mModule;
        mModuleType = data->mModuleType;
        EndDialog(IDOK);
    }
    *result = 0;
}

// 0x100082a0 ?OnDblClkList@MfPdbModuleSelector@@IAEXPAUtagNMHDR@@PAJ@Z
void MfPdbModuleSelector::OnDblClkList(NMHDR *, LRESULT *result)
{
    MfPdbModuleItem *data = (MfPdbModuleItem *)mList.GetItemData(mList.GetSelectedItem());
    if (data) {
        mModule = data->mModule;
        mModuleType = data->mModuleType;
        EndDialog(IDOK);
    }
    *result = 0;
}

// 0x10008310 ?OnShowTree@MfPdbModuleSelector@@IAEXXZ
void MfPdbModuleSelector::OnShowTree()
{
    if (mShowTree.GetCheck()) {
        mTree.ShowWindow(SW_SHOW);
        mList.ShowWindow(SW_HIDE);
    } else {
        mTree.ShowWindow(SW_HIDE);
        mList.ShowWindow(SW_SHOW);
    }
}
