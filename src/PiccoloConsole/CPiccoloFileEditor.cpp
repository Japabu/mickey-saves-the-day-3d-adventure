// PiccoloConsole.dll 0x100089f0-0x100092ee: a frame window editing one file.
#include "PiccoloConsole.h"

// The id of the timer that keeps the title up to date.
enum { kTitleTimer = 10 };

// 0x100089f0 ??0CPiccoloFileEditor@@QAE@ABVMfString@@0@Z
CPiccoloFileEditor::CPiccoloFileEditor(const MfString &fileName, const MfString &baseTitle)
    : mBufferModified(1), mTextModified(0), mPiccoloWnd(0), mFlags(0x70), mPaneSize(-1), mFileName(fileName),
      mBaseTitle(baseTitle), mTimerId(kTitleTimer)
{
}

// 0x10008ab0 ??_ECPiccoloFileEditor@@UAEPAXI@Z

// 0x10008b10 ??1CPiccoloFileEditor@@UAE@XZ
CPiccoloFileEditor::~CPiccoloFileEditor()
{
    if (mPiccoloWnd)
        delete mPiccoloWnd;
}

// 0x10008ba0 ?DisableConsole@CPiccoloFileEditor@@QAEXXZ
void CPiccoloFileEditor::DisableConsole()
{
    mFlags |= 8;
}

// EQUIV 0x10008bb0 ?SetOutputWndHints@CPiccoloFileEditor@@QAEXH@Z: reviewed: the masked-bit merge loads its two operands in the other order (xor/and/xor, same result)
void CPiccoloFileEditor::SetOutputWndHints(int hints)
{
    mFlags ^= (hints ^ mFlags) & 0x70;
}

// EQUIV 0x10008bd0 ?SetEditorFlagHints@CPiccoloFileEditor@@QAEXH@Z: reviewed: the masked-bit merge loads its two operands in the other order (xor/and/xor, same result)
void CPiccoloFileEditor::SetEditorFlagHints(int hints)
{
    mFlags ^= (hints ^ mFlags) & 7;
}

