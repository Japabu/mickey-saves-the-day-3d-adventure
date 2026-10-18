// The plugin loader (RuntimeLib.dll 1008c3b0-1008c8c0): lists and loads the
// plugin DLLs of the plugin directory (renderer, sound and hook plugins);
// problems go to the log.
#include <windows.h>
#include "MfString.h"
#include "MfStringList.h"
#include "MfDirectory.h"
#include "MfRegularExpression.h"
#include "MfLogger.h"
#include "MfRenderer.h"
#include "UtilityApp.h"

static void LogPluginError(const MfString &type, const MfString &message);

// TODO 0x1008c3b0 ?MfFindPlugins@@YAXABVMfString@@0AAVMfStringList@@@Z: (80% similar) the frame has one
// slot more below the directory (all locals 4 bytes higher) and the loop keeps the
// list item in other registers; a named message is further off (35% similar)
// Adds the plugin files of the plugin directory whose names start with
// prefix (<prefix>*.dll) to list.
void MfFindPlugins(const MfString &prefix, const MfString &type, MfStringList &list)
{
    MfDirectory dir;
    UtilityApp app;
    MfString path = app.PluginDirectory();
    if (path == NULL_STRING || !dir.Open(path)) {
        LogPluginError(type, MfString("There are no plugin directories defined.") +
                                 MfString("Plugin directories are specified in the") +
                                 MfString("PLUGINS/PLUGINSD registry entry."));
    } else {
        MfString pattern("^");
        pattern << prefix;
        pattern << ".*\\.dll$";
        MfRegularExpression expression(pattern);
        expression.Optimize();
        for (MfDirectoryFileInfo *file = dir.First(); file; file = dir.Next()) {
            MfString name = file->GetFileName();
            if (expression.Matches(name) == 1)
                list.Add(name);
        }
        dir.Close();
    }
}

// 0x1008c650 ?MfLoadPlugin@@YAHABVMfString@@0PAUMfPluginInfo@@@Z
// Loads the plugin name (from the plugin directory) and finds its entry
// point; 0 if either fails.
int MfLoadPlugin(const MfString &type, const MfString &name, MfPluginInfo *info)
{
    UtilityApp app;
    MfString dir = app.PluginDirectory();
    MfString path;
    if (dir != NULL_STRING)
        path = dir + MfString("\\") + name;
    else
        path = name;
    HMODULE module = LoadLibrary(path);
    if (!module) {
        MfString message = MfString("Could not load plugin: ") + name;
        LogPluginError(type, message);
        return 0;
    }
    FARPROC init = GetProcAddress(module, "_InitPlugin@4");
    if (!init) {
        MfString message = MfString("Could not find entry point in plugin: ") + name;
        LogPluginError(type, message);
        FreeLibrary(module);
        return 0;
    }
    info->m00 = (int)module;
    info->mCreate = (void(__stdcall *)(void **))init;
    return 1;
}

// 0x1008c850 ?LogPluginError@@YAXABVMfString@@0@Z
static void LogPluginError(const MfString &type, const MfString &message)
{
    MfString source("Plugin Loader");
    MfLogger::Log(1, type, source, message);
}
