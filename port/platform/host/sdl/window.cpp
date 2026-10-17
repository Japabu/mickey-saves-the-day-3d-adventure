// window.cpp - the host's window, frames, mouse pointer and events on SDL3: one window
// and renderer, the frames scaled to fit with letterboxing (the renderer's logical size
// is the game's screen, so positions convert through it).
#include <SDL3/SDL.h>

#include "../host.h"

#include <stdio.h>
#include <string.h>

namespace host {

void startup() {}

static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static int s_tex_w, s_tex_h;
static int s_game_w = 640, s_game_h = 480;   // the game's screen: the logical size

bool window_open(const char *title_utf8, int w, int h, bool fullscreen)
{
    if (s_window)
        return true;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return false;
    }
    s_game_w = w > 0 ? w : 640;
    s_game_h = h > 0 ? h : 480;
    // twice the game's size on a large display, and it stays 4:3 while resized, so the
    // picture fills the window while it is dragged and after (no stretching, no bars)
    s_window = SDL_CreateWindow(title_utf8, s_game_w * 2, s_game_h * 2,
                                SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
    if (!s_window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetWindowAspectRatio(s_window, (float)s_game_w / s_game_h, (float)s_game_w / s_game_h);
    s_renderer = SDL_CreateRenderer(s_window, 0);
    if (!s_renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
    } else {
        // no vsync wait: presenting blocks the game's thread (its message loop presents),
        // and Metal blocks for long while the window is covered
        SDL_SetRenderVSync(s_renderer, 0);
        SDL_SetRenderLogicalPresentation(s_renderer, s_game_w, s_game_h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    if (fullscreen)
        SDL_SetWindowFullscreen(s_window, true);
    return true;
}

void window_close()
{
    if (s_texture)
        SDL_DestroyTexture(s_texture);
    if (s_renderer)
        SDL_DestroyRenderer(s_renderer);
    if (s_window)
        SDL_DestroyWindow(s_window);
    s_texture = 0;
    s_renderer = 0;
    s_window = 0;
}

void window_show(bool shown)
{
    if (!s_window)
        return;
    if (!shown) {
        SDL_HideWindow(s_window);
        return;
    }
    SDL_ShowWindow(s_window);
    SDL_RaiseWindow(s_window);
    SDL_StartTextInput(s_window);
}

void window_title(const char *title_utf8)
{
    if (s_window)
        SDL_SetWindowTitle(s_window, title_utf8);
}

void window_fullscreen(bool on)
{
    if (s_window)
        SDL_SetWindowFullscreen(s_window, on);
}

bool window_is_fullscreen() { return s_window && (SDL_GetWindowFlags(s_window) & SDL_WINDOW_FULLSCREEN); }

void window_minimize(bool minimized)
{
    if (!s_window)
        return;
    if (minimized)
        SDL_MinimizeWindow(s_window);
    else
        SDL_RestoreWindow(s_window);
}

bool window_is_minimized() { return s_window && (SDL_GetWindowFlags(s_window) & SDL_WINDOW_MINIMIZED); }

bool window_visible()
{
    return s_window &&
           !(SDL_GetWindowFlags(s_window) & (SDL_WINDOW_OCCLUDED | SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN));
}

void present(const uint32_t *argb, int w, int h, int scale, bool smooth)
{
    if (!s_renderer)
        return;
    if (!s_texture || s_tex_w != w || s_tex_h != h) {
        if (s_texture)
            SDL_DestroyTexture(s_texture);
        s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        s_tex_w = w;
        s_tex_h = h;
        s_game_w = w / scale;
        s_game_h = h / scale;
        SDL_SetRenderLogicalPresentation(s_renderer, s_game_w, s_game_h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    SDL_SetTextureScaleMode(s_texture, smooth ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
    SDL_UpdateTexture(s_texture, 0, argb, w * 4);
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
    SDL_RenderClear(s_renderer);
    SDL_RenderTexture(s_renderer, s_texture, 0, 0);
    SDL_RenderPresent(s_renderer);
}

// A window position to the game's screen, clamped to it.
static void to_game(float x, float y, int *gx, int *gy)
{
    float lx = x, ly = y;
    if (s_renderer)
        SDL_RenderCoordinatesFromWindow(s_renderer, x, y, &lx, &ly);
    int ix = (int)lx, iy = (int)ly;
    *gx = ix < 0 ? 0 : ix >= s_game_w ? s_game_w - 1 : ix;
    *gy = iy < 0 ? 0 : iy >= s_game_h ? s_game_h - 1 : iy;
}

void cursor_visible(bool visible)
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
        return;
    if (visible)
        SDL_ShowCursor();
    else
        SDL_HideCursor();
}

void cursor_move(int x, int y)
{
    if (!s_window)
        return;
    float wx = (float)x, wy = (float)y;
    if (s_renderer)
        SDL_RenderCoordinatesToWindow(s_renderer, (float)x, (float)y, &wx, &wy);
    SDL_WarpMouseInWindow(s_window, wx, wy);
}

bool cursor_position(int *x, int *y)
{
    if (!s_window)
        return false;
    float wx, wy;
    SDL_GetMouseState(&wx, &wy);
    to_game(wx, wy, x, y);
    return true;
}

static int key_of(SDL_Scancode sc)
{
    if (sc >= SDL_SCANCODE_A && sc <= SDL_SCANCODE_Z)
        return 'A' + (sc - SDL_SCANCODE_A);
    if (sc >= SDL_SCANCODE_1 && sc <= SDL_SCANCODE_9)
        return '1' + (sc - SDL_SCANCODE_1);
    if (sc >= SDL_SCANCODE_F1 && sc <= SDL_SCANCODE_F12)
        return KeyF1 + (sc - SDL_SCANCODE_F1);
    if (sc >= SDL_SCANCODE_KP_1 && sc <= SDL_SCANCODE_KP_9)
        return KeyNumpad0 + 1 + (sc - SDL_SCANCODE_KP_1);
    switch (sc) {
    case SDL_SCANCODE_0: return '0';
    case SDL_SCANCODE_RETURN: return KeyReturn;
    case SDL_SCANCODE_ESCAPE: return KeyEscape;
    case SDL_SCANCODE_BACKSPACE: return KeyBack;
    case SDL_SCANCODE_TAB: return KeyTab;
    case SDL_SCANCODE_SPACE: return KeySpace;
    case SDL_SCANCODE_MINUS: return KeyMinus;
    case SDL_SCANCODE_EQUALS: return KeyEquals;
    case SDL_SCANCODE_LEFTBRACKET: return KeyLeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET: return KeyRightBracket;
    case SDL_SCANCODE_BACKSLASH: return KeyBackslash;
    case SDL_SCANCODE_SEMICOLON: return KeySemicolon;
    case SDL_SCANCODE_APOSTROPHE: return KeyApostrophe;
    case SDL_SCANCODE_GRAVE: return KeyGrave;
    case SDL_SCANCODE_COMMA: return KeyComma;
    case SDL_SCANCODE_PERIOD: return KeyPeriod;
    case SDL_SCANCODE_SLASH: return KeySlash;
    case SDL_SCANCODE_CAPSLOCK: return KeyCapsLock;
    case SDL_SCANCODE_PRINTSCREEN: return KeyPrintScreen;
    case SDL_SCANCODE_SCROLLLOCK: return KeyScrollLock;
    case SDL_SCANCODE_PAUSE: return KeyPause;
    case SDL_SCANCODE_INSERT: return KeyInsert;
    case SDL_SCANCODE_HOME: return KeyHome;
    case SDL_SCANCODE_PAGEUP: return KeyPageUp;
    case SDL_SCANCODE_DELETE: return KeyDelete;
    case SDL_SCANCODE_END: return KeyEnd;
    case SDL_SCANCODE_PAGEDOWN: return KeyPageDown;
    case SDL_SCANCODE_RIGHT: return KeyRight;
    case SDL_SCANCODE_LEFT: return KeyLeft;
    case SDL_SCANCODE_DOWN: return KeyDown;
    case SDL_SCANCODE_UP: return KeyUp;
    case SDL_SCANCODE_NUMLOCKCLEAR: return KeyNumLock;
    case SDL_SCANCODE_KP_DIVIDE: return KeyDivide;
    case SDL_SCANCODE_KP_MULTIPLY: return KeyMultiply;
    case SDL_SCANCODE_KP_MINUS: return KeySubtract;
    case SDL_SCANCODE_KP_PLUS: return KeyAdd;
    case SDL_SCANCODE_KP_ENTER: return KeyReturn;
    case SDL_SCANCODE_KP_0: return KeyNumpad0;
    case SDL_SCANCODE_KP_PERIOD: return KeyDecimal;
    case SDL_SCANCODE_LCTRL: return KeyControl;
    case SDL_SCANCODE_RCTRL: return KeyControl;
    case SDL_SCANCODE_LSHIFT: return KeyShift;
    case SDL_SCANCODE_RSHIFT: return KeyShift;
    case SDL_SCANCODE_LALT: return KeyAlt;
    case SDL_SCANCODE_RALT: return KeyAlt;
    case SDL_SCANCODE_LGUI: return KeyLeftWin;
    case SDL_SCANCODE_RGUI: return KeyRightWin;
    default: return 0;
    }
}

static int side_key_of(SDL_Scancode sc)
{
    switch (sc) {
    case SDL_SCANCODE_LCTRL: return KeyLeftControl;
    case SDL_SCANCODE_RCTRL: return KeyRightControl;
    case SDL_SCANCODE_LSHIFT: return KeyLeftShift;
    case SDL_SCANCODE_RSHIFT: return KeyRightShift;
    case SDL_SCANCODE_LALT: return KeyLeftAlt;
    case SDL_SCANCODE_RALT: return KeyRightAlt;
    default: return 0;
    }
}

bool poll_event(Event *out)
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
        return false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Event ev = {};
        switch (e.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: ev.type = Event::Close; break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED: ev.type = Event::FocusIn; break;
        case SDL_EVENT_WINDOW_FOCUS_LOST: ev.type = Event::FocusOut; break;
        case SDL_EVENT_WINDOW_MINIMIZED: ev.type = Event::Minimized; break;
        case SDL_EVENT_WINDOW_RESTORED: ev.type = Event::Restored; break;
        case SDL_EVENT_WINDOW_EXPOSED: ev.type = Event::Exposed; break;
        case SDL_EVENT_MOUSE_MOTION:
            ev.type = Event::MouseMove;
            to_game(e.motion.x, e.motion.y, &ev.x, &ev.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            ev.type = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? Event::MouseDown : Event::MouseUp;
            ev.button = e.button.button == SDL_BUTTON_LEFT ? 0 : e.button.button == SDL_BUTTON_RIGHT ? 1 : 2;
            ev.clicks = e.button.clicks;
            to_game(e.button.x, e.button.y, &ev.x, &ev.y);
            break;
        case SDL_EVENT_MOUSE_WHEEL: {
            ev.type = Event::Wheel;
            ev.wheel = e.wheel.y;
            float wx, wy;
            SDL_GetMouseState(&wx, &wy);
            to_game(wx, wy, &ev.x, &ev.y);
            break;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            ev.type = e.type == SDL_EVENT_KEY_DOWN ? Event::KeyDown : Event::KeyUp;
            ev.vk = key_of(e.key.scancode);
            ev.side_vk = side_key_of(e.key.scancode);
            ev.scancode = e.key.scancode;
            ev.repeat = e.key.repeat;
#ifdef __APPLE__
            ev.command = (e.key.mod & SDL_KMOD_GUI) != 0;
#else
            ev.command = (e.key.mod & SDL_KMOD_CTRL) != 0;
#endif
            break;
        case SDL_EVENT_TEXT_INPUT:
            ev.type = Event::Text;
            strncpy(ev.text, e.text.text, sizeof(ev.text) - 1);
            break;
        default:
            continue;
        }
        *out = ev;
        return true;
    }
    return false;
}

void wait_event(int ms)
{
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_WaitEventTimeout(0, ms);
}

}  // namespace host
