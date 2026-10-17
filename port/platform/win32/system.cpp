// system.cpp - the process environment (command line, startup info, environment
// strings), the Windows version, debug output, the lstr* and wsprintf string calls, and
// FormatMessage.
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

extern char **environ;

namespace win32 {

// The command line the game sees: the executable's Windows path in quotes, then the
// arguments (set by the executable's main through port_set_command_line).
static std::string s_cmdline = "\"C:\\setup\\hd\\binaries\\Steamboat.exe\"";

}  // namespace win32

using namespace win32;

extern "C" {

void port_set_command_line(int argc, char **argv)
{
    set_main_thread();
    enter_overlay();
    s_cmdline = "\"C:\\setup\\hd\\binaries\\Steamboat.exe\"";
    for (int i = 1; i < argc; i++) {
        s_cmdline += " ";
        s_cmdline += argv[i];
    }
}

LPSTR WINAPI GetCommandLineA(void) { return (LPSTR)s_cmdline.c_str(); }

void WINAPI GetStartupInfoA(LPSTARTUPINFOA si)
{
    memset(si, 0, sizeof(*si));
    si->cb = sizeof(*si);
    si->wShowWindow = SW_SHOWNORMAL;
}

// Environment blocks: NAME=value\0...\0\0.
LPSTR WINAPI GetEnvironmentStrings(void)
{
    size_t n = 1;
    for (char **e = environ; *e; e++)
        n += strlen(*e) + 1;
    char *block = (char *)malloc(n), *p = block;
    for (char **e = environ; *e; e++) {
        size_t l = strlen(*e) + 1;
        memcpy(p, *e, l);
        p += l;
    }
    *p = 0;
    return block;
}

LPWSTR WINAPI GetEnvironmentStringsW(void)
{
    LPSTR a = GetEnvironmentStrings();
    size_t n = 0;
    while (a[n] || a[n + 1])
        n++;
    n += 2;
    LPWSTR w = (LPWSTR)malloc(n * sizeof(WCHAR));
    for (size_t i = 0; i < n; i++)
        w[i] = (WCHAR)(unsigned char)a[i];
    free(a);
    return w;
}

BOOL WINAPI FreeEnvironmentStringsA(LPSTR penv)
{
    free(penv);
    return TRUE;
}

BOOL WINAPI FreeEnvironmentStringsW(LPWSTR penv)
{
    free(penv);
    return TRUE;
}

BOOL WINAPI SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue)
{
    if (lpValue)
        return setenv(lpName, lpValue, 1) == 0;
    return unsetenv(lpName) == 0;
}

DWORD WINAPI GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer, DWORD nSize)
{
    const char *v = getenv(lpName);
    if (!v) {
        SetLastError(203);  // ERROR_ENVVAR_NOT_FOUND
        return 0;
    }
    DWORD n = (DWORD)strlen(v);
    if (nSize <= n)
        return n + 1;
    strcpy(lpBuffer, v);
    return n;
}

// Windows 98 is what the game was tested on.
DWORD WINAPI GetVersion(void) { return 0xC0000A04; }

BOOL WINAPI GetVersionExA(LPOSVERSIONINFOA v)
{
    v->dwMajorVersion = 4;
    v->dwMinorVersion = 10;
    v->dwBuildNumber = 0x040A08AE;
    v->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
    strcpy(v->szCSDVersion, " A ");
    return TRUE;
}

void WINAPI OutputDebugStringA(LPCSTR s)
{
    static bool on = getenv("PORT_DEBUGSTRINGS") != 0;
    if (on && s)
        fputs(s, stderr);
    else if (s)
        log("debugstring", "%s", s);
}

BOOL WINAPI Beep(DWORD, DWORD) { return TRUE; }

