// Runtime/Win/MBWinDllMain.cpp: every MB DLL links this DllMain. The other DLLs
// build it with MBRUNTIME_IMPORT (MBCom/MBWinDllMain.cpp, MBEngine/MBWinDllMain.cpp)
// and call MBDebugStr through the mbruntime.dll import.
#include <windows.h>

#ifdef MBRUNTIME_IMPORT
extern "C" __declspec(dllimport) void __cdecl MBDebugStr(const char *fmt, ...);
#else
extern "C" void __cdecl MBDebugStr(const char *fmt, ...);
#endif

// 0x10001850 _DllMain@12
// Logs the module path and its creation time and size when the DLL is loaded.
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    char moduleName[MAX_PATH];

    if (dwReason == DLL_PROCESS_ATTACH && GetModuleFileName(hInstance, moduleName, sizeof(moduleName))) {
        moduleName[sizeof(moduleName) - 1] = 0;
        MBDebugStr("INFO: Module load: %s.\n", moduleName);
        HANDLE file = CreateFile(moduleName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file) {
            BY_HANDLE_FILE_INFORMATION fileInfo;
            if (GetFileInformationByHandle(file, &fileInfo)) {
                char *days[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
                char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
                SYSTEMTIME systime;
                FileTimeToSystemTime(&fileInfo.ftCreationTime, &systime);
                MBDebugStr("      Created: %s, %d-%s-%d %02d:%02d:%02d. %d bytes.\n",
                           days[systime.wDayOfWeek], systime.wDay, months[systime.wMonth - 1],
                           systime.wYear, systime.wHour, systime.wMinute, systime.wSecond,
                           fileInfo.nFileSizeLow);
            } else {
                MBDebugStr("      No information available (GetFileInfo failed, GLE=%d).\n", GetLastError());
            }
            CloseHandle(file);
        } else {
            MBDebugStr("      No information available (CreateFile failed, GLE=%d).\n", GetLastError());
        }
    }
    return TRUE;
}
