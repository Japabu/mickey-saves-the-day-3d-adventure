// resource.cpp - module resources, read from the original binaries' .rsrc sections on the
// user's CD (C:\setup\hd\binaries\<dll>, the plugins in its plugins directory): FindResource,
// LoadResource, LockResource and SizeofResource on the raw data (the version block
// MfVersionResource binds to, dialog templates, ...), LoadString from the string tables,
// and bitmaps from the packed DIBs. A module's file is read once and kept; HRSRC points to
// its entry, HGLOBAL to its data.
//
// Icons and cursors have no images in the port (the window has no icon and SDL draws the
// cursor): LoadImage of an icon or cursor answers with LoadIcon/LoadCursor's handle when
// the module has the resource.
#include "internal.h"
#include "vfs.h"
#include <map>
#include <memory>
#include <mutex>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <vector>

namespace win32 {
const char *module_dll_name(HMODULE h);  // module.cpp

namespace {

struct Res {
    const uint8_t *data;
    DWORD size;
};

struct Image {
    std::vector<uint8_t> file;
    size_t rsrc = 0;      // file offset of the resource directory, 0 for none
    uint32_t rsrc_rva = 0;
    std::vector<uint32_t> sec_va, sec_size, sec_raw;
    std::map<size_t, std::unique_ptr<Res>> entries;  // by the data entry's file offset

    template <class T> bool get(size_t off, T &v) const
    {
        if (off > file.size() || file.size() - off < sizeof(T))
            return false;
        memcpy(&v, file.data() + off, sizeof(T));
        return true;
    }

    // An RVA as a file offset (0 when no section holds it).
    size_t to_off(uint32_t rva) const
    {
        for (size_t i = 0; i < sec_va.size(); i++)
            if (rva >= sec_va[i] && rva - sec_va[i] < sec_size[i])
                return (size_t)(rva - sec_va[i]) + sec_raw[i];
        return 0;
    }

    bool parse()
    {
        uint32_t pe = 0;
        uint16_t nsec = 0, optsize = 0, magic = 0;
        if (file.size() < 0x40 || file[0] != 'M' || file[1] != 'Z' || !get(0x3c, pe) || file.size() - 4 < pe ||
            memcmp(file.data() + pe, "PE\0\0", 4) != 0)
            return false;
        if (!get(pe + 6, nsec) || !get(pe + 20, optsize) || !get(pe + 24, magic))
            return false;
        size_t dirs = pe + 24 + (magic == 0x20b ? 112 : 96);
        uint32_t size = 0;
        if (!get(dirs + 2 * 8, rsrc_rva) || !get(dirs + 2 * 8 + 4, size))
            return false;
        for (uint16_t i = 0; i < nsec; i++) {
            size_t s = pe + 24 + optsize + 40 * (size_t)i;
            uint32_t vsize = 0, va = 0, rawsize = 0, raw = 0;
            if (!get(s + 8, vsize) || !get(s + 12, va) || !get(s + 16, rawsize) || !get(s + 20, raw))
                return false;
            sec_va.push_back(va);
            sec_size.push_back(vsize > rawsize ? vsize : rawsize);
            sec_raw.push_back(raw);
        }
        if (rsrc_rva && size)
            rsrc = to_off(rsrc_rva);
        return true;
    }

    // The entry of a directory that matches id (IS_INTRESOURCE) or name (case-insensitive,
    // "#123" for an id); with neither, the first entry (the language level). Returns the
    // entry's offset field, or ~0u.
    uint32_t find(size_t dir, LPCSTR name, bool first) const
    {
        uint16_t named = 0, ids = 0;
        if (!get(dir + 12, named) || !get(dir + 14, ids))
            return ~0u;
        uintptr_t id = 0;
        bool by_id = false;
        if (!first) {
            if (IS_INTRESOURCE(name)) {
                id = (uintptr_t)name;
                by_id = true;
            } else if (name[0] == '#') {
                id = (uintptr_t)strtoul(name + 1, 0, 10);
                by_id = true;
            }
        }
        for (unsigned i = 0; i < (unsigned)named + ids; i++) {
            uint32_t n = 0, o = 0;
            if (!get(dir + 16 + 8 * (size_t)i, n) || !get(dir + 20 + 8 * (size_t)i, o))
                return ~0u;
            if (first)
                return o;
            if (n & 0x80000000) {
                if (by_id)
                    continue;
                size_t p = rsrc + (n & 0x7fffffff);
                uint16_t len = 0;
                if (!get(p, len) || strlen(name) != len)
                    continue;
                bool same = true;
                for (unsigned k = 0; k < len && same; k++) {
                    uint16_t c = 0;
                    get(p + 2 + 2 * (size_t)k, c);
                    unsigned char a = (unsigned char)name[k];
                    same = c < 0x80 && toupper(a) == toupper((int)c);
                }
                if (same)
                    return o;
            } else if (by_id && n == id) {
                return o;
            }
        }
        return ~0u;
    }

