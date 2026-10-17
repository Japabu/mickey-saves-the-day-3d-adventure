// MfPoLibrary: registers MfPoLibrary's script classes with Piccolo and keeps
// the application interfaces they use.
#ifndef MFPOLIBRARY_MFPOLIBRARY_H
#define MFPOLIBRARY_MFPOLIBRARY_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
class MfAiAppWorld;
class MfAiKeyboard;
class MfAiMouse;
class MfAiWorld;

class MFPO_API MfPoLibrary {
public:
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
private:
    MfPoLibrary() {}
    ~MfPoLibrary();
};

extern MFPO_API MfAiMouse *g_paiMouse;
extern MFPO_API MfAiWorld *g_paiWorld;
extern MFPO_API MfAiAppWorld *g_paiAppWorld;
extern MFPO_API MfAiKeyboard *g_paiKeyboard;

#endif
