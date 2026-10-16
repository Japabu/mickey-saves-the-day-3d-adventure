// UtilityApp (mfutil.dll): the application's profile (registry) settings and
// the plugin directory. Declaration as in src/mfutil.
#ifndef RUNTIMELIB_UTILITYAPP_H
#define RUNTIMELIB_UTILITYAPP_H
#include <windows.h>
class MfString;
class MfStringList;

class __declspec(dllimport) UtilityApp {
public:
    UtilityApp() {}
    ~UtilityApp() {}
    static void SetRegistryKey(const MfString &key);

    int GetProfileInt(const MfString &section, const MfString &entry, int nDefault);
    MfString GetProfileString(const MfString &section, const MfString &entry, const MfString &sDefault);
    int WriteProfileInt(const MfString &section, const MfString &entry, int nValue);
    int WriteProfileString(const MfString &section, const MfString &entry, const MfString &sValue);
    void SetPluginDirectory(const MfString &dir);
    MfString PluginDirectory();
    int PluginPath(MfStringList *list);
};

#endif
