// crash.cpp - what a crash prints to the terminal: the signal and faulting address, the
// thread, and a backtrace with each frame's binary and symbol (the game is one executable:
// the symbol says which module). Also for an uncaught C++ exception and for abort(). The
// player's report of this output is our runtime data.
#include <cxxabi.h>
#include <dlfcn.h>
#include <exception>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include <sys/ucontext.h>
#include <unistd.h>

#pragma GCC visibility push(hidden)

namespace {

const char *base_name(const char *path)
{
    const char *s = path ? strrchr(path, '/') : 0;
    return s ? s + 1 : (path ? path : "?");
}

void print_frames(void **frames, int n, int skip)
{
    for (int i = skip; i < n; i++) {
        Dl_info info;
        memset(&info, 0, sizeof(info));
        uintptr_t pc = (uintptr_t)frames[i];
        if (dladdr(frames[i], &info) && info.dli_fname) {
            const char *sym = info.dli_sname;
            int status = -1;
            char *demangled = sym ? abi::__cxa_demangle(sym, 0, 0, &status) : 0;
            uintptr_t off_sym = info.dli_saddr ? pc - (uintptr_t)info.dli_saddr : 0;
            uintptr_t off_lib = pc - (uintptr_t)info.dli_fbase;
            fprintf(stderr, "  #%-2d %-24s +0x%-8lx %s + %lu\n", i - skip, base_name(info.dli_fname), (unsigned long)off_lib,
                    demangled ? demangled : (sym ? sym : "?"), (unsigned long)off_sym);
            free(demangled);
        } else {
            fprintf(stderr, "  #%-2d ?                        0x%lx\n", i - skip, (unsigned long)pc);
        }
    }
}

void print_backtrace(int skip)
{
    void *frames[128];
    int n = backtrace(frames, 128);
    print_frames(frames, n, skip);
}

const char *signal_name(int sig)
{
    switch (sig) {
    case SIGSEGV: return "SIGSEGV (invalid memory access)";
    case SIGBUS: return "SIGBUS (misaligned or unmapped memory)";
    case SIGILL: return "SIGILL (illegal instruction)";
    case SIGFPE: return "SIGFPE (arithmetic error)";
    case SIGABRT: return "SIGABRT (abort)";
    case SIGTRAP: return "SIGTRAP (trap)";
    }
    return "signal";
}

volatile sig_atomic_t s_in_handler;

void on_signal(int sig, siginfo_t *si, void *uc)
{
    if (s_in_handler++) {
        // A crash inside the report: give up quietly.
        signal(sig, SIG_DFL);
        raise(sig);
        return;
    }
    uint64_t tid = 0;
    pthread_threadid_np(0, &tid);
    fprintf(stderr, "\n==== Mickey Saves the Day (macOS port) crashed ====\n");
    fprintf(stderr, "%s at address %p, thread %llu%s\n", signal_name(sig), si ? si->si_addr : 0, (unsigned long long)tid,
            pthread_main_np() ? " (main)" : "");
    fprintf(stderr, "Backtrace (library, offset in library, function):\n");
    // The faulting instruction and its caller (from the registers), then the stack walk
    // above the signal frame.
    if (uc) {
        ucontext_t *ctx = (ucontext_t *)uc;
        void *at[2] = {(void *)ctx->uc_mcontext->__ss.__pc, (void *)ctx->uc_mcontext->__ss.__lr};
        fprintf(stderr, "  at:\n");
        print_frames(at, 2, 0);
        fprintf(stderr, "  stack:\n");
    }
    print_backtrace(2);
    fprintf(stderr, "==== please send this text with your report ====\n");
    fflush(stderr);
    signal(sig, SIG_DFL);
    raise(sig);
}

void on_terminate()
{
    fprintf(stderr, "\n==== Mickey Saves the Day (macOS port): uncaught exception ====\n");
    if (std::type_info *t = abi::__cxa_current_exception_type()) {
        int status = -1;
        char *name = abi::__cxa_demangle(t->name(), 0, 0, &status);
        fprintf(stderr, "exception type: %s\n", name ? name : t->name());
        free(name);
    }
    fprintf(stderr, "Backtrace:\n");
    print_backtrace(1);
    fprintf(stderr, "==== please send this text with your report ====\n");
    fflush(stderr);
    signal(SIGABRT, SIG_DFL);
    abort();
}

__attribute__((constructor)) void install_crash_handler()
{
    if (getenv("PORT_NO_CRASH_HANDLER"))
        return;
    // An alternate stack, so a stack overflow can still be reported.
    static char altstack[1 << 16];
    stack_t ss;
    ss.ss_sp = altstack;
    ss.ss_size = sizeof(altstack);
    ss.ss_flags = 0;
    sigaltstack(&ss, 0);
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = on_signal;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    const int sigs[] = {SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGABRT, SIGTRAP};
    for (int s : sigs)
        sigaction(s, &sa, 0);
    std::set_terminate(on_terminate);
}

}  // namespace
