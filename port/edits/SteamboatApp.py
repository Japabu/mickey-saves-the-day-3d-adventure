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
    # One definition each: SimpleNode.cpp and ScriptHelpNode.cpp compile MBEngine's units
    # into the DLL with their own copies of DatabaseMgr::Database::AddDBRef and
    # ScriptManager::AddRef (VC6 linked the duplicates; the macOS linker refuses them). The
    # copies kept are those of the classes the engine units declare in full (Database's
    # MBComPtr is pointer-sized; DatabaseMgr.h's draft has an int there).
    'SteamboatApp/DatabaseMgr_Database.cpp': [
        ('unsigned long DatabaseMgr::Database::AddDBRef()\n{\n    return ++field_4;\n}\n',
         '#if 0 // port: defined by SimpleNode.cpp (MBEngine/SimpleNode.cpp)\n{\n    return ++field_4;\n#endif\n', 1),
    ],
    'SteamboatApp/ScriptManager.cpp': [
        ('unsigned long ScriptManager::AddRef()\n{\n    return ++_mRefCnt;\n}\n',
         '#if 0 // port: defined by ScriptHelpNode.cpp (MBEngine/ScriptHelpNode.cpp)\n{\n    return ++_mRefCnt;\n#endif\n', 1),
    ],
}
