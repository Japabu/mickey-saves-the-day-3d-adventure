// message.cpp - message queues, the host's events (mouse, keyboard, text, focus, close)
// translated into window messages, timers, keyboard and mouse state, the mouse capture,
// and the cursor.
#include "user.h"
#include "../host/host.h"

#include <deque>
#include <signal.h>
#include <mutex>
#include <string>
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
static volatile int s_mouse_x, s_mouse_y;
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

static Window *event_window()
{
    HWND h = main_window();
    return h ? window(h) : 0;  // the game has one top-level window
}

static void record_input(const char *action, int a, int b);

static void mouse_message(Window *w, UINT msg, int cx, int cy)
{
    s_mouse_x = cx;
    s_mouse_y = cy;
    if (msg == WM_MOUSEMOVE)
        record_input("move", cx, cy);
    else if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
        record_input("down", cx, cy);
    else if (msg == WM_LBUTTONUP)
        record_input("up", cx, cy);
    HWND target = s_capture ? s_capture : (HWND)w;
    post(target, msg, mouse_keys(), MAKELPARAM((short)cx, (short)cy));
}

// Characters typed: UTF-8 from the host, in code page 1252 for the game.
static void text_message(Window *w, const char *utf8)
{
    WCHAR wbuf[64];
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, 64);
    for (int i = 0; i + 1 < n; i++) {
        char c;
        BOOL bad = FALSE;
        WideCharToMultiByte(CP_ACP, 0, &wbuf[i], 1, &c, 1, 0, &bad);
        if (!bad)
        {
            post((HWND)w, WM_CHAR, (unsigned char)c, 1);
            record_input("key", (unsigned char)c, 0);
        }
    }
}

// PORT_INPUT="<ms> <action> [args]; ...": input played back for tests, at the given
// milliseconds after the first message pump. Actions: "move x y", "click x y" (left
// button, game coordinates), "key c" (a character or its code, WM_KEYDOWN/WM_CHAR/WM_KEYUP). While a
// script runs, the cursor is the script's, not the real mouse's. "down x y"/"up x y" press
// and release the button separately (PORT_RECORD writes those).
struct ScriptedInput {
    DWORD at;
    char action[8];
    int a, b;
};
static std::vector<ScriptedInput> s_script;
static size_t s_script_next;
static DWORD s_script_start;
static bool s_scripted_mouse;

static void load_input_script()
{
    const char *e = getenv("PORT_INPUT");
    if (!e || !*e)
        return;
    s_scripted_mouse = true;
    std::string all = e;
    size_t pos = 0;
    while (pos < all.size()) {
        size_t end = all.find(';', pos);
        std::string item = all.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
        ScriptedInput in = {};
        char key[8] = {};
        if (sscanf(item.c_str(), " %u %7s %7s", &in.at, in.action, key) >= 2) {
            if (!strcmp(in.action, "key"))   // a character code, or the character itself
                in.a = key[0] >= '0' && key[0] <= '9' && key[1] ? atoi(key) : (unsigned char)key[0];
            else
                sscanf(item.c_str(), " %*u %*s %d %d", &in.a, &in.b);
            s_script.push_back(in);
        }
        if (end == std::string::npos)
            break;
        pos = end + 1;
    }
    log("input", "PORT_INPUT: %zu actions\n", s_script.size());
}

// PORT_RECORD=<file>: the player's mouse and keyboard input is written to the file in
// PORT_INPUT's format (milliseconds since the first message pump), so a playtest can be
// replayed exactly: PORT_INPUT="$(cat file)". Mouse moves are kept every 50 ms at most.
static FILE *s_record;
static DWORD s_record_last_move;

static void record_input(const char *action, int a, int b)
{
    if (!s_record)
        return;
    DWORD now = ticks() - s_script_start;
    if (!strcmp(action, "move")) {
        if (now - s_record_last_move < 50)
            return;
        s_record_last_move = now;
    }
    if (!strcmp(action, "key"))
        fprintf(s_record, "%u key %d;\n", (unsigned)now, a);
    else
        fprintf(s_record, "%u %s %d %d;\n", (unsigned)now, action, a, b);
    fflush(s_record);
}

