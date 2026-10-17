// display.cpp - the game's top-level window on the host's window (host.h): the frames of
// the DirectDraw layer (port_frame_changed, port_frame_pixels) shown at most once per
// display refresh, full screen or not, and the "screen" size the game sees.
#include "user.h"
#include "../port.h"
#include "../host/host.h"

#include <atomic>
#include <chrono>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace win32 {

static Window *s_window;   // the window the host shows

// The primary surface changed since the window last showed it (port_frame_changed);
// shown when the game's thread next pumps messages or waits for the vertical blank.
static std::atomic<bool> s_frame_new;

int display_width()
{
    if (const char *e = getenv("MICKEY_SCREEN_W"))
        return atoi(e);
    return 640;
}

int display_height()
{
    if (const char *e = getenv("MICKEY_SCREEN_H"))
        return atoi(e);
    return 480;
}

static int s_fullscreen_request = -1;  // from port_set_fullscreen; -1 until the game says

static bool fullscreen_wanted()
{
    const char *e = getenv("MICKEY_WINDOWED");
    if (e && *e && *e != '0')
        return false;
    int chosen = port_setting("fullscreen", -1);  // the options panel's choice
    if (chosen >= 0)
        return chosen != 0;
    return s_fullscreen_request != 0;
}

static std::string utf8_title(const std::string &ansi)
{
    int n = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, 0, 0);
    std::vector<WCHAR> wt((size_t)n);
    MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, wt.data(), n);
    int nb = WideCharToMultiByte(CP_UTF8, 0, wt.data(), n, 0, 0, 0, 0);
    std::string title((size_t)nb, 0);
    WideCharToMultiByte(CP_UTF8, 0, wt.data(), n, &title[0], nb, 0, 0);
    return title.c_str();
}

bool display_open() { return s_window != 0; }

void display_show(Window *w)
{
    if (!w->on_host) {
        if (s_window)
            return;  // the host shows one window, the game's first
        int cw = w->client_w ? w->client_w : 640, ch = w->client_h ? w->client_h : 480;
        if (!host::window_open(utf8_title(w->text).c_str(), cw, ch, fullscreen_wanted()))
            return;
        w->on_host = true;
        s_window = w;
        host::cursor_visible(false);  // the game draws its own cursor
    }
    // PORT_HIDDEN=1 (tests): the window stays hidden and the game is told it is active,
    // as a shown window with the focus would make it
    static const bool hidden = getenv("PORT_HIDDEN") && atoi(getenv("PORT_HIDDEN"));
    if (hidden) {
        PostMessageA((HWND)w, WM_ACTIVATEAPP, TRUE, 0);
        PostMessageA((HWND)w, WM_ACTIVATE, WA_ACTIVE, 0);
        return;
    }
    host::window_show(true);
}

void display_hide(Window *w)
{
    if (w->on_host)
        host::window_show(false);
}

void display_destroy(Window *w)
{
    if (!w->on_host)
        return;
    host::window_close();
    w->on_host = false;
    s_window = 0;
}

void display_title(Window *w)
{
    if (w->on_host)
        host::window_title(utf8_title(w->text).c_str());
}

// PORT_SHOTS="<dir>:<ms>": every <ms> milliseconds the shown frame is saved as
// <dir>/shot_<ms since start>.bmp, for tests that look at the picture.
static const char *s_shots = getenv("PORT_SHOTS");
static std::string s_shot_dir;
static DWORD s_shot_every, s_shot_start, s_shot_next;

static bool shots_due()
{
    if (!s_shots || !*s_shots)
        return false;
    if (s_shot_dir.empty()) {
        const char *colon = strrchr(s_shots, ':');
        s_shot_dir = colon ? std::string(s_shots, colon - s_shots) : std::string(s_shots);
        s_shot_every = colon ? (DWORD)atoi(colon + 1) : 1000;
        s_shot_start = s_shot_next = ticks();
    }
    return (int)(ticks() - s_shot_next) >= 0;
}

// A 32-bit BMP, bottom row first.
static void write_bmp(const char *path, const uint32_t *px, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return;
    uint32_t size = 54 + (uint32_t)w * (uint32_t)h * 4;
    uint8_t hd[54] = {'B', 'M'};
    auto put32 = [&](int at, uint32_t v) { memcpy(hd + at, &v, 4); };
    put32(2, size);
    put32(10, 54);
    put32(14, 40);
    put32(18, (uint32_t)w);
    put32(22, (uint32_t)h);
    hd[26] = 1;
    hd[28] = 32;
    put32(34, size - 54);
    fwrite(hd, 1, sizeof(hd), f);
    for (int y = h - 1; y >= 0; y--)
        fwrite(px + (size_t)y * (size_t)w, 4, (size_t)w, f);
    fclose(f);
}

static void save_shot(const uint32_t *frame, int w, int h)
{
    if (!shots_due())
        return;
    DWORD now = ticks();
    s_shot_next = now + s_shot_every;
    char path[1024];
    snprintf(path, sizeof(path), "%s/shot_%06u.bmp", s_shot_dir.c_str(), (unsigned)(now - s_shot_start));
    write_bmp(path, frame, w, h);
}

// Shows the latest frame, if there is a new one. Only the main thread draws (the host's
// window belongs to it); frames presented by other threads (the game's cursor thread)
// are shown by the main thread's next message pump.
static void show_frame()
{
    if (!s_window || !on_main_thread())
        return;
    if (!s_frame_new)
        return;
    // at most one frame per 60 Hz refresh (real time, also on the virtual clock); a newer
    // one waits for the next pump
    static std::chrono::steady_clock::time_point s_last;
    auto now = std::chrono::steady_clock::now();
    if (now - s_last < std::chrono::milliseconds(16))
        return;
    // nothing to show while the window can't be seen: the frame stays new for later
    // (PORT_SHOTS still saves it)
    bool unseen = !host::window_visible();
    if (unseen && !shots_due())
        return;
    s_last = now;
    if (!unseen)
        s_frame_new = false; // a change while the pixels are taken shows next time
    const uint32_t *pixels;
    int w, h, scale;
    if (!port_frame_pixels(&pixels, &w, &h, &scale))
        return;
    options_draw((uint32_t *)pixels, w, h, scale);  // a copy made for this frame
    save_shot(pixels, w, h);
    if (unseen)
        return;
    static const bool smooth = getenv("MICKEY_SMOOTH") != 0;
    host::present(pixels, w, h, scale, smooth);
}

void display_pump() { show_frame(); }

}  // namespace win32

using namespace win32;

extern "C" {

void port_frame_changed(void)
{
    // Shown by the next pump, between two frames of the game: it copies a frame to the
    // screen in pieces (the dirty rectangles: background back where a character was, the
    // character where it is now), and showing one in between makes the characters jump.
    s_frame_new = true;
}

void port_set_fullscreen(int fullscreen)
{
    s_fullscreen_request = fullscreen != 0;
    if (s_window)
        host::window_fullscreen(fullscreen_wanted());
}

void port_wait_vblank(void)
{
    if (s_frame_new && on_main_thread()) {
        show_frame();
        return;
    }
    // Sleep to the next 60 Hz boundary.
    DWORD now = ticks();
    DWORD next = (now / 16 + 1) * 16;
    Sleep(next - now);
}

}  // extern "C"
