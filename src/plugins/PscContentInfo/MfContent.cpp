// PscContentInfo.dll 0x10001000-0x10002c4f: the plugin entry points and
// MfContent, the "ContentInfoI" interface object.
#include "PscContentInfo.h"

void CreateContentObject();
void DestroyContentObject();

// 0x1000caf0
HINSTANCE g_hInstance;

// 0x10001000 _DllMain@12
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_hInstance = hInstance;
        return TRUE;
    }
    if (dwReason == DLL_PROCESS_DETACH)
        g_hInstance = NULL;
    return TRUE;
}

// 0x10001030 _PscInitialize@8
extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *err)
{
    *err = NULL_STRING;
    CreateContentObject();
    return 1;
}

// 0x10001050 _PscShutdown@4
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err)
{
    *err = NULL_STRING;
    DestroyContentObject();
    return 1;
}

// 0x10001070 ??0MfContent@@QAE@XZ
MfContent::MfContent()
    : m_nRefs(1), m_notify(0)
{
    m_dataPath.GetVariable("MFDATAPATH");
    if (m_dataPath == NULL_STRING) {
        {
            MfString function("Content");
            MfString module("Content Info");
            MfLogger::Log(2, module, function,
                          MfString("MFDATAPATH is undefined.\n") +
                          MfString("MFDATAPATH should be defined in the registry for this application\n"));
        }
        m_dataPath = ".";
    }
    MfString piccoloPath(m_dataPath);
    piccoloPath << "\\Piccolo";
    MfScPiccolo::fsDataPath = piccoloPath;

    m_cache = new MfContentInfoCache;
    MfString contentPath;
    contentPath.GetVariable("MFCONTENT");
    if (contentPath != NULL_STRING)
        m_cache->AddPath(contentPath);
    UtilityApp app;
    contentPath = app.GetGlobalString("PATHS", "MFCONTENT", NULL_STRING);
    if (contentPath == NULL_STRING) {
        {
            MfString function("Content");
            MfString module("Content Info");
            MfLogger::Log(2, module, function,
                          MfString("MFCONTENT is undefined.\n") +
                          MfString("MFCONTENT should be defined in the registry for this application\n"));
        }
        contentPath = ".";
    }
    if (!MfCopyFolder::Exists(contentPath))
        MfCopyFolder::CreateFolder(contentPath);
    m_cache->AddPath(contentPath);
    m_cache->Refresh();
}

// 0x10001450 ??_GMfContent@@UAEPAXI@Z
// 0x10001470 ??1MfContent@@UAE@XZ
MfContent::~MfContent()
{
    delete m_cache;
}

// 0x100014f0 ?GetInterfaceName@MfContent@@UAEXPADHPAH1@Z
void MfContent::GetInterfaceName(char *name, int size, int *major, int *minor)
{
    MfString interfaceName("ContentInfoI");
    strncpy(name, interfaceName, size - 1);
    *major = 1;
    *minor = 0;
}

// 0x10001580 ?AddRef@MfContent@@UAEHXZ
// 0x10001590 ?Release@MfContent@@UAEHXZ

// 0x100015b0 ?SetDataPath@MfContent@@QAEXABVMfString@@@Z
void MfContent::SetDataPath(const MfString &path)
{
    if (m_notify)
        m_notify(0);
    MfScPiccolo::FlushPiccoloDataPathFunctions();
    if (m_notify)
        m_notify(1);
    m_dataPath = path;
    UtilityApp app;
    app.WriteProfileStringA("PATHS", "MFDATAPATH", m_dataPath);
    if (!MfCopyFolder::Exists(m_dataPath))
        MfCopyFolder::CreateFolder(m_dataPath);
    MfString piccoloPath(m_dataPath);
    piccoloPath << "\\Piccolo";
    MfScPiccolo::fsDataPath = piccoloPath;
    if (m_notify)
        m_notify(2);
    MfScPiccolo::LoadPiccoloLibFunctions(0);
    MfScPiccolo::LoadPiccoloDataPathFunctions(1);
    if (m_notify)
        m_notify(3);
}

// 0x10001700 ?SelectContent@MfContent@@UAEHABVMfString@@@Z
int MfContent::SelectContent(const MfString &name)
{
    MfContentInfo info;
    if (m_cache->Find(name, info)) {
        m_contentName = name;
        SetDataPath(info.GetFolder());
        return 1;
    }
    return 0;
}

// 0x100018b0 ??1MfContentInfo@@QAE@XZ
// 0x10001940 ??1MfContentDescription@@QAE@XZ

