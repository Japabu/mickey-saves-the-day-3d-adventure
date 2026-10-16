// PiccoloConsole.dll 0x1000c160-0x1000c49f: MfPiccoloConsoleWnd, the console
// of the scripts: an editor window that prints what the scripts print.
#include "PiccoloConsole.h"

// 0x1000c160 ??0MfPiccoloConsoleWnd@@QAE@XZ
MfPiccoloConsoleWnd::MfPiccoloConsoleWnd()
{
    MfScConsoleManager::pcmManager->AddOutput(this);
}

// 0x1000c1d0 ??_EMfPiccoloConsoleWnd@@UAEPAXI@Z

// 0x1000c230 ??1MfPiccoloConsoleWnd@@UAE@XZ
MfPiccoloConsoleWnd::~MfPiccoloConsoleWnd()
{
    if (MfScConsoleManager::pcmManager)
        MfScConsoleManager::pcmManager->RemoveOutput(this);
}

// 0x1000c2a0 ?Enable@MfPiccoloConsoleWnd@@QAEXXZ
void MfPiccoloConsoleWnd::Enable()
{
    MfScConsoleManager::pcmManager->AddOutput(this);
}

// 0x1000c2d0 ?Disable@MfPiccoloConsoleWnd@@QAEXXZ
void MfPiccoloConsoleWnd::Disable()
{
    MfScConsoleManager::pcmManager->RemoveOutput(this);
}

// 0x1000c300 ?Create@MfPiccoloConsoleWnd@@UAEHABUtagRECT@@PAVCWnd@@IKK@Z
BOOL MfPiccoloConsoleWnd::Create(const RECT &rect, CWnd *parent, UINT id, DWORD style, DWORD exStyle)
{
    BOOL result = CreateEx(exStyle, NULL, "", style | WS_VSCROLL | WS_HSCROLL, rect.left, rect.top,
                           rect.right - rect.left, rect.bottom - rect.top, parent->GetSafeHwnd(), NULL);
    DoSetReadOnly();
    DisableSyntaxHighlight();
    return result;
}

// 0x1000c370 ?GetInterfaceName@MfPiccoloConsoleWnd@@UAEXPADHAAH1@Z
void MfPiccoloConsoleWnd::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfPiccoloConsoleWnd::ConsoleOutput", size);
    major = 1;
    minor = 0;
}

// The same code as CPiccoloWnd::AddRef and Release, which are kept.
int MfPiccoloConsoleWnd::AddRef()
{
    return 1;
}

int MfPiccoloConsoleWnd::Release()
{
    return 0;
}

// 0x1000c3c0 ?PrintString@MfPiccoloConsoleWnd@@UAEXABVMfString@@@Z
void MfPiccoloConsoleWnd::PrintString(const MfString &text)
{
    Print(text, 0);
}

// 0x1000c3e0 ?PrintStringNl@MfPiccoloConsoleWnd@@UAEXABVMfString@@@Z
void MfPiccoloConsoleWnd::PrintStringNl(const MfString &text)
{
    Print(text, 1);
}

// 0x1000c400 ?Print@MfPiccoloConsoleWnd@@IAEXABVMfString@@H@Z
void MfPiccoloConsoleWnd::Print(const MfString &text, int newLine)
{
    MfString line(text);
    if (newLine)
        line << "\r\n";
    if (m_hWnd) {
        AppendText(line);
        PostMessage(WM_PE_UPDATE_CARET);
    }
}

// 0x1000c490 ?ClearConsole@MfPiccoloConsoleWnd@@UAEXXZ
void MfPiccoloConsoleWnd::ClearConsole()
{
    Clear();
}
