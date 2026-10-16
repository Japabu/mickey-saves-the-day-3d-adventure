#pragma once
// CMBCursor.h: the software mouse cursor, drawn by a background thread straight onto the front
// buffer (declaration from SteamboatApp.pdb).

class IMBAnim;

class CMBCursor {
public:
    static bool __cdecl Initialize();
    static void __cdecl Terminate();
    static void __cdecl SetCursor(IMBAnim *anim);
    static long __cdecl GetHideCount();
    static void __cdecl SetHideCount(long newCount);
    static long __cdecl Show();
    static long __cdecl Hide();
    static void __cdecl ForceShow(void);
    static void __cdecl ForceHide(void);
    static void __cdecl Pause();
    static void __cdecl Resume();
    static bool __cdecl IsPaused();
};
