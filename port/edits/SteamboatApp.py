# SteamboatApp edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    # MBGUID as the 16-byte GUID layout (C long is 64 bits here; MBCom's override does the
    # same): data1 is 32 bits and data4 is compared as two 32-bit words.
    'SteamboatApp/MBGUID.h': [
        ('    unsigned long data1;\n', '    port_u32 data1; // port: 32 bits, the GUID layout\n', 1),
        ('((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0]',
         '((const port_u32 *)data4)[0] == ((const port_u32 *)g.data4)[0]', 1),
        ('((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1]',
         '((const port_u32 *)data4)[1] == ((const port_u32 *)g.data4)[1]', 1),
    ],
    # msvcirt's flush/setprecision/setw, statically linked into the DLL and written on
    # msvcirt's own ios/ostream layout (SteamboatApp/ios.h, ostream.h). The port's
    # iostreams are the standard ones, whose manipulators serve; the file compiles empty.
    'SteamboatApp/Globals.cpp': [
        ('#include "ios.h"\n', '#if 0 /* port: msvcirt internals, see port/edits */ \n', 1),
        ('    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n',
         '    *(int *)((char *)&r + 0x30) = w;\n    return r;\n}\n#endif', 1),
    ],
}