// 0x10008bf0 ?CreateFrameWindow@CPiccoloFileEditor@@UAEXPAVCWnd@@@Z
void CPiccoloFileEditor::CreateFrameWindow(CWnd *parent)
{
    PiccoloConsoleContext context;
    LoadFrame(0x81, WS_OVERLAPPEDWINDOW, parent, NULL);
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

// 0x10008c90 ?UpdateWindowTitle@CPiccoloFileEditor@@QAEXXZ
void CPiccoloFileEditor::UpdateWindowTitle()
{
    CConsoleEdit *editor = mPiccoloWnd->mEditor;
    if (!editor)
        return;
    CString text;
    GetWindowText(text);
    MfString current = (const char *)text;
    MfString title;
    if (mFileName != NULL_STRING) {
        title.Format(!editor->IsTextModified() ? "%s [%s]" : "%s [%s*]", (const char *)mBaseTitle,
                     (const char *)mFileName);
    } else
        title.Format("%s%c", (const char *)mBaseTitle, editor->IsTextModified() ? '*' : ' ');
    if (title != current)
        SetWindowText(title);
}

// 0x10008de0 ?ReadCurrentFile@CPiccoloFileEditor@@UAEXPAVMfString@@@Z
void CPiccoloFileEditor::ReadCurrentFile(MfString *text)
{
    CConsoleEdit *editor = mPiccoloWnd->mEditor;
    if (!editor)
        return;
    MfString contents;
    if (mFileName != NULL_STRING)
        contents.ReadFromFile(mFileName);
    editor->SetEntireContents(contents, 0);
    editor->SetUnModified();
    if (text)
        *text = contents;
}

// 0x10008f70 ?WriteCurrentFile@CPiccoloFileEditor@@UAEXPAVMfString@@@Z
void CPiccoloFileEditor::WriteCurrentFile(MfString *text)
{
    CConsoleEdit *editor = mPiccoloWnd->mEditor;
    if (editor) {
        MfString contents = editor->GetEntireContents();
        contents.WriteToFile(mFileName);
        editor->SetUnModified();
        if (text)
            *text = contents;
    } else if (text) {
        *text = NULL_STRING;
    }
}

// 0x10009030 ?SetFileName@CPiccoloFileEditor@@QAEXABVMfString@@@Z
void CPiccoloFileEditor::SetFileName(const MfString &fileName)
{
    mFileName = fileName;
    if (mPiccoloWnd) {
        UpdateWindowTitle();
        mPiccoloWnd->SetModuleName(mFileName);
    }
}

// 0x10009070 ?EditorWnd@CPiccoloFileEditor@@QAEPAVCConsoleEdit@@XZ
CConsoleEdit *CPiccoloFileEditor::EditorWnd()
{
    return mPiccoloWnd->mEditor;
}

// 0x10009080 ?GetMessageMap@CPiccoloFileEditor@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CPiccoloFileEditor, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(0x9c4e, OnPfeClose)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// 0x10009090 ?OnCreate@CPiccoloFileEditor@@IAEHPAUtagCREATESTRUCTA@@@Z
int CPiccoloFileEditor::OnCreate(LPCREATESTRUCT cs)
{
    if (CFrameWnd::OnCreate(cs) == -1)
        return -1;
    mPiccoloWnd = new CPiccoloWnd(mFlags, mPaneSize);
    CRect rect(0, 0, 10, 10);
    mPiccoloWnd->Create(NULL, "Piccolo", WS_CHILD | WS_VISIBLE, rect, this, 0, NULL);
    mPiccoloWnd->SetModuleName(mFileName);
    CConsoleEdit *editor = EditorWnd();
    editor->InstallBufferModifiedCallBack(this);
    UpdateWindowTitle();
    ReadCurrentFile(0);
    StartUpdateTimer();
    return 0;
}

// 0x100091b0 ?OnSize@CPiccoloFileEditor@@IAEXIHH@Z
void CPiccoloFileEditor::OnSize(UINT type, int cx, int cy)
{
    CFrameWnd::OnSize(type, cx, cy);
    if (mPiccoloWnd)
        mPiccoloWnd->MoveWindow(0, 0, cx, cy, TRUE);
}

// 0x100091f0 ?OnPfeClose@CPiccoloFileEditor@@IAEXXZ
void CPiccoloFileEditor::OnPfeClose()
{
    PostMessage(WM_CLOSE);
}

// 0x10009210 ?OnTimer@CPiccoloFileEditor@@IAEXI@Z
void CPiccoloFileEditor::OnTimer(UINT id)
{
    if (id == (UINT)mTimerId)
        UpdateWindowTitle();
}

// 0x10009230 ?OnClose@CPiccoloFileEditor@@IAEXXZ
void CPiccoloFileEditor::OnClose()
{
    StopUpdateTimer();
    CFrameWnd::OnClose();
}

// 0x10009250 ?OutputSwitchToWindow@CPiccoloFileEditor@@QAEXH@Z
void CPiccoloFileEditor::OutputSwitchToWindow(int window)
{
    if (mPiccoloWnd->mOutput)
        mPiccoloWnd->mOutput->SwitchToWindow(window);
}

// 0x10009270 ?OutputPrint@CPiccoloFileEditor@@QAEXHABVMfString@@H@Z
void CPiccoloFileEditor::OutputPrint(int window, const MfString &text, int flags)
{
    if (mPiccoloWnd->mOutput)
        mPiccoloWnd->mOutput->Print(window, text, flags);
}

// 0x100092a0 ?OutputClear@CPiccoloFileEditor@@QAEXH@Z
void CPiccoloFileEditor::OutputClear(int window)
{
    if (mPiccoloWnd->mOutput)
        mPiccoloWnd->mOutput->Clear(window);
}

// 0x100092c0 ?BmcbNotify@CPiccoloFileEditor@@MAEXH@Z
void CPiccoloFileEditor::BmcbNotify(int modified)
{
    mBufferModified = modified;
    int textModified = mPiccoloWnd->mEditor->IsTextModified();
    if (mTextModified != textModified) {
        mTextModified = textModified;
        UpdateWindowTitle();
    }
}

// The timer is never started: the bytes of StartUpdateTimer and
// StopUpdateTimer (an empty function) were folded into 0x10006370.
void CPiccoloFileEditor::StartUpdateTimer()
{
}

void CPiccoloFileEditor::StopUpdateTimer()
{
}
