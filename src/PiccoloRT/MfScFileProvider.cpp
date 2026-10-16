// PiccoloRT.dll 0x100097a0-0x1000a3d5: the source type "FILE" and the
// runtime's module handle.
#define STRICT
#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include "PiccoloRuntime.h"
#include "MfScFileProvider.h"
#include "MfString.h"
#include "MfStringList.h"
#include "MfScPiccolo.h"

HINSTANCE PiccoloRuntime::hInstHandle;

// 0x100097c0 _$E22 (static initializer of sFileName, with the three below)
// 0x100097d0 _$E18
// 0x100097f0 _$E21
// 0x10009800 _$E19
MfString MfScFileProvider::sFileName((MfStringSize(256)));

// 0x10009820 _$E27 (static initializer of sWindowClass, with the three below)
// 0x10009830 _$E24
// 0x10009850 _$E26
// 0x10009860 _$E25
// the class of the hidden window that owns the file dialog
static MfString sWindowClass("FileProviderHiddenClass");

// the file dialog's filter
static const char *sFilter = "All Files (*.*) \0 *.* \0";
// the hidden window the dialog hook talks to, and whether its class is registered
static HWND hHiddenWindow;
static int bHiddenClassRegistered;
static char szSelectedFile[256];

// 0x100097a0 ?InstHandle@PiccoloRuntime@@SAPAUHINSTANCE__@@XZ
HINSTANCE PiccoloRuntime::InstHandle()
{
    return hInstHandle;
}

// 0x10009870 ??0MfScFileProvider@@QAE@XZ
MfScFileProvider::MfScFileProvider()
    : mRefCount(1)
{
}

// 0x10009880 ??_GMfScFileProvider@@UAEPAXI@Z
// 0x100098a0 ??1MfScFileProvider@@UAE@XZ
MfScFileProvider::~MfScFileProvider()
{
}

// 0x100098b0 ?GetInterfaceName@MfScFileProvider@@UAEXPADHAAH1@Z
void MfScFileProvider::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "AiFileProvider", size);
    major = 1;
    minor = 0;
}

// 0x100098e0 ?AddRef@MfScFileProvider@@UAEHXZ
int MfScFileProvider::AddRef()
{
    return ++mRefCount;
}

// 0x100098f0 ?Release@MfScFileProvider@@UAEHXZ
int MfScFileProvider::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x10009910 ?GetType@MfScFileProvider@@UAEHAAVMfString@@@Z
int MfScFileProvider::GetType(MfString &type)
{
    type = "FILE";
    return 0;
}

// 0x10009c30 ?WriteSource@MfScFileProvider@@UAEHABVMfString@@AAV2@H@Z
int MfScFileProvider::WriteSource(const MfString &file, MfString &text, int)
{
    return text.WriteToFile(file) ? 0 : 4;
}

// 0x10009da0 ?IsSameModule@MfScFileProvider@@UAEHABVMfString@@0@Z
int MfScFileProvider::IsSameModule(const MfString &module, const MfString &name)
{
    MfString a = module;
    MfString b = name;
    a.ConvertToLower();
    b.ConvertToLower();
    return a == b;
}

// 0x10009fb0 ?FileExists@MfScFileProvider@@SAHABVMfString@@@Z
int MfScFileProvider::FileExists(const MfString &file)
{
    int exists = 0;
    HANDLE handle = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
        exists = 1;
    }
    return exists;
}

// 0x10009930 ?ReadSource@MfScFileProvider@@UAEHAAVMfString@@0H@Z
int MfScFileProvider::ReadSource(MfString &module, MfString &text, int)
{
    int result = 0;
    if (!FileExists(module)) {
        MfString name;
        MfString ext;
        module.SplitPath(0, 0, &name, &ext);
        sFileName = name + ext;
        MfString path = MfScPiccolo::fsDataPath + sFileName;
        if (!FileExists(path)) {
            MfString dir;
            if (BrowseForFile(sFileName, &dir)) {
                module = dir + MfString("\\") + sFileName;
                result = 5;
            } else
                return 2;
        } else {
            module = path;
            result = 5;
        }
    }
    if (!text.ReadFromFile(module))
        result = 3;
    return result;
}

