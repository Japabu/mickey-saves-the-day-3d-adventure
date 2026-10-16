// PscDebugger.dll 0x1000ad80-0x1000b3c5: the dialog that shows the value of a
// variable of the stopped script thread.
#include "PscDebugger.h"

// Controls of the dialog.
enum {
    IDC_VARIABLE_NAME = 0x3e8,
    IDC_VARIABLE_VALUE = 0x3f8
};

// 0x1000ad80 ??0MfPdbViewVariable@@QAE@PAVCWnd@@@Z
MfPdbViewVariable::MfPdbViewVariable(CWnd *parent)
    : CDialog(IDD, parent)
{
    mEdit = new MfPdbViewVariableEdit(this);
    mDisplay = new MfPdbVarDispWnd(pdbVarDisplayState, this);
}

// 0x1000ae50 ??_GMfPdbViewVariableEdit@@UAEPAXI@Z
// 0x1000ae70 ??1MfPdbViewVariableEdit@@UAE@XZ
// 0x1000aec0 ??_GMfPdbViewVariable@@UAEPAXI@Z

// 0x1000aee0 ??1MfPdbViewVariable@@UAE@XZ
MfPdbViewVariable::~MfPdbViewVariable()
{
    delete mDisplay;
}

// 0x1000af40 ?DoDataExchange@MfPdbViewVariable@@MAEXPAVCDataExchange@@@Z
void MfPdbViewVariable::DoDataExchange(CDataExchange *dx)
{
    DDX_Control(dx, IDC_VARIABLE_NAME, *mEdit);
}

// 0x1000af60 ?Evaluate@MfPdbViewVariable@@QAEXXZ
void MfPdbViewVariable::Evaluate()
{
    mDisplay->Clear(0);
    CString text;
    mEdit->GetWindowText(text);
    MfString name = (const char *)text;
    name.StripWhiteSpace();
    mEdit->SetWindowText(name);
    mEdit->SetSel(0, -1);
    MfScDataHandle *value = 0;
    int found = pdbDebugger->GetVariable(name, &value);
    mDisplay->SetVariable(value, MfString(text), found, 1);
    if (value)
        value->Release();
}

// 0x1000b100 ??_GMfStringRef@@UAEPAXI@Z

// 0x1000b130 ?GetMessageMap@MfPdbViewVariable@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbViewVariable, CDialog)
END_MESSAGE_MAP()

// 0x1000b140 ?OnInitDialog@MfPdbViewVariable@@MAEHXZ
BOOL MfPdbViewVariable::OnInitDialog()
{
    CDialog::OnInitDialog();
    MfAiPiccoloEditor *editor = mFrame->EditorWnd()->GetIEditor();
    MfString word;
    editor->GetWordAtCursor(&word, 0);
    mEdit->SetWindowText(word);
    mEdit->SetSel(0, -1);
    CWnd *placeholder = GetDlgItem(IDC_VARIABLE_VALUE);
    if (placeholder) {
        CRect rect;
        placeholder->GetWindowRect(&rect);
        ScreenToClient(&rect);
        placeholder->DestroyWindow();
        mDisplay->CreateEx(WS_EX_CLIENTEDGE, NULL, pdbVarDisplayName, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect,
                           this, IDC_VARIABLE_VALUE, NULL);
        Evaluate();
    }
    return TRUE;
}

// 0x1000b270 ?OnOK@MfPdbViewVariable@@MAEXXZ
void MfPdbViewVariable::OnOK()
{
    pdbVarDisplayState = mDisplay->mNameWidth;
    CDialog::OnOK();
}

// 0x1000b290 ?OnCancel@MfPdbViewVariable@@MAEXXZ
void MfPdbViewVariable::OnCancel()
{
    pdbVarDisplayState = mDisplay->mNameWidth;
    CDialog::OnCancel();
}

// 0x1000b2b0 ?FocusPrevious@MfPdbViewVariable@@QAEXPAVCWnd@@@Z
void MfPdbViewVariable::FocusPrevious(CWnd *from)
{
    if (from == mDisplay)
        mEdit->SetFocus();
    else if (from == mEdit)
        GetDlgItem(IDOK)->SetFocus();
}

// 0x1000b2e0 ?FocusNext@MfPdbViewVariable@@QAEXPAVCWnd@@@Z
void MfPdbViewVariable::FocusNext(CWnd *from)
{
    if (from == mEdit)
        mDisplay->SetFocus();
    else if (from == mDisplay)
        GetDlgItem(IDOK)->SetFocus();
}

// 0x1000b310 ?GetMessageMap@MfPdbViewVariableEdit@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbViewVariableEdit, CEdit)
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

// 0x1000b320 ?GetSuperWndProcAddr@MfPdbViewVariableEdit@@MAEPAP6GJPAUHWND__@@IIJ@ZXZ
WNDPROC *MfPdbViewVariableEdit::GetSuperWndProcAddr()
{
    return &mSuperProc;
}

// 0x1000b330 ?OnKeyDown@MfPdbViewVariableEdit@@IAEXIII@Z
void MfPdbViewVariableEdit::OnKeyDown(UINT ch, UINT repeat, UINT flags)
{
    int shift = GetKeyState(VK_SHIFT) < 0;
    if (ch == VK_RETURN)
        mDialog->Evaluate();
    else if (ch == VK_TAB) {
        if (shift)
            mDialog->FocusPrevious(this);
        else
            mDialog->FocusNext(this);
    } else if (ch == VK_ESCAPE)
        mDialog->EndDialog(0);
    else
        CEdit::OnKeyDown(ch, repeat, flags);
}

// 0x1000b3a0 ?OnChar@MfPdbViewVariableEdit@@IAEXIII@Z
void MfPdbViewVariableEdit::OnChar(UINT ch, UINT repeat, UINT flags)
{
    if (ch != VK_RETURN && ch != VK_TAB)
        CEdit::OnChar(ch, repeat, flags);
}

// 0x1000b3c0 ?OnGetDlgCode@MfPdbViewVariableEdit@@IAEIXZ
UINT MfPdbViewVariableEdit::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS | DLGC_WANTCHARS;
}
