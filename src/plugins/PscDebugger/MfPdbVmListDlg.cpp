// PscDebugger.dll 0x1000b3d0-0x1000b93d: the dialog listing the script
// threads (virtual machines) of the runtime.
#include "PscDebugger.h"

// Controls of the dialog.
enum {
    IDC_STOP_THREAD = 0x3fb,
    IDC_REFRESH = 0x3fc,
    IDC_THREADS = 0x3fd
};

// Columns of the list.
enum { kColumnId, kColumnExecuting, kColumnTime, kColumnModule };

// 0x1000b3d0 ??0MfPdbVmListDlg@@QAE@PAVCWnd@@@Z
MfPdbVmListDlg::MfPdbVmListDlg(CWnd *parent)
    : CDialog(IDD, parent)
{
}

// 0x1000b430 ??_GMfPdbVmListDlg@@UAEPAXI@Z
// 0x1000b450 ??1MfPdbVmListDlg@@UAE@XZ

// 0x1000b4a0 ?DoDataExchange@MfPdbVmListDlg@@MAEXPAVCDataExchange@@@Z
void MfPdbVmListDlg::DoDataExchange(CDataExchange *dx)
{
    DDX_Control(dx, IDC_THREADS, mList);
}

// 0x1000b4c0 ?Refresh@MfPdbVmListDlg@@QAEXXZ
void MfPdbVmListDlg::Refresh()
{
    MfAi *manager;
    if (MfInterfaceRegistry::GetInterface("VMManagerI", &manager)) {
        mList.DeleteAllItems();
        for (MfScVmEnv *env = ((MfAiVmManager *)manager)->First(); env;
             env = ((MfAiVmManager *)manager)->Next()) {
            LV_ITEM item;
            item.mask = LVIF_TEXT | LVIF_PARAM;
            item.iItem = 0;
            item.iSubItem = 0;
            item.pszText = LPSTR_TEXTCALLBACK;
            item.lParam = (LPARAM)env;
            mList.InsertItem(&item);
        }
        manager->Release();
    }
}

// 0x1000b5a0 ?_GetBaseMessageMap@MfPdbVmListDlg@@KGPBUAFX_MSGMAP@@XZ
// 0x1000b5b0 ?GetMessageMap@MfPdbVmListDlg@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbVmListDlg, CDialog)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_THREADS, OnGetDispInfo)
    ON_BN_CLICKED(IDC_STOP_THREAD, OnStopThread)
    ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// 0x1000b5c0 ?OnInitDialog@MfPdbVmListDlg@@MAEHXZ
BOOL MfPdbVmListDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    mList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT,
                     LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT);
    mList.InsertColumn(kColumnId, "VM Id", LVCFMT_RIGHT, 50);
    mList.InsertColumn(kColumnExecuting, "Executing", LVCFMT_CENTER, 75);
    mList.InsertColumn(kColumnTime, "Time", LVCFMT_RIGHT, 75);
    mList.InsertColumn(kColumnModule, "Module", LVCFMT_LEFT, 300);
    Refresh();
    return TRUE;
}

// 0x1000b650 ?OnGetDispInfo@MfPdbVmListDlg@@IAEXPAUtagNMHDR@@PAJ@Z
void MfPdbVmListDlg::OnGetDispInfo(NMHDR *nmhdr, LRESULT *result)
{
    LV_DISPINFO *info = (LV_DISPINFO *)nmhdr;
    MfScVmEnv *env = (MfScVmEnv *)info->item.lParam;
    if (info->item.mask & LVIF_TEXT) {
        MfString text;
        switch (info->item.iSubItem) {
        case kColumnId:
            text.Format("%d", env->m_id);
            lstrcpy(info->item.pszText, text);
            break;
        case kColumnExecuting:
            text.Format("%s", env->m_running ? "yes" : "no");
            lstrcpy(info->item.pszText, text);
            break;
        case kColumnTime: {
            if (env->m_running) {
                env->m_timer.Stop();
                env->m_timer.AccumTimer(env->m_processTime);
            }
            double seconds = (double)env->m_processTime / (double)MfHPTimer::liFrequency.QuadPart;
            text.Format("%d:%02d:%02d.%03d", (int)(seconds * (1.0 / 3600)), (int)(seconds * (1.0 / 60)) % 60,
                        (int)seconds % 60, (int)(seconds * 1000) % 1000);
            lstrcpy(info->item.pszText, text);
            if (env->m_running)
                env->m_timer.Start();
            break;
        }
        case kColumnModule:
            if (env->m_running) {
                MfScBytecodeRoutine *routine =
                    env->m_function ? dynamic_cast<MfScBytecodeRoutine *>(env->m_function->GetData()) : 0;
                if (routine)
                    text = routine->ModuleName();
            } else {
                text = NULL_STRING;
            }
            lstrcpy(info->item.pszText, text);
            break;
        }
    }
    *result = 0;
}

// 0x1000b880 ?OnStopThread@MfPdbVmListDlg@@IAEXXZ
void MfPdbVmListDlg::OnStopThread()
{
    int sel = mList.GetNextItem(-1, LVNI_SELECTED);
    if (sel >= 0) {
        LV_ITEM item;
        item.mask = LVIF_PARAM;
        item.iItem = sel;
        item.iSubItem = 0;
        mList.GetItem(&item);
        MfScVmEnv *env = (MfScVmEnv *)item.lParam;
        if (env->m_running)
            env->m_stop = 1;
    }
}

// 0x1000b8f0 ?OnRefresh@MfPdbVmListDlg@@IAEXXZ
void MfPdbVmListDlg::OnRefresh()
{
    Refresh();
}

// 0x1000b900 ?OnTimer@MfPdbVmListDlg@@IAEXI@Z
void MfPdbVmListDlg::OnTimer(UINT id)
{
    Refresh();
    CDialog::OnTimer(id);
}

// 0x1000b920 ??_GCListCtrl@@UAEPAXI@Z
