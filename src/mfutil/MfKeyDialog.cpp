// MfKeyDialog (mfutil.dll 0x10005580-0x10005830).
#ifndef STRICT
#define STRICT  // HWND__ / HINSTANCE__ in the export names
#endif
#include <windows.h>
#include "MfKeyDialog.h"

#define IDD_KEYDIALOG   107
#define IDC_EDITSERIAL  1010
#define IDC_EDITKEY     1011

// 0x10005580 ??0MfKeyDialog@@QAE@VMfString@@00PAUHWND__@@@Z
MfKeyDialog::MfKeyDialog(MfString title, MfString serial, MfString key, HWND parent)
    : m_hParent(parent), m_title(title), m_serial(serial), m_key(key)
{
}

// 0x10005620 ?DoModal@MfKeyDialog@@QAEHXZ
int MfKeyDialog::DoModal()
{
    return DialogBoxParamA(UtilityApp::hLibInstance, MAKEINTRESOURCE(IDD_KEYDIALOG),
                           m_hParent, DialogProc, (LPARAM)this);
}

// 0x10005640 ?OnInitDialog@MfKeyDialog@@IAEHXZ
int MfKeyDialog::OnInitDialog()
{
    SetWindowTextA(m_hDlg, m_title);
    HWND key = GetDlgItem(m_hDlg, IDC_EDITKEY);
    SetWindowTextA(key, m_key);
    HWND serial = GetDlgItem(m_hDlg, IDC_EDITSERIAL);
    SetWindowTextA(serial, m_serial);
    SendMessageA(serial, EM_SETSEL, 0, -1);
    return TRUE;
}

// 0x100056b0 ?DialogProc@MfKeyDialog@@KGHPAUHWND__@@IIJ@Z
BOOL CALLBACK MfKeyDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = FALSE;
    if (msg == WM_INITDIALOG) {
        MfKeyDialog *dlg = (MfKeyDialog *)lParam;
        SetWindowLongA(hwnd, GWL_USERDATA, lParam);
        dlg->m_hDlg = hwnd;
        result = dlg->OnInitDialog();
        RECT r;
        GetWindowRect(hwnd, &r);
        MoveWindow(dlg->m_hDlg,
                   (GetSystemMetrics(SM_CXMAXIMIZED) - (r.right - r.left)) / 2,
                   (GetSystemMetrics(SM_CYMAXIMIZED) - (r.bottom - r.top)) / 2,
                   r.right - r.left, r.bottom - r.top, TRUE);
        SetFocus(GetDlgItem(hwnd, IDC_EDITSERIAL));
        return result;
    }
    MfKeyDialog *dlg = (MfKeyDialog *)GetWindowLongA(hwnd, GWL_USERDATA);
    if (msg == WM_COMMAND) {
        WORD id = LOWORD(wParam);
        if (id == IDOK || id == IDCANCEL) {
            char buf[128];
            HWND edit = GetDlgItem(dlg->m_hDlg, IDC_EDITSERIAL);
            GetWindowTextA(edit, buf, sizeof(buf));
            dlg->m_serial = buf;
            edit = GetDlgItem(dlg->m_hDlg, IDC_EDITKEY);
            GetWindowTextA(edit, buf, sizeof(buf));
            dlg->m_key = buf;
            EndDialog(hwnd, id);
            result = TRUE;
        }
    }
    return result;
}

// Inline or compiler-generated members this object emits:
// 0x10004a40 ?GetSerial@MfKeyDialog@@QAE?AVMfString@@XZ (inline, header)
// 0x10004a60 ??0MfKeyDialog@@QAE@ABV0@@Z
// 0x10004ae0 ??4MfKeyDialog@@QAEAAV0@ABV0@@Z
// 0x10004b20 ??1MfKeyDialog@@QAE@XZ
