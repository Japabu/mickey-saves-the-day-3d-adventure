// message.cpp - message queues, the SDL event pump (mouse, keyboard, text, focus,
// close) translated into window messages, timers, keyboard and mouse state, the mouse
// capture, and the cursor.
#include <SDL3/SDL.h>

#include "user.h"

#include <deque>
#include <mutex>
#include <vector>

namespace win32 {

struct Queue {
    std::deque<MSG> msgs;
    bool quit = false;
    int quit_code = 0;
};

static std::mutex s_mu;
static std::map<DWORD, Queue> s_queues;  // by thread id

static Queue &queue(DWORD thread) { return s_queues[thread]; }

static DWORD s_main_thread;

void post(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    Window *w = h ? window(h) : 0;
    DWORD thread = w ? w->thread : GetCurrentThreadId();
    MSG m = {h, msg, wp, lp, ticks(), {0, 0}};
    std::lock_guard<std::mutex> lock(s_mu);
    queue(thread).msgs.push_back(m);
}

// Timers.
struct Timer {
    HWND hwnd;
    UINT_PTR id;
    UINT elapse;
    DWORD due;
    TIMERPROC proc;
    DWORD thread;
};
static std::vector<Timer> s_timers;
static UINT_PTR s_next_timer = 1;

// Keyboard and mouse state, as the game saw it in its messages.
static BYTE s_keys[256];  // 0x80 down, 0x01 toggled
static int s_mouse_x, s_mouse_y;
static HWND s_capture;
static int s_cursor_count = 0;  // ShowCursor's display counter

static void key_state(int vk, bool down)
{
    if (vk <= 0 || vk > 255)
        return;
    if (down && !(s_keys[vk] & 0x80))
        s_keys[vk] ^= 0x01;
    s_keys[vk] = (BYTE)((s_keys[vk] & 0x01) | (down ? 0x80 : 0));
}

static int vk_of(SDL_Scancode sc)
{
    if (sc >= SDL_SCANCODE_A && sc <= SDL_SCANCODE_Z)
        return 'A' + (sc - SDL_SCANCODE_A);
    if (sc >= SDL_SCANCODE_1 && sc <= SDL_SCANCODE_9)
        return '1' + (sc - SDL_SCANCODE_1);
    if (sc >= SDL_SCANCODE_F1 && sc <= SDL_SCANCODE_F12)
        return VK_F1 + (sc - SDL_SCANCODE_F1);
    if (sc >= SDL_SCANCODE_KP_1 && sc <= SDL_SCANCODE_KP_9)
        return VK_NUMPAD1 + (sc - SDL_SCANCODE_KP_1);
    switch (sc) {
    case SDL_SCANCODE_0: return '0';
    case SDL_SCANCODE_RETURN: return VK_RETURN;
    case SDL_SCANCODE_ESCAPE: return VK_ESCAPE;
    case SDL_SCANCODE_BACKSPACE: return VK_BACK;
    case SDL_SCANCODE_TAB: return VK_TAB;
    case SDL_SCANCODE_SPACE: return VK_SPACE;
    case SDL_SCANCODE_MINUS: return VK_OEM_MINUS;
    case SDL_SCANCODE_EQUALS: return VK_OEM_PLUS;
    case SDL_SCANCODE_LEFTBRACKET: return VK_OEM_4;
    case SDL_SCANCODE_RIGHTBRACKET: return VK_OEM_6;
    case SDL_SCANCODE_BACKSLASH: return VK_OEM_5;
    case SDL_SCANCODE_SEMICOLON: return VK_OEM_1;
    case SDL_SCANCODE_APOSTROPHE: return VK_OEM_7;
    case SDL_SCANCODE_GRAVE: return VK_OEM_3;
    case SDL_SCANCODE_COMMA: return VK_OEM_COMMA;
    case SDL_SCANCODE_PERIOD: return VK_OEM_PERIOD;
    case SDL_SCANCODE_SLASH: return VK_OEM_2;
    case SDL_SCANCODE_CAPSLOCK: return VK_CAPITAL;
    case SDL_SCANCODE_PRINTSCREEN: return VK_SNAPSHOT;
    case SDL_SCANCODE_SCROLLLOCK: return VK_SCROLL;
    case SDL_SCANCODE_PAUSE: return VK_PAUSE;
    case SDL_SCANCODE_INSERT: return VK_INSERT;
    case SDL_SCANCODE_HOME: return VK_HOME;
    case SDL_SCANCODE_PAGEUP: return VK_PRIOR;
    case SDL_SCANCODE_DELETE: return VK_DELETE;
    case SDL_SCANCODE_END: return VK_END;
    case SDL_SCANCODE_PAGEDOWN: return VK_NEXT;
    case SDL_SCANCODE_RIGHT: return VK_RIGHT;
    case SDL_SCANCODE_LEFT: return VK_LEFT;
    case SDL_SCANCODE_DOWN: return VK_DOWN;
    case SDL_SCANCODE_UP: return VK_UP;
    case SDL_SCANCODE_NUMLOCKCLEAR: return VK_NUMLOCK;
    case SDL_SCANCODE_KP_DIVIDE: return VK_DIVIDE;
    case SDL_SCANCODE_KP_MULTIPLY: return VK_MULTIPLY;
    case SDL_SCANCODE_KP_MINUS: return VK_SUBTRACT;
    case SDL_SCANCODE_KP_PLUS: return VK_ADD;
    case SDL_SCANCODE_KP_ENTER: return VK_RETURN;
    case SDL_SCANCODE_KP_0: return VK_NUMPAD0;
    case SDL_SCANCODE_KP_PERIOD: return VK_DECIMAL;
    case SDL_SCANCODE_LCTRL: return VK_CONTROL;
    case SDL_SCANCODE_RCTRL: return VK_CONTROL;
    case SDL_SCANCODE_LSHIFT: return VK_SHIFT;
    case SDL_SCANCODE_RSHIFT: return VK_SHIFT;
    case SDL_SCANCODE_LALT: return VK_MENU;
    case SDL_SCANCODE_RALT: return VK_MENU;
    case SDL_SCANCODE_LGUI: return VK_LWIN;
    case SDL_SCANCODE_RGUI: return VK_RWIN;
    default: return 0;
    }
}

static int side_vk(SDL_Scancode sc)
{
    switch (sc) {
    case SDL_SCANCODE_LCTRL: return VK_LCONTROL;
    case SDL_SCANCODE_RCTRL: return VK_RCONTROL;
    case SDL_SCANCODE_LSHIFT: return VK_LSHIFT;
    case SDL_SCANCODE_RSHIFT: return VK_RSHIFT;
    case SDL_SCANCODE_LALT: return VK_LMENU;
    case SDL_SCANCODE_RALT: return VK_RMENU;
    default: return 0;
    }
}

static WPARAM mouse_keys()
{
    WPARAM k = 0;
    if (s_keys[VK_LBUTTON] & 0x80) k |= MK_LBUTTON;
    if (s_keys[VK_RBUTTON] & 0x80) k |= MK_RBUTTON;
    if (s_keys[VK_MBUTTON] & 0x80) k |= MK_MBUTTON;
    if (s_keys[VK_SHIFT] & 0x80) k |= MK_SHIFT;
    if (s_keys[VK_CONTROL] & 0x80) k |= MK_CONTROL;
    return k;
}

static Window *event_window(SDL_WindowID id)
{
    HWND h = main_window();
    Window *w = h ? window(h) : 0;
    (void)id;  // the game has one top-level window
    return w;
}

static void mouse_message(Window *w, UINT msg, float x, float y)
{
    int cx, cy;
    display_to_client(w, x, y, &cx, &cy);
    s_mouse_x = cx;
    s_mouse_y = cy;
    HWND target = s_capture ? s_capture : (HWND)w;
    post(target, msg, mouse_keys(), MAKELPARAM((short)cx, (short)cy));
}

// Characters typed: UTF-8 from SDL, in code page 1252 for the game.
static void text_message(Window *w, const char *utf8)
{
    WCHAR wbuf[64];
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, 64);
    for (int i = 0; i + 1 < n; i++) {
        char c;
        BOOL bad = FALSE;
        WideCharToMultiByte(CP_ACP, 0, &wbuf[i], 1, &c, 1, 0, &bad);
        if (!bad)
            post((HWND)w, WM_CHAR, (unsigned char)c, 1);
    }
}

