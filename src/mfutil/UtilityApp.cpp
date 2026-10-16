// UtilityApp (mfutil.dll 0x100116e0-0x10012820) and the DLL entry point.
#include "MfString.h"
#include <commctrl.h>
#include "MfStringList.h"
#include "MfDirectory.h"
#include "MfRegVariable.h"
#include "MfLogger.h"
#include "UtilityApp.h"

// 0x10004b80 ?LibInstance@UtilityAppSAPAUHINSTANCE__XZ (inline, header)
// 0x10004b90 ?AppName@UtilityAppSAABVMfStringXZ (inline, header)

// 0x10004ba0 ?SetRegistryKey@UtilityAppSAXABVMfString@Z
void UtilityApp::SetRegistryKey(const MfString &key)
{
    fsRegKey = key;
}

// Separators of the directories in the PLUGINS variable.
static const char *const szPathSeparators = ";";

// 0x100116e0 _$E42 (calls the constructor and registers the destructor)
// 0x100116f0 _$E39
// 0x10011700 _$E41
// 0x10011710 _$E40
static MfString fsPluginsName("PLUGINS");
// 0x10011720 _$E47
// 0x10011730 _$E44
// 0x10011740 _$E46
// 0x10011750 _$E45
static MfString fsGlobalDataName("Global Data");

// 0x10011760 _DllMain@12
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        UtilityApp::hLibInstance = hInstance;
        char szPath[MAX_PATH];
        GetModuleFileNameA(NULL, szPath, MAX_PATH);
        MfString path(szPath);
        path.SplitPath(NULL, NULL, &UtilityApp::fsAppName, NULL);
        UtilityApp::InitKeyEntry();
        InitCommonControls();
        MfLogger::GetProgramStartupInfo();
    } else if (dwReason == DLL_PROCESS_DETACH) {
        MfLogger::Shutdown();
    }
    return TRUE;
}

// 0x10011820 _$E53
// 0x10011830 _$E49
// 0x10011840 _$E52
// 0x10011850 _$E50
MfString UtilityApp::fsAppName;
// 0x10011870 _$E58
// 0x10011880 _$E55
// 0x10011890 _$E57
// 0x100118a0 _$E56
MfString UtilityApp::fsKeyEntry;
// 0x100118c0 _$E63
// 0x100118d0 _$E60
// 0x100118e0 _$E62
// 0x100118f0 _$E61
MfString UtilityApp::fsGlobalKey;
// 0x10011910 _$E68
// 0x10011920 _$E65
// 0x10011930 _$E67
// 0x10011940 _$E66
MfString UtilityApp::fsRegKey("MotionFactory");
// 0x10011960 _$E73
// 0x10011970 _$E70
// 0x10011980 _$E72
// 0x10011990 _$E71
MfString UtilityApp::fsLastSuccessfulPluginDir;
HINSTANCE UtilityApp::hLibInstance;
int UtilityApp::bOverrideOnPluginPath;

// 0x100119b0 ?InitKeyEntry@UtilityAppCAXXZ
void UtilityApp::InitKeyEntry()
{
    fsKeyEntry = NULL_STRING;
    fsKeyEntry << "Software\\" << fsRegKey << "\\" << fsAppName;
    fsGlobalKey = NULL_STRING;
    fsGlobalKey << "Software\\" << fsRegKey << "\\" << fsGlobalDataName;
}

// 0x10011a40 ?GetProfileIntA@UtilityAppQAEHABVMfString0H@Z
int UtilityApp::GetProfileInt(const MfString &section, const MfString &entry, int nDefault)
{
    InitKeyEntry();
    int nValue = nDefault;
    MfString key;
    key << fsKeyEntry << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value = var.Get();
    if (value != NULL_STRING)
        nValue = value;
    return nValue;
}

// 0x10011b30 ?GetProfileStringA@UtilityAppQAE?AVMfStringABV2@00@Z
MfString UtilityApp::GetProfileString(const MfString &section, const MfString &entry, const MfString &sDefault)
{
    InitKeyEntry();
    MfString result(sDefault);
    MfString key;
    key << fsKeyEntry << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value = var.Get();
    if (value != NULL_STRING)
        result = value;
    return result;
}

// 0x10011c60 ?WriteProfileInt@UtilityAppQAEHABVMfString0H@Z
int UtilityApp::WriteProfileInt(const MfString &section, const MfString &entry, int nValue)
{
    InitKeyEntry();
    MfString key;
    key << fsKeyEntry << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value(nValue);
    var.Set(value);
    return 1;
}

// 0x10011d40 ?WriteProfileStringA@UtilityAppQAEHABVMfString00@Z
int UtilityApp::WriteProfileString(const MfString &section, const MfString &entry, const MfString &sValue)
{
    InitKeyEntry();
    MfString key;
    key << fsKeyEntry << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    var.Set(sValue);
    return 1;
}

