#pragma once
// mfutil.dll: per-application settings (registry keys, the plugin folders).
// The constructor and destructor are empty inlines.
class MfString;
class MfStringList;

class __declspec(dllimport) UtilityApp {
public:
    UtilityApp() {}
    ~UtilityApp() {}
    // appends the plugin search path (the PLUGINS variable) to list
    int PluginPath(MfStringList *list);
};