// 0x100019c0 ?GetCurrentContent@MfContent@@UAEHPAUMfContentDescription@@@Z
int MfContent::GetCurrentContent(MfContentDescription *desc)
{
    if (m_contentName != NULL_STRING) {
        MfContentInfo info;
        if (m_cache->Find(m_contentName, info)) {
            info.GetDescription(*desc);
            return 1;
        }
    }
    return 0;
}

// 0x10001b60 ?GetContent@MfContent@@UAEHABVMfString@@PAUMfContentDescription@@@Z
int MfContent::GetContent(const MfString &name, MfContentDescription *desc)
{
    MfContentInfo info;
    if (m_cache->Find(name, info)) {
        info.GetDescription(*desc);
        return 1;
    } else
        return 0;
    return 0;
}

// 0x10001cf0 ?GetDataPath@MfContent@@UAE?AVMfString@@XZ
MfString MfContent::GetDataPath()
{
    return m_dataPath;
}

// TODO 0x10001d20 ?ChooseContentProc@MfContent@@SGHPAXIIJ@Z: (71% similar) WM_INITDIALOG lines up apart from registers (the original keeps msg and then hDlg in ebx, 0 in ebp, the string conversion import in edi); in WM_COMMAND the original keeps IDCANCEL's NULL_STRING assignment as its own block before IDOK and merges only the call with IDOK's failure path (ours merges the whole tail); tried a switch on msg, function-scope content/hList
BOOL CALLBACK MfContent::ChooseContentProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = FALSE;
    if (msg == WM_INITDIALOG) {
        MfContent *content = (MfContent *)lParam;
        SetWindowLong(hDlg, GWL_USERDATA, lParam);
        MfStringList names;
        content->m_cache->GetNames(names);
        HWND hList = GetDlgItem(hDlg, 1000);
        MfDblListItem *pos;
        for (MfString *p = (MfString *)names.GetFirst(pos); p; p = (MfString *)names.GetNext(pos))
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)(const char *)*p);
        int i = SendMessage(hList, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(const char *)content->m_contentName);
        if (i != LB_ERR)
            SendMessage(hList, LB_SETCURSEL, i, 0);
        else
            SendMessage(hList, LB_SETCURSEL, 0, 0);
        RECT rect;
        GetWindowRect(hDlg, &rect);
        MoveWindow(hDlg,
                   (GetSystemMetrics(SM_CXMAXIMIZED) - (rect.right - rect.left)) / 2,
                   (GetSystemMetrics(SM_CYMAXIMIZED) - (rect.bottom - rect.top)) / 2,
                   rect.right - rect.left, rect.bottom - rect.top, TRUE);
        result = TRUE;
    } else {
        MfContent *content = (MfContent *)GetWindowLong(hDlg, GWL_USERDATA);
        HWND hList = GetDlgItem(hDlg, 1000);
        if (msg == WM_COMMAND) {
            switch (LOWORD(wParam)) {
            case 1000:
                if (HIWORD(wParam) == LBN_DBLCLK) {
                    int sel = SendMessage(hList, LB_GETCURSEL, 0, 0);
                    char *text = new char[SendMessage(hList, LB_GETTEXTLEN, sel, 0) + 1];
                    SendMessage(hList, LB_GETTEXT, sel, (LPARAM)text);
                    content->m_chosen = text;
                    delete text;
                    EndDialog(hDlg, IDOK);
                }
                break;
            case IDCANCEL:
                content->m_chosen = NULL_STRING;
                EndDialog(hDlg, IDCANCEL);
                break;
            case IDOK: {
                int sel = SendMessage(hList, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR) {
                    char *text = new char[SendMessage(hList, LB_GETTEXTLEN, sel, 0) + 1];
                    SendMessage(hList, LB_GETTEXT, sel, (LPARAM)text);
                    content->m_chosen = text;
                    delete text;
                    EndDialog(hDlg, IDOK);
                } else {
                    content->m_chosen = NULL_STRING;
                    EndDialog(hDlg, IDCANCEL);
                }
                break;
            }
            }
        }
    }
    return result;
}

// 0x10002010 ?ChooseContent@MfContent@@UAEHPAXPAVMfString@@@Z
int MfContent::ChooseContent(HWND hParent, MfString *name)
{
    m_cache->Refresh();
    int result = DialogBoxParam(g_hInstance, MAKEINTRESOURCE(0x68), hParent, (DLGPROC)ChooseContentProc, (LPARAM)this);
    *name = m_chosen;
    m_chosen = NULL_STRING;
    return result == IDOK;
}