static void play_input_script(Window *w)
{
    if (s_script_next >= s_script.size() || !w)
        return;
    DWORD now = ticks() - s_script_start;
    while (s_script_next < s_script.size() && s_script[s_script_next].at <= now) {
        const ScriptedInput &in = s_script[s_script_next++];
        HWND h = (HWND)w;
        log("input", "%u ms: %s %d %d\n", (unsigned)now, in.action, in.a, in.b);
        if (!strcmp(in.action, "key")) {
            WPARAM vk = in.a >= 'a' && in.a <= 'z' ? in.a - 32 : in.a;
            post(h, WM_KEYDOWN, vk, 1);
            post(h, WM_CHAR, in.a, 1);
            post(h, WM_KEYUP, vk, 0xC0000001);
            continue;
        }
        s_mouse_x = in.a;
        s_mouse_y = in.b;
        LPARAM xy = MAKELPARAM((short)in.a, (short)in.b);
        post(h, WM_MOUSEMOVE, 0, xy);
        if (!strcmp(in.action, "click")) {
            post(h, WM_LBUTTONDOWN, MK_LBUTTON, xy);
            post(h, WM_LBUTTONUP, 0, xy);
        } else if (!strcmp(in.action, "down")) {
            key_state(VK_LBUTTON, true);
            post(h, WM_LBUTTONDOWN, MK_LBUTTON, xy);
        } else if (!strcmp(in.action, "up")) {
            key_state(VK_LBUTTON, false);
            post(h, WM_LBUTTONUP, 0, xy);
        }
    }
}

// SIGTERM (kill, a logout) closes the window as the close button does, so the game
// shuts down and exits instead of dying in the middle of a frame.
static volatile sig_atomic_t s_terminate;
static void on_sigterm(int) { s_terminate = 1; }

