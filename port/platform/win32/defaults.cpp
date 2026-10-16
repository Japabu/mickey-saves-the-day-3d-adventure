// defaults.cpp - what the game's installer left in the binaries directory, made on first
// start in the overlay: every file of resources\universal\ini (engine.ini, steamboat.ini,
// the scenes' <code>scene.ini and <code>dialogue.ini, gamestate.ini, steamboat.stl), which the
// game opens relative to its start directory C:\setup\hd\binaries, with steamboat.ini's
// [directories] written for the install location.
//
// steamboat.ini's directories are joined to its other entries as they are (ReadPrefs:
// hd + "\resources", hd + "\resources\universal\mofa", cd + "\resources"): the shipped
// values of mofa, fly and muffin begin with a backslash, so hd and cd end without one.
#include "internal.h"
#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

namespace win32 {
namespace vfs {

static std::string trim(const std::string &s)
{
    size_t b = s.find_first_not_of(" \t\r");
    if (b == std::string::npos)
        return "";
    size_t e = s.find_last_not_of(" \t\r");
    return s.substr(b, e - b + 1);
}

static bool same(const std::string &a, const char *b) { return strcasecmp(a.c_str(), b) == 0; }

// The whole file through the CD/overlay reader.
static bool read_all(const Path &p, std::string &text)
{
    Reader r;
    if (!open_read(p, r))
        return false;
    char buf[4096];
    uint64_t off = 0;
    for (;;) {
        size_t want = sizeof(buf);
        if (r.iso) {
            if (off >= r.size)
                break;
            if (r.size - off < want)
                want = (size_t)(r.size - off);
        }
        ssize_t n = pread(r.fd, buf, want, (off_t)(r.base + off));
        if (n <= 0)
            break;
        text.append(buf, (size_t)n);
        off += (uint64_t)n;
    }
    ::close(r.fd);
    return true;
}

// steamboat.ini's [directories] with hd and cd as the installer wrote them (the shipped copy
// names cd relative to the CD root and leaves hd out).
static std::string set_directories(const std::string &text)
{
    std::string out;
    bool in_dirs = false;
    size_t i = 0;
    while (i < text.size()) {
        size_t eol = text.find('\n', i);
        if (eol == std::string::npos)
            eol = text.size();
        std::string line = text.substr(i, eol - i);
        i = eol + 1;
        std::string t = trim(line);
        if (!t.empty() && t[0] == '[') {
            in_dirs = same(t, "[directories]");
            out += line + "\n";
            if (in_dirs)
                out += "hd       = C:\\setup\\hd\r\ncd       = C:\\setup\\cd\r\n";
            continue;
        }
        if (in_dirs) {
            size_t eq = t.find('=');
            std::string key = eq == std::string::npos ? "" : trim(t.substr(0, eq));
            if (same(key, "hd") || same(key, "cd"))
                continue;
        }
        out += line + "\n";
    }
    return out;
}

// MICKEY_WINDOWED (not 0) on first start: engine.ini's Fullscreen = 0.
static void set_windowed(std::string &text)
{
    size_t at = text.find("\nFullscreen");
    if (at == std::string::npos)
        return;
    size_t eq = text.find('=', at), eol = text.find('\n', at + 1);
    if (eq != std::string::npos && eol != std::string::npos && eq < eol)
        text.replace(eq + 1, eol - eq - 1, " 0\r");
}

void install_defaults()
{
    Path ini = {"setup", "hd", "resources", "universal", "ini"};
    std::vector<Entry> files;
    if (!list(ini, files)) {
        log("defaults", "no resources\\universal\\ini on the CD\n");
        return;
    }
    for (const Entry &f : files) {
        if (f.dir)
            continue;
        Path dst = {"setup", "hd", "binaries", f.name};
        Path probe = dst;
        Entry e;
        if (stat(probe, e) && e.from_overlay)
            continue;
        Path src = ini;
        src.push_back(f.name);
        std::string text;
        if (!read_all(src, text))
            continue;
        if (same(f.name, "steamboat.ini"))
            text = set_directories(text);
        const char *windowed = getenv("MICKEY_WINDOWED");
        if (same(f.name, "engine.ini") && windowed && *windowed && *windowed != '0')
            set_windowed(text);
        std::string host;
        if (!overlay_path(dst, host, false))
            continue;
        FILE *out = fopen(host.c_str(), "wb");
        if (out) {
            fwrite(text.data(), 1, text.size(), out);
            fclose(out);
        }
    }
}

}  // namespace vfs
}  // namespace win32
