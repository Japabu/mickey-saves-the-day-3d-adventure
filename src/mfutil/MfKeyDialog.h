// MfKeyDialog (mfutil.dll 0x10005580-0x10005830): modal dialog (resource 107)
// asking for a serial number and an access key (MfKeyManager::CheckKey). Reconstructed declarations (no PDB):
// layout from the code, signatures from the export names.
#ifndef MFUTIL_MFKEYDIALOG_H
#define MFUTIL_MFKEYDIALOG_H

#ifndef STRICT
#define STRICT  // HWND__ / HINSTANCE__ in the export names
#endif
#include <windows.h>
#include "MfString.h"

#include "UtilityApp.h"

// size 0x14, no vtable. Copy ctor / operator= / dtor are the compiler-generated
// ones (exported; their copies live elsewhere).
class MFUTIL_API MfKeyDialog {
public:
    MfKeyDialog(MfString title, MfString serial, MfString key, HWND parent);
    int DoModal();
    MfString GetKey() { return m_key; }
    MfString GetSerial() { return m_serial; }
protected:
    int OnInitDialog();
    static BOOL CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hParent;     // +0
    HWND m_hDlg;        // +4
    MfString m_title;   // +8
    MfString m_serial;  // +0xc  (edit 1010)
    MfString m_key;     // +0x10 (edit 1011)
};

#endif
