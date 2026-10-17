// vfs.cpp - drive C: as the game CD plus a writable overlay (see vfs.h), with a read-only
// ISO 9660 reader (Joliet names when the image has them) for running from the .iso.
#include "vfs.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <map>

namespace win32 {
namespace vfs {

static bool same(const std::string &a, const std::string &b) { return strcasecmp(a.c_str(), b.c_str()) == 0; }

// ---------------------------------------------------------------------------------------
// Host directories (the overlay, or a CD copied or mounted as a directory).

static bool host_find(const std::string &dir, const std::string &name, std::string &spelled)
{
    std::string exact = dir + "/" + name;
    struct stat st;
    if (::lstat(exact.c_str(), &st) == 0) {
        spelled = name;
        return true;
    }
    DIR *d = opendir(dir.c_str());
    if (!d)
        return false;
    bool found = false;
    while (struct dirent *e = readdir(d)) {
        if (strcasecmp(e->d_name, name.c_str()) == 0) {
            spelled = e->d_name;
            found = true;
            break;
        }
    }
    closedir(d);
    return found;
}

// Resolves p below root; on success host is the full host path and p is respelled.
static bool host_resolve(const std::string &root, Path &p, std::string &host)
{
    std::string cur = root;
    for (auto &c : p) {
        std::string s;
        if (!host_find(cur, c, s))
            return false;
        c = s;
        cur += "/" + s;
    }
    host = cur;
    return true;
}

static bool host_stat(const std::string &root, Path &p, Entry &e)
{
    std::string host;
    if (!host_resolve(root, p, host))
        return false;
    struct stat st;
    if (::stat(host.c_str(), &st) != 0)
        return false;
    e.name = p.empty() ? "" : p.back();
    e.dir = S_ISDIR(st.st_mode);
    e.size = (uint64_t)st.st_size;
    e.mtime = st.st_mtime;
    return true;
}

static void host_list(const std::string &root, Path p, std::vector<Entry> &out)
{
    std::string host;
    if (!host_resolve(root, p, host))
        return;
    DIR *d = opendir(host.c_str());
    if (!d)
        return;
    while (struct dirent *de = readdir(d)) {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") || de->d_name[0] == '.')
            continue;
        struct stat st;
        std::string full = host + "/" + de->d_name;
        if (::stat(full.c_str(), &st) != 0)
            continue;
        Entry e;
        e.name = de->d_name;
        e.dir = S_ISDIR(st.st_mode);
        e.size = (uint64_t)st.st_size;
        e.mtime = st.st_mtime;
        out.push_back(e);
    }
    closedir(d);
}

// ---------------------------------------------------------------------------------------
// ISO 9660 images.

struct IsoNode {
    std::string name;
    bool dir = false;
    uint32_t extent = 0;  // in 2048-byte sectors
    uint32_t size = 0;
    int64_t mtime = 0;
    bool loaded = false;  // children read
    std::vector<IsoNode> children;
};

class Iso {
public:
    bool open(const char *path)
    {
        fd_ = ::open(path, O_RDONLY);
        if (fd_ < 0)
            return false;
        unsigned char vd[2048];
        bool have_primary = false, have_joliet = false;
        unsigned char primary_root[34], joliet_root[34];
        for (int s = 16; s < 64; s++) {
            if (pread(fd_, vd, 2048, (off_t)s * 2048) != 2048 || memcmp(vd + 1, "CD001", 5))
                break;
            if (vd[0] == 255)
                break;
            if (vd[0] == 1 && !have_primary) {
                memcpy(primary_root, vd + 156, 34);
                have_primary = true;
            } else if (vd[0] == 2 && vd[88] == 0x25 && vd[89] == 0x2f &&
                       (vd[90] == 0x40 || vd[90] == 0x43 || vd[90] == 0x45)) {
                memcpy(joliet_root, vd + 156, 34);
                have_joliet = true;
            }
        }
        if (!have_primary && !have_joliet) {
            ::close(fd_);
            fd_ = -1;
            return false;
        }
        joliet_ = have_joliet;
        root_ = node_from(have_joliet ? joliet_root : primary_root);
        root_.name = "";
        return true;
    }