static bool s_text_input_started;

void pump_events()
{
    if (GetCurrentThreadId() != s_main_thread && s_main_thread)
        return;
    if (!SDL_WasInit(SDL_INIT_VIDEO))
        return;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Window *w = event_window(0);
        if (!w)
            continue;
        if (!s_text_input_started && w->sdl) {
            SDL_StartTextInput(w->sdl);
            s_text_input_started = true;
        }
        switch (e.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            post((HWND)w, WM_CLOSE, 0, 0);
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            post((HWND)w, WM_ACTIVATEAPP, TRUE, 0);
            post((HWND)w, WM_ACTIVATE, WA_ACTIVE, 0);
            post((HWND)w, WM_SETFOCUS, 0, 0);
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            post((HWND)w, WM_ACTIVATE, WA_INACTIVE, 0);
            post((HWND)w, WM_ACTIVATEAPP, FALSE, 0);
            post((HWND)w, WM_KILLFOCUS, 0, 0);
            break;
        case SDL_EVENT_WINDOW_MINIMIZED:
            post((HWND)w, WM_SIZE, SIZE_MINIMIZED, 0);
            break;
        case SDL_EVENT_WINDOW_RESTORED:
            post((HWND)w, WM_SIZE, SIZE_RESTORED, MAKELPARAM(w->client_w, w->client_h));
            w->invalid = true;
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            w->invalid = true;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouse_message(w, WM_MOUSEMOVE, e.motion.x, e.motion.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            bool down = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            int vk = e.button.button == SDL_BUTTON_LEFT ? VK_LBUTTON : e.button.button == SDL_BUTTON_RIGHT ? VK_RBUTTON : VK_MBUTTON;
            key_state(vk, down);
            UINT base = vk == VK_LBUTTON ? WM_LBUTTONDOWN : vk == VK_RBUTTON ? WM_RBUTTONDOWN : WM_MBUTTONDOWN;
            UINT msg = down ? (e.button.clicks == 2 && (w->cls->wc.style & CS_DBLCLKS) ? base + 2 : base) : base + 1;
            mouse_message(w, msg, e.button.x, e.button.y);
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            int delta = (int)(e.wheel.y * WHEEL_DELTA);
            post((HWND)w, WM_MOUSEWHEEL, MAKEWPARAM(mouse_keys(), (short)delta), MAKELPARAM(s_mouse_x, s_mouse_y));
            break;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            bool down = e.type == SDL_EVENT_KEY_DOWN;
            int vk = vk_of(e.key.scancode);
            if (!vk)
                break;
            bool was_down = (s_keys[vk] & 0x80) != 0;
            key_state(vk, down);
            key_state(side_vk(e.key.scancode), down);
            bool alt = (s_keys[VK_MENU] & 0x80) != 0;
            // lParam: repeat count, scan code, extended, context (Alt), previous state, transition.
            LPARAM lp = 1 | ((LPARAM)(e.key.scancode & 0xff) << 16) | (alt ? (1 << 29) : 0) |
                        (was_down ? (1 << 30) : 0) | (down ? 0 : (1u << 31));
            UINT msg = alt || vk == VK_MENU || vk == VK_F10 ? (down ? WM_SYSKEYDOWN : WM_SYSKEYUP)
                                                           : (down ? WM_KEYDOWN : WM_KEYUP);
            post((HWND)w, msg, (WPARAM)vk, lp);
            break;
        }
        case SDL_EVENT_TEXT_INPUT:
            text_message(w, e.text.text);
            break;
        }
    }
    display_pump();
}

