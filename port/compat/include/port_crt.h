// port_crt.h - the Microsoft C runtime functions the source uses that the macOS C
// library lacks or names differently. Included by prelude.h, so they are there
// wherever Visual C++'s headers declared them (string.h, stdlib.h, float.h, ...).
// The simple ones map onto POSIX here; the rest are in platform/crt.cpp.
#ifndef PORT_CRT_H
#define PORT_CRT_H
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wchar.h>
#include <math.h>
#include <alloca.h>
#include <time.h>
#include <ctype.h>
#include <wctype.h>

#ifdef __cplusplus
extern "C" {
#endif

// string.h
static inline int _stricmp(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int _strcmpi(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int _strnicmp(const char *a, const char *b, size_t n) { return strncasecmp(a, b, n); }
static inline char *_strdup(const char *s) { return strdup(s); }
// the old names without the underscore
static inline int stricmp(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int strcmpi(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int strnicmp(const char *a, const char *b, size_t n) { return strncasecmp(a, b, n); }
char *_strlwr(char *s);
char *_strupr(char *s);
char *_strrev(char *s);
char *_strset(char *s, int c);
// The source's wchar_t is the compiler's (4 bytes here, 2 on Windows): text it reads
// from Windows data as UTF-16 is WCHAR (unsigned short), not wchar_t. In C++ _wcsicmp
// compares any mix of the two (below).
#ifndef __cplusplus
static inline int _wcsicmp(const wchar_t *a, const wchar_t *b) { return wcscasecmp(a, b); }
#endif

// stdio.h: Microsoft's _snprintf does not terminate a truncated string and returns -1;
// callers of the game terminate it themselves, so the C99 functions serve.
static inline int _vsnprintf(char *buf, size_t n, const char *fmt, va_list ap) {
    int r = vsnprintf(buf, n, fmt, ap);
    return r >= 0 && (size_t)r < n ? r : -1;
}
static inline int _snprintf(char *buf, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = _vsnprintf(buf, n, fmt, ap);
    va_end(ap);
    return r;
}

// stdlib.h
char *_itoa(int value, char *buf, int radix);
char *_ltoa(long value, char *buf, int radix);
static inline char *itoa(int value, char *buf, int radix) { return _itoa(value, buf, radix); }
static inline char *ltoa(long value, char *buf, int radix) { return _ltoa(value, buf, radix); }
static inline int _putenv(const char *s) { return putenv(strdup(s)); }
#define _MAX_PATH 260
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
void _makepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext);

// malloc.h
#define _alloca alloca

// stdio.h: the game's file functions on its virtual file system (platform/win32/crtfs.cpp;
// port/rewrite.py maps the source's fopen, freopen and rename calls onto them)
FILE *port_fopen(const char *path, const char *mode);
FILE *port_freopen(const char *path, const char *mode, FILE *stream);
int port_rename(const char *from, const char *to);
int port_host_path(const char *path, int for_write, char *out, size_t outsize);
static inline FILE *_fdopen(int fd, const char *mode) { return fdopen(fd, mode); }

// ctype.h: Microsoft's character class masks, for iswctype() (whose masks on macOS are
// the _CTYPE_ bits)
#define _UPPER _CTYPE_U
#define _LOWER _CTYPE_L
#define _DIGIT _CTYPE_D
#define _SPACE _CTYPE_S
#define _PUNCT _CTYPE_P
#define _CONTROL _CTYPE_C
#define _BLANK _CTYPE_B
#define _HEX _CTYPE_X
#define _ALPHA _CTYPE_A

// float.h and math.h
static inline int _isnan(double x) { return isnan(x); }
static inline int _finite(double x) { return isfinite(x); }
static inline double _copysign(double x, double y) { return copysign(x, y); }
static inline double _hypot(double x, double y) { return hypot(x, y); }

// process.h
// the result is the thread's Win32 handle (a HANDLE as uintptr_t), -1 on failure
uintptr_t _beginthread(void (*start)(void *), unsigned stack, void *arg);
void _endthread(void);

#ifdef __cplusplus
}

// The game's 16-bit characters (WCHAR, UTF-16 in Windows data): the wide string
// functions the source calls with them. The multibyte side is the "C" locale of the
// Microsoft runtime, one byte per character (Latin-1).
static inline size_t wcslen(const unsigned short *s) { size_t n = 0; while (s[n]) n++; return n; }
static inline size_t wcstombs(char *d, const unsigned short *s, size_t n)
{
    size_t i = 0;
    for (; s[i]; i++) {
        if (d) {
            if (i >= n) return i;
            d[i] = s[i] < 0x100 ? (char)s[i] : '?';
        }
    }
    if (d && i < n) d[i] = 0;
    return i;
}
static inline size_t mbstowcs(unsigned short *d, const char *s, size_t n)
{
    size_t i = 0;
    for (; s[i]; i++) {
        if (d) {
            if (i >= n) return i;
            d[i] = (unsigned char)s[i];
        }
    }
    if (d && i < n) d[i] = 0;
    return i;
}
template <class A, class B> static inline int _wcsicmp(const A *a, const B *b)
{
    for (;; a++, b++) {
        unsigned ca = *a < 0x80 ? (unsigned)tolower(*a) : *a, cb = *b < 0x80 ? (unsigned)tolower(*b) : *b;
        if (ca != cb || !ca) return (int)ca - (int)cb;
    }
}

// time_t was the 32-bit long of Windows: the source passes the address of a 32-bit
// time (its long is int in the port, see rewrite.py)
static inline struct tm *localtime(const int *t) { time_t v = *t; return localtime(&v); }
static inline struct tm *gmtime(const int *t) { time_t v = *t; return gmtime(&v); }

// VC6's <xutility> min and max (the standard ones under other names)
namespace std {
template <class T> inline const T &_cpp_min(const T &a, const T &b) { return b < a ? b : a; }
template <class T> inline const T &_cpp_max(const T &a, const T &b) { return a < b ? b : a; }
}
#endif
#endif
