// handle.cpp - kernel object handles, the last error, logging and DuplicateHandle.
#include "internal.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace win32 {

static thread_local DWORD t_last_error;

void log(const char *channel, const char *fmt, ...)
{
    static const char *filter = getenv("PORT_LOG");
    if (!filter || !*filter)
        return;
    if (strcmp(filter, "1") && strcmp(filter, "all") && !strstr(filter, channel))
        return;
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[%s] ", channel);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

// The three standard handles are fixed pseudo-objects; everything else is heap allocated.
struct ConsoleObject : FdObject {
    explicit ConsoleObject(int fd) : FdObject(Console, fd) { refs = 1 << 30; }
};
static ConsoleObject s_stdin(0), s_stdout(1), s_stderr(2);
static HANDLE s_std[3] = {&s_stdin, &s_stdout, &s_stderr};

// Pseudo-handles of GetCurrentProcess/GetCurrentThread.
#define PSEUDO_PROCESS ((HANDLE)(LONG_PTR)-1)
#define PSEUDO_THREAD ((HANDLE)(LONG_PTR)-2)

Object *object(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE || h == PSEUDO_THREAD) {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    return static_cast<Object *>(h);
}

HANDLE handle(Object *o) { return o; }

void retain(Object *o) { o->refs.fetch_add(1); }

void release(Object *o)
{
    if (o->refs.fetch_sub(1) == 1)
        delete o;
}

DWORD error_from_errno(int e)
{
    switch (e) {
    case 0: return ERROR_SUCCESS;
    case ENOENT: return ERROR_FILE_NOT_FOUND;
    case ENOTDIR: return ERROR_PATH_NOT_FOUND;
    case EACCES:
    case EPERM:
    case EISDIR: return ERROR_ACCESS_DENIED;
    case EEXIST: return ERROR_FILE_EXISTS;
    case EMFILE:
    case ENFILE: return ERROR_TOO_MANY_OPEN_FILES;
    case ENOMEM: return ERROR_NOT_ENOUGH_MEMORY;
    case EBADF: return ERROR_INVALID_HANDLE;
    case EINVAL: return ERROR_INVALID_PARAMETER;
    case EROFS: return ERROR_WRITE_PROTECT;
    case EPIPE: return ERROR_BROKEN_PIPE;
    case ENOTEMPTY: return 145;  // ERROR_DIR_NOT_EMPTY
    default: return ERROR_INVALID_FUNCTION;
    }
}

}  // namespace win32

using namespace win32;

extern "C" {

DWORD WINAPI GetLastError(void) { return t_last_error; }
void WINAPI SetLastError(DWORD dwErrCode) { t_last_error = dwErrCode; }

BOOL WINAPI CloseHandle(HANDLE h)
{
    if (h == PSEUDO_PROCESS || h == PSEUDO_THREAD)
        return TRUE;
    Object *o = object(h);
    if (!o)
        return FALSE;
    if (o->kind != Object::Console)
        release(o);
    return TRUE;
}

BOOL WINAPI DuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle,
                            LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    (void)hSourceProcessHandle;
    (void)hTargetProcessHandle;
    (void)dwDesiredAccess;
    (void)bInheritHandle;
    // The pseudo-handle of the current thread becomes a real handle (thread.cpp).
    Object *o = hSourceHandle == PSEUDO_THREAD ? object(thread_self_handle()) : object(hSourceHandle);
    if (!o)
        return FALSE;
    if (hSourceHandle != PSEUDO_THREAD)
        retain(o);
    if (lpTargetHandle)
        *lpTargetHandle = handle(o);
    if (dwOptions & DUPLICATE_CLOSE_SOURCE)
        CloseHandle(hSourceHandle);
    return TRUE;
}

HANDLE WINAPI GetStdHandle(DWORD nStdHandle)
{
    switch (nStdHandle) {
    case STD_INPUT_HANDLE: return s_std[0];
    case STD_OUTPUT_HANDLE: return s_std[1];
    case STD_ERROR_HANDLE: return s_std[2];
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return INVALID_HANDLE_VALUE;
}

BOOL WINAPI SetStdHandle(DWORD nStdHandle, HANDLE hHandle)
{
    switch (nStdHandle) {
    case STD_INPUT_HANDLE: s_std[0] = hHandle; return TRUE;
    case STD_OUTPUT_HANDLE: s_std[1] = hHandle; return TRUE;
    case STD_ERROR_HANDLE: s_std[2] = hHandle; return TRUE;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}

HANDLE WINAPI GetCurrentProcess(void) { return PSEUDO_PROCESS; }
HANDLE WINAPI GetCurrentThread(void) { return PSEUDO_THREAD; }

}  // extern "C"
