// platform.cpp - placeholders until the Win32 layer (port/platform/win32/) provides them
#include <windows.h>

// Called by each module's generated attach stub (port/flags.py) before its DllMain:
// returns the module's HINSTANCE. The Win32 layer's module table replaces this.
extern "C" __attribute__((visibility("default"), weak))
HINSTANCE port_module_attach(const char *, const void *anchor)
{
    return (HINSTANCE)anchor;
}