    // type / name / first language: the data entry.
    Res *lookup(LPCSTR type, LPCSTR name)
    {
        if (!rsrc)
            return 0;
        uint32_t o = find(rsrc, type, false);
        if (o == ~0u || !(o & 0x80000000))
            return 0;
        o = find(rsrc + (o & 0x7fffffff), name, false);
        if (o == ~0u || !(o & 0x80000000))
            return 0;
        o = find(rsrc + (o & 0x7fffffff), 0, true);
        if (o == ~0u || (o & 0x80000000))
            return 0;
        size_t entry = rsrc + o;
        auto it = entries.find(entry);
        if (it != entries.end())
            return it->second.get();
        uint32_t rva = 0, size = 0;
        if (!get(entry, rva) || !get(entry + 4, size))
            return 0;
        size_t off = to_off(rva);
        if (!off || off > file.size() || file.size() - off < size)
            return 0;
        Res *r = new Res{file.data() + off, size};
        entries[entry].reset(r);
        return r;
    }
};

std::recursive_mutex s_mu;
std::map<std::string, std::unique_ptr<Image>> s_images;  // by the lower-case DLL name

bool read_file(const char *winpath, std::vector<uint8_t> &out)
{
    vfs::Path p;
    vfs::Reader r;
    if (!vfs::parse(winpath, p) || !vfs::open_read(p, r))
        return false;
    uint8_t buf[65536];
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
        out.insert(out.end(), buf, buf + n);
        off += (uint64_t)n;
    }
    ::close(r.fd);
    return true;
}

// The module's image (null when its binary is not on the CD or has no resources).
Image *image_of(HMODULE h)
{
    std::string dll = module_dll_name(h);
    std::string key = dll;
    for (char &c : key)
        c = (char)tolower((unsigned char)c);
    auto it = s_images.find(key);
    if (it != s_images.end())
        return it->second.get();
    std::unique_ptr<Image> img(new Image);
    static const char *dirs[] = {"C:\\setup\\hd\\binaries\\", "C:\\setup\\hd\\binaries\\plugins\\"};
    bool found = false;
    for (const char *d : dirs)
        if (read_file((std::string(d) + dll).c_str(), img->file)) {
            found = true;
            break;
        }
    if (!found || !img->parse()) {
        log("resource", "%s: no binary with resources on the CD\n", dll.c_str());
        img.reset();
    }
    Image *r = img.get();
    s_images[key] = std::move(img);
    return r;
}

Res *find(HMODULE h, LPCSTR name, LPCSTR type)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    Image *img = image_of(h);
    Res *r = img ? img->lookup(type, name) : 0;
    if (!r)
        log("resource", "%s: no resource %s%lu of type %lu\n", module_dll_name(h), IS_INTRESOURCE(name) ? "#" : "",
            IS_INTRESOURCE(name) ? (unsigned long)(uintptr_t)name : 0ul, (unsigned long)(uintptr_t)type);
    return r;
}

// A packed DIB (BITMAPINFOHEADER, colour table, bits) as a DIB section or a screen bitmap.
HBITMAP bitmap_from_dib(const uint8_t *dib, DWORD size, UINT flags)
{
    BITMAPINFOHEADER h;
    if (size < sizeof(h))
        return 0;
    memcpy(&h, dib, sizeof(h));
    if (h.biSize < sizeof(h) || h.biSize > size || (h.biCompression != BI_RGB && h.biCompression != BI_BITFIELDS))
        return 0;
    size_t colors = h.biClrUsed ? h.biClrUsed : (h.biBitCount <= 8 ? (size_t)1 << h.biBitCount : 0);
    size_t table = colors * sizeof(RGBQUAD);
    if (h.biCompression == BI_BITFIELDS && h.biSize == sizeof(h))
        table += 3 * sizeof(DWORD);
    int rows = h.biHeight < 0 ? -h.biHeight : h.biHeight;
    size_t pitch = (((size_t)h.biWidth * h.biBitCount + 31) / 32) * 4;
    size_t head = h.biSize + table;
    if (head > size || size - head < pitch * (size_t)rows)
        return 0;
    // the header and table copied so that BITMAPINFO is aligned
    std::vector<uint8_t> info(dib, dib + head);
    const BITMAPINFO *bmi = (const BITMAPINFO *)info.data();
    const uint8_t *bits = dib + head;
    if (flags & LR_CREATEDIBSECTION) {
        void *dst = 0;
        HBITMAP hb = CreateDIBSection(0, bmi, DIB_RGB_COLORS, &dst, 0, 0);
        if (hb && dst)
            memcpy(dst, bits, pitch * (size_t)rows);
        return hb;
    }
    HDC screen = GetDC(0);
    HBITMAP hb = CreateDIBitmap(screen, &bmi->bmiHeader, CBM_INIT, bits, bmi, DIB_RGB_COLORS);
    ReleaseDC(0, screen);
    return hb;
}

}  // namespace
}  // namespace win32

