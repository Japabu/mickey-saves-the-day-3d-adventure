// internal.h - shared pieces of the Win32 layer of the macOS port: kernel objects
// behind HANDLEs, the thread's last error, logging, and the path mapping.
#ifndef PORT_WIN32_INTERNAL_H
#define PORT_WIN32_INTERNAL_H

#include <windows.h>
#include <atomic>
#include <string>

namespace win32 {

// Diagnostics: PORT_LOG=1 (or a list of channels) in the environment turns them on.
void log(const char *channel, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#define PORT_STUB(name) win32::log("stub", "%s is not implemented\n", name)

// A kernel object. A HANDLE is a pointer to one; CloseHandle drops a reference.
struct Object {
    enum Kind { File, Find, Event, Mutex, Semaphore, Thread, Process, Mapping, Heap, Console };
    explicit Object(Kind k) : kind(k), refs(1) {}
    virtual ~Object() {}
    // Waitable objects: wait until signalled (true) or timeout (false).
    virtual bool wait(DWORD ms) { (void)ms; return true; }
    Kind kind;
    std::atomic<int> refs;
};

// Objects with a POSIX file descriptor: files and the console streams.
struct FdObject : Object {
    FdObject(Kind k, int fd) : Object(k), fd(fd) {}
    ~FdObject();
    int fd;
};

// A real handle for the calling thread (the pseudo-handle of GetCurrentThread
// duplicated), made on first use.
HANDLE thread_self_handle();

// The object behind a handle, if it is of the given kind (0 otherwise; the last error is
// set to ERROR_INVALID_HANDLE).
Object *object(HANDLE h);
template <class T> T *object_as(HANDLE h, Object::Kind kind)
{
    Object *o = object(h);
    if (!o || o->kind != kind) {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    return static_cast<T *>(o);
}
HANDLE handle(Object *o);
void retain(Object *o);
void release(Object *o);

// errno to a Win32 error code.
DWORD error_from_errno(int e);

// Paths: see vfs.h. The overlay directory (~/Library/Application Support/<game>/), created on first use.
const std::string &overlay_dir();
// The game's current directory, as a Windows path ending in a backslash.
std::string current_dir();
// A full Windows path for a relative one (current directory, drive, . and ..).
std::string full_path(const char *winpath);
bool set_current_dir(const char *winpath);
// The directory the process was started in (its own working directory is then the
// overlay, crtfs.cpp).
std::string &launch_dir();
// Makes the overlay the process's working directory (crtfs.cpp).
void enter_overlay();

// True if the calling thread's message queue (or the host's events) has input
// (message.cpp).
bool messages_pending();

// The game's main thread: the one that started it (port_set_command_line); the window,
// its frames and the host's events belong to it (thread.cpp).
void set_main_thread();
bool on_main_thread();

// Milliseconds since the process started (GetTickCount, timeGetTime).
DWORD ticks();
void advance_frame();   // PORT_DETERMINISTIC: one pass of the game loop (time.cpp)
bool virtual_clock();

}  // namespace win32

#endif