bool messages_pending()
{
    pump_events();
    std::lock_guard<std::mutex> lock(s_mu);
    return !queue(GetCurrentThreadId()).msgs.empty();
}

// The next message for the thread: posted ones, then timers, then WM_PAINT.
static bool next_message(LPMSG msg, HWND hWnd, UINT lo, UINT hi, bool remove)
{
    DWORD thread = GetCurrentThreadId();
    pump_events();
    {
        std::lock_guard<std::mutex> lock(s_mu);
        Queue &q = queue(thread);
        for (auto it = q.msgs.begin(); it != q.msgs.end(); ++it) {
            if (hWnd && it->hwnd != hWnd)
                continue;
            if ((lo || hi) && (it->message < lo || it->message > hi))
                continue;
            *msg = *it;
            msg->pt.x = s_mouse_x;
            msg->pt.y = s_mouse_y;
            if (remove)
                q.msgs.erase(it);
            return true;
        }
        if (q.quit && !hWnd) {
            *msg = MSG{0, WM_QUIT, (WPARAM)q.quit_code, 0, ticks(), {s_mouse_x, s_mouse_y}};
            if (remove)
                q.quit = false;
            return true;
        }
    }
    DWORD now = ticks();
    for (auto &t : s_timers) {
        if (t.thread != thread || (int)(now - t.due) < 0 || (hWnd && t.hwnd != hWnd))
            continue;
        if ((lo || hi) && (WM_TIMER < lo || WM_TIMER > hi))
            continue;
        *msg = MSG{t.hwnd, WM_TIMER, t.id, (LPARAM)t.proc, now, {s_mouse_x, s_mouse_y}};
        if (remove)
            t.due = now + t.elapse;
        return true;
    }
    if (!lo && !hi) {
        HWND m = main_window();
        Window *w = m ? window(m) : 0;
        if (w && w->invalid && w->thread == thread && (!hWnd || hWnd == m)) {
            *msg = MSG{m, WM_PAINT, 0, 0, now, {s_mouse_x, s_mouse_y}};
            if (remove)
                w->invalid = false;
            return true;
        }
    }
    return false;
}

}  // namespace win32