using namespace win32;

extern "C" {

HRSRC WINAPI FindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType)
{
    Res *r = find(hModule, lpName, lpType);
    if (!r)
        SetLastError(1814);  // ERROR_RESOURCE_NAME_NOT_FOUND
    return (HRSRC)r;
}

HGLOBAL WINAPI LoadResource(HMODULE, HRSRC hResInfo)
{
    return hResInfo ? (HGLOBAL)((Res *)hResInfo)->data : 0;
}

LPVOID WINAPI LockResource(HGLOBAL hResData) { return hResData; }

DWORD WINAPI SizeofResource(HMODULE, HRSRC hResInfo) { return hResInfo ? ((Res *)hResInfo)->size : 0; }

// String tables: block id/16 + 1 holds 16 counted UTF-16 strings.
int WINAPI LoadStringA(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    if (cchBufferMax <= 0 || !lpBuffer)
        return 0;
    lpBuffer[0] = 0;
    Res *r = find(hInstance, MAKEINTRESOURCEA((uID >> 4) + 1), (LPCSTR)RT_STRING);
    if (!r)
        return 0;
    size_t p = 0;
    for (unsigned i = 0;; i++) {
        uint16_t len = 0;
        if (p + 2 > r->size)
            return 0;
        memcpy(&len, r->data + p, 2);
        p += 2;
        if (p + 2 * (size_t)len > r->size)
            return 0;
        if (i == (uID & 15)) {
            if (!len)
                return 0;
            std::vector<WCHAR> w(len);
            memcpy(w.data(), r->data + p, 2 * (size_t)len);
            int n = WideCharToMultiByte(CP_ACP, 0, w.data(), len, 0, 0, 0, 0);
            std::string s((size_t)n, 0);
            WideCharToMultiByte(CP_ACP, 0, w.data(), len, &s[0], n, 0, 0);
            if (n > cchBufferMax - 1)
                n = cchBufferMax - 1;
            memcpy(lpBuffer, s.data(), (size_t)n);
            lpBuffer[n] = 0;
            return n;
        }
        p += 2 * (size_t)len;
    }
}

// gdi.cpp: a .bmp file as a bitmap or DIB section
HBITMAP port_load_bitmap_file(const char *path, UINT flags);

HANDLE WINAPI LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int, int, UINT flags)
{
    if (flags & LR_LOADFROMFILE) {
        if (type == IMAGE_BITMAP && !IS_INTRESOURCE(name))
            return port_load_bitmap_file(name, flags);
        log("resource", "LoadImage(%s, %u) from a file: not available\n", IS_INTRESOURCE(name) ? "#id" : name, type);
        return 0;
    }
    switch (type) {
    case IMAGE_BITMAP: {
        if (!hInst && IS_INTRESOURCE(name))
            return 0;  // the system's OEM bitmaps
        Res *r = find(hInst, name, (LPCSTR)RT_BITMAP);
        return r ? bitmap_from_dib(r->data, r->size, flags) : 0;
    }
    case IMAGE_ICON:
        if (hInst && !find(hInst, name, (LPCSTR)RT_GROUP_ICON))
            return 0;
        return LoadIconA(hInst, name);
    case IMAGE_CURSOR:
        if (hInst && !find(hInst, name, (LPCSTR)RT_GROUP_CURSOR))
            return 0;
        return LoadCursorA(hInst, name);
    }
    return 0;
}

HBITMAP WINAPI LoadBitmapA(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
    return (HBITMAP)LoadImageA(hInstance, lpBitmapName, IMAGE_BITMAP, 0, 0, 0);
}

}  // extern "C"