void pump_events()
{
    if (GetCurrentThreadId() != s_main_thread && s_main_thread)
        return;
    static bool s_sigterm_set = [] {
        signal(SIGTERM, on_sigterm);
        load_input_script();
        s_script_start = ticks();
        if (const char *r = getenv("PORT_RECORD"))
            s_record = fopen(r, "w");
        return true;
    }();
    (void)s_sigterm_set;
    play_input_script(event_window());
    if (s_terminate == 1) {
        s_terminate = 2;
        if (HWND m = main_window()) {
            log("message", "SIGTERM: closing the window\n");
            post(m, WM_CLOSE, 0, 0);
        }
    }
    if (!display_open())
        return;
    host::Event e;
    while (host::poll_event(&e)) {
        Window *w = event_window();
        if (!w)
            continue;
        if (options_event(w, e))
            continue;
        switch (e.type) {
        case host::Event::Close:
            log("message", "close: WM_CLOSE\n");
            post((HWND)w, WM_CLOSE, 0, 0);
            break;
        case host::Event::FocusIn:
            if (options_open())
                break;  // the game stays paused under the options panel
            post((HWND)w, WM_ACTIVATEAPP, TRUE, 0);
            post((HWND)w, WM_ACTIVATE, WA_ACTIVE, 0);
            post((HWND)w, WM_SETFOCUS, 0, 0);
            break;
        case host::Event::FocusOut: {
            // The game pauses while it is not the active application, as on Windows.
            // A window started from a terminal loses and regains focus erratically on
            // macOS, so by default the port keeps it running in the background;
            // MICKEY_PAUSE_IN_BACKGROUND=1 restores the original behaviour.
            static const bool pause = [] {
                const char *e = getenv("MICKEY_PAUSE_IN_BACKGROUND");
                return e && *e && strcmp(e, "0") != 0;
            }();
            if (!pause)
                break;
            post((HWND)w, WM_ACTIVATE, WA_INACTIVE, 0);
            post((HWND)w, WM_ACTIVATEAPP, FALSE, 0);
            post((HWND)w, WM_KILLFOCUS, 0, 0);
            break;
        }
        case host::Event::Minimized:
            post((HWND)w, WM_SIZE, SIZE_MINIMIZED, 0);
            break;
        case host::Event::Restored:
            post((HWND)w, WM_SIZE, SIZE_RESTORED, MAKELPARAM(w->client_w, w->client_h));
            w->invalid = true;
            break;
        case host::Event::Exposed:
            w->invalid = true;
            break;
        case host::Event::MouseMove:
            if (s_scripted_mouse)
                break;
            mouse_message(w, WM_MOUSEMOVE, e.x, e.y);
            break;
        case host::Event::MouseDown:
        case host::Event::MouseUp: {
            bool down = e.type == host::Event::MouseDown;
            int vk = e.button == 0 ? VK_LBUTTON : e.button == 1 ? VK_RBUTTON : VK_MBUTTON;
            key_state(vk, down);
            UINT base = vk == VK_LBUTTON ? WM_LBUTTONDOWN : vk == VK_RBUTTON ? WM_RBUTTONDOWN : WM_MBUTTONDOWN;
            UINT msg = down ? (e.clicks == 2 && (w->cls->wc.style & CS_DBLCLKS) ? base + 2 : base) : base + 1;
            mouse_message(w, msg, e.x, e.y);
            break;
        }
        case host::Event::Wheel: {
            int delta = (int)(e.wheel * WHEEL_DELTA);
            post((HWND)w, WM_MOUSEWHEEL, MAKEWPARAM(mouse_keys(), (short)delta), MAKELPARAM(s_mouse_x, s_mouse_y));
            break;
        }
        case host::Event::KeyDown:
        case host::Event::KeyUp: {
            bool down = e.type == host::Event::KeyDown;
            int vk = e.vk;
            if (!vk)
                break;
            bool was_down = (s_keys[vk] & 0x80) != 0;
            key_state(vk, down);
            key_state(e.side_vk, down);
            bool alt = (s_keys[VK_MENU] & 0x80) != 0;
            // lParam: repeat count, scan code, extended, context (Alt), previous state, transition.
            LPARAM lp = 1 | ((LPARAM)(e.scancode & 0xff) << 16) | (alt ? (1 << 29) : 0) |
                        (was_down ? (1 << 30) : 0) | (down ? 0 : (1u << 31));
            UINT msg = alt || vk == VK_MENU || vk == VK_F10 ? (down ? WM_SYSKEYDOWN : WM_SYSKEYUP)
                                                           : (down ? WM_KEYDOWN : WM_KEYUP);
            post((HWND)w, msg, (WPARAM)vk, lp);
            break;
        }
        case host::Event::Text:
            text_message(w, e.text);
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
    BOOL got = next_message(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, (wRemoveMsg & PM_REMOVE) != 0);
    // nothing to do: the game loop idles a frame (the virtual clock's step)
    if (!got && GetCurrentThreadId() == s_main_thread)
        advance_frame();
    return got;
}

BOOL WINAPI GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (!s_main_thread)
        s_main_thread = GetCurrentThreadId();
    while (!next_message(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, true)) {
        if (display_open() && GetCurrentThreadId() == s_main_thread)
            host::wait_event(10);
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
    log("message", "PostQuitMessage(%d)\n", nExitCode);
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
    // Other threads (the game's cursor thread) get the position the main thread saw last;
    // the host's window belongs to the main thread.
    int x, y;
    if (!s_scripted_mouse && (GetCurrentThreadId() == s_main_thread || !s_main_thread) && host::cursor_position(&x, &y)) {
        s_mouse_x = x;
        s_mouse_y = y;
    }
    lpPoint->x = s_mouse_x;
    lpPoint->y = s_mouse_y;
    return TRUE;
}

BOOL WINAPI SetCursorPos(int X, int Y)
{
    s_mouse_x = X;
    s_mouse_y = Y;
    host::cursor_move(X, Y);
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
    if (w && w->on_host)
        host::window_show(true);
    return w != 0;
}

// The game keeps the mouse in its full-screen window; the port leaves the system's
// pointer free (the game's cursor stays in the picture).
BOOL WINAPI ClipCursor(const RECT *) { return TRUE; }

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
    host::cursor_visible(s_cursor_count >= 0);
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