using namespace win32;

extern "C" {

BOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    if (!s_main_thread)
        s_main_thread = GetCurrentThreadId();
    return next_message(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, (wRemoveMsg & PM_REMOVE) != 0);
}

BOOL WINAPI GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (!s_main_thread)
        s_main_thread = GetCurrentThreadId();
    while (!next_message(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, true)) {
        if (SDL_WasInit(SDL_INIT_VIDEO) && GetCurrentThreadId() == s_main_thread)
            SDL_WaitEventTimeout(0, 10);
        else
            Sleep(5);
    }
    return lpMsg->message != WM_QUIT;
}

BOOL WINAPI WaitMessage(void)
{
    MSG m;
    while (!PeekMessageA(&m, 0, 0, 0, PM_NOREMOVE))
        Sleep(5);
    return TRUE;
}

BOOL WINAPI TranslateMessage(const MSG *m)
{
    // Text arrives as WM_CHAR from the text input events; the control characters that
    // TranslateMessage made from keys come from here.
    if (m->message == WM_KEYDOWN) {
        switch (m->wParam) {
        case VK_RETURN: post(m->hwnd, WM_CHAR, '\r', m->lParam); return TRUE;
        case VK_BACK: post(m->hwnd, WM_CHAR, '\b', m->lParam); return TRUE;
        case VK_ESCAPE: post(m->hwnd, WM_CHAR, 0x1b, m->lParam); return TRUE;
        case VK_TAB: post(m->hwnd, WM_CHAR, '\t', m->lParam); return TRUE;
        }
    }
    return FALSE;
}

