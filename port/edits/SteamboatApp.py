# SteamboatApp edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    # msvcirt's flush/setprecision/setw, statically linked into the DLL and written on
    # msvcirt's own ios/ostream layout (SteamboatApp/ios.h, ostream.h). The port's
    # iostreams are the standard ones, whose manipulators serve; the file compiles empty.
    'SteamboatApp/Globals.cpp': [
        ('#include "ios.h"\n', '#if 0 /* port: msvcirt internals, see port/edits */ \n', 1),
        ('    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n',
         '    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n#endif', 1),
    ],
}
