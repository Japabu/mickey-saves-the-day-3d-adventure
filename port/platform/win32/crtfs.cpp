// crtfs.cpp - the virtual file system (vfs.h) for the C runtime's file functions
// (fopen, open, stat, remove, ...; port_crt.h routes the game's calls here) and for the
// iostream file classes (port_host_path): game paths (backslashes, any case, C:\ or
// relative to the game's current directory) resolve to the CD or the overlay; writes go
// to the overlay.
//
// Paths that are already host paths (absolute, starting with '/') pass through unchanged:
// the port's own code and its libraries use those.
#include "internal.h"
#include "vfs.h"
#include "../port.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

namespace win32 {

static bool is_host_path(const char *p) { return p && p[0] == '/'; }

// A file inside the ISO image has no host path: open() and the stream classes get a copy
// in the overlay's cache directory, made once.
static bool cached_copy(const vfs::Path &p, std::string &host)
{
    std::string rel;
    for (auto &c : p)
        rel += "/" + c;
    host = overlay_dir() + "/.cd-cache" + rel;
    struct stat st;
    if (::stat(host.c_str(), &st) == 0)
        return true;
    vfs::Reader r;
    if (!vfs::open_read(p, r))
        return false;
    for (size_t i = 1; i < host.size(); i++)
        if (host[i] == '/') {
            host[i] = 0;
            ::mkdir(host.c_str(), 0755);
            host[i] = '/';
        }
    std::string tmp = host + ".part";
    int out = ::open(tmp.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        ::close(r.fd);
        return false;
    }
    char buf[65536];
    uint64_t off = 0;
    while (off < r.size) {
        size_t want = r.size - off < sizeof(buf) ? (size_t)(r.size - off) : sizeof(buf);
        ssize_t n = pread(r.fd, buf, want, (off_t)(r.base + off));
        if (n <= 0)
            break;
        write(out, buf, (size_t)n);
        off += (uint64_t)n;
    }
    ::close(out);
    ::close(r.fd);
    return ::rename(tmp.c_str(), host.c_str()) == 0;
}

// The host path of a game path. Reading: the file must exist (overlay, CD folder, or a
// cached copy from the ISO). Writing: the overlay's path; with copy the CD's version is
// copied there first (for "r+" and "a").
static bool host_for(const char *path, bool write, bool copy, std::string &host)
{
    if (is_host_path(path)) {
        host = path;
        return true;
    }
    vfs::Path p;
    if (!vfs::parse(path, p) || p.empty()) {
        errno = ENOENT;
        return false;
    }
    if (write)
        return vfs::overlay_path(p, host, copy);
    vfs::Entry e;
    if (!vfs::stat(p, e)) {
        errno = ENOENT;
        return false;
    }
    if (vfs::host_file(p, host))
        return true;
    if (e.dir) {
        errno = EISDIR;
        return false;
    }
    return cached_copy(p, host);
}

// A file inside the ISO through stdio: its bytes in a memory stream (the game reads small
// text and settings files this way).
static FILE *iso_stream(const vfs::Reader &r)
{
    // a buffer fmemopen allocates (and frees at fclose); the stream starts empty, so a
    // file of 0 bytes reads as one
    FILE *f = fmemopen(0, r.size ? (size_t)r.size : 1, "w+");
    if (!f)
        return 0;
    std::vector<char> buf(1 << 16);
    for (uint64_t pos = 0; pos < r.size;) {
        size_t want = r.size - pos < buf.size() ? (size_t)(r.size - pos) : buf.size();
        ssize_t got = pread(r.fd, buf.data(), want, (off_t)(r.base + pos));
        if (got <= 0 || fwrite(buf.data(), 1, (size_t)got, f) != (size_t)got) {
            fclose(f);
            errno = EIO;
            return 0;
        }
        pos += (uint64_t)got;
    }
    rewind(f);
    return f;
}

}  // namespace win32

using namespace win32;