    int fd() const { return fd_; }

    // Finds a node by path, respelling it; 0 if absent.
    IsoNode *find(Path &p)
    {
        std::lock_guard<std::mutex> lock(mu_);
        IsoNode *n = &root_;
        for (auto &c : p) {
            if (!n->dir)
                return 0;
            load(*n);
            IsoNode *next = 0;
            for (auto &ch : n->children)
                if (same(ch.name, c)) {
                    next = &ch;
                    break;
                }
            if (!next)
                return 0;
            c = next->name;
            n = next;
        }
        if (n->dir)
            load(*n);
        return n;
    }

private:
    static uint32_t le32(const unsigned char *b) { return b[0] | (b[1] << 8) | (b[2] << 16) | ((uint32_t)b[3] << 24); }

    IsoNode node_from(const unsigned char *r)
    {
        IsoNode n;
        n.extent = le32(r + 2);
        n.size = le32(r + 10);
        n.dir = (r[25] & 2) != 0;
        struct tm tm = {};
        tm.tm_year = r[18];
        tm.tm_mon = r[19] - 1;
        tm.tm_mday = r[20];
        tm.tm_hour = r[21];
        tm.tm_min = r[22];
        tm.tm_sec = r[23];
        n.mtime = (int64_t)timegm(&tm) - (int64_t)(signed char)r[24] * 15 * 60;
        int len = r[32];
        const unsigned char *id = r + 33;
        std::string name;
        if (joliet_) {
            for (int i = 0; i + 1 < len; i += 2) {
                unsigned c = (id[i] << 8) | id[i + 1];
                if (c < 0x80) {
                    name += (char)c;
                } else if (c < 0x800) {
                    name += (char)(0xC0 | (c >> 6));
                    name += (char)(0x80 | (c & 0x3F));
                } else {
                    name += (char)(0xE0 | (c >> 12));
                    name += (char)(0x80 | ((c >> 6) & 0x3F));
                    name += (char)(0x80 | (c & 0x3F));
                }
            }
        } else {
            name.assign((const char *)id, (size_t)len);
        }
        size_t semi = name.find(';');
        if (semi != std::string::npos)
            name.erase(semi);
        if (!n.dir && !name.empty() && name.back() == '.')
            name.pop_back();
        n.name = name;
        return n;
    }

    void load(IsoNode &n)
    {
        if (n.loaded)
            return;
        n.loaded = true;
        std::vector<unsigned char> data(n.size);
        if (pread(fd_, data.data(), n.size, (off_t)n.extent * 2048) != (ssize_t)n.size)
            return;
        size_t i = 0;
        while (i < data.size()) {
            unsigned len = data[i];
            if (len == 0) {
                i = (i / 2048 + 1) * 2048;
                continue;
            }
            if (i + len > data.size())
                break;
            const unsigned char *r = &data[i];
            i += len;
            if (r[32] == 1 && (r[33] == 0 || r[33] == 1))
                continue;  // . and ..
            n.children.push_back(node_from(r));
        }
    }

    int fd_ = -1;
    bool joliet_ = false;
    IsoNode root_;
    std::mutex mu_;
};

// ---------------------------------------------------------------------------------------
// Configuration: where the CD and the overlay are.

static std::string s_cd_dir;  // the CD as a host directory, or
static Iso *s_iso;            // the CD as an image
static Path s_cwd = {"setup", "hd", "binaries"};
static std::mutex s_cwd_mu;

static std::string trim(const std::string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == std::string::npos ? "" : s.substr(a, b - a + 1);
}

// MICKEY_CD, else `cd = <path>` in <data>/port.ini.
static std::string cd_setting()
{
    if (const char *e = getenv("MICKEY_CD"))
        return e;
    FILE *f = fopen((data_dir() + "/port.ini").c_str(), "r");
    std::string out;
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            std::string l = line;
            size_t eq = l.find('=');
            if (eq != std::string::npos && same(trim(l.substr(0, eq)), "cd"))
                out = trim(l.substr(eq + 1));
        }
        fclose(f);
    }
    return out;
}

