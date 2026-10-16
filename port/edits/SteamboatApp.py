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
    # The cursor thread: Terminate waits on the result of _beginthread, which Visual C++
    # returned as the thread's handle; here _beginthread gives a thread number. The thread
    # is made with CreateThread (a handle Terminate can wait on and closes), and it leaves
    # through ExitThread, which marks the handle signalled (_endthread would skip that).
    'SteamboatApp/CMBCursor.cpp': [
        ('        sThread = (void *)_beginthread(CursorThread, 0, 0);\n',
         '        sThread = CreateThread(0, 0, [](LPVOID p) -> DWORD { CursorThread(p); return 0; }, 0, 0, 0);\n', 1),
        ('    _endthread();\n', '    ExitThread(0);\n', 1),
        ('        WaitForSingleObject(sThread, INFINITE);\n',
         '        WaitForSingleObject(sThread, INFINITE); CloseHandle(sThread);\n', 1),
    ],
    # "SBSPbirdHigh" and a letter are 14 bytes with the terminator, in a 10-byte buffer (the
    # original overran its stack too; the macOS fortified sprintf aborts). The song key's
    # count is a size_t (8 bytes) printed with %d.
    'SteamboatApp/SBSongbirdNode.cpp': [
        ('        char name[10];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n',
         '        char name[16];\n        sprintf(name, "SBSPbirdHigh%c", \'C\');\n', 1),
        ('sprintf(key, "song%d", mSongs.size() - 1);', 'sprintf(key, "song%d", (int)(mSongs.size() - 1));', 1),
    ],
}
