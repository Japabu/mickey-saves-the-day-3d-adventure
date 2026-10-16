// file.cpp - kernel32 files on the virtual drive (vfs.h): CreateFile and the handle
// calls, attributes, directories, find, file mappings, the old _lopen/OpenFile, and the
// console streams.
#include "internal.h"
#include "vfs.h"

#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace win32 {

void unix_to_filetime(time_t sec, long nsec, FILETIME *ft);

FdObject::~FdObject()
{
    if (kind != Console && fd >= 0)
        ::close(fd);
}

struct FileObject : FdObject {
    FileObject() : FdObject(File, -1) {}
    ~FileObject()
    {
        if (delete_on_close && !host.empty())
            ::unlink(host.c_str());
    }
    bool iso = false;    // a read-only byte range of the CD image
    uint64_t base = 0;   // ISO: start of the file in the image
    uint64_t size = 0;   // ISO: the file's size
    uint64_t pos = 0;    // ISO: the file pointer (host files use the descriptor's)
    bool writable = false;
    bool delete_on_close = false;
    std::string host;    // the overlay path, for written files
    int64_t mtime = 0;
};

static void fail(DWORD err) { SetLastError(err); }
static void fail_errno() { SetLastError(error_from_errno(errno)); }

static void entry_times(const vfs::Entry &e, FILETIME *c, FILETIME *a, FILETIME *w)
{
    FILETIME t;
    unix_to_filetime((time_t)e.mtime, 0, &t);
    if (c)
        *c = t;
    if (a)
        *a = t;
    if (w)
        *w = t;
}

static DWORD attributes_of(const vfs::Entry &e)
{
    if (e.dir)
        return FILE_ATTRIBUTE_DIRECTORY;
    // Files on the CD were read-only for the game; overlay files are normal.
    return e.from_overlay ? FILE_ATTRIBUTE_ARCHIVE : (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE);
}

struct FindObject : Object {
    FindObject() : Object(Find) {}
    std::vector<vfs::Entry> entries;
    size_t next = 0;
    std::string pattern;
};

static void fill_find_data(const vfs::Entry &e, LPWIN32_FIND_DATAA fd)
{
    memset(fd, 0, sizeof(*fd));
    fd->dwFileAttributes = attributes_of(e);
    entry_times(e, &fd->ftCreationTime, &fd->ftLastAccessTime, &fd->ftLastWriteTime);
    fd->nFileSizeHigh = (DWORD)(e.size >> 32);
    fd->nFileSizeLow = (DWORD)e.size;
    strncpy(fd->cFileName, e.name.c_str(), MAX_PATH - 1);
    // An 8.3 name for the alternate name field (the game never needs the real one).
    strncpy(fd->cAlternateFileName, e.name.size() <= 12 ? e.name.c_str() : "", 13);
}

static bool find_next(FindObject *f, LPWIN32_FIND_DATAA fd)
{
    while (f->next < f->entries.size()) {
        const vfs::Entry &e = f->entries[f->next++];
        if (fnmatch(f->pattern.c_str(), e.name.c_str(), FNM_CASEFOLD) == 0) {
            fill_find_data(e, fd);
            return true;
        }
    }
    return false;
}

}  // namespace win32

using namespace win32;