// The CD is missing or not the game's: say so in a message box and quit, instead of
// letting the game fail somewhere deep inside.
static void cd_problem(const std::string &what)
{
    std::string text = what +
                       "\n\nPoint MICKEY_CD at your game CD image (.iso, or the folder of the "
                       "copied or mounted CD), for example:\n\n  MICKEY_CD=/path/to/Micky_auf_Ganovenjagd.iso\n\n"
                       "or write cd = <path> into " + data_dir() + "/port.ini.";
    fprintf(stderr, "%s\n", text.c_str());
    _exit(1);
}

static void init_cd()
{
    static std::once_flag once;
    std::string problem;
    std::call_once(once, [&problem] {
        std::string cd = cd_setting();
        if (cd.empty()) {
            problem = "The game CD is not set.";
            return;
        }
        if (cd[0] == '~' && getenv("HOME"))
            cd = std::string(getenv("HOME")) + cd.substr(1);
        // A relative path is taken from where the game was started (the process itself
        // works in the overlay directory, crtfs.cpp).
        if (cd[0] != '/' && !launch_dir().empty())
            cd = launch_dir() + "/" + cd;
        struct stat st;
        if (::stat(cd.c_str(), &st) != 0) {
            problem = "The game CD " + cd + " does not exist.";
            return;
        }
        if (S_ISDIR(st.st_mode)) {
            s_cd_dir = cd;
        } else {
            char magic[2] = {0, 0};
            if (FILE *f = fopen(cd.c_str(), "rb")) {
                fread(magic, 1, 2, f);
                fclose(f);
            }
            if (magic[0] == 'P' && magic[1] == 'K') {
                problem = cd + " is a zip archive: unpack it first (double-click it in the Finder), then use the "
                               ".iso file inside.";
                return;
            }
            s_iso = new Iso;
            if (!s_iso->open(cd.c_str())) {
                delete s_iso;
                s_iso = 0;
                problem = cd + " is not a CD image (ISO 9660).";
                return;
            }
        }
        // The game's CD has the installer's tree: setup\hd\binaries and setup\cd.
        Path probe = {"setup", "hd", "binaries"};
        std::string host;
        bool ok = s_iso ? s_iso->find(probe) != 0 : host_resolve(s_cd_dir, probe, host);
        if (!ok) {
            problem = cd + " does not look like the game CD (no setup\\hd\\binaries on it).";
            delete s_iso;
            s_iso = 0;
            s_cd_dir.clear();
            return;
        }
        log("vfs", "CD: %s (%s)\n", cd.c_str(), s_iso ? "image" : "directory");
    });
    if (!problem.empty())
        cd_problem(problem);
}

// ---------------------------------------------------------------------------------------

bool parse(const char *winpath, Path &out)
{
    out.clear();
    if (!winpath)
        return false;
    std::string s = winpath;
    for (auto &c : s)
        if (c == '/')
            c = '\\';
    Path base;
    size_t i = 0;
    if (s.size() >= 2 && s[1] == ':') {
        // C: is the game's drive, D: the CD drive: both show the CD (with the overlay).
        char d = (char)tolower((unsigned char)s[0]);
        if (d != 'c' && d != 'd')
            return false;
        i = 2;
        if (i >= s.size() || s[i] != '\\') {
            std::lock_guard<std::mutex> lock(s_cwd_mu);
            base = s_cwd;  // "C:name" is relative to the current directory
        }
    } else if (!s.empty() && s[0] == '\\') {
        // rooted on the current drive
    } else {
        std::lock_guard<std::mutex> lock(s_cwd_mu);
        base = s_cwd;
    }
    out = base;
    std::string comp;
    for (; i <= s.size(); i++) {
        if (i == s.size() || s[i] == '\\') {
            if (comp == "..") {
                if (!out.empty())
                    out.pop_back();
            } else if (!comp.empty() && comp != ".") {
                // Windows drops trailing dots and spaces of a name.
                while (!comp.empty() && (comp.back() == '.' || comp.back() == ' '))
                    comp.pop_back();
                if (!comp.empty())
                    out.push_back(comp);
            }
            comp.clear();
        } else {
            comp += s[i];
        }
    }
    return true;
}

