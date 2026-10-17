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

// ---- Improvements of the game's dialogue files (an improvement over the original)

// Goofy reads the recipe on Von Drake's tissue only once (PPdialogue.ini, PPSection6_3, which
// clears the flags that lead to it), and the soup machine needs it. Using the tissue under
// the magnifying glass (PRmglassAndHankie, which the original gives to nobody) on him makes
// him read it again: a block of his four lines, first in the list so that it wins.
static const char kRecipeMarker[] = "[blockPortRecipeAgain]";

static std::string improve_park_dialogue(const std::string &text)
{
    if (text.find(kRecipeMarker) != std::string::npos)
        return text;
    std::string out;
    size_t i = 0;
    int last_prop = -1;
    bool in_props = false, in_dialogue = false, added_prop = false, added_block = false;
    while (i < text.size()) {
        size_t eol = text.find('\n', i);
        std::string line = text.substr(i, eol == std::string::npos ? std::string::npos : eol - i + 1);
        i = eol == std::string::npos ? text.size() : eol + 1;
        std::string t = trim(line.substr(0, line.find('\n')));
        if (!t.empty() && t[0] == '[') {
            in_props = same(t, "[DialogueProperties]");
            in_dialogue = same(t, "[Dialogue]");
        }
        int n;
        if (in_props && sscanf(t.c_str(), "prop%d", &n) == 1) {
            last_prop = n > last_prop ? n : last_prop;
        } else if (in_props && !added_prop && last_prop >= 0) {
            // after the last propN: declare the item's flag
            out += "prop" + std::to_string(last_prop + 1) + " = PRmglassAndHankie\r\n";
            added_prop = true;
        }
        if (in_dialogue && sscanf(t.c_str(), "dblock%d", &n) == 1) {
            if (!added_block) {
                out += "dblock0 = blockPortRecipeAgain\r\n";
                added_block = true;
            }
            size_t eq = t.find('=');
            out += "dblock" + std::to_string(n + 1) + " " + t.substr(eq) + "\r\n";
            continue;
        }
        out += line;
    }
    if (!added_prop || !added_block)
        return text;  // not the file this was written for
    out += "\r\n"
           "[blockPortRecipeAgain]\r\n"
           "lineGroup0 = groupPortRecipeAgain\r\n"
           "conditional = PRmglassAndHankie\r\n"
           "section id = PPSection6_3Again\r\n"
           "trigger = GDgoofy\r\n"
           "block type = in order\r\n"
           "\r\n"
           "[groupPortRecipeAgain]\r\n"
           "line0 = pp06xgf0050\r\n"
           "line1 = pp06xgf0060\r\n"
           "line2 = pp06xxx0070\r\n"
           "line3 = pp06xgf0080\r\n"
           "false = PRmglassAndHankie\r\n";
    return out;
}

// The improved text of one of the game's ini files (the text unchanged if it has none).
static std::string improve(const std::string &name, const std::string &text)
{
    if (same(name, "PPdialogue.ini"))
        return improve_park_dialogue(text);
    return text;
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
        if (stat(probe, e) && e.from_overlay) {
            // installed before: it still gets the improvements made since
            std::string text, host;
            if (read_all(dst, text) && host_file(dst, host)) {
                std::string better = improve(f.name, text);
                log("defaults", "%s: %s\n", f.name.c_str(), better != text ? "improved" : "as it is");
                if (better != text)
                    if (FILE *out = fopen(host.c_str(), "wb")) {
                        fwrite(better.data(), 1, better.size(), out);
                        fclose(out);
                    }
            }
            continue;
        }
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
        text = improve(f.name, text);
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
