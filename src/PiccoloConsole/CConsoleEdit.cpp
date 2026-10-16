// CConsoleEdit: the code editor of the console windows.
#include "PiccoloConsole.h"

// The exported inline functions of the classes in PiccoloConsole.h come first
// in this object (the first one linked):
// 0x10001000 ?IsAttached@MfPeCodeEditor@@QAEHPAVMfPeEditorData@@@Z
// 0x10001020 ?DoSetReadOnly@MfPeCodeEditor@@QAEXXZ
// 0x10001030 ?DoSetReadWrite@MfPeCodeEditor@@QAEXXZ
// 0x10001040 ?SetDblClickSelectsLine@MfPeCodeEditor@@QAEXXZ
// 0x10001050 ?SetDblClickSelectsWord@MfPeCodeEditor@@QAEXXZ
// 0x10001060 ?InstallDblClickHandler@MfPeCodeEditor@@QAEXPAVMfPeCeDblClickHandler@@@Z
// 0x10001070 ?SetSearchString@MfPeCodeEditor@@QAEXABVMfString@@@Z
// 0x10001090 ?SetPiccoloWindow@MfPeCodeEditor@@QAEXPAVCPiccoloWnd@@@Z
// 0x100010a0 ?StartSlowTimer@MfPeCodeEditor@@AAEXXZ
// 0x100010d0 ?StartFastTimer@MfPeCodeEditor@@AAEXXZ
// 0x10001100 ?StartSetFocusTimer@MfPeCodeEditor@@AAEXXZ
// 0x10001130 ?KillSlowTimer@MfPeCodeEditor@@AAEXXZ
// 0x10001160 ?KillFastTimer@MfPeCodeEditor@@AAEXXZ
// 0x10001190 ?KillSetFocusTimer@MfPeCodeEditor@@AAEXXZ
// runtimelib.dll 0x100687e0 ?SetDblClickSelectsLine@MfPeCodeEditor@@QAEXXZ
// runtimelib.dll 0x100687f0 ?SetDblClickSelectsWord@MfPeCodeEditor@@QAEXXZ

// 0x100011c0 ?SetReadOnly@CConsoleEdit@@QAEXH@Z
void CConsoleEdit::SetReadOnly(int readOnly)
{
    if (readOnly)
        DoSetReadOnly();
    else
        DoSetReadWrite();
}

// 0x100011e0 ?CreateObject@CConsoleEdit@@SGPAVCObject@@XZ
// 0x10001240 ?GetRuntimeClass@CConsoleEdit@@UBEPAUCRuntimeClass@@XZ
IMPLEMENT_DYNCREATE(CConsoleEdit, MfPeCodeEditor)

// 0x10001250 ??0CConsoleEdit@@QAE@XZ
CConsoleEdit::CConsoleEdit()
{
}

// 0x10001270 ??_ECConsoleEdit@@UAEPAXI@Z
// 0x100012d0 ??1CConsoleEdit@@UAE@XZ
CConsoleEdit::~CConsoleEdit()
{
}

// 0x100012e0 ?GetMessageMap@CConsoleEdit@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CConsoleEdit, MfPeCodeEditor)
    ON_WM_CREATE()
END_MESSAGE_MAP()

// 0x100012f0 ?GetEntireContents@CConsoleEdit@@QAE?AVMfString@@XZ
MfString CConsoleEdit::GetEntireContents()
{
    return GetTextData();
}

// 0x10001310 ?SetEntireContents@CConsoleEdit@@QAEXABVMfString@@H@Z
void CConsoleEdit::SetEntireContents(const MfString &text, int keepPosition)
{
    SetTextData(text, keepPosition);
}

// 0x10001330 ?IsModified@CConsoleEdit@@QAEHXZ
int CConsoleEdit::IsModified()
{
    return IsTextModified();
}

// 0x10001340 ?SetModified@CConsoleEdit@@QAEXXZ
void CConsoleEdit::SetModified()
{
    SetModificationFlag(1);
}

// 0x10001350 ?SetUnModified@CConsoleEdit@@QAEXXZ
void CConsoleEdit::SetUnModified()
{
    SetModificationFlag(0);
}

// 0x10001360 ?PreCreateWindow@CConsoleEdit@@MAEHAAUtagCREATESTRUCTA@@@Z
// plugins/pscdebugger.dll 0x10006df0 ?PreCreateWindow@CConsoleEdit@@MAEHAAUtagCREATESTRUCTA@@@Z
// plugins/pscdebugger.dll 0x10008ab0 ?PreCreateWindow@CConsoleEdit@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL CConsoleEdit::PreCreateWindow(CREATESTRUCT &cs)
{
    return MfPeCodeEditor::PreCreateWindow(cs);
}

// 0x10001370 ?OnCreate@CConsoleEdit@@IAEHPAUtagCREATESTRUCTA@@@Z
int CConsoleEdit::OnCreate(LPCREATESTRUCT cs)
{
    if (MfPeCodeEditor::OnCreate(cs) == -1)
        return -1;
    return 0;
}

// The inline functions of MFC this object needs (afx.inl, afxwin.inl):
// 0x10001390 ??3CObject@@SGXPAX@Z
// 0x100013a0 ?BeginModalState@CWnd@@UAEXXZ
// 0x100013b0 ?EndModalState@CWnd@@UAEXXZ
// plugins/pscdebugger.dll 0x10005920 ??3CObject@@SGXPAX@Z
// plugins/pscdebugger.dll 0x10006980 ?BeginModalState@CWnd@@UAEXXZ
// plugins/pscdebugger.dll 0x10006990 ?EndModalState@CWnd@@UAEXXZ

// The rest of the exported inline functions:
// 0x100013c0 ?FileName@CPiccoloFileEditor@@QBEABVMfString@@XZ
// 0x100013d0 ?SetBaseTitle@CPiccoloFileEditor@@QAEXABVMfString@@@Z
// 0x100013f0 ?ErrorHandler@CConsoleOutput@@QAEAAVMfPcErrorCallBack@@XZ
// 0x10001400 ?FromTab@CConsoleOutput@@IAEHH@Z
// 0x10001410 ?ToTab@CConsoleOutput@@IAEHH@Z
// 0x10001420 ?GetModuleName@CPiccoloWnd@@QBE?AVMfString@@XZ
// 0x10001450 ?EditorWnd@CPiccoloWnd@@QBEPAVCConsoleEdit@@XZ
// 0x10001460 ?OutputWnd@CPiccoloWnd@@QBEPAVCConsoleOutput@@XZ
// 0x10001470 ??_FCPiccoloWnd@@QAEXXZ
// 0x10001480 ??_FMfPiccoloConsoleFrame@@QAEXXZ
// 0x10001490 ?PiccoloWindowFlags@CConsoleFrame@@IAEHXZ