std::string to_windows(const Path &p)
{
    std::string s = "C:";
    for (auto &c : p)
        s += "\\" + c;
    if (p.empty())
        s += "\\";
    return s;
}

static bool cd_stat(Path &p, Entry &e)
{
    init_cd();
    if (s_iso) {
        IsoNode *n = s_iso->find(p);
        if (!n)
            return false;
        e.name = n->name;
        e.dir = n->dir;
        e.size = n->size;
        e.mtime = n->mtime;
        return true;
    }
    return !s_cd_dir.empty() && host_stat(s_cd_dir, p, e);
}

bool stat(Path &p, Entry &e)
{
    if (p.empty()) {
        e = Entry();
        e.dir = true;
        return true;
    }
    Path q = p;
    if (host_stat(overlay_dir(), q, e)) {
        e.from_overlay = true;
        p = q;
        return true;
    }
    return cd_stat(p, e);
}

bool list(const Path &dir, std::vector<Entry> &out)
{
    out.clear();
    Entry e;
    Path p = dir;
    if (!stat(p, e) || !e.dir) {
        // The directory may exist in only one of the two.
        Path q = dir;
        if (!cd_stat(q, e) || !e.dir)
            return false;
    }
    std::vector<Entry> over, cd;
    host_list(overlay_dir(), dir, over);
    init_cd();
    if (s_iso) {
        Path q = dir;
        if (IsoNode *n = s_iso->find(q))
            for (auto &c : n->children) {
                Entry x;
                x.name = c.name;
                x.dir = c.dir;
                x.size = c.size;
                x.mtime = c.mtime;
                cd.push_back(x);
            }
    } else if (!s_cd_dir.empty()) {
        host_list(s_cd_dir, dir, cd);
    }
    for (auto &o : over) {
        o.from_overlay = true;
        out.push_back(o);
    }
    for (auto &c : cd) {
        bool shadowed = false;
        for (auto &o : over)
            if (same(o.name, c.name))
                shadowed = true;
        if (!shadowed)
            out.push_back(c);
    }
    return true;
}

bool open_read(const Path &path, Reader &r)
{
    Path p = path;
    std::string host;
    if (host_resolve(overlay_dir(), p, host)) {
        r.fd = ::open(host.c_str(), O_RDONLY);
        return r.fd >= 0;
    }
    init_cd();
    p = path;
    if (s_iso) {
        IsoNode *n = s_iso->find(p);
        if (!n || n->dir)
            return false;
        r.fd = dup(s_iso->fd());
        r.base = (uint64_t)n->extent * 2048;
        r.size = n->size;
        r.iso = true;
        return r.fd >= 0;
    }
    if (!s_cd_dir.empty() && host_resolve(s_cd_dir, p, host)) {
        r.fd = ::open(host.c_str(), O_RDONLY);
        return r.fd >= 0;
    }
    errno = ENOENT;
    return false;
}

static bool mkdirs(const std::string &host)
{
    struct stat st;
    if (::stat(host.c_str(), &st) == 0)
        return S_ISDIR(st.st_mode);
    size_t slash = host.rfind('/');
    if (slash != std::string::npos && slash > 0 && !mkdirs(host.substr(0, slash)))
        return false;
    return ::mkdir(host.c_str(), 0755) == 0 || errno == EEXIST;
}