// 0x10011e00 ?GetGlobalInt@UtilityAppQAEHABVMfString0H@Z
int UtilityApp::GetGlobalInt(const MfString &section, const MfString &entry, int nDefault)
{
    InitKeyEntry();
    int nValue = nDefault;
    MfString key;
    key << fsGlobalKey << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value = var.Get();
    if (value != NULL_STRING)
        nValue = value;
    return nValue;
}

// 0x10011ef0 ?GetGlobalString@UtilityAppQAE?AVMfStringABV2@00@Z
MfString UtilityApp::GetGlobalString(const MfString &section, const MfString &entry, const MfString &sDefault)
{
    InitKeyEntry();
    MfString result(sDefault);
    MfString key;
    key << fsGlobalKey << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value = var.Get();
    if (value != NULL_STRING)
        result = value;
    return result;
}

// 0x10012020 ?WriteGlobalInt@UtilityAppQAEHABVMfString0H@Z
int UtilityApp::WriteGlobalInt(const MfString &section, const MfString &entry, int nValue)
{
    InitKeyEntry();
    MfString key;
    key << fsGlobalKey << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    MfString value(nValue);
    var.Set(value);
    return 1;
}

// 0x10012100 ?WriteGlobalString@UtilityAppQAEHABVMfString00@Z
int UtilityApp::WriteGlobalString(const MfString &section, const MfString &entry, const MfString &sValue)
{
    InitKeyEntry();
    MfString key;
    key << fsGlobalKey << "\\" << section;
    MfRegVariable var(HKEY_CURRENT_USER, key, entry);
    var.Set(sValue);
    return 1;
}

// 0x100121c0 ?SetPluginDirectory@UtilityAppQAEXABVMfString@Z
void UtilityApp::SetPluginDirectory(const MfString &dir)
{
    fsLastSuccessfulPluginDir = dir;
    bOverrideOnPluginPath = 1;
}

// EQUIV 0x100121e0 ?PluginDirectory@UtilityApp@@QAE?AVMfString@@XZ: reviewed: one EH funclet temporary at ebp-0x278 instead of ebp-0x27c
MfString UtilityApp::PluginDirectory()
{
    if (fsLastSuccessfulPluginDir != NULL_STRING)
        return fsLastSuccessfulPluginDir;

    MfDirectory dir;
    MfString name(fsPluginsName);
    MfString dirName;
    dirName.GetVariable(name);
    if (dirName == NULL_STRING)
        dirName = GetGlobalString("PATHS", name, NULL_STRING);
    {
        // a path: take its first directory
        MfStringList paths;
        dirName.Tokenize(szPathSeparators, paths);
        if (paths.GetCount() > 1) {
            MfDblListItem *pli;
            dirName = *paths.First(pli);
        }
    }
    if (dirName == NULL_STRING || (dirName != NULL_STRING && !dir.Open(dirName))) {
        dirName = fsPluginsName;
        if (!dir.Open(fsPluginsName))
            dirName = NULL_STRING;
    }
    if (dirName != NULL_STRING) {
        MfString drive;
        MfString path;
        dirName.SplitPath(&drive, &path, NULL, NULL);
        if (drive == NULL_STRING && path.GetCharacter(0) != '\\') {
            char szCurrentDir[512];
            GetCurrentDirectoryA(511, szCurrentDir);
            dirName = MfString(szCurrentDir) + MfString("\\") + dirName;
        }
        fsLastSuccessfulPluginDir = dirName;
    }
    return dirName;
}

// EQUIV 0x10012620 ?PluginPath@UtilityApp@@QAEHPAVMfStringList@@@Z: reviewed: test and mov of the first string swapped
int UtilityApp::PluginPath(MfStringList *list)
{
    // The original assigns an overriding plugin directory to this first
    // string, not to the one it tokenizes: with an override the list is
    // filled by the PluginDirectory() call at the end.
    MfString dirName = PluginDirectory();
    MfDirectory dir;
    MfString name(fsPluginsName);
    MfString path;
    if (bOverrideOnPluginPath)
        dirName = fsLastSuccessfulPluginDir;
    else
        path.GetVariable(name);
    path.Tokenize(szPathSeparators, *list);

    {
        MfDblListItem *pli;
        for (MfString *s = list->First(pli); s; ) {
            if (*s == NULL_STRING) {
                list->Delete(pli);
                s = list->Current(pli);
            } else {
                s = list->Next(pli);
            }
        }
    }
    if (list->GetCount() == 0)
        list->Add(PluginDirectory());
    if (list->GetCount() > 0)
        return 1;
    return 0;
}
