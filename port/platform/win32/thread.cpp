// thread.cpp - threads on pthreads, processes, and the exception calls.
#include "internal.h"
#include "sync.h"

#include <atomic>
#include <pthread.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

namespace win32 {

struct ThreadObject : Waitable {
    ThreadObject() : Waitable(Thread) {}
    bool signalled() override { return done; }
    pthread_t thread = 0;
    DWORD id = 0;
    LPTHREAD_START_ROUTINE start = 0;
    LPVOID param = 0;
    bool suspended = false;  // created suspended, not resumed yet
    bool done = false;
    DWORD exit_code = STILL_ACTIVE;
};

static thread_local ThreadObject *t_self;

// What ExitThread unwinds with.
struct ThreadExit {
    DWORD code;
};

// Thread ids: numbered as threads first need one (CreateThread numbers its thread
// before it runs).
static std::atomic<DWORD> s_next_id{1};
static thread_local DWORD t_id;

static DWORD current_thread_id()
{
    if (!t_id)
        t_id = s_next_id++;
    return t_id;
}

static void *thread_main(void *arg)
{
    ThreadObject *t = (ThreadObject *)arg;
    t_self = t;
    t_id = t->id;
    pthread_mutex_lock(&g_wait_lock);
    while (t->suspended)
        pthread_cond_wait(&g_wait_cond, &g_wait_lock);
    pthread_mutex_unlock(&g_wait_lock);
    DWORD code;
    try {
        code = t->start(t->param);
    } catch (ThreadExit &e) {
        code = e.code;
    }
    pthread_mutex_lock(&g_wait_lock);
    t->exit_code = code;
    t->done = true;
    signal_all();
    pthread_mutex_unlock(&g_wait_lock);
    release(t);  // the thread's own reference
    return 0;
}

HANDLE thread_self_handle()
{
    if (!t_self) {
        // A thread the platform did not start (the main thread, SDL's threads).
        t_self = new ThreadObject;
        t_self->thread = pthread_self();
        t_self->id = current_thread_id();
        t_self->refs = 1 << 30;
    }
    retain(t_self);
    return handle(t_self);
}

// A raised exception, caught by the prelude's __try/__except (catch (...)).
struct RaisedException {
    EXCEPTION_RECORD record;
};

static LPTOP_LEVEL_EXCEPTION_FILTER s_filter;

}  // namespace win32

using namespace win32;

extern "C" {

HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
                           LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
    ThreadObject *t = new ThreadObject;
    t->start = lpStartAddress;
    t->param = lpParameter;
    t->suspended = (dwCreationFlags & CREATE_SUSPENDED) != 0;
    retain(t);  // one for the handle, one for the running thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, dwStackSize > (1 << 20) ? dwStackSize : (8 << 20));
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    t->id = s_next_id++;
    int err = pthread_create(&t->thread, &attr, thread_main, t);
    pthread_attr_destroy(&attr);
    if (err) {
        release(t);
        release(t);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    if (lpThreadId)
        *lpThreadId = t->id;
    return handle(t);
}

DWORD WINAPI ResumeThread(HANDLE hThread)
{
    ThreadObject *t = object_as<ThreadObject>(hThread, Object::Thread);
    if (!t)
        return (DWORD)-1;
    pthread_mutex_lock(&g_wait_lock);
    DWORD was = t->suspended ? 1 : 0;
    t->suspended = false;
    signal_all();
    pthread_mutex_unlock(&g_wait_lock);
    return was;
}

DWORD WINAPI SuspendThread(HANDLE hThread)
{
    // Stopping a running thread at an arbitrary point is not possible with pthreads.
    (void)hThread;
    PORT_STUB("SuspendThread");
    return 0;
}

void WINAPI ExitThread(DWORD dwExitCode)
{
    if (t_self && t_self->start)
        throw ThreadExit{dwExitCode};
    pthread_exit(0);
}

DWORD WINAPI GetCurrentThreadId(void) { return current_thread_id(); }
DWORD WINAPI GetCurrentProcessId(void) { return (DWORD)getpid(); }

BOOL WINAPI SetThreadPriority(HANDLE, int) { return TRUE; }

BOOL WINAPI GetThreadContext(HANDLE, LPCONTEXT lpContext)
{
    memset(lpContext, 0, sizeof(*lpContext));
    return FALSE;
}

// Processes.
struct ProcessObject : Waitable {
    ProcessObject() : Waitable(Process) {}
    bool signalled() override
    {
        if (!done) {
            int status;
            if (waitpid(pid, &status, WNOHANG) == pid) {
                done = true;
                code = WIFEXITED(status) ? (DWORD)WEXITSTATUS(status) : 1;
            }
        }
        return done;
    }
    bool wait(DWORD ms) override
    {
        // A child's exit is not broadcast: poll.
        DWORD start = ticks();
        for (;;) {
            pthread_mutex_lock(&g_wait_lock);
            bool s = signalled();
            pthread_mutex_unlock(&g_wait_lock);
            if (s)
                return true;
            if (ms != INFINITE && ticks() - start >= ms)
                return false;
            Sleep(10);
        }
    }
    pid_t pid = 0;
    bool done = false;
    DWORD code = STILL_ACTIVE;
};

BOOL WINAPI CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES,
                           LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA,
                           LPPROCESS_INFORMATION lpProcessInformation)
{
    // The game starts Windows programs (its own installer, a browser); there is nothing
    // to run them with. Report the attempt and fail.
    log("process", "CreateProcess(%s, %s) refused\n", lpApplicationName ? lpApplicationName : "-",
        lpCommandLine ? lpCommandLine : "-");
    if (lpProcessInformation)
        memset(lpProcessInformation, 0, sizeof(*lpProcessInformation));
    SetLastError(ERROR_FILE_NOT_FOUND);
    return FALSE;
}

