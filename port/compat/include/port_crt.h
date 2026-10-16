// port_crt.h - the Microsoft C runtime functions the source uses that the macOS C
// library lacks or names differently. Included by prelude.h, so they are there
// wherever Visual C++'s headers declared them (string.h, stdlib.h, float.h, ...).
// The simple ones map onto POSIX here; the rest are in platform/crt.cpp.
#ifndef PORT_CRT_H
#define PORT_CRT_H
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wchar.h>
#include <math.h>
#include <alloca.h>

#ifdef __cplusplus
extern "C" {
#endif

// string.h
static inline int _stricmp(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int _strcmpi(const char *a, const char *b) { return strcasecmp(a, b); }
static inline int _strnicmp(const char *a, const char *b, size_t n) { return strncasecmp(a, b, n); }
static inline char *_strdup(const char *s) { return strdup(s); }
char *_strlwr(char *s);
char *_strupr(char *s);
char *_strrev(char *s);
char *_strset(char *s, int c);
// The source's wchar_t is the compiler's (4 bytes here, 2 on Windows): text it reads
// from Windows data as UTF-16 is WCHAR (unsigned short), not wchar_t.
static inline int _wcsicmp(const wchar_t *a, const wchar_t *b) { return wcscasecmp(a, b); }

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

// float.h and math.h
static inline int _isnan(double x) { return isnan(x); }
static inline int _finite(double x) { return isfinite(x); }
static inline double _copysign(double x, double y) { return copysign(x, y); }
static inline double _hypot(double x, double y) { return hypot(x, y); }

// process.h
unsigned long _beginthread(void (*start)(void *), unsigned stack, void *arg);
void _endthread(void);

#ifdef __cplusplus
}
#endif
#endif
