// UtilityApp: per-application settings of mfutil's users: the registry keys for
// the profile ("Software\<RegKey>\<AppName>") and the machine-wide data
// ("Software\<RegKey>\Global Data"), and the directory the plugins load from.
// Exported names only (no PDB); the DLL entry point fills in the statics.
#ifndef MFUTIL_UTILITYAPP_H
#define MFUTIL_UTILITYAPP_H

#include "mfutil_api.h"
#include <windows.h>

class MfString;
class MfStringList;

class MFUTIL_API UtilityApp {
    friend class MfKeyDialog;
    friend class MfNameDialog;
    friend BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved);
public:
    UtilityApp() {}
    ~UtilityApp() {}
    static HINSTANCE LibInstance() { return hLibInstance; }
    static const MfString &AppName() { return fsAppName; }
    static void SetRegistryKey(const MfString &key);

    int GetProfileInt(const MfString &section, const MfString &entry, int nDefault);
    MfString GetProfileString(const MfString &section, const MfString &entry, const MfString &sDefault);
    int WriteProfileInt(const MfString &section, const MfString &entry, int nValue);
    int WriteProfileString(const MfString &section, const MfString &entry, const MfString &sValue);
    int GetGlobalInt(const MfString &section, const MfString &entry, int nDefault);
    MfString GetGlobalString(const MfString &section, const MfString &entry, const MfString &sDefault);
    int WriteGlobalInt(const MfString &section, const MfString &entry, int nValue);
    int WriteGlobalString(const MfString &section, const MfString &entry, const MfString &sValue);

    // The plugin directory: the one set last, else the PLUGINS environment
    // variable, the global PATHS setting or "PLUGINS" (made absolute).
    void SetPluginDirectory(const MfString &dir);
    MfString PluginDirectory();
    // Appends the plugin search path (the PLUGINS variable) to list.
    int PluginPath(MfStringList *list);

private:
    static void InitKeyEntry();

    static int bOverrideOnPluginPath;
    static MfString fsAppName;
    static MfString fsGlobalKey;
    static MfString fsKeyEntry;
    static MfString fsLastSuccessfulPluginDir;
    static MfString fsRegKey;
    static HINSTANCE hLibInstance;
};

#endif
