// display.cpp - the game window on screen: one SDL window and renderer, the frames of
// the DirectDraw layer (port_frame_changed, port_frame_pixels), scaled to fit with
// letterboxing, vsync, and the "screen" size the game sees.
#include <SDL3/SDL.h>

#include "user.h"
#include "../port.h"

#include <atomic>
#include <mutex>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace win32 {

static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static int s_tex_w, s_tex_h, s_tex_scale;
static Window *s_window;

// The primary surface changed since the window last showed it (port_frame_changed);
// shown when the game's thread next pumps messages or waits for the vertical blank.
static std::atomic<bool> s_frame_new;

static void init_video()
{
    if (SDL_WasInit(SDL_INIT_VIDEO))
        return;
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
}

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

void display_show(Window *w)
{
    init_video();
    if (!w->sdl) {
        int cw = w->client_w ? w->client_w : 640, ch = w->client_h ? w->client_h : 480;
        std::string title;
        int n = MultiByteToWideChar(CP_ACP, 0, w->text.c_str(), -1, 0, 0);
        std::vector<WCHAR> wt((size_t)n);
        MultiByteToWideChar(CP_ACP, 0, w->text.c_str(), -1, wt.data(), n);
        int nb = WideCharToMultiByte(CP_UTF8, 0, wt.data(), n, 0, 0, 0, 0);
        title.resize((size_t)nb);
        WideCharToMultiByte(CP_UTF8, 0, wt.data(), n, &title[0], nb, 0, 0);
        SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (getenv("PORT_HIDDEN") && atoi(getenv("PORT_HIDDEN")))
            flags |= SDL_WINDOW_HIDDEN;
        // The window is twice the game's size on a large display, and stays 4:3.
        w->sdl = SDL_CreateWindow(title.c_str(), cw * 2, ch * 2, flags);
        if (!w->sdl) {
            fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
            return;
        }
        // resizing keeps the game's shape, so the picture fills the window while it is
        // dragged and after (no stretching, no bars)
        if (ch > 0)
            SDL_SetWindowAspectRatio(w->sdl, (float)cw / ch, (float)cw / ch);
        if (!s_renderer) {
            s_renderer = SDL_CreateRenderer(w->sdl, 0);
            if (!s_renderer)
                fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
            else
                // no vsync wait: presenting blocks the game's thread (its message loop
                // presents), and Metal blocks for long while the window is covered
                SDL_SetRenderVSync(s_renderer, 0);
        }
        if (s_renderer)
            SDL_SetRenderLogicalPresentation(s_renderer, cw, ch, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        if (fullscreen_wanted())
            SDL_SetWindowFullscreen(w->sdl, true);
        s_window = w;
        SDL_HideCursor();  // the game draws its own cursor
        SDL_StartTextInput(w->sdl);
    }
    // PORT_HIDDEN=1 (tests): the window stays hidden and the game is told it is active,
    // as a shown window with the focus would make it
    static const bool hidden = getenv("PORT_HIDDEN") && atoi(getenv("PORT_HIDDEN"));
    if (hidden) {
        PostMessageA((HWND)w, WM_ACTIVATEAPP, TRUE, 0);
        PostMessageA((HWND)w, WM_ACTIVATE, WA_ACTIVE, 0);
        return;
    }
    SDL_ShowWindow(w->sdl);
    SDL_RaiseWindow(w->sdl);
}

void display_hide(Window *w)
{
    if (w->sdl)
        SDL_HideWindow(w->sdl);
}

void display_destroy(Window *w)
{
    if (!w->sdl)
        return;
    if (s_window == w) {
        if (s_texture)
            SDL_DestroyTexture(s_texture);
        if (s_renderer)
            SDL_DestroyRenderer(s_renderer);
        s_texture = 0;
        s_renderer = 0;
        s_window = 0;
    }
    SDL_DestroyWindow(w->sdl);
    w->sdl = 0;
}

void display_title(Window *w)
{
    if (w->sdl)
        SDL_SetWindowTitle(w->sdl, w->text.c_str());
}

void display_to_client(Window *w, float x, float y, int *cx, int *cy)
{
    float lx = x, ly = y;
    if (s_renderer && w == s_window)
        SDL_RenderCoordinatesFromWindow(s_renderer, x, y, &lx, &ly);
    int ix = (int)lx, iy = (int)ly;
    if (ix < 0) ix = 0;
    if (iy < 0) iy = 0;
    if (w->client_w && ix >= w->client_w) ix = w->client_w - 1;
    if (w->client_h && iy >= w->client_h) iy = w->client_h - 1;
    *cx = ix;
    *cy = iy;
}

void display_from_client(Window *w, int cx, int cy, float *x, float *y)
{
    *x = (float)cx;
    *y = (float)cy;
    if (s_renderer && w == s_window)
        SDL_RenderCoordinatesToWindow(s_renderer, (float)cx, (float)cy, x, y);
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

static void save_shot(const uint32_t *frame, int w, int h)
{
    if (!shots_due())
        return;
    DWORD now = ticks();
    s_shot_next = now + s_shot_every;
    SDL_Surface *surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_XRGB8888, (void *)frame, w * 4);
    if (!surface)
        return;
    char path[1024];
    snprintf(path, sizeof(path), "%s/shot_%06u.bmp", s_shot_dir.c_str(), (unsigned)(now - s_shot_start));
    SDL_SaveBMP(surface, path);
    SDL_DestroySurface(surface);
}

// Shows the latest frame, if there is a new one. Only the main thread draws (SDL's
// renderer belongs to it); frames presented by other threads (the game's cursor thread)
// are shown by the main thread's next message pump.
static void show_frame()
{
    if (!s_renderer || !pthread_main_np())
        return;
    if (!s_frame_new)
        return;
    // at most one frame per 60 Hz refresh; a newer one waits for the next pump
    static Uint64 s_last;
    Uint64 now = SDL_GetTicksNS();
    if (now - s_last < 16000000)
        return;
    // nothing to show while the window can't be seen: the frame stays new for later
    // (PORT_SHOTS still saves it)
    bool unseen = s_window && s_window->sdl &&
        (SDL_GetWindowFlags(s_window->sdl) & (SDL_WINDOW_OCCLUDED | SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN));
    if (unseen && !shots_due())
        return;
    s_last = now;
    if (!unseen)
        s_frame_new = false; // a change while the pixels are taken shows next time
    const uint32_t *pixels;
    int s_frame_w, s_frame_h, s_frame_scale;
    if (!port_frame_pixels(&pixels, &s_frame_w, &s_frame_h, &s_frame_scale))
        return;
    options_draw((uint32_t *)pixels, s_frame_w, s_frame_h, s_frame_scale);  // a copy made for this frame
    save_shot(pixels, s_frame_w, s_frame_h);
    if (unseen)
        return;
    if (!s_texture || s_tex_w != s_frame_w || s_tex_h != s_frame_h || s_tex_scale != s_frame_scale) {
        if (s_texture)
            SDL_DestroyTexture(s_texture);
        s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, s_frame_w, s_frame_h);
        SDL_SetTextureScaleMode(s_texture, getenv("MICKEY_SMOOTH") ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
        s_tex_w = s_frame_w;
        s_tex_h = s_frame_h;
        s_tex_scale = s_frame_scale;
        // the logical size is the game's screen (mouse positions are in it)
        SDL_SetRenderLogicalPresentation(s_renderer, s_frame_w / s_frame_scale, s_frame_h / s_frame_scale,
                                         SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    SDL_UpdateTexture(s_texture, 0, pixels, s_frame_w * 4);
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
    SDL_RenderClear(s_renderer);
    SDL_RenderTexture(s_renderer, s_texture, 0, 0);
    SDL_RenderPresent(s_renderer);
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
    if (s_window && s_window->sdl)
        SDL_SetWindowFullscreen(s_window->sdl, fullscreen_wanted());
}

void port_wait_vblank(void)
{
    if (s_frame_new && pthread_main_np()) {
        show_frame();  // presents with vsync
        return;
    }
    // Sleep to the next 60 Hz boundary.
    DWORD now = ticks();
    DWORD next = (now / 16 + 1) * 16;
    Sleep(next - now);
}

}  // extern "C"
