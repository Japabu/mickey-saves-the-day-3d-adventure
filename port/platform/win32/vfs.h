// vfs.h - the game's file system: drive C: is the game CD (a mounted or copied directory,
// or the .iso image itself) with a writable overlay on top.
//
//   C:\setup\hd\...   the installed game (binaries, resources), as the installer laid it out
//   C:\setup\cd\...   the CD-only resources
//
// The CD is never written; a write goes to the overlay directory (Application Support),
// which shadows the CD file of the same path from then on. Paths are matched without
// regard to case, as on Windows.
#ifndef PORT_WIN32_VFS_H
#define PORT_WIN32_VFS_H

#include "internal.h"
#include <memory>
#include <string>
#include <vector>

namespace win32 {
namespace vfs {

struct Entry {
    std::string name;  // as spelled on the medium
    bool dir = false;
    uint64_t size = 0;
    int64_t mtime = 0;  // seconds since 1970
    bool from_overlay = false;
};

// A path inside the virtual tree: components below the drive root, in the medium's own
// spelling once resolved.
typedef std::vector<std::string> Path;

// Splits a Windows path ("C:\a\b", "\a\b", "a\.\b\..\c", with / or \) into components
// below the root; relative paths are taken against the current directory.
// Returns false for another drive letter.
bool parse(const char *winpath, Path &out);
std::string to_windows(const Path &p);  // "C:\a\b"

// Looks the path up (overlay first, then the CD). Fills e and sets p to its spelling.
bool stat(Path &p, Entry &e);
// The entries of a directory, overlay and CD merged (the overlay wins for equal names).
bool list(const Path &dir, std::vector<Entry> &out);

// An open file's bytes: a host file descriptor (overlay, or a CD directory), or a byte
// range of the ISO image.
struct Reader {
    int fd = -1;          // host file
    uint64_t base = 0;    // offset of the file in fd (ISO image), 0 for a host file
    uint64_t size = 0;    // the file's size (ISO)
    bool iso = false;
};
// Opens a CD or overlay file for reading.
bool open_read(const Path &p, Reader &r);
// The host path of the file in the overlay (creating the directories). With copy, the
// CD's version is copied there first if the overlay has none.
bool overlay_path(const Path &p, std::string &host, bool copy);
// Creates a directory in the overlay.
bool make_dir(const Path &p);

// Sets the files the game expects from its installer on first start (engine.ini,
// steamboat.ini in the binaries directory), if the overlay does not have them.
void install_defaults();

}  // namespace vfs
}  // namespace win32

#endif