extern "C" {

HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES,
                          DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE)
{
    (void)dwShareMode;
    vfs::Path p;
    if (!vfs::parse(lpFileName, p) || p.empty()) {
        fail(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    vfs::Entry e;
    vfs::Path q = p;
    bool exists = vfs::stat(q, e);
    if (exists && e.dir) {
        fail(ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    bool write = (dwDesiredAccess & (GENERIC_WRITE | GENERIC_ALL)) != 0;
    switch (dwCreationDisposition) {
    case CREATE_NEW:
        if (exists) {
            fail(ERROR_FILE_EXISTS);
            return INVALID_HANDLE_VALUE;
        }
        break;
    case OPEN_EXISTING:
    case TRUNCATE_EXISTING:
        if (!exists) {
            // The parent directory decides between the two errors.
            vfs::Path parent = p;
            parent.pop_back();
            vfs::Entry pe;
            fail(vfs::stat(parent, pe) ? ERROR_FILE_NOT_FOUND : ERROR_PATH_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
        break;
    }
    FileObject *f = new FileObject;
    f->delete_on_close = (dwFlagsAndAttributes & FILE_FLAG_DELETE_ON_CLOSE) != 0;
    if (!write && exists) {
        vfs::Reader r;
        if (!vfs::open_read(q, r)) {
            delete f;
            fail_errno();
            return INVALID_HANDLE_VALUE;
        }
        f->fd = r.fd;
        f->iso = r.iso;
        f->base = r.base;
        f->size = r.size;
        f->mtime = e.mtime;
        SetLastError(ERROR_SUCCESS);
        return handle(f);
    }
    // Writing (or creating): the overlay's copy.
    bool keep = exists && (dwCreationDisposition == OPEN_EXISTING || dwCreationDisposition == OPEN_ALWAYS);
    if (!vfs::overlay_path(exists ? q : p, f->host, keep)) {
        delete f;
        fail(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    int flags = O_CREAT | ((dwDesiredAccess & GENERIC_READ) || (dwDesiredAccess & GENERIC_ALL) ? O_RDWR : O_WRONLY);
    if (dwCreationDisposition == CREATE_ALWAYS || dwCreationDisposition == TRUNCATE_EXISTING)
        flags |= O_TRUNC;
    if (!write)
        flags = O_RDONLY | O_CREAT;
    f->fd = ::open(f->host.c_str(), flags, 0644);
    if (f->fd < 0) {
        f->host.clear();
        delete f;
        fail_errno();
        return INVALID_HANDLE_VALUE;
    }
    f->writable = write;
    // As on Windows: ERROR_ALREADY_EXISTS reports that OPEN_ALWAYS/CREATE_ALWAYS found a file.
    SetLastError(exists && (dwCreationDisposition == OPEN_ALWAYS || dwCreationDisposition == CREATE_ALWAYS)
                     ? ERROR_ALREADY_EXISTS
                     : ERROR_SUCCESS);
    return handle(f);
}

static FdObject *fd_object(HANDLE h)
{
    Object *o = object(h);
    if (!o || (o->kind != Object::File && o->kind != Object::Console)) {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    return static_cast<FdObject *>(o);
}

BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                     LPOVERLAPPED lpOverlapped)
{
    if (lpNumberOfBytesRead)
        *lpNumberOfBytesRead = 0;
    FdObject *o = fd_object(hFile);
    if (!o)
        return FALSE;
    ssize_t n;
    if (o->kind == Object::File && static_cast<FileObject *>(o)->iso) {
        FileObject *f = static_cast<FileObject *>(o);
        uint64_t pos = lpOverlapped ? ((uint64_t)lpOverlapped->OffsetHigh << 32 | lpOverlapped->Offset) : f->pos;
        uint64_t left = pos < f->size ? f->size - pos : 0;
        size_t want = nNumberOfBytesToRead < left ? nNumberOfBytesToRead : (size_t)left;
        n = want ? pread(f->fd, lpBuffer, want, (off_t)(f->base + pos)) : 0;
        if (n > 0 && !lpOverlapped)
            f->pos = pos + (uint64_t)n;
    } else if (lpOverlapped) {
        n = pread(o->fd, lpBuffer, nNumberOfBytesToRead,
                  (off_t)((uint64_t)lpOverlapped->OffsetHigh << 32 | lpOverlapped->Offset));
    } else {
        n = ::read(o->fd, lpBuffer, nNumberOfBytesToRead);
    }
    if (n < 0) {
        fail_errno();
        return FALSE;
    }
    if (lpNumberOfBytesRead)
        *lpNumberOfBytesRead = (DWORD)n;
    return TRUE;
}

BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                      LPOVERLAPPED lpOverlapped)
{
    if (lpNumberOfBytesWritten)
        *lpNumberOfBytesWritten = 0;
    FdObject *o = fd_object(hFile);
    if (!o)
        return FALSE;
    if (o->kind == Object::File && !static_cast<FileObject *>(o)->writable) {
        fail(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    ssize_t n = lpOverlapped ? pwrite(o->fd, lpBuffer, nNumberOfBytesToWrite,
                                      (off_t)((uint64_t)lpOverlapped->OffsetHigh << 32 | lpOverlapped->Offset))
                             : ::write(o->fd, lpBuffer, nNumberOfBytesToWrite);
    if (n < 0) {
        fail_errno();
        return FALSE;
    }
    if (lpNumberOfBytesWritten)
        *lpNumberOfBytesWritten = (DWORD)n;
    return TRUE;
}

DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    FdObject *o = fd_object(hFile);
    if (!o)
        return INVALID_SET_FILE_POINTER;
    int64_t dist = lpDistanceToMoveHigh ? ((int64_t)*lpDistanceToMoveHigh << 32) | (uint32_t)lDistanceToMove
                                        : (int64_t)lDistanceToMove;
    int64_t pos;
    if (o->kind == Object::File && static_cast<FileObject *>(o)->iso) {
        FileObject *f = static_cast<FileObject *>(o);
        int64_t from = dwMoveMethod == FILE_BEGIN ? 0 : dwMoveMethod == FILE_CURRENT ? (int64_t)f->pos : (int64_t)f->size;
        pos = from + dist;
        if (pos < 0) {
            fail(131);  // ERROR_NEGATIVE_SEEK
            return INVALID_SET_FILE_POINTER;
        }
        f->pos = (uint64_t)pos;
    } else {
        int whence = dwMoveMethod == FILE_BEGIN ? SEEK_SET : dwMoveMethod == FILE_CURRENT ? SEEK_CUR : SEEK_END;
        pos = lseek(o->fd, (off_t)dist, whence);
        if (pos < 0) {
            fail_errno();
            return INVALID_SET_FILE_POINTER;
        }
    }
    if (lpDistanceToMoveHigh)
        *lpDistanceToMoveHigh = (LONG)(pos >> 32);
    SetLastError(ERROR_SUCCESS);
    return (DWORD)pos;
}

BOOL WINAPI SetEndOfFile(HANDLE hFile)
{
    FileObject *f = object_as<FileObject>(hFile, Object::File);
    if (!f || f->iso || !f->writable) {
        fail(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    off_t pos = lseek(f->fd, 0, SEEK_CUR);
    return ftruncate(f->fd, pos) == 0;
}

BOOL WINAPI FlushFileBuffers(HANDLE hFile)
{
    FdObject *o = fd_object(hFile);
    if (!o)
        return FALSE;
    fsync(o->fd);
    return TRUE;
}

DWORD WINAPI GetFileType(HANDLE hFile)
{
    FdObject *o = fd_object(hFile);
    if (!o)
        return FILE_TYPE_UNKNOWN;
    return o->kind == Object::Console ? FILE_TYPE_CHAR : FILE_TYPE_DISK;
}

DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    FdObject *o = fd_object(hFile);
    if (!o)
        return (DWORD)-1;
    uint64_t size;
    if (o->kind == Object::File && static_cast<FileObject *>(o)->iso) {
        size = static_cast<FileObject *>(o)->size;
    } else {
        struct stat st;
        if (fstat(o->fd, &st) != 0) {
            fail_errno();
            return (DWORD)-1;
        }
        size = (uint64_t)st.st_size;
    }
    if (lpFileSizeHigh)
        *lpFileSizeHigh = (DWORD)(size >> 32);
    return (DWORD)size;
}

BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
    FileObject *f = object_as<FileObject>(hFile, Object::File);
    if (!f)
        return FALSE;
    vfs::Entry e;
    if (f->iso) {
        e.mtime = f->mtime;
    } else {
        struct stat st;
        fstat(f->fd, &st);
        e.mtime = st.st_mtime;
    }
    entry_times(e, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
    return TRUE;
}

BOOL WINAPI GetFileInformationByHandle(HANDLE hFile, LPBY_HANDLE_FILE_INFORMATION info)
{
    FileObject *f = object_as<FileObject>(hFile, Object::File);
    if (!f)
        return FALSE;
    memset(info, 0, sizeof(*info));
    vfs::Entry e;
    uint64_t size;
    if (f->iso) {
        e.mtime = f->mtime;
        size = f->size;
        info->dwFileAttributes = FILE_ATTRIBUTE_READONLY;
    } else {
        struct stat st;
        fstat(f->fd, &st);
        e.mtime = st.st_mtime;
        size = (uint64_t)st.st_size;
        info->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
        info->nFileIndexLow = (DWORD)st.st_ino;
        info->nFileIndexHigh = (DWORD)(st.st_ino >> 32);
    }
    entry_times(e, &info->ftCreationTime, &info->ftLastAccessTime, &info->ftLastWriteTime);
    info->nFileSizeHigh = (DWORD)(size >> 32);
    info->nFileSizeLow = (DWORD)size;
    info->nNumberOfLinks = 1;
    return TRUE;
}

DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName)
{
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(lpFileName, p) || !vfs::stat(p, e)) {
        fail(ERROR_FILE_NOT_FOUND);
        return INVALID_FILE_ATTRIBUTES;
    }
    return attributes_of(e);
}

BOOL WINAPI SetFileAttributesA(LPCSTR lpFileName, DWORD)
{
    // Attributes are not kept; the file must exist.
    return GetFileAttributesA(lpFileName) != INVALID_FILE_ATTRIBUTES;
}

BOOL WINAPI DeleteFileA(LPCSTR lpFileName)
{
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(lpFileName, p) || !vfs::stat(p, e)) {
        fail(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }
    if (!e.from_overlay) {
        fail(ERROR_ACCESS_DENIED);  // a CD file
        return FALSE;
    }
    std::string host;
    vfs::overlay_path(p, host, false);
    if (::unlink(host.c_str()) != 0) {
        fail_errno();
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists)
{
    HANDLE in = CreateFileA(lpExistingFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (in == INVALID_HANDLE_VALUE)
        return FALSE;
    HANDLE out = CreateFileA(lpNewFileName, GENERIC_WRITE, 0, 0, bFailIfExists ? CREATE_NEW : CREATE_ALWAYS, 0, 0);
    if (out == INVALID_HANDLE_VALUE) {
        CloseHandle(in);
        return FALSE;
    }
    char buf[65536];
    DWORD n;
    BOOL ok = TRUE;
    while (ReadFile(in, buf, sizeof(buf), &n, 0) && n) {
        DWORD w;
        if (!WriteFile(out, buf, n, &w, 0) || w != n) {
            ok = FALSE;
            break;
        }
    }
    CloseHandle(in);
    CloseHandle(out);
    return ok;
}

BOOL WINAPI CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES)
{
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(lpPathName, p)) {
        fail(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    vfs::Path q = p;
    if (vfs::stat(q, e)) {
        fail(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    return vfs::make_dir(p);
}

BOOL WINAPI RemoveDirectoryA(LPCSTR lpPathName)
{
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(lpPathName, p) || !vfs::stat(p, e) || !e.dir) {
        fail(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    if (!e.from_overlay) {
        fail(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    std::string host;
    vfs::overlay_path(p, host, false);
    if (::rmdir(host.c_str()) != 0) {
        fail_errno();
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer)
{
    std::string d = current_dir();
    if (d.size() > 3)
        d.pop_back();  // no trailing backslash except for the root
    if (nBufferLength <= d.size())
        return (DWORD)d.size() + 1;
    strcpy(lpBuffer, d.c_str());
    return (DWORD)d.size();
}

BOOL WINAPI SetCurrentDirectoryA(LPCSTR lpPathName)
{
    if (!set_current_dir(lpPathName)) {
        fail(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    log("vfs", "current directory %s\n", current_dir().c_str());
    return TRUE;
}

DWORD WINAPI GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart)
{
    std::string full = full_path(lpFileName);
    if (nBufferLength <= full.size())
        return (DWORD)full.size() + 1;
    strcpy(lpBuffer, full.c_str());
    if (lpFilePart) {
        char *slash = strrchr(lpBuffer, '\\');
        *lpFilePart = slash && slash[1] ? slash + 1 : 0;
    }
    return (DWORD)full.size();
}

UINT WINAPI GetDriveTypeA(LPCSTR lpRootPathName)
{
    if (!lpRootPathName)
        return DRIVE_FIXED;  // the current drive
    char d = (char)tolower((unsigned char)lpRootPathName[0]);
    if (d == 'c' || lpRootPathName[0] == '\\')
        return DRIVE_FIXED;
    return DRIVE_NO_ROOT_DIR;
}

// Find.
HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    vfs::Path p;
    if (!vfs::parse(lpFileName, p) || p.empty()) {
        fail(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    FindObject *f = new FindObject;
    f->pattern = p.back();
    p.pop_back();
    // "*.*" matches every name, also those without a dot.
    if (f->pattern == "*.*")
        f->pattern = "*";
    if (f->pattern.find_first_of("*?") == std::string::npos) {
        // A plain name: the entry itself.
        vfs::Path q = p;
        q.push_back(f->pattern);
        vfs::Entry e;
        if (vfs::stat(q, e)) {
            e.name = q.back();
            f->entries.push_back(e);
        }
    } else if (!vfs::list(p, f->entries)) {
        delete f;
        fail(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    if (!find_next(f, lpFindFileData)) {
        delete f;
        fail(ERROR_FILE_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    return handle(f);
}

BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    FindObject *f = object_as<FindObject>(hFindFile, Object::Find);
    if (!f)
        return FALSE;
    if (!find_next(f, lpFindFileData)) {
        fail(ERROR_NO_MORE_FILES);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI FindClose(HANDLE hFindFile)
{
    FindObject *f = object_as<FindObject>(hFindFile, Object::Find);
    if (!f)
        return FALSE;
    release(f);
    return TRUE;
}

// File mappings: the whole file is mapped (or read) at MapViewOfFile.
struct MappingObject : Object {
    MappingObject() : Object(Mapping) {}
    ~MappingObject()
    {
        if (file)
            release(file);
    }
    FileObject *file = 0;
    uint64_t size = 0;
    bool write = false;
};

struct View {
    void *addr;
    size_t len;
    bool malloced;
};
static std::vector<View> s_views;

HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES, DWORD flProtect, DWORD dwMaximumSizeHigh,
                                 DWORD dwMaximumSizeLow, LPCSTR)
{
    MappingObject *m = new MappingObject;
    if (hFile != INVALID_HANDLE_VALUE) {
        FileObject *f = object_as<FileObject>(hFile, Object::File);
        if (!f) {
            delete m;
            return 0;
        }
        retain(f);
        m->file = f;
        DWORD hi = 0;
        m->size = GetFileSize(hFile, &hi);
        m->size |= (uint64_t)hi << 32;
    }
    uint64_t want = ((uint64_t)dwMaximumSizeHigh << 32) | dwMaximumSizeLow;
    if (want > m->size)
        m->size = want;
    m->write = (flProtect & 0xff) == PAGE_READWRITE || (flProtect & 0xff) == PAGE_WRITECOPY;
    return handle(m);
}

LPVOID WINAPI MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh,
                            DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap)
{
    MappingObject *m = object_as<MappingObject>(hFileMappingObject, Object::Mapping);
    if (!m)
        return 0;
    uint64_t off = ((uint64_t)dwFileOffsetHigh << 32) | dwFileOffsetLow;
    size_t len = dwNumberOfBytesToMap ? dwNumberOfBytesToMap : (size_t)(m->size - off);
    if (!len)
        len = 1;
    bool write = (dwDesiredAccess & (FILE_MAP_WRITE | FILE_MAP_COPY)) != 0;
    void *p;
    bool malloced = false;
    if (!m->file) {
        p = mmap(0, len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    } else if (m->file->iso || (off % (uint64_t)getpagesize()) != 0) {
        // A view of a CD image range (or unaligned): a private copy.
        p = calloc(1, len);
        malloced = true;
        if (p) {
            uint64_t base = m->file->iso ? m->file->base : 0;
            pread(m->file->fd, p, len, (off_t)(base + off));
        }
    } else {
        int prot = PROT_READ | (write ? PROT_WRITE : 0);
        int flags = (dwDesiredAccess & FILE_MAP_COPY) ? MAP_PRIVATE : MAP_SHARED;
        p = mmap(0, len, prot, flags, m->file->fd, (off_t)off);
    }
    if (!p || p == MAP_FAILED) {
        fail(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    s_views.push_back({p, len, malloced});
    return p;
}

BOOL WINAPI UnmapViewOfFile(LPCVOID lpBaseAddress)
{
    for (size_t i = 0; i < s_views.size(); i++)
        if (s_views[i].addr == lpBaseAddress) {
            if (s_views[i].malloced)
                free(s_views[i].addr);
            else
                munmap(s_views[i].addr, s_views[i].len);
            s_views.erase(s_views.begin() + (long)i);
            return TRUE;
        }
    fail(ERROR_INVALID_PARAMETER);
    return FALSE;
}

// The 16-bit era file calls. An HFILE is a small number standing for a HANDLE.
static std::vector<HANDLE> s_hfiles;

static HFILE hfile_of(HANDLE h)
{
    for (size_t i = 0; i < s_hfiles.size(); i++)
        if (!s_hfiles[i]) {
            s_hfiles[i] = h;
            return (HFILE)(i + 1);
        }
    s_hfiles.push_back(h);
    return (HFILE)s_hfiles.size();
}

static HANDLE handle_of(HFILE f)
{
    if (f <= 0 || (size_t)f > s_hfiles.size())
        return INVALID_HANDLE_VALUE;
    return s_hfiles[(size_t)f - 1];
}

HFILE WINAPI _lopen(LPCSTR lpPathName, int iReadWrite)
{
    DWORD access = (iReadWrite & 3) == OF_WRITE ? GENERIC_WRITE
                   : (iReadWrite & 3) == OF_READWRITE ? GENERIC_READ | GENERIC_WRITE
                                                      : GENERIC_READ;
    HANDLE h = CreateFileA(lpPathName, access, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    return h == INVALID_HANDLE_VALUE ? HFILE_ERROR : hfile_of(h);
}

UINT WINAPI _lread(HFILE hFile, LPVOID lpBuffer, UINT uBytes)
{
    DWORD n = 0;
    if (!ReadFile(handle_of(hFile), lpBuffer, uBytes, &n, 0))
        return (UINT)-1;
    return n;
}

int WINAPI _lclose(HFILE hFile)
{
    HANDLE h = handle_of(hFile);
    if (h == INVALID_HANDLE_VALUE)
        return HFILE_ERROR;
    s_hfiles[(size_t)hFile - 1] = 0;
    CloseHandle(h);
    return 0;
}

HFILE WINAPI OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
    if (lpReOpenBuff) {
        memset(lpReOpenBuff, 0, sizeof(*lpReOpenBuff));
        lpReOpenBuff->cBytes = sizeof(*lpReOpenBuff);
        strncpy(lpReOpenBuff->szPathName, full_path(lpFileName).c_str(), OFS_MAXPATHNAME - 1);
    }
    if (uStyle & OF_EXIST) {
        if (GetFileAttributesA(lpFileName) == INVALID_FILE_ATTRIBUTES) {
            if (lpReOpenBuff)
                lpReOpenBuff->nErrCode = (WORD)GetLastError();
            return HFILE_ERROR;
        }
        return 1;
    }
    DWORD access = (uStyle & 3) == OF_WRITE ? GENERIC_WRITE
                   : (uStyle & 3) == OF_READWRITE ? GENERIC_READ | GENERIC_WRITE
                                                  : GENERIC_READ;
    HANDLE h = CreateFileA(lpFileName, access, FILE_SHARE_READ, 0, (uStyle & OF_CREATE) ? CREATE_ALWAYS : OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) {
        if (lpReOpenBuff)
            lpReOpenBuff->nErrCode = (WORD)GetLastError();
        return HFILE_ERROR;
    }
    return hfile_of(h);
}

UINT WINAPI SetHandleCount(UINT uNumber) { return uNumber; }

// The console: the game's debug console goes to the terminal.
BOOL WINAPI AllocConsole(void) { return TRUE; }
BOOL WINAPI FreeConsole(void) { return TRUE; }
BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE, BOOL) { return TRUE; }
BOOL WINAPI SetConsoleWindowInfo(HANDLE, BOOL, const SMALL_RECT *) { return TRUE; }

BOOL WINAPI WriteConsoleA(HANDLE hConsoleOutput, const void *lpBuffer, DWORD n, LPDWORD written, LPVOID)
{
    return WriteFile(hConsoleOutput, lpBuffer, n, written, 0);
}

BOOL WINAPI ReadConsoleA(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD n, LPDWORD read, LPVOID)
{
    return ReadFile(hConsoleInput, lpBuffer, n, read, 0);
}

DWORD WINAPI GetProfileStringA(LPCSTR, LPCSTR, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize)
{
    // win.ini: nothing is kept there; the default comes back.
    if (!nSize)
        return 0;
    strncpy(lpReturnedString, lpDefault ? lpDefault : "", nSize - 1);
    lpReturnedString[nSize - 1] = 0;
    return (DWORD)strlen(lpReturnedString);
}

}  // extern "C"
