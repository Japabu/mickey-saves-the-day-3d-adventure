# zlib edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# zlib/zconf.h: TARGET_OS_MAC (set by the macOS SDK) means Classic Mac OS to zlib (Byte from MacTypes.h); the port defines Byte itself.
EDITS['zlib/zconf.h'] = [
    ('#endif\n'
     '\n'
     '#if !defined(MACOS) && !defined(TARGET_OS_MAC)\n'
     'typedef unsigned char  Byte;  /* 8 bits */\n'
     '#endif\n'
     'typedef unsigned int   uInt;  /* 16 bits or more */\n'
     'typedef unsigned long  uLong; /* 32 bits or more */\n'
     '\n'
     '#ifdef SMALL_MEDIUM\n',
     '#endif\n'
     '\n'
     '#if !defined(MACOS)\n'
     'typedef unsigned char  Byte;  /* 8 bits */\n'
     '#endif\n'
     'typedef unsigned int   uInt;  /* 16 bits or more */\n'
     'typedef unsigned int  uLong; /* 32 bits or more */\n'
     '\n'
     '#ifdef SMALL_MEDIUM\n', 1),
    ('#endif\n'
     '#ifndef z_off_t\n'
     '#  define  z_off_t long\n'
     '#endif\n'
     '\n',
     '#endif\n'
     '#ifndef z_off_t\n'
     '#  define  z_off_t int\n'
     '#endif\n'
     '\n', 1),
]