// The overlay's spelling of p: existing overlay names first, then the CD's, then as given.
static std::string overlay_host(const Path &path)
{
    std::string cur = overlay_dir();
    Path cdp;
    for (auto &c : path) {
        std::string s;
        if (host_find(cur, c, s)) {
            cur += "/" + s;
            cdp.push_back(s);
            continue;
        }
        Path q = cdp;
        q.push_back(c);
        Entry e;
        std::string name = cd_stat(q, e) ? q.back() : c;
        cur += "/" + name;
        cdp.push_back(name);
    }
    return cur;
}

bool overlay_path(const Path &p, std::string &host, bool copy)
{
    host = overlay_host(p);
    size_t slash = host.rfind('/');
    if (!mkdirs(host.substr(0, slash)))
        return false;
    struct stat st;
    if (copy && ::stat(host.c_str(), &st) != 0) {
        Reader r;
        if (open_read(p, r)) {
            int out = ::open(host.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out >= 0) {
                char buf[65536];
                uint64_t off = 0;
                for (;;) {
                    ssize_t want = sizeof(buf);
                    if (r.iso && r.size - off < (uint64_t)want)
                        want = (ssize_t)(r.size - off);
                    if (want <= 0)
                        break;
                    ssize_t n = pread(r.fd, buf, (size_t)want, (off_t)(r.base + off));
                    if (n <= 0)
                        break;
                    write(out, buf, (size_t)n);
                    off += (uint64_t)n;
                }
                ::close(out);
            }
            ::close(r.fd);
        }
    }
    return true;
}

bool make_dir(const Path &p) { return mkdirs(overlay_host(p)); }

bool host_file(const Path &path, std::string &host)
{
    Path p = path;
    if (host_resolve(overlay_dir(), p, host))
        return true;
    init_cd();
    p = path;
    return !s_cd_dir.empty() && host_resolve(s_cd_dir, p, host);
}

// install_defaults() is in defaults.cpp.

// ---- Reading the ISO image: a small cache of 64 KB blocks for the game's many tiny reads

namespace {

struct IsoCache {
    static const size_t BLOCK = 64 * 1024;
    static const int BLOCKS = 64;  // 4 MB
    struct Block {
        bool valid = false;
        uint64_t index = 0;
        size_t length = 0;  // bytes read (less at the end of the image)
        uint64_t used = 0;
        char data[BLOCK];
    };
    std::mutex lock;
    Block *blocks = new Block[BLOCKS];
    uint64_t clock = 0;

    // The block holding byte index * BLOCK of fd, read if needed (0 on a read error).
    Block *get(int fd, uint64_t index)
    {
        Block *victim = &blocks[0];
        for (int i = 0; i < BLOCKS; i++) {
            Block &b = blocks[i];
            if (b.valid && b.index == index) {
                b.used = ++clock;
                return &b;
            }
            if (b.used < victim->used)
                victim = &b;
        }
        ssize_t n = pread(fd, victim->data, BLOCK, (off_t)(index * BLOCK));
        if (n < 0)
            return 0;
        victim->valid = true;
        victim->index = index;
        victim->length = (size_t)n;
        victim->used = ++clock;
        return victim;
    }
};

IsoCache &iso_cache()
{
    static IsoCache c;
    return c;
}

}  // namespace

ssize_t read_iso(int fd, void *buf, size_t n, uint64_t off)
{
    if (n >= IsoCache::BLOCK)
        return pread(fd, buf, n, (off_t)off);
    IsoCache &c = iso_cache();
    std::lock_guard<std::mutex> g(c.lock);
    size_t done = 0;
    while (done < n) {
        uint64_t at = off + done;
        IsoCache::Block *b = c.get(fd, at / IsoCache::BLOCK);
        if (!b)
            return done ? (ssize_t)done : -1;
        size_t in = (size_t)(at % IsoCache::BLOCK);
        if (in >= b->length)
            break;  // the end of the image
        size_t k = b->length - in < n - done ? b->length - in : n - done;
        memcpy((char *)buf + done, b->data + in, k);
        done += k;
    }
    return (ssize_t)done;
}

}  // namespace vfs

// ---------------------------------------------------------------------------------------

