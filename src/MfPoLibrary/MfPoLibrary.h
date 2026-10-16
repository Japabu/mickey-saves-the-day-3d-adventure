// MfPoLibrary: registers MfPoLibrary's script classes with Piccolo and keeps
// the application interfaces they use.
#pragma once
class MfAiAppWorld;
class MfAiKeyboard;
class MfAiMouse;
class MfAiWorld;

class __declspec(dllexport) MfPoLibrary {
public:
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
private:
    MfPoLibrary() {}
    ~MfPoLibrary();
};

extern __declspec(dllexport) MfAiMouse *g_paiMouse;
extern __declspec(dllexport) MfAiWorld *g_paiWorld;
extern __declspec(dllexport) MfAiAppWorld *g_paiAppWorld;
extern __declspec(dllexport) MfAiKeyboard *g_paiKeyboard;