LRESULT WINAPI DispatchMessageA(const MSG *m)
{
    if (m->message == WM_TIMER && m->lParam) {
        ((TIMERPROC)m->lParam)(m->hwnd, WM_TIMER, m->wParam, ticks());
        return 0;
    }
    if (!m->hwnd)
        return 0;
    return send(m->hwnd, m->message, m->wParam, m->lParam);
}

LRESULT WINAPI SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return send(hWnd, Msg, wParam, lParam); }

LRESULT WINAPI CallWindowProcA(WNDPROC proc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return proc ? proc(hWnd, Msg, wParam, lParam) : 0;
}

BOOL WINAPI PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (hWnd && !window(hWnd))
        return FALSE;
    post(hWnd, Msg, wParam, lParam);
    return TRUE;
}

BOOL WINAPI PostThreadMessageA(DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    MSG m = {0, Msg, wParam, lParam, ticks(), {0, 0}};
    std::lock_guard<std::mutex> lock(s_mu);
    queue(idThread).msgs.push_back(m);
    return TRUE;
}

void WINAPI PostQuitMessage(int nExitCode)
{
    std::lock_guard<std::mutex> lock(s_mu);
    Queue &q = queue(GetCurrentThreadId());
    q.quit = true;
    q.quit_code = nExitCode;
}

UINT WINAPI RegisterWindowMessageA(LPCSTR)
{
    static UINT next = 0xC000;
    return next++;
}

LRESULT WINAPI DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg) {
    case WM_NCCREATE: return TRUE;
    case WM_CLOSE: DestroyWindow(hWnd); return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_CLOSE)
            send(hWnd, WM_CLOSE, 0, 0);
        return 0;
    case WM_NCHITTEST: return HTCLIENT;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_SETCURSOR: return FALSE;
    case WM_MOUSEACTIVATE: return 1;  // MA_ACTIVATE
    case WM_GETMINMAXINFO: return 0;
    }
    (void)lParam;
    return 0;
}

UINT_PTR WINAPI SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
{
    for (auto &t : s_timers)
        if (hWnd && t.hwnd == hWnd && t.id == nIDEvent) {
            t.elapse = uElapse;
            t.due = ticks() + uElapse;
            t.proc = lpTimerFunc;
            return nIDEvent;
        }
    UINT_PTR id = hWnd ? nIDEvent : s_next_timer++;
    s_timers.push_back({hWnd, id, uElapse, ticks() + uElapse, lpTimerFunc, GetCurrentThreadId()});
    return id;
}

BOOL WINAPI KillTimer(HWND hWnd, UINT_PTR uIDEvent)
{
    for (size_t i = 0; i < s_timers.size(); i++)
        if (s_timers[i].hwnd == hWnd && s_timers[i].id == uIDEvent) {
            s_timers.erase(s_timers.begin() + (long)i);
            return TRUE;
        }
    return FALSE;
}

SHORT WINAPI GetKeyState(int nVirtKey)
{
    if (nVirtKey < 0 || nVirtKey > 255)
        return 0;
    BYTE s = s_keys[nVirtKey];
    return (SHORT)(((s & 0x80) ? 0x8000 : 0) | (s & 0x01));
}

SHORT WINAPI GetAsyncKeyState(int vKey)
{
    pump_events();
    return (SHORT)(GetKeyState(vKey) & 0x8000);
}

HKL WINAPI GetKeyboardLayout(DWORD) { return (HKL)(uintptr_t)0x04070407; }

BOOL WINAPI GetCursorPos(LPPOINT lpPoint)
{
    HWND m = main_window();
    Window *w = m ? window(m) : 0;
    if (w && w->sdl) {
        float x, y;
        SDL_GetMouseState(&x, &y);
        display_to_client(w, x, y, &s_mouse_x, &s_mouse_y);
    }
    lpPoint->x = s_mouse_x;
    lpPoint->y = s_mouse_y;
    return TRUE;
}