extern "C" {

int port_host_path(const char *path, int for_write, char *out, size_t outsize)
{
    std::string host;
    if (!host_for(path, for_write != 0, for_write == 2, host))
        return -1;
    if (host.size() + 1 > outsize) {
        errno = ENAMETOOLONG;
        return -1;
    }
    memcpy(out, host.c_str(), host.size() + 1);
    return 0;
}

FILE *port_fopen(const char *path, const char *mode)
{
    if (!path || !mode) {
        errno = EINVAL;
        return 0;
    }
    if (is_host_path(path))
        return fopen(path, mode);
    bool write = strpbrk(mode, "wa+") != 0;
    if (!write) {
        vfs::Path p;
        vfs::Entry e;
        if (!vfs::parse(path, p) || !vfs::stat(p, e) || e.dir) {
            errno = e.dir ? EISDIR : ENOENT;
            return 0;
        }
        std::string host;
        if (vfs::host_file(p, host))
            return fopen(host.c_str(), mode);
        vfs::Reader r;
        if (!vfs::open_read(p, r))
            return 0;
        FILE *f = iso_stream(r);
        ::close(r.fd);
        return f;
    }
    // "r+" and "a" keep the CD's contents: copy them to the overlay first.
    bool keep = mode[0] == 'r' || mode[0] == 'a';
    std::string host;
    if (!host_for(path, true, keep, host))
        return 0;
    return fopen(host.c_str(), mode);
}

FILE *port_freopen(const char *path, const char *mode, FILE *stream)
{
    if (!path)
        return freopen(0, mode, stream);
    if (is_host_path(path))
        return freopen(path, mode, stream);
    bool write = strpbrk(mode, "wa+") != 0;
    std::string host;
    if (!host_for(path, write, write && (mode[0] == 'r' || mode[0] == 'a'), host))
        return 0;
    return freopen(host.c_str(), mode, stream);
}

int port_open(const char *path, int flags, int mode)
{
    bool write = (flags & (O_WRONLY | O_RDWR | O_CREAT | O_TRUNC | O_APPEND)) != 0;
    std::string host;
    if (!host_for(path, write, write && !(flags & O_TRUNC), host))
        return -1;
    return ::open(host.c_str(), flags, mode);
}

int port_stat(const char *path, struct stat *st)
{
    if (is_host_path(path))
        return ::stat(path, st);
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(path, p) || !vfs::stat(p, e)) {
        errno = ENOENT;
        return -1;
    }
    memset(st, 0, sizeof(*st));
    st->st_mode = e.dir ? (S_IFDIR | 0755) : (S_IFREG | (e.from_overlay ? 0644 : 0444));
    st->st_size = (off_t)e.size;
    st->st_mtime = st->st_atime = st->st_ctime = (time_t)e.mtime;
    st->st_nlink = 1;
    return 0;
}

int port_access(const char *path, int mode)
{
    if (is_host_path(path))
        return ::access(path, mode);
    struct stat st;
    if (port_stat(path, &st) != 0)
        return -1;
    // CD files are read-only; asking for write access to one fails as on a CD drive.
    if ((mode & W_OK) && !(st.st_mode & 0200)) {
        errno = EACCES;
        return -1;
    }
    return 0;
}

int port_remove(const char *path)
{
    if (is_host_path(path))
        return ::remove(path);
    struct stat st;
    if (port_stat(path, &st) != 0)
        return -1;
    BOOL ok = S_ISDIR(st.st_mode) ? RemoveDirectoryA(path) : DeleteFileA(path);
    if (!ok) {
        errno = GetLastError() == ERROR_ACCESS_DENIED ? EACCES : ENOENT;
        return -1;
    }
    return 0;
}

int port_rename(const char *from, const char *to)
{
    if (is_host_path(from) && is_host_path(to))
        return ::rename(from, to);
    return MoveFileA(from, to) ? 0 : (errno = EACCES, -1);
}

int port_mkdir(const char *path)
{
    if (is_host_path(path))
        return ::mkdir(path, 0755);
    if (!CreateDirectoryA(path, 0)) {
        errno = GetLastError() == ERROR_ALREADY_EXISTS ? EEXIST : ENOENT;
        return -1;
    }
    return 0;
}

int port_rmdir(const char *path)
{
    if (is_host_path(path))
        return ::rmdir(path);
    if (!RemoveDirectoryA(path)) {
        errno = ENOENT;
        return -1;
    }
    return 0;
}

int port_chdir(const char *path)
{
    if (!SetCurrentDirectoryA(path)) {
        errno = ENOENT;
        return -1;
    }
    return 0;
}

char *port_getcwd(char *buf, size_t size)
{
    char tmp[MAX_PATH];
    DWORD n = GetCurrentDirectoryA(sizeof(tmp), tmp);
    if (!buf) {
        size = size > n ? size : n + 1;
        buf = (char *)malloc(size);
    }
    if (n + 1 > size) {
        errno = ERANGE;
        return 0;
    }
    memcpy(buf, tmp, n + 1);
    return buf;
}

// MoveFile: within the overlay, or a CD file copied to its new name.
BOOL WINAPI MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
    vfs::Path from, to;
    vfs::Entry e;
    if (!vfs::parse(lpExistingFileName, from) || !vfs::stat(from, e)) {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }
    if (!vfs::parse(lpNewFileName, to)) {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    vfs::Entry e2;
    vfs::Path to_probe = to;
    if (vfs::stat(to_probe, e2)) {
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if (!e.from_overlay) {
        if (!CopyFileA(lpExistingFileName, lpNewFileName, TRUE))
            return FALSE;
        SetLastError(ERROR_SUCCESS);
        return TRUE;  // the CD original stays
    }
    std::string src, dst;
    vfs::overlay_path(from, src, false);
    vfs::overlay_path(to, dst, false);
    if (::rename(src.c_str(), dst.c_str()) != 0) {
        SetLastError(error_from_errno(errno));
        return FALSE;
    }
    return TRUE;
}

}  // extern "C"

// The process's own working directory: the overlay root, so that anything that still
// opens a relative path behind the virtual file system's back lands there and not in
// the player's directory. A relative MICKEY_CD is resolved before the change. Done when
// the game starts (port_set_command_line), after the host named the data directory.
namespace win32 {
std::string &launch_dir()
{
    static std::string dir;
    return dir;
}

void enter_overlay()
{
    char buf[4096];
    if (getcwd(buf, sizeof(buf)))
        launch_dir() = buf;
    if (chdir(overlay_dir().c_str()) != 0)
        log("vfs", "chdir to the overlay failed\n");
}
}
