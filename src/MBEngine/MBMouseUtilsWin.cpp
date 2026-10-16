// MBMouseUtilsWin.cpp - Win32 mouse helpers.
#include <windows.h>

// 0x1000b780 _MBShowSystemCursor
// Shows or hides the Windows cursor; ShowCursor counts, so only call it on a change.
extern "C" bool __cdecl MBShowSystemCursor(bool show)
{
    static bool s_visible = true;
    if (show != s_visible)
        ShowCursor(show);
    s_visible = show;
    return s_visible;
}