// 0x10009c50 ?SplitPath@MfScFileProvider@@UAEHABVMfString@@PAVMfStringList@@@Z
int MfScFileProvider::SplitPath(const MfString &path, MfStringList *dirs)
{
    dirs->Clear();
    if (path.GetCharacter(1) == ':')
        dirs->Add(path.LeftString(2));
    int start = path.HaveCharactersInSet("\\", 0);
    while (start >= 0) {
        int end = path.HaveCharactersInSet("\\", start + 1);
        dirs->Add(path.MidString(start + 1, (end == -1 ? path.Length() : end) - start - 1));
        if (end < 0)
            break;
        start = end;
    }
    return 1;
}

// 0x1000a1c0 ?RegisterHiddenClass@MfScFileProvider@@CAXXZ
void MfScFileProvider::RegisterHiddenClass()
{
    if (!bHiddenClassRegistered) {
        bHiddenClassRegistered = 1;
        WNDCLASS wc;
        wc.style = CS_CLASSDC;
        wc.lpfnWndProc = HiddenWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = PiccoloRuntime::hInstHandle;
        wc.hIcon = 0;
        wc.hCursor = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = sWindowClass;
        RegisterClass(&wc);
    }
}

// 0x1000a230 ?FileDialogHook@MfScFileProvider@@CGIPAUHWND__@@IIJ@Z
// a double click on the folder list or OK lets the hidden window check the folder
UINT CALLBACK MfScFileProvider::FileDialogHook(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_COMMAND) {
        int notify = HIWORD(wParam);
        int id = LOWORD(wParam);
        if (notify == LBN_DBLCLK || notify == BN_CLICKED) {
            if (id == lst2 || id == IDOK) {
                PostMessage(hHiddenWindow, WM_USER + 1, 0, (LPARAM)dialog);
                return 0;
            }
        }
    } else if (message == WM_USER + 2) {
        EndDialog(dialog, 1);
        return 0;
    }
    return 0;
}

// 0x1000a2a0 ?HiddenWindowProc@MfScFileProvider@@CGJPAUHWND__@@IIJ@Z
// closes the dialog once the current folder holds the file looked for
LRESULT CALLBACK MfScFileProvider::HiddenWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_USER + 1) {
        char dir[256];
        GetCurrentDirectory(255, dir);
        MfString path = MfString(dir) + MfString("\\") + MfScFileProvider::sFileName;
        if (MfScFileProvider::FileExists(path))
            PostMessage((HWND)lParam, WM_USER + 2, 0, 0);
    }
    return DefWindowProc(window, message, wParam, lParam);
}

// 0x10009ff0 ?BrowseForFile@MfScFileProvider@@SAHABVMfString@@PAV2@@Z
// lets the user pick the folder that holds the file name
int MfScFileProvider::BrowseForFile(const MfString &name, MfString *dir)
{
    int result;
    RegisterHiddenClass();
    {
        char fileName[1];
        OPENFILENAME ofn;
        szSelectedFile[0] = 0;
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = 0;
        ofn.hInstance = PiccoloRuntime::hInstHandle;
        ofn.lpstrFilter = sFilter;
        ofn.lpstrCustomFilter = 0;
        ofn.nFilterIndex = 0;
        fileName[0] = 0;
        ofn.lpstrFile = fileName;
        ofn.nFileOffset = 0;
        ofn.nFileExtension = 0;
        ofn.lpstrDefExt = 0;
        ofn.nMaxFile = 255;
        ofn.lpstrFileTitle = 0;
        ofn.lpstrInitialDir = 0;
        ofn.lpstrTitle = 0;
        ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_NOVALIDATE;
        ofn.lpfnHook = FileDialogHook;
        ofn.lpTemplateName = MAKEINTRESOURCE(410);
        hHiddenWindow = CreateWindowEx(0, sWindowClass, "", 0, 0, 0, 0, 0, 0, 0, PiccoloRuntime::hInstHandle, 0);
        result = GetOpenFileName(&ofn);
    }
    DestroyWindow(hHiddenWindow);
    hHiddenWindow = 0;
    CommDlgExtendedError();
    if (result == 1) {
        char current[256];
        GetCurrentDirectory(255, current);
        *dir = current;
        return FileExists(*dir + MfString("\\") + name);
    }
    return 0;
}
