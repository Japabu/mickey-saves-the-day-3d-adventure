// module.cpp - the module table: every original DLL is a dylib (lib<Name>.dylib next to
// the executable). Libraries the executable links load with it and register through
// their attach stub (port_module_attach); LoadLibrary finds a registered one by name or
// dlopens it. GetProcAddress looks up the exported C name, also for the decorated
// stdcall spelling (_Name@N) the game asks for.
#include "internal.h"
#include "../port.h"

#include <dirent.h>
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mutex>
#include <string.h>
#include <strings.h>
#include <vector>

namespace win32 {

struct Module {
    std::string dll;      // "MBEngine.dll", as Windows named it
    std::string winpath;  // the path the game loaded it by (GetModuleFileName)
    void *dl = 0;         // dlopen handle (or null for the executable's own libraries)
    int refs = 1;
};

static std::recursive_mutex s_mu;
static std::vector<Module *> &modules()
{
    static std::vector<Module *> v;
    return v;
}
static Module *s_exe;

static std::string base_name(const std::string &path)
{
    size_t s = path.find_last_of("\\/");
    std::string b = s == std::string::npos ? path : path.substr(s + 1);
    return b;
}

static std::string strip_ext(const std::string &name)
{
    size_t dot = name.rfind('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

static Module *find_module(const std::string &dllname)
{
    std::string want = strip_ext(base_name(dllname));
    for (Module *m : modules())
        if (!strcasecmp(strip_ext(m->dll).c_str(), want.c_str()))
            return m;
    return 0;
}

// The directory with the executable and the dylibs.
static std::string lib_dir()
{
    if (const char *e = getenv("PORT_LIBDIR"))
        return e;
    char buf[4096];
    uint32_t n = sizeof(buf);
    if (_NSGetExecutablePath(buf, &n) != 0)
        return ".";
    std::string p = buf;
    size_t s = p.rfind('/');
    return s == std::string::npos ? "." : p.substr(0, s);
}

// lib<name>.dylib in dir, matched without regard to case.
static std::string find_dylib(const std::string &dir, const std::string &name)
{
    std::string want = "lib" + name + ".dylib";
    DIR *d = opendir(dir.c_str());
    if (!d)
        return "";
    std::string found;
    while (struct dirent *e = readdir(d))
        if (!strcasecmp(e->d_name, want.c_str())) {
            found = dir + "/" + e->d_name;
            break;
        }
    closedir(d);
    return found;
}

static std::string default_winpath(const std::string &dll)
{
    return "C:\\setup\\hd\\binaries\\" + dll;
}

// The Windows system DLLs the game looks up (ddraw.dll for DirectDrawEnumerateExA, ...)
// are the platform library itself.
static Module *system_module(const std::string &dll)
{
    static const char *names[] = {"ddraw", "d3dim", "d3dim700", "dinput", "dsound", "kernel32", "user32", "gdi32",
                                  "advapi32", "winmm", "shell32", "ole32", "comctl32", "comdlg32", "imm32"};
    std::string want = strip_ext(dll);
    for (const char *n : names) {
        if (strcasecmp(n, want.c_str()))
            continue;
        Module *m = new Module;
        m->dll = dll;
        m->winpath = "C:\\WINDOWS\\SYSTEM\\" + dll;
        Dl_info info;
        if (dladdr((const void *)&system_module, &info) && info.dli_fname)
            m->dl = dlopen(info.dli_fname, RTLD_NOLOAD | RTLD_LAZY);
        modules().push_back(m);
        return m;
    }
    return 0;
}

}  // namespace win32

using namespace win32;

extern "C" {

HINSTANCE port_module_attach(const char *dllname, const void *anchor)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    Module *m = find_module(dllname);
    if (!m) {
        m = new Module;
        m->dll = dllname;
        m->winpath = default_winpath(dllname);
        Dl_info info;
        if (dladdr(anchor, &info) && info.dli_fname)
            m->dl = dlopen(info.dli_fname, RTLD_NOLOAD | RTLD_LAZY);
        modules().push_back(m);
        log("module", "attach %s\n", dllname);
    }
    const char *dot = strrchr(dllname, '.');
    if (dot && !strcasecmp(dot, ".exe"))
        s_exe = m;
    return (HINSTANCE)m;
}

HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName)
{
    if (!lpLibFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    std::string dll = base_name(lpLibFileName);
    if (dll.find('.') == std::string::npos)
        dll += ".dll";
    if (Module *m = find_module(dll)) {
        m->refs++;
        return (HMODULE)m;
    }
    if (Module *m = system_module(dll))
        return (HMODULE)m;
    std::string name = strip_ext(dll);
    std::string path = find_dylib(lib_dir(), name);
    if (path.empty())
        path = find_dylib(lib_dir() + "/plugins", name);
    if (path.empty()) {
        log("module", "LoadLibrary(%s): no lib%s.dylib\n", lpLibFileName, name.c_str());
        SetLastError(ERROR_MOD_NOT_FOUND);
        return 0;
    }
    // dlopen runs the library's attach stub, which registers it (and calls DllMain).
    void *dl = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!dl) {
        log("module", "LoadLibrary(%s): %s\n", lpLibFileName, dlerror());
        SetLastError(ERROR_MOD_NOT_FOUND);
        return 0;
    }
    Module *m = find_module(dll);
    if (!m) {
        // A library without the stub (should not happen): register it here.
        m = new Module;
        m->dll = dll;
        modules().push_back(m);
    }
    m->dl = dl;
    m->winpath = full_path(lpLibFileName);
    if (strchr(lpLibFileName, '\\') == 0 && strchr(lpLibFileName, '/') == 0)
        m->winpath = default_winpath(dll);
    log("module", "LoadLibrary(%s) -> %s\n", lpLibFileName, path.c_str());
    return (HMODULE)m;
}

BOOL WINAPI FreeLibrary(HMODULE hLibModule)
{
    // Libraries stay loaded: the game frees its game DLL right after loading it and keeps
    // using it through the COM server table, which a real unload would break.
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    Module *m = (Module *)hLibModule;
    if (m && m->refs > 1)
        m->refs--;
    return TRUE;
}

// A .def file let a DLL export a C++ function under its plain name (MBEngine's
// MBComRegisterServer, really ?MBComRegisterServer@@YA_NXZ). Here it only exists with its
// Itanium name, _Z<len><name><parameters>: find the free function with exactly that
// name among the module's own symbols. Several overloads would be ambiguous; the first wins.
static void *find_cxx_free_function(const Module *m, const char *name)
{
    std::string lib = "lib" + strip_ext(m->dll) + ".dylib";
    std::string prefix = "__Z" + std::to_string(strlen(name)) + name;
    for (uint32_t i = 0; i < _dyld_image_count(); i++) {
        const char *path = _dyld_get_image_name(i);
        const char *slash = strrchr(path, '/');
        if (strcasecmp(slash ? slash + 1 : path, lib.c_str()) != 0)
            continue;
        const mach_header_64 *mh = (const mach_header_64 *)_dyld_get_image_header(i);
        intptr_t slide = _dyld_get_image_vmaddr_slide(i);
        const symtab_command *symtab = 0;
        const segment_command_64 *linkedit = 0;
        const load_command *lc = (const load_command *)(mh + 1);
        for (uint32_t c = 0; c < mh->ncmds; c++, lc = (const load_command *)((const char *)lc + lc->cmdsize)) {
            if (lc->cmd == LC_SYMTAB)
                symtab = (const symtab_command *)lc;
            else if (lc->cmd == LC_SEGMENT_64 && !strcmp(((const segment_command_64 *)lc)->segname, SEG_LINKEDIT))
                linkedit = (const segment_command_64 *)lc;
        }
        if (!symtab || !linkedit)
            return 0;
        const char *base = (const char *)(slide + linkedit->vmaddr - linkedit->fileoff);
        const nlist_64 *syms = (const nlist_64 *)(base + symtab->symoff);
        const char *strs = base + symtab->stroff;
        for (uint32_t k = 0; k < symtab->nsyms; k++) {
            const nlist_64 &s = syms[k];
            if ((s.n_type & N_TYPE) != N_SECT || s.n_un.n_strx == 0)
                continue;
            const char *sym = strs + s.n_un.n_strx;
            if (!strncmp(sym, prefix.c_str(), prefix.size()))
                return (void *)(slide + s.n_value);
        }
        return 0;
    }
    return 0;
}

FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    Module *m = (Module *)hModule;
    if (!m || IS_INTRESOURCE(lpProcName)) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0;
    }
    void *dl = m->dl ? m->dl : RTLD_DEFAULT;
    void *p = dlsym(dl, lpProcName);
    if (!p) {
        // _Name@N (stdcall) and _Name (cdecl) as exported from the C name.
        std::string n = lpProcName;
        if (!n.empty() && n[0] == '_')
            n.erase(0, 1);
        size_t at = n.rfind('@');
        if (at != std::string::npos && at > 0)
            n.erase(at);
        p = dlsym(dl, n.c_str());
    }
    if (!p)
        p = find_cxx_free_function(m, lpProcName);
    if (!p) {
        log("module", "GetProcAddress(%s, %s): not found\n", m->dll.c_str(), lpProcName);
        SetLastError(ERROR_PROC_NOT_FOUND);
    }
    return (FARPROC)p;
}

HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    if (!lpModuleName)
        return (HMODULE)s_exe;
    std::string dll = base_name(lpModuleName);
    if (dll.find('.') == std::string::npos)
        dll += ".dll";
    Module *m = find_module(dll);
    if (!m)
        m = system_module(dll);
    if (!m)
        SetLastError(ERROR_MOD_NOT_FOUND);
    return (HMODULE)m;
}

DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    Module *m = hModule ? (Module *)hModule : s_exe;
    std::string path = m ? m->winpath : default_winpath("Steamboat.exe");
    if (!nSize)
        return 0;
    strncpy(lpFilename, path.c_str(), nSize - 1);
    lpFilename[nSize - 1] = 0;
    return (DWORD)strlen(lpFilename);
}

BOOL WINAPI DisableThreadLibraryCalls(HMODULE) { return TRUE; }

}  // extern "C"

// The module's name, for the resource loader.
namespace win32 {
const char *module_dll_name(HMODULE h)
{
    Module *m = h ? (Module *)h : s_exe;
    return m ? m->dll.c_str() : "Steamboat.exe";
}
}  // namespace win32