const std::string &data_dir()
{
    static std::string dir = [] {
        std::string d;
        if (const char *e = getenv("MICKEY_DATA"))
            d = e;
        else
            d = std::string(getenv("HOME") ? getenv("HOME") : "/tmp") +
                "/Library/Application Support/Mickey Saves the Day";
        ::mkdir(d.c_str(), 0755);
        return d;
    }();
    return dir;
}

namespace vfs {

// The `locale = ...` of a steamboat.ini text ("ger", "eng_gbr"), or "".
static std::string ini_locale(const std::string &text)
{
    size_t i = 0;
    while (i < text.size()) {
        size_t eol = text.find('\n', i);
        if (eol == std::string::npos)
            eol = text.size();
        std::string line = trim(text.substr(i, eol - i));
        i = eol + 1;
        size_t eq = line.find('=');
        if (eq != std::string::npos && same(trim(line.substr(0, eq)), "locale"))
            return trim(line.substr(eq + 1));
    }
    return "";
}

// The language of the CD (its steamboat.ini), read from the CD itself, not the overlay.
static std::string cd_locale()
{
    init_cd();
    Path p = {"setup", "hd", "resources", "universal", "ini", "steamboat.ini"};
    std::string text;
    if (s_iso) {
        if (IsoNode *n = s_iso->find(p)) {
            text.resize(n->size);
            if (pread(s_iso->fd(), &text[0], n->size, (off_t)n->extent * 2048) != (ssize_t)n->size)
                text.clear();
        }
    } else {
        std::string host;
        if (host_resolve(s_cd_dir, p, host))
            if (FILE *f = fopen(host.c_str(), "rb")) {
                char buf[4096];
                size_t n;
                while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
                    text.append(buf, n);
                fclose(f);
            }
    }
    std::string l = ini_locale(text);
    for (auto &c : l)
        if (!isalnum((unsigned char)c) && c != '_')
            c = '_';
    return l.empty() ? "cd" : l;
}

}  // namespace vfs

const std::string &overlay_dir()
{
    static std::string dir = [] {
        std::string locale = vfs::cd_locale();
        const std::string &data = data_dir();
        std::string d = data + "/" + locale;
        struct stat st;
        // Before a directory per language, the overlay was the data directory itself:
        // its files move to the directory of the language they were made for.
        if (::stat(d.c_str(), &st) != 0 && ::stat((data + "/setup").c_str(), &st) == 0) {
            std::string text;
            if (FILE *f = fopen((data + "/setup/hd/binaries/steamboat.ini").c_str(), "rb")) {
                char buf[4096];
                size_t n;
                while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
                    text.append(buf, n);
                fclose(f);
            }
            std::string was = vfs::ini_locale(text);
            if (was.empty() || was == locale) {
                ::mkdir(d.c_str(), 0755);
                for (const char *name : {"setup", ".cd-cache", "registry.ini"})
                    ::rename((data + "/" + name).c_str(), (d + "/" + name).c_str());
                log("vfs", "moved the game's files of %s into %s\n", locale.c_str(), d.c_str());
            }
        }
        ::mkdir(d.c_str(), 0755);
        // the language of the port's own texts and the locale queries, unless set
        setenv("MICKEY_LANG", locale.c_str(), 0);
        return d;
    }();
    return dir;
}

std::string current_dir()
{
    std::lock_guard<std::mutex> lock(vfs::s_cwd_mu);
    std::string s = vfs::to_windows(vfs::s_cwd);
    if (s.back() != '\\')
        s += "\\";
    return s;
}

std::string full_path(const char *winpath)
{
    vfs::Path p;
    if (!vfs::parse(winpath, p))
        return winpath ? winpath : "";
    return vfs::to_windows(p);
}

bool set_current_dir(const char *winpath)
{
    vfs::Path p;
    vfs::Entry e;
    if (!vfs::parse(winpath, p) || !vfs::stat(p, e) || !e.dir)
        return false;
    std::lock_guard<std::mutex> lock(vfs::s_cwd_mu);
    vfs::s_cwd = p;
    return true;
}

}  // namespace win32
