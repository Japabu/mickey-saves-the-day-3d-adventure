// MfNameDialog (mfutil.dll 0x10009860-0x10009c20): modal dialog (resource 277)
// asking for a name, with a prompt text. Reconstructed declarations (no PDB):
// layout from the code, signatures from the export names; modelled on MfKeyDialog.
#ifndef MFUTIL_MFNAMEDIALOG_H
#define MFUTIL_MFNAMEDIALOG_H

#include "MfString.h"
#include "UtilityApp.h"

// size 0x14, no vtable. The copy constructor, operator= and destructor are the
// compiler-generated ones (exported).
class MFUTIL_API MfNameDialog {
public:
    MfNameDialog(MfString title, MfString prompt, MfString name, HWND parent);
    int DoModal();
    MfString GetName() { return m_name; }
protected:
    int OnInitDialog();
    void OnChangeEditname();
    static BOOL CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hParent;     // +0
    HWND m_hDlg;        // +4
    MfString m_title;   // +8
    MfString m_prompt;  // +0xc  (static text 1015)
    MfString m_name;    // +0x10 (edit 1016)
};

#endif
