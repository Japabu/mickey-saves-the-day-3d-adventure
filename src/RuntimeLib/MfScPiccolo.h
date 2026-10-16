// MfScPiccolo (PiccoloRT.dll): the Piccolo runtime's start, plugins,
// extensions and source registry. The statics RuntimeLib uses; signatures as
// in src/PiccoloRT/MfScPiccolo.h (bIsRunning is private there, MfRuntime sets
// it).
#ifndef RUNTIMELIB_MFSCPICCOLO_H
#define RUNTIMELIB_MFSCPICCOLO_H
class MfAiSourceResolver;
class MfString;

class __declspec(dllimport) MfScPiccolo {
    friend class MfRuntime;
public:
    static MfAiSourceResolver *__cdecl GetSourceResolver();
    static int __cdecl InitializeExtensions(MfString *errors);
    static int __cdecl InitializePlugins(MfString *errors);
    static void __cdecl Initialize();
    static void __cdecl LoadPiccoloDataPathFunctions(int);
    static void __cdecl LoadPiccoloLibFunctions(int);
    static void __cdecl Shutdown(MfString *errors);
private:
    static int bIsRunning;
};

// MfPoLibrary.dll: the script classes of the library
class __declspec(dllimport) MfPoLibrary {
public:
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
};

#endif