// 0x10002070 ?MakeContentPath@MfContent@@QAE?AVMfString@@ABV2@@Z
MfString MfContent::MakeContentPath(const MfString &name)
{
    if (name.GetCharacter(1) == ':')
        return name;
    MfString path(name);
    if (name.GetCharacter(0) == '\\' || name.GetCharacter(0) == '/') {
        path = name.RightString(name.Length() - 1);
    } else if (name.GetCharacter(0) == '.') {
        int i = name.HaveCharactersInSet("/\\", 0);
        if (i >= 0)
            path = name.RightString(name.Length() - i - 1);
        else
            path = NULL_STRING;
    }
    MfString root;
    UtilityApp app;
    root = app.GetGlobalString("PATHS", "MFCONTENT", NULL_STRING);
    if (path != NULL_STRING)
        root << "\\" << path;
    return root;
}

// 0x100022d0 ?SetContentPath@MfContent@@UAEHABVMfString@@@Z
int MfContent::SetContentPath(const MfString &name)
{
    MfString path = MakeContentPath(name);
    m_contentName = NULL_STRING;
    SetDataPath(path);
    return 1;
}

// 0x10002350 ?SaveContentInfoToPath@MfContent@@UAEHABVMfString@@0PAUMfContentDescription@@@Z
int MfContent::SaveContentInfoToPath(const MfString &name, const MfString &fileName, MfContentDescription *desc)
{
    MfString folder = MakeContentPath(name);
    if (!MfCopyFolder::Exists(folder))
        MfCopyFolder::CreateFolder(folder);
    MfString file = folder + MfString("\\") + fileName + MfString(".cif");
    ofstream out;
    out.open(file, ios::out, filebuf::openprot);
    if (!out.is_open())
        return 0;
    struct timeb now;
    ftime(&now);
    MfTime time(now);
    MfString created;
    created.FormatTime("%c", time);
    out << "#" << endl;
    out << "# Content information file: " << fileName << endl;
    out << "# Created: " << created << endl;
    out << "#" << endl;
    if (desc->name != NULL_STRING) {
        out << "name " << desc->name << endl;
        if (desc->vendor != NULL_STRING)
            out << "vendor " << desc->vendor << endl;
        if (desc->description != NULL_STRING)
            out << "description " << desc->description << endl;
        if (desc->version != NULL_STRING)
            out << "version " << desc->version << endl;
        if (desc->defaultSpace != NULL_STRING)
            out << "default-space " << desc->defaultSpace << endl;
        out.close();
        return 1;
    }
    {
        MfString function("SaveContentInfoToPath");
        MfString module("Content Info");
        MfLogger::Log(2, module, function,
                      MfString("Missing content name.\n") +
                      MfString("When writing the content information to the file named\n") +
                      file + MfString(", there was no content name given."));
    }
    out.close();
    return 0;
}

// 0x100028c0 ?CreateDataFolder@MfContent@@QAEHABVMfString@@@Z
int MfContent::CreateDataFolder(const MfString &folder)
{
    if (!MfCopyFolder::Exists(folder) && !MfCopyFolder::CreateFolder(folder)) {
        MfString msg("Cannot create folder\n");
        msg << "Cannot create the '" << folder << "' folder in the data folder.";
        MfString function("Create Datapath Folder");
        MfString module("Content Info");
        MfLogger::Log(1, module, function, 0, &msg, 0);
        return 0;
    }
    return 1;
}

// 0x100029e0 ?CreateDataFolders@MfContent@@UAEHXZ
int MfContent::CreateDataFolders()
{
    if (m_dataPath == NULL_STRING)
        return 0;
    MfString folder;
    folder = m_dataPath + MfString("\\Actors");
    CreateDataFolder(folder);
    folder = m_dataPath + MfString("\\Spaces");
    CreateDataFolder(folder);
    folder = m_dataPath + MfString("\\Images");
    CreateDataFolder(folder);
    folder = m_dataPath + MfString("\\Sounds");
    CreateDataFolder(folder);
    folder = m_dataPath + MfString("\\Piccolo");
    CreateDataFolder(folder);
    return 1;
}

// 0x10002c00 ?SetNotify@MfContent@@UAEP6AXH@ZP6AXH@Z@Z
MfContentNotify MfContent::SetNotify(MfContentNotify notify)
{
    MfContentNotify old = m_notify;
    m_notify = notify;
    return old;
}

// 0x10002c10 ?GetContentNames@MfContent@@UAEHPAVMfStringList@@@Z
int MfContent::GetContentNames(MfStringList *names)
{
    m_cache->Refresh();
    m_cache->GetNames(*names);
    names->Sort(1);
    return 1;
}

// 0x10002c40 ?Refresh@MfContent@@UAEXXZ
void MfContent::Refresh()
{
    m_cache->Refresh();
}
