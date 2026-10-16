// crt.cpp - the Microsoft C runtime functions of compat/include/port_crt.h and eh.h
// that are more than a renamed POSIX call.
#include <port_crt.h>
#include <eh.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

char *_strlwr(char *s)
{
    for (char *p = s; *p; p++)
        *p = (char)tolower((unsigned char)*p);
    return s;
}

char *_strupr(char *s)
{
    for (char *p = s; *p; p++)
        *p = (char)toupper((unsigned char)*p);
    return s;
}

char *_strrev(char *s)
{
    size_t n = strlen(s);
    for (size_t i = 0; i < n / 2; i++) {
        char c = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = c;
    }
    return s;
}

char *_strset(char *s, int c)
{
    for (char *p = s; *p; p++)
        *p = (char)c;
    return s;
}

// Microsoft's _itoa/_ltoa: base 2..36, lower-case digits, a minus sign only in base 10
// (other bases show the 32-bit two's complement).
static char *ToText(unsigned long value, int negative, char *buf, int radix)
{
    char tmp[40];
    int n = 0;
    if (radix < 2 || radix > 36) {
        buf[0] = 0;
        return buf;
    }
    do {
        int d = (int)(value % (unsigned)radix);
        tmp[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        value /= (unsigned)radix;
    } while (value);
    char *p = buf;
    if (negative)
        *p++ = '-';
    while (n)
        *p++ = tmp[--n];
    *p = 0;
    return buf;
}

char *_itoa(int value, char *buf, int radix)
{
    if (radix == 10 && value < 0)
        return ToText((unsigned long)-(long)value, 1, buf, radix);
    return ToText((unsigned int)value, 0, buf, radix);
}

char *_ltoa(long value, char *buf, int radix)
{
    // the game's long is the 32-bit Windows long
    return _itoa((int)value, buf, radix);
}

// _splitpath: drive "X:", directory up to the last separator (with it), file name, and
// extension from the last dot of the name (with the dot). Both separators are accepted.
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
    const char *p = path;
    if (p[0] && p[1] == ':') {
        if (drive) {
            drive[0] = p[0];
            drive[1] = ':';
            drive[2] = 0;
        }
        p += 2;
    } else if (drive)
        drive[0] = 0;
    const char *lastSep = 0;
    for (const char *q = p; *q; q++)
        if (*q == '\\' || *q == '/')
            lastSep = q;
    const char *name = lastSep ? lastSep + 1 : p;
    if (dir) {
        size_t n = (size_t)(name - p);
        if (n > _MAX_DIR - 1)
            n = _MAX_DIR - 1;
        memcpy(dir, p, n);
        dir[n] = 0;
    }
    const char *dot = 0;
    for (const char *q = name; *q; q++)
        if (*q == '.')
            dot = q;
    const char *end = name + strlen(name);
    if (!dot)
        dot = end;
    if (fname) {
        size_t n = (size_t)(dot - name);
        if (n > _MAX_FNAME - 1)
            n = _MAX_FNAME - 1;
        memcpy(fname, name, n);
        fname[n] = 0;
    }
    if (ext) {
        size_t n = (size_t)(end - dot);
        if (n > _MAX_EXT - 1)
            n = _MAX_EXT - 1;
        memcpy(ext, dot, n);
        ext[n] = 0;
    }
}

// _makepath: the inverse; a separator is added after a directory without one and a dot
// before an extension without one.
void _makepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext)
{
    path[0] = 0;
    if (drive && *drive) {
        path[0] = drive[0];
        path[1] = ':';
        path[2] = 0;
    }
    if (dir && *dir) {
        strcat(path, dir);
        char last = dir[strlen(dir) - 1];
        if (last != '\\' && last != '/')
            strcat(path, "\\");
    }
    if (fname)
        strcat(path, fname);
    if (ext && *ext) {
        if (*ext != '.')
            strcat(path, ".");
        strcat(path, ext);
    }
}

// _beginthread: a detached thread; the result is -1 on failure.
struct ThreadStart {
    void (*start)(void *);
    void *arg;
};

static void *RunThread(void *p)
{
    ThreadStart s = *(ThreadStart *)p;
    free(p);
    s.start(s.arg);
    return 0;
}

unsigned long _beginthread(void (*start)(void *), unsigned stack, void *arg)
{
    ThreadStart *s = (ThreadStart *)malloc(sizeof *s);
    s->start = start;
    s->arg = arg;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (stack)
        pthread_attr_setstacksize(&attr, stack < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack);
    pthread_t thread;
    int err = pthread_create(&thread, &attr, RunThread, s);
    pthread_attr_destroy(&attr);
    if (err) {
        free(s);
        return (unsigned long)-1;
    }
    return (unsigned long)thread;
}

void _endthread(void)
{
    pthread_exit(0);
}

} // extern "C"

// There are no structured exceptions to translate: the translator is only kept.
static _se_translator_function gTranslator;

_se_translator_function _set_se_translator(_se_translator_function f)
{
    _se_translator_function old = gTranslator;
    gTranslator = f;
    return old;
}