BOOL WINAPI SetCursorPos(int X, int Y)
{
    HWND m = main_window();
    Window *w = m ? window(m) : 0;
    s_mouse_x = X;
    s_mouse_y = Y;
    if (w && w->sdl) {
        float x, y;
        display_from_client(w, X, Y, &x, &y);
        SDL_WarpMouseInWindow(w->sdl, x, y);
    }
    return TRUE;
}

HWND WINAPI SetCapture(HWND hWnd)
{
    HWND old = s_capture;
    s_capture = hWnd;
    return old;
}

HWND WINAPI GetCapture(void) { return s_capture; }

BOOL WINAPI ReleaseCapture(void)
{
    s_capture = 0;
    return TRUE;
}

HWND WINAPI SetFocus(HWND hWnd) { return hWnd; }
HWND WINAPI GetFocus(void) { return main_window(); }
HWND WINAPI GetActiveWindow(void) { return main_window(); }
HWND WINAPI GetForegroundWindow(void) { return main_window(); }
HWND WINAPI SetActiveWindow(HWND hWnd) { return hWnd; }

BOOL WINAPI SetForegroundWindow(HWND hWnd)
{
    Window *w = window(hWnd);
    if (w && w->sdl)
        SDL_RaiseWindow(w->sdl);
    return w != 0;
}

BOOL WINAPI ClipCursor(const RECT *lpRect)
{
    HWND m = main_window();
    Window *w = m ? window(m) : 0;
    if (!w || !w->sdl)
        return TRUE;
    if (!lpRect) {
        SDL_SetWindowMouseRect(w->sdl, 0);
        return TRUE;
    }
    float x0, y0, x1, y1;
    display_from_client(w, lpRect->left, lpRect->top, &x0, &y0);
    display_from_client(w, lpRect->right, lpRect->bottom, &x1, &y1);
    SDL_Rect r = {(int)x0, (int)y0, (int)(x1 - x0), (int)(y1 - y0)};
    SDL_SetWindowMouseRect(w->sdl, &r);
    return TRUE;
}

BOOL WINAPI GetClipCursor(LPRECT lpRect)
{
    SetRect(lpRect, 0, 0, display_width(), display_height());
    return TRUE;
}

// The cursor: the game draws its own and hides the system one. ShowCursor's counter
// decides whether the system arrow shows over the window.
int WINAPI ShowCursor(BOOL bShow)
{
    s_cursor_count += bShow ? 1 : -1;
    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        if (s_cursor_count >= 0)
            SDL_ShowCursor();
        else
            SDL_HideCursor();
    }
    return s_cursor_count;
}

HCURSOR WINAPI SetCursor(HCURSOR hCursor)
{
    static HCURSOR current;
    HCURSOR old = current;
    current = hCursor;
    return old;
}

static int s_cursor_dummy, s_icon_dummy;
HCURSOR WINAPI LoadCursorA(HINSTANCE, LPCSTR) { return (HCURSOR)&s_cursor_dummy; }
HICON WINAPI LoadIconA(HINSTANCE, LPCSTR) { return (HICON)&s_icon_dummy; }
BOOL WINAPI DestroyIcon(HICON) { return TRUE; }
BOOL WINAPI DestroyCursor(HCURSOR) { return TRUE; }
BOOL WINAPI DrawIcon(HDC, int, int, HICON) { return TRUE; }

HHOOK WINAPI SetWindowsHookExA(int, HOOKPROC, HINSTANCE, DWORD)
{
    PORT_STUB("SetWindowsHookEx");
    return 0;
}
BOOL WINAPI UnhookWindowsHookEx(HHOOK) { return TRUE; }
LRESULT WINAPI CallNextHookEx(HHOOK, int, WPARAM, LPARAM) { return 0; }

}  // extern "C"