HANDLE WINAPI OpenProcess(DWORD, BOOL, DWORD dwProcessId)
{
    if (dwProcessId == (DWORD)getpid())
        return GetCurrentProcess();
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}

BOOL WINAPI GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode)
{
    if (hProcess == GetCurrentProcess()) {
        *lpExitCode = STILL_ACTIVE;
        return TRUE;
    }
    ProcessObject *p = object_as<ProcessObject>(hProcess, Object::Process);
    if (!p)
        return FALSE;
    pthread_mutex_lock(&g_wait_lock);
    p->signalled();
    *lpExitCode = p->code;
    pthread_mutex_unlock(&g_wait_lock);
    return TRUE;
}

BOOL WINAPI TerminateProcess(HANDLE hProcess, UINT uExitCode)
{
    if (hProcess == GetCurrentProcess())
        _exit((int)uExitCode);
    ProcessObject *p = object_as<ProcessObject>(hProcess, Object::Process);
    if (!p)
        return FALSE;
    kill(p->pid, SIGTERM);
    return TRUE;
}

void WINAPI ExitProcess(UINT uExitCode)
{
    fflush(0);
    exit((int)uExitCode);
}

void WINAPI FatalAppExitA(UINT, LPCSTR lpMessageText)
{
    fprintf(stderr, "fatal: %s\n", lpMessageText ? lpMessageText : "");
    MessageBoxA(0, lpMessageText, "Fatal error", MB_OK | MB_ICONSTOP);
    _exit(1);
}

// Exceptions. RaiseException throws; the prelude turns __except into catch (...).
void WINAPI RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                           const ULONG_PTR *lpArguments)
{
    RaisedException e;
    memset(&e, 0, sizeof(e));
    e.record.ExceptionCode = dwExceptionCode;
    e.record.ExceptionFlags = dwExceptionFlags;
    if (nNumberOfArguments > EXCEPTION_MAXIMUM_PARAMETERS)
        nNumberOfArguments = EXCEPTION_MAXIMUM_PARAMETERS;
    e.record.NumberParameters = nNumberOfArguments;
    for (DWORD i = 0; i < nNumberOfArguments; i++)
        e.record.ExceptionInformation[i] = lpArguments[i];
    log("exception", "RaiseException(0x%08X)\n", dwExceptionCode);
    throw e;
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER f)
{
    LPTOP_LEVEL_EXCEPTION_FILTER old = s_filter;
    s_filter = f;
    return old;
}

LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

void WINAPI RtlUnwind(PVOID, PVOID, PEXCEPTION_RECORD, PVOID) {}

void WINAPI DebugBreak(void)
{
    if (getenv("PORT_DEBUGBREAK"))
        __builtin_debugtrap();
    log("debug", "DebugBreak\n");
}

}  // extern "C"
