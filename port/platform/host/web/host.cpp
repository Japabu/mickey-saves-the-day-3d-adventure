// host.cpp - the host in a browser (Emscripten): the game runs on a worker
// (PROXY_TO_PTHREAD) and shares a block of memory with the page (shared.h,
// port/web/mickey.js), which draws the frames, collects the input and plays the sound.
// The CD image and the data directory are files in the browser's private storage for
// the site (OPFS, through WasmFS): the page copies the ISO there once, saved games stay.
#include "../host.h"
#include "shared.h"

#include <emscripten.h>
#include <emscripten/threading.h>
#include <emscripten/wasmfs.h>

#include <stdlib.h>
#include <string.h>

namespace host {
namespace web {

Shared &shared()
{
    static Shared s;
    return s;
}

}  // namespace web

using namespace web;

extern "C" EMSCRIPTEN_KEEPALIVE Shared *host_web_shared() { return &shared(); }

static std::atomic<int32_t> *F() { return shared().f; }

void startup()
{
    Shared &s = shared();
    s.f[MAGIC] = 0x4D49434B;
    s.f[FRAME_FRONT] = -1;
    s.f[FRAME_READING] = -1;
    s.f[MOUSE_X] = -1;
    s.f[MOUSE_Y] = -1;
    s.f[GAME_W] = 640;
    s.f[GAME_H] = 480;
    // the page put the CD at /opfs/cd.iso; everything the game writes goes to /opfs/data
    backend_t opfs = wasmfs_create_opfs_backend();
    wasmfs_create_directory("/opfs", 0777, opfs);
    setenv("MICKEY_CD", "/opfs/cd.iso", 0);
    setenv("MICKEY_DATA", "/opfs/data", 0);
}

// ---- The window: the page's canvas

bool window_open(const char *, int w, int h, bool fullscreen)
{
    F()[GAME_W] = w > 0 ? w : 640;
    F()[GAME_H] = h > 0 ? h : 480;
    F()[FULLSCREEN_WANT] = fullscreen;
    return true;
}

void window_close() { F()[WINDOW_SHOWN] = 0; }
void window_show(bool shown) { F()[WINDOW_SHOWN] = shown; }
void window_title(const char *) {}
void window_fullscreen(bool on) { F()[FULLSCREEN_WANT] = on; }
bool window_is_fullscreen() { return F()[FULLSCREEN_IS] != 0; }
void window_minimize(bool) {}
bool window_is_minimized() { return false; }
bool window_visible() { return F()[WINDOW_SHOWN] && F()[PAGE_VISIBLE]; }

// Three buffers: the page copies the newest (FRAME_FRONT) while the next is written into
// one it is not reading.
static uint32_t *s_buf[3];
static size_t s_buf_size[3];

void present(const uint32_t *argb, int w, int h, int scale, bool smooth)
{
    std::atomic<int32_t> *f = F();
    int front = f[FRAME_FRONT], reading = f[FRAME_READING];
    int i = 0;
    while (i == front || i == reading)
        i++;
    size_t n = (size_t)w * (size_t)h;
    if (s_buf_size[i] < n) {
        free(s_buf[i]);
        s_buf[i] = (uint32_t *)malloc(n * 4);
        s_buf_size[i] = n;
    }
    memcpy(s_buf[i], argb, n * 4);
    f[FRAME_PTR + i] = (int32_t)(uintptr_t)s_buf[i];
    f[FRAME_W + i] = w;
    f[FRAME_H + i] = h;
    f[FRAME_SCALE + i] = scale;
    f[SMOOTH] = smooth;
    f[GAME_W] = w / scale;
    f[GAME_H] = h / scale;
    f[FRAME_FRONT] = i;
    f[FRAME_SEQ]++;
}

void cursor_visible(bool visible) { F()[CURSOR_VISIBLE] = visible; }

// A page cannot move the pointer: the game's cursor goes where the game says until the
// pointer moves.
void cursor_move(int, int) {}

bool cursor_position(int *x, int *y)
{
    int mx = F()[MOUSE_X], my = F()[MOUSE_Y];
    if (mx < 0)
        return false;
    *x = mx;
    *y = my;
    return true;
}

// ---- Events: the page's ring

bool poll_event(Event *e)
{
    Shared &s = shared();
    int32_t tail = s.f[EV_TAIL];
    if (tail == s.f[EV_HEAD])
        return false;
    const int32_t *r = s.events[(uint32_t)tail % EVENTS];
    memset(e, 0, sizeof(*e));
    e->type = (Event::Type)r[E_TYPE];
    e->x = r[E_X];
    e->y = r[E_Y];
    e->button = r[E_BUTTON];
    e->clicks = r[E_CLICKS];
    e->wheel = r[E_WHEEL_MILLI] / 1000.0f;
    e->vk = r[E_VK];
    e->side_vk = r[E_SIDE_VK];
    e->scancode = r[E_SCANCODE];
    e->repeat = (r[E_FLAGS] & E_FLAG_REPEAT) != 0;
    e->command = (r[E_FLAGS] & E_FLAG_COMMAND) != 0;
    memcpy(e->text, &r[E_TEXT], sizeof(e->text) - 1);
    s.f[EV_TAIL] = tail + 1;
    return true;
}

void wait_event(int ms)
{
    Shared &s = shared();
    int32_t head = s.f[EV_HEAD];
    if (head != s.f[EV_TAIL])
        return;
    emscripten_futex_wait(&s.f[EV_HEAD], (uint32_t)head, ms);
}

}  // namespace host
