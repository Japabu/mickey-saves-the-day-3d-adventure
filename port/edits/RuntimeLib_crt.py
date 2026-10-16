# RuntimeLib edits for the C runtime (see port/rewrite.py, EDITS).
EDITS = {
    # the scanner names msvcrt's _iob array for stdin, stdout and stderr; here they are
    # the C library's streams
    'RuntimeLib/MfStateLexer.cpp': [
        ('#define stdin (&_iob[0])\n#define stdout (&_iob[1])\n#define stderr (&_iob[2])\n',
         '#define stdin __stdinp\n#define stdout __stdoutp\n#define stderr __stderrp\n', 1),
    ],
    # the planning threads' handles (from _beginthread, a uintptr_t); only stored
    'RuntimeLib/MfMsgDispatcher.h': [
        ('    unsigned long mThreads[2];                              // 0x1c\n',
         '    uintptr_t mThreads[2];                                  // 0x1c\n', 1),
    ],
}
