// user.h - windows of the user32 layer (window.cpp, message.cpp, display.cpp).
//
// A top-level window of the game is one SDL window showing the frames of DirectDraw's
// primary surface (port_frame_pixels), scaled to fit. The game's coordinates stay those of its 640x480
// client area; screen coordinates equal client coordinates of the main window (its
// client origin is the screen origin, as in the game's full-screen mode).
#ifndef PORT_WIN32_USER_H
#define PORT_WIN32_USER_H

#include "internal.h"
#include <map>
#include <string>

struct SDL_Window;
union SDL_Event;

namespace win32 {

struct WindowClass {
    std::string name;
    WNDCLASSEXA wc;
};

struct Window {
    WindowClass *cls = 0;
    WNDPROC proc = 0;
    DWORD style = 0, exstyle = 0;
    HWND parent = 0;
    HMENU menu = 0;
    HINSTANCE instance = 0;
    std::string text;
    RECT rect = {0, 0, 0, 0};  // window rect in screen coordinates
    int client_w = 0, client_h = 0;
    bool visible = false;
    bool destroyed = false;
    bool invalid = false;      // a WM_PAINT is due
    std::map<int, LONG_PTR> longs;  // GWL_* and the extra bytes
    DWORD thread = 0;
    SDL_Window *sdl = 0;       // top-level windows
};

Window *window(HWND h);  // 0 (and ERROR_INVALID_WINDOW_HANDLE) if not a live window
HWND main_window();      // the first visible top-level window
LRESULT send(HWND h, UINT msg, WPARAM wp, LPARAM lp);

// display.cpp
void display_show(Window *w);     // creates or shows the SDL window
void display_hide(Window *w);
void display_destroy(Window *w);
void display_title(Window *w);
// SDL window coordinates to the game's client coordinates.
void display_to_client(Window *w, float x, float y, int *cx, int *cy);
// options.cpp: the options panel (Escape): takes the input while it is open, and is
// drawn over the frames the window shows (k times the game's 640x480).
bool options_event(Window *w, const SDL_Event &e);
bool options_open();
void options_draw(uint32_t *px, int w, int h, int k);
void display_from_client(Window *w, int cx, int cy, float *x, float *y);
void display_pump();  // shows a frame presented since the last pump
int display_width();   // the "screen": the game's display mode, else the desktop
int display_height();
// message.cpp
void post(HWND h, UINT msg, WPARAM wp, LPARAM lp);
void pump_events();    // SDL events into the queue (main thread)

}  // namespace win32

#endif
