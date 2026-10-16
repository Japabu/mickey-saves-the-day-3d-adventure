// PORT: the window keeps its C++ object in GWLP_USERDATA through Set/GetWindowLongPtr (a LONG truncates the pointer).
// MfNameDialog (mfutil.dll 0x10009860-0x10009c20).
#include "MfNameDialog.h"

#define IDD_NAMEDIALOG  277
#define IDC_PROMPT      1015
#define IDC_EDITNAME    1016

// Inline or compiler-generated members this object emits:
// 0x10009860 ?GetName@MfNameDialog@@QAE?AVMfString@@XZ (inline; folded with MfKeyDialog::GetKey)
// 0x10009880 ??0MfNameDialog@@QAE@ABV0@@Z
// 0x10009900 ??1MfNameDialog@@QAE@XZ

// 0x10009960 ??0MfNameDialog@@QAE@VMfString@@00PAUHWND__@@@Z
MfNameDialog::MfNameDialog(MfString title, MfString prompt, MfString name, HWND parent)
    : m_hParent(parent), m_title(title), m_prompt(prompt), m_name(name)
{
}

// 0x10009a00 ?DoModal@MfNameDialog@@QAEHXZ
int MfNameDialog::DoModal()
{
    return DialogBoxParamA(UtilityApp::hLibInstance, MAKEINTRESOURCE(IDD_NAMEDIALOG),
                           m_hParent, DialogProc, (LPARAM)this);
}

// 0x10009a20 ?OnInitDialog@MfNameDialog@@IAEHXZ
int MfNameDialog::OnInitDialog()
{
    HWND prompt = GetDlgItem(m_hDlg, IDC_PROMPT);
    SetWindowTextA(prompt, m_prompt);
    HWND name = GetDlgItem(m_hDlg, IDC_EDITNAME);
    SetWindowTextA(name, m_name);
    SetWindowTextA(m_hDlg, m_title);
    return TRUE;
}

// 0x10009a80 ?OnChangeEditname@MfNameDialog@@IAEXXZ
void MfNameDialog::OnChangeEditname()
{
    char buf[128];
    HWND edit = GetDlgItem(m_hDlg, IDC_EDITNAME);
    GetWindowTextA(edit, buf, sizeof(buf));
    m_name = buf;
}

// 0x10009ac0 ?DialogProc@MfNameDialog@@KGHPAUHWND__@@IIJ@Z
BOOL CALLBACK MfNameDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = FALSE;
    if (msg == WM_INITDIALOG) {
        MfNameDialog *dlg = (MfNameDialog *)lParam;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        dlg->m_hDlg = hwnd;
        result = dlg->OnInitDialog();
        RECT r;
        GetWindowRect(hwnd, &r);
        MoveWindow(dlg->m_hDlg,
                   (GetSystemMetrics(SM_CXMAXIMIZED) - (r.right - r.left)) / 2,
                   (GetSystemMetrics(SM_CYMAXIMIZED) - (r.bottom - r.top)) / 2,
                   r.right - r.left, r.bottom - r.top, TRUE);
    } else {
        MfNameDialog *dlg = (MfNameDialog *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)
        if (msg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {
            char buf[128];
            HWND edit = GetDlgItem(dlg->m_hDlg, IDC_EDITNAME);
            GetWindowTextA(edit, buf, sizeof(buf));
            dlg->m_name = buf;
            EndDialog(hwnd, LOWORD(wParam));
            result = TRUE;
        }
        // the original compares the message with the EN_CHANGE notification code
        else if (msg == EN_CHANGE)
            dlg->OnChangeEditname();
    }
    return result;
}
