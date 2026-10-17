# RuntimeLib edits for the C runtime (see port/rewrite.py, EDITS).
EDITS = {
    # the scanner names msvcrt's _iob array for stdin, stdout and stderr; here they are
    # the C library's streams, as <stdio.h> defines them
    'RuntimeLib/MfStateLexer.cpp': [
        ('#undef _iob\nextern "C" __declspec(dllimport) FILE _iob[];\n#undef stdin\n#undef stdout\n'
         '#undef stderr\n#define stdin (&_iob[0])\n#define stdout (&_iob[1])\n#define stderr (&_iob[2])\n',
         '\n' * 8, 1),
    ],
    # the planning threads' handles (from _beginthread, a uintptr_t); only stored
    'RuntimeLib/MfMsgDispatcher.h': [
        ('    unsigned long mThreads[2];                              // 0x1c\n',
         '    uintptr_t mThreads[2];                                  // 0x1c\n', 1),
    ],
}
