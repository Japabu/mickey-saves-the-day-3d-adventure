# SteamboatApp edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    # e[verify(i)]: Visual C++ calls verify (which may reallocate e) before it loads e;
    # C++17 loads e first, so the store went to the old (on the first call null) array.
    'SteamboatApp/Vec.h': [
        ('    void setValueVerify(int i, const T &t) { e[verify(i)] = t; }',
         '    void setValueVerify(int i, const T &t) { int k = verify(i); e[k] = t; }', 1),
    ],
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
    # "SBSPbirdHigh" and a letter are 14 bytes with the terminator, in a 10-byte buffer (the
    # original overran its stack too; the macOS fortified sprintf aborts). The song key's
    # count is a size_t (8 bytes) printed with %d.
    'SteamboatApp/SBSongbirdNode.cpp': [
        ('        char name[10];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n',
         '        char name[16];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n', 1),
        ('sprintf(key, "song%d", mSongs.size() - 1);', 'sprintf(key, "song%d", (int)(mSongs.size() - 1));', 1),
    ],
    # 64-bit: string positions kept in unsigned int are compared with MBString::npos, which
    # is a 64-bit size_t here, so a failed find never matches: GetTokenList loops forever on
    # its last token, and a setting without '=' is cut at a wrapped position. The positions
    # become size_t.
    'SteamboatApp/CSBGridLockGameBoard.cpp': [
        ("unsigned int nPos = (*iSettings).find('=');", "size_t nPos = (*iSettings).find('=');", 1),
        ('unsigned int nStart = strValue.find_first_of("0123456789");', 'size_t nStart = strValue.find_first_of("0123456789");', 1),
        ('unsigned int nEnd = strValue.find_first_not_of("0123456789", nStart);', 'size_t nEnd = strValue.find_first_not_of("0123456789", nStart);', 1),
        ('unsigned int nPos = 0;', 'size_t nPos = 0;', 1),
        ('unsigned int nStart = 0;', 'size_t nStart = 0;', 1),
    ],
    'SteamboatApp/CSBLinkingLogsGameBoard.cpp': [
        ("unsigned int nPos = strToken.find('=');", "size_t nPos = strToken.find('=');", 1),
        ('unsigned int nStart = strType.find_first_of("0123456789");', 'size_t nStart = strType.find_first_of("0123456789");', 1),
        ('unsigned int nEnd = strType.find_first_not_of("0123456789", nStart);', 'size_t nEnd = strType.find_first_not_of("0123456789", nStart);', 1),
        ('unsigned int nPos = 0;', 'size_t nPos = 0;', 1),
        ('unsigned int nStart = 0;', 'size_t nStart = 0;', 1),
    ],
}
