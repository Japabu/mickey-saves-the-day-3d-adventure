// display.cpp - the game window on screen: one SDL window and renderer, the frames the
// DirectDraw layer presents (port_present), scaled to fit with letterboxing, vsync, and
// the "screen" size the game sees.
#include <SDL3/SDL.h>

#include "user.h"
#include "../port.h"

#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace win32 {

static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static int s_tex_w, s_tex_h;
static Window *s_window;

// The latest frame (copied by port_present, shown when the game's thread next pumps or
// waits for the vertical blank).
static std::mutex s_frame_mu;
static std::vector<uint32_t> s_frame;
static int s_frame_w, s_frame_h;
static bool s_frame_new;
static DWORD s_last_present;

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

static bool fullscreen_wanted()
{
    const char *e = getenv("MICKEY_WINDOWED");
    return !(e && *e && *e != '0');
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
        // The window is twice the game's size on a large display, and stays 4:3.
        w->sdl = SDL_CreateWindow(title.c_str(), cw * 2, ch * 2, flags);
        if (!w->sdl) {
            fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
            return;
        }
        if (!s_renderer) {
            s_renderer = SDL_CreateRenderer(w->sdl, 0);
            if (!s_renderer)
                fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
            else
                SDL_SetRenderVSync(s_renderer, 1);
        }
        if (s_renderer)
            SDL_SetRenderLogicalPresentation(s_renderer, cw, ch, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        if (fullscreen_wanted())
            SDL_SetWindowFullscreen(w->sdl, true);
        s_window = w;
        SDL_HideCursor();  // the game draws its own cursor
        SDL_StartTextInput(w->sdl);
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

// Shows the latest frame, if there is a new one.
static void show_frame()
{
    if (!s_renderer)
        return;
    std::lock_guard<std::mutex> lock(s_frame_mu);
    if (!s_frame_new)
        return;
    s_frame_new = false;
    if (!s_texture || s_tex_w != s_frame_w || s_tex_h != s_frame_h) {
        if (s_texture)
            SDL_DestroyTexture(s_texture);
        s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, s_frame_w, s_frame_h);
        SDL_SetTextureScaleMode(s_texture, getenv("MICKEY_SMOOTH") ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
        s_tex_w = s_frame_w;
        s_tex_h = s_frame_h;
        SDL_SetRenderLogicalPresentation(s_renderer, s_frame_w, s_frame_h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    SDL_UpdateTexture(s_texture, 0, s_frame.data(), s_frame_w * 4);
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
    SDL_RenderClear(s_renderer);
    SDL_RenderTexture(s_renderer, s_texture, 0, 0);
    SDL_RenderPresent(s_renderer);
}

void display_pump() { show_frame(); }

}  // namespace win32

using namespace win32;

extern "C" {

void port_present(const void *argb8888, int w, int h, int pitch)
{
    {
        std::lock_guard<std::mutex> lock(s_frame_mu);
        s_frame.resize((size_t)w * (size_t)h);
        for (int y = 0; y < h; y++)
            memcpy(&s_frame[(size_t)y * (size_t)w], (const char *)argb8888 + (size_t)y * (size_t)pitch, (size_t)w * 4);
        s_frame_w = w;
        s_frame_h = h;
        s_frame_new = true;
    }
    // Several presents can come per frame (dirty rectangles): show at most one per
    // display refresh here; the rest is shown by the next pump.
    DWORD now = ticks();
    if (now - s_last_present >= 15) {
        s_last_present = now;
        show_frame();
    }
}

void port_wait_vblank(void)
{
    if (s_frame_new) {
        s_last_present = ticks();
        show_frame();  // presents with vsync
        return;
    }
    // Sleep to the next 60 Hz boundary.
    DWORD now = ticks();
    DWORD next = (now / 16 + 1) * 16;
    Sleep(next - now);
}

}  // extern "C"