LPSTR WINAPI lstrcatA(LPSTR a, LPCSTR b) { return strcat(a, b); }
int WINAPI lstrcmpA(LPCSTR a, LPCSTR b) { return CompareStringA(LOCALE_USER_DEFAULT, 0, a, -1, b, -1) - 2; }
int WINAPI lstrcmpiA(LPCSTR a, LPCSTR b) { return CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNORECASE, a, -1, b, -1) - 2; }
LPSTR WINAPI lstrcpyA(LPSTR a, LPCSTR b) { return strcpy(a, b); }
int WINAPI lstrlenA(LPCSTR s) { return s ? (int)strlen(s) : 0; }

LPSTR WINAPI lstrcpynA(LPSTR a, LPCSTR b, int n)
{
    if (n <= 0)
        return a;
    strncpy(a, b, (size_t)n - 1);
    a[n - 1] = 0;
    return a;
}

// wsprintf: printf without floating point, at most 1024 characters.
int WINAPI wvsprintfA(LPSTR buf, LPCSTR fmt, va_list ap)
{
    int n = vsnprintf(buf, 1025, fmt, ap);
    return n > 1024 ? 1024 : n;
}

int WINAPIV wsprintfA(LPSTR buf, LPCSTR fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = wvsprintfA(buf, fmt, ap);
    va_end(ap);
    return n;
}

LPSTR WINAPI CharUpperA(LPSTR s)
{
    if (IS_INTRESOURCE(s))
        return (LPSTR)(uintptr_t)toupper((int)(uintptr_t)s);
    for (char *p = s; *p; p++)
        *p = (char)toupper((unsigned char)*p);
    return s;
}

LPSTR WINAPI CharLowerA(LPSTR s)
{
    if (IS_INTRESOURCE(s))
        return (LPSTR)(uintptr_t)tolower((int)(uintptr_t)s);
    for (char *p = s; *p; p++)
        *p = (char)tolower((unsigned char)*p);
    return s;
}

LPSTR WINAPI CharNextA(LPCSTR s) { return (LPSTR)(*s ? s + 1 : s); }
LPSTR WINAPI CharPrevA(LPCSTR start, LPCSTR cur) { return (LPSTR)(cur > start ? cur - 1 : start); }

static const char *message_text(DWORD id)
{
    switch (id) {
    case ERROR_SUCCESS: return "The operation completed successfully.";
    case ERROR_FILE_NOT_FOUND: return "The system cannot find the file specified.";
    case ERROR_PATH_NOT_FOUND: return "The system cannot find the path specified.";
    case ERROR_ACCESS_DENIED: return "Access is denied.";
    case ERROR_INVALID_HANDLE: return "The handle is invalid.";
    case ERROR_NOT_ENOUGH_MEMORY: return "Not enough storage is available to process this command.";
    case ERROR_INVALID_PARAMETER: return "The parameter is incorrect.";
    case ERROR_MOD_NOT_FOUND: return "The specified module could not be found.";
    case ERROR_PROC_NOT_FOUND: return "The specified procedure could not be found.";
    }
    return 0;
}

DWORD WINAPI FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD, LPSTR lpBuffer, DWORD nSize,
                            va_list *)
{
    char text[512];
    if (dwFlags & FORMAT_MESSAGE_FROM_STRING) {
        snprintf(text, sizeof(text), "%s", (const char *)lpSource);
    } else {
        const char *m = message_text(dwMessageId);
        if (m)
            snprintf(text, sizeof(text), "%s\r\n", m);
        else
            snprintf(text, sizeof(text), "Error %u.\r\n", dwMessageId);
    }
    size_t n = strlen(text);
    if (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
        char *p = (char *)LocalAlloc(LMEM_FIXED, n + 1 > nSize ? n + 1 : nSize);
        memcpy(p, text, n + 1);
        *(LPSTR *)lpBuffer = p;
        return (DWORD)n;
    }
    if (nSize <= n) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    memcpy(lpBuffer, text, n + 1);
    return (DWORD)n;
}

}  // extern "C"
