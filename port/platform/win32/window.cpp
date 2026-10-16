// window.cpp - window classes and windows: creation and destruction, window longs, text,
// position and visibility, rectangles, system metrics, message boxes, and the dialog,
// menu and tray calls of the developer tools (logged stubs).
#include <SDL3/SDL.h>

#include "user.h"

#include <mutex>
#include <set>
#include <string.h>
#include <strings.h>
#include <vector>

namespace win32 {

static std::recursive_mutex s_mu;
static std::vector<WindowClass *> s_classes;
static std::set<Window *> s_windows;
static ATOM s_next_atom = 0xC000;

static WindowClass *find_class(LPCSTR name)
{
    for (WindowClass *c : s_classes) {
        if (IS_INTRESOURCE(name)) {
            if ((ATOM)(uintptr_t)name == (ATOM)(uintptr_t)c->wc.lpszClassName)
                return c;
        } else if (!strcasecmp(c->name.c_str(), name)) {
            return c;
        }
    }
    return 0;
}

Window *window(HWND h)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    Window *w = (Window *)h;
    if (!h || !s_windows.count(w) || w->destroyed) {
        SetLastError(1400);  // ERROR_INVALID_WINDOW_HANDLE
        return 0;
    }
    return w;
}

HWND main_window()
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    for (Window *w : s_windows)
        if (!w->parent && !w->destroyed && w->sdl)
            return (HWND)w;
    return 0;
}

LRESULT send(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    Window *w = window(h);
    if (!w || !w->proc)
        return 0;
    return w->proc(h, msg, wp, lp);
}

// The frame the game adds around its client area (GetSystemMetrics below).
static const int FRAME = 3, CAPTION = 19;

static void client_from_window(Window *w)
{
    int cw = w->rect.right - w->rect.left, ch = w->rect.bottom - w->rect.top;
    if (!w->parent && (w->style & WS_CAPTION) == WS_CAPTION) {
        cw -= 2 * FRAME;
        ch -= 2 * FRAME + CAPTION;
    }
    w->client_w = cw > 0 ? cw : 0;
    w->client_h = ch > 0 ? ch : 0;
}

}  // namespace win32

using namespace win32;

extern "C" {

ATOM WINAPI RegisterClassExA(const WNDCLASSEXA *wcx)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    if (find_class(wcx->lpszClassName)) {
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }
    WindowClass *c = new WindowClass;
    c->wc = *wcx;
    c->name = IS_INTRESOURCE(wcx->lpszClassName) ? "" : wcx->lpszClassName;
    c->wc.lpszClassName = c->name.c_str();
    ATOM atom = s_next_atom++;
    s_classes.push_back(c);
    log("window", "RegisterClass %s\n", c->name.c_str());
    return atom;
}

ATOM WINAPI RegisterClassA(const WNDCLASSA *wc)
{
    WNDCLASSEXA x;
    memset(&x, 0, sizeof(x));
    x.cbSize = sizeof(x);
    x.style = wc->style;
    x.lpfnWndProc = wc->lpfnWndProc;
    x.cbClsExtra = wc->cbClsExtra;
    x.cbWndExtra = wc->cbWndExtra;
    x.hInstance = wc->hInstance;
    x.hIcon = wc->hIcon;
    x.hCursor = wc->hCursor;
    x.hbrBackground = wc->hbrBackground;
    x.lpszMenuName = wc->lpszMenuName;
    x.lpszClassName = wc->lpszClassName;
    return RegisterClassExA(&x);
}

BOOL WINAPI UnregisterClassA(LPCSTR lpClassName, HINSTANCE)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    for (size_t i = 0; i < s_classes.size(); i++)
        if (s_classes[i] == find_class(lpClassName)) {
            s_classes.erase(s_classes.begin() + (long)i);
            return TRUE;
        }
    return FALSE;
}

HWND WINAPI CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
                            int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    WindowClass *c;
    {
        std::lock_guard<std::recursive_mutex> lock(s_mu);
        c = find_class(lpClassName);
    }
    if (!c) {
        log("window", "CreateWindow: class %s not registered\n",
            IS_INTRESOURCE(lpClassName) ? "#atom" : lpClassName);
        SetLastError(1407);  // ERROR_CANNOT_FIND_WND_CLASS
        return 0;
    }
    Window *w = new Window;
    w->cls = c;
    w->proc = c->wc.lpfnWndProc;
    w->style = dwStyle;
    w->exstyle = dwExStyle;
    w->parent = hWndParent;
    w->menu = hMenu;
    w->instance = hInstance;
    w->text = lpWindowName ? lpWindowName : "";
    w->thread = GetCurrentThreadId();
    if (X == CW_USEDEFAULT)
        X = Y = 0;
    if (nWidth == CW_USEDEFAULT) {
        nWidth = 640 + 2 * FRAME;
        nHeight = 480 + 2 * FRAME + CAPTION;
    }
    w->rect = {X, Y, X + nWidth, Y + nHeight};
    client_from_window(w);
    {
        std::lock_guard<std::recursive_mutex> lock(s_mu);
        s_windows.insert(w);
    }
    HWND h = (HWND)w;
    CREATESTRUCTA cs = {lpParam, hInstance, hMenu, hWndParent, nHeight, nWidth, Y, X, (LONG)dwStyle,
                        lpWindowName, IS_INTRESOURCE(lpClassName) ? c->name.c_str() : lpClassName, dwExStyle};
    if (!send(h, WM_NCCREATE, 0, (LPARAM)&cs) && w->proc != DefWindowProcA) {
        // A window procedure that does not pass WM_NCCREATE on refuses the window only
        // when it returns 0 on purpose; DefWindowProc returns TRUE.
    }
    if (send(h, WM_CREATE, 0, (LPARAM)&cs) == -1) {
        DestroyWindow(h);
        return 0;
    }
    if (dwStyle & WS_VISIBLE)
        ShowWindow(h, SW_SHOW);
    log("window", "CreateWindow %s \"%s\" %dx%d -> %p\n", c->name.c_str(), w->text.c_str(), nWidth, nHeight, (void *)w);
    return h;
}

BOOL WINAPI DestroyWindow(HWND hWnd)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    // Children first.
    std::vector<Window *> kids;
    {
        std::lock_guard<std::recursive_mutex> lock(s_mu);
        for (Window *k : s_windows)
            if (k->parent == hWnd && !k->destroyed)
                kids.push_back(k);
    }
    for (Window *k : kids)
        DestroyWindow((HWND)k);
    send(hWnd, WM_DESTROY, 0, 0);
    send(hWnd, WM_NCDESTROY, 0, 0);
    display_destroy(w);
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    w->destroyed = true;  // kept allocated: stale handles stay recognisable
    return TRUE;
}

BOOL WINAPI IsWindow(HWND hWnd) { return window(hWnd) != 0; }
BOOL WINAPI IsWindowVisible(HWND hWnd)
{
    Window *w = window(hWnd);
    return w && w->visible;
}
BOOL WINAPI IsIconic(HWND hWnd)
{
    Window *w = window(hWnd);
    return w && w->sdl && (SDL_GetWindowFlags(w->sdl) & SDL_WINDOW_MINIMIZED);
}
BOOL WINAPI IsZoomed(HWND) { return FALSE; }

BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    BOOL was = w->visible;
    bool show = nCmdShow != SW_HIDE;
    if (show != (was != 0)) {
        w->visible = show;
        if (show)
            w->style |= WS_VISIBLE;
        else
            w->style &= ~WS_VISIBLE;
        send(hWnd, WM_SHOWWINDOW, show, 0);
        if (!w->parent) {
            if (show)
                display_show(w);
            else
                display_hide(w);
        }
        if (show) {
            send(hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(w->client_w, w->client_h));
            w->invalid = true;
        }
    }
    if (w->sdl && nCmdShow == SW_MINIMIZE)
        SDL_MinimizeWindow(w->sdl);
    if (w->sdl && nCmdShow == SW_RESTORE)
        SDL_RestoreWindow(w->sdl);
    return was;
}

BOOL WINAPI UpdateWindow(HWND hWnd)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    if (w->invalid) {
        w->invalid = false;
        send(hWnd, WM_PAINT, 0, 0);
    }
    return TRUE;
}

BOOL WINAPI InvalidateRect(HWND hWnd, const RECT *, BOOL)
{
    Window *w = hWnd ? window(hWnd) : 0;
    if (w)
        w->invalid = true;
    return TRUE;
}

BOOL WINAPI ValidateRect(HWND hWnd, const RECT *)
{
    Window *w = hWnd ? window(hWnd) : 0;
    if (w)
        w->invalid = false;
    return TRUE;
}

BOOL WINAPI GetUpdateRect(HWND hWnd, LPRECT lpRect, BOOL)
{
    Window *w = window(hWnd);
    if (!w || !w->invalid)
        return FALSE;
    if (lpRect)
        SetRect(lpRect, 0, 0, w->client_w, w->client_h);
    return TRUE;
}

// Window longs. GWL_USERDATA and the extra bytes keep pointer-sized values
// (Get/SetWindowLongPtr); the 32-bit calls see their low half.
LONG_PTR WINAPI GetWindowLongPtrA(HWND hWnd, int nIndex)
{
    Window *w = window(hWnd);
    if (!w)
        return 0;
    switch (nIndex) {
    case GWL_WNDPROC: return (LONG_PTR)w->proc;
    case GWL_STYLE: return (LONG_PTR)(LONG)w->style;
    case GWL_EXSTYLE: return (LONG_PTR)(LONG)w->exstyle;
    case GWL_HINSTANCE: return (LONG_PTR)w->instance;
    case GWL_HWNDPARENT: return (LONG_PTR)w->parent;
    case GWL_ID: return (LONG_PTR)w->menu;
    }
    auto it = w->longs.find(nIndex);
    return it == w->longs.end() ? 0 : it->second;
}

LONG_PTR WINAPI SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR v)
{
    Window *w = window(hWnd);
    if (!w)
        return 0;
    LONG_PTR old = GetWindowLongPtrA(hWnd, nIndex);
    switch (nIndex) {
    case GWL_WNDPROC: w->proc = (WNDPROC)v; break;
    case GWL_STYLE: w->style = (DWORD)v; break;
    case GWL_EXSTYLE: w->exstyle = (DWORD)v; break;
    case GWL_HINSTANCE: w->instance = (HINSTANCE)v; break;
    case GWL_HWNDPARENT: w->parent = (HWND)v; break;
    case GWL_ID: w->menu = (HMENU)v; break;
    default: w->longs[nIndex] = v;
    }
    return old;
}

LONG WINAPI GetWindowLongA(HWND hWnd, int nIndex)
{
    if (nIndex == GWL_WNDPROC || nIndex == GWL_USERDATA || nIndex >= 0)
        log("window", "GetWindowLong(%d) of a pointer-sized value: use GetWindowLongPtr\n", nIndex);
    return (LONG)GetWindowLongPtrA(hWnd, nIndex);
}

LONG WINAPI SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
{
    return (LONG)SetWindowLongPtrA(hWnd, nIndex, (LONG_PTR)dwNewLong);
}

BOOL WINAPI SetWindowTextA(HWND hWnd, LPCSTR lpString)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    w->text = lpString ? lpString : "";
    display_title(w);
    return TRUE;
}

int WINAPI GetWindowTextA(HWND hWnd, LPSTR lpString, int nMaxCount)
{
    Window *w = window(hWnd);
    if (!w || nMaxCount <= 0)
        return 0;
    strncpy(lpString, w->text.c_str(), (size_t)nMaxCount - 1);
    lpString[nMaxCount - 1] = 0;
    return (int)strlen(lpString);
}

int WINAPI GetWindowTextLengthA(HWND hWnd)
{
    Window *w = window(hWnd);
    return w ? (int)w->text.size() : 0;
}

BOOL WINAPI GetClientRect(HWND hWnd, LPRECT lpRect)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    SetRect(lpRect, 0, 0, w->client_w, w->client_h);
    return TRUE;
}

BOOL WINAPI GetWindowRect(HWND hWnd, LPRECT lpRect)
{
    Window *w = hWnd == GetDesktopWindow() ? 0 : window(hWnd);
    if (!w) {
        SetRect(lpRect, 0, 0, display_width(), display_height());
        return hWnd == GetDesktopWindow();
    }
    *lpRect = w->rect;
    return TRUE;
}

// The main window's client area starts at the screen origin.
BOOL WINAPI ClientToScreen(HWND hWnd, LPPOINT) { return window(hWnd) != 0; }
BOOL WINAPI ScreenToClient(HWND hWnd, LPPOINT) { return window(hWnd) != 0; }

BOOL WINAPI MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL)
{
    return SetWindowPos(hWnd, 0, X, Y, nWidth, nHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL WINAPI SetWindowPos(HWND hWnd, HWND, int X, int Y, int cx, int cy, UINT uFlags)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    RECT r = w->rect;
    if (!(uFlags & SWP_NOMOVE)) {
        OffsetRect(&r, X - r.left, Y - r.top);
    }
    if (!(uFlags & SWP_NOSIZE)) {
        r.right = r.left + cx;
        r.bottom = r.top + cy;
    }
    bool resized = (r.right - r.left) != (w->rect.right - w->rect.left) || (r.bottom - r.top) != (w->rect.bottom - w->rect.top);
    w->rect = r;
    client_from_window(w);
    if (resized)
        send(hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(w->client_w, w->client_h));
    if (uFlags & SWP_SHOWWINDOW)
        ShowWindow(hWnd, SW_SHOW);
    if (uFlags & SWP_HIDEWINDOW)
        ShowWindow(hWnd, SW_HIDE);
    return TRUE;
}

HWND WINAPI GetParent(HWND hWnd)
{
    Window *w = window(hWnd);
    return w ? w->parent : 0;
}

HWND WINAPI GetDesktopWindow(void)
{
    static int desktop;
    return (HWND)&desktop;
}

HWND WINAPI FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    for (Window *w : s_windows) {
        if (w->destroyed)
            continue;
        if (lpClassName && strcasecmp(w->cls->name.c_str(), lpClassName))
            continue;
        if (lpWindowName && w->text != lpWindowName)
            continue;
        return (HWND)w;
    }
    return 0;
}

BOOL WINAPI EnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    std::vector<Window *> list;
    {
        std::lock_guard<std::recursive_mutex> lock(s_mu);
        for (Window *w : s_windows)
            if (!w->destroyed && !w->parent)
                list.push_back(w);
    }
    for (Window *w : list)
        if (!lpEnumFunc((HWND)w, lParam))
            break;
    return TRUE;
}

DWORD WINAPI GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId)
{
    Window *w = window(hWnd);
    if (lpdwProcessId)
        *lpdwProcessId = GetCurrentProcessId();
    return w ? w->thread : 0;
}

BOOL WINAPI EnableWindow(HWND hWnd, BOOL bEnable)
{
    Window *w = window(hWnd);
    if (!w)
        return FALSE;
    BOOL was = (w->style & WS_DISABLED) != 0;
    if (bEnable)
        w->style &= ~WS_DISABLED;
    else
        w->style |= WS_DISABLED;
    send(hWnd, WM_ENABLE, bEnable, 0);
    return was;
}

// Rectangles.
BOOL WINAPI SetRect(LPRECT r, int l, int t, int rt, int b)
{
    if (!r)
        return FALSE;
    r->left = l;
    r->top = t;
    r->right = rt;
    r->bottom = b;
    return TRUE;
}

BOOL WINAPI SetRectEmpty(LPRECT r) { return SetRect(r, 0, 0, 0, 0); }

BOOL WINAPI CopyRect(LPRECT d, const RECT *s)
{
    if (!d || !s)
        return FALSE;
    *d = *s;
    return TRUE;
}

BOOL WINAPI OffsetRect(LPRECT r, int dx, int dy)
{
    if (!r)
        return FALSE;
    r->left += dx;
    r->right += dx;
    r->top += dy;
    r->bottom += dy;
    return TRUE;
}

BOOL WINAPI InflateRect(LPRECT r, int dx, int dy)
{
    if (!r)
        return FALSE;
    r->left -= dx;
    r->right += dx;
    r->top -= dy;
    r->bottom += dy;
    return TRUE;
}

BOOL WINAPI IsRectEmpty(const RECT *r) { return !r || r->right <= r->left || r->bottom <= r->top; }

BOOL WINAPI EqualRect(const RECT *a, const RECT *b)
{
    return a && b && a->left == b->left && a->top == b->top && a->right == b->right && a->bottom == b->bottom;
}

BOOL WINAPI PtInRect(const RECT *r, POINT p) { return r && p.x >= r->left && p.x < r->right && p.y >= r->top && p.y < r->bottom; }

BOOL WINAPI IntersectRect(LPRECT d, const RECT *a, const RECT *b)
{
    RECT r = {a->left > b->left ? a->left : b->left, a->top > b->top ? a->top : b->top,
              a->right < b->right ? a->right : b->right, a->bottom < b->bottom ? a->bottom : b->bottom};
    if (IsRectEmpty(&r)) {
        SetRectEmpty(d);
        return FALSE;
    }
    *d = r;
    return TRUE;
}

BOOL WINAPI UnionRect(LPRECT d, const RECT *a, const RECT *b)
{
    if (IsRectEmpty(a)) {
        *d = IsRectEmpty(b) ? RECT{0, 0, 0, 0} : *b;
        return !IsRectEmpty(b);
    }
    if (IsRectEmpty(b)) {
        *d = *a;
        return TRUE;
    }
    RECT r = {a->left < b->left ? a->left : b->left, a->top < b->top ? a->top : b->top,
              a->right > b->right ? a->right : b->right, a->bottom > b->bottom ? a->bottom : b->bottom};
    *d = r;
    return TRUE;
}

BOOL WINAPI SubtractRect(LPRECT d, const RECT *a, const RECT *b)
{
    // Only the cases where b covers a whole side of a shrink the result, as on Windows.
    RECT r = *a;
    RECT i;
    if (IntersectRect(&i, a, b)) {
        if (i.top == a->top && i.bottom == a->bottom) {
            if (i.left == a->left)
                r.left = i.right;
            else if (i.right == a->right)
                r.right = i.left;
        } else if (i.left == a->left && i.right == a->right) {
            if (i.top == a->top)
                r.top = i.bottom;
            else if (i.bottom == a->bottom)
                r.bottom = i.top;
        }
    }
    *d = r;
    return !IsRectEmpty(&r);
}

int WINAPI GetSystemMetrics(int nIndex)
{
    switch (nIndex) {
    case SM_CXSCREEN:
    case SM_CXFULLSCREEN:
    case SM_CXMAXIMIZED:
    case SM_CXMAXTRACK: return display_width();
    case SM_CYSCREEN:
    case SM_CYFULLSCREEN:
    case SM_CYMAXIMIZED:
    case SM_CYMAXTRACK: return display_height();
    case SM_CXFIXEDFRAME:
    case SM_CYFIXEDFRAME:
    case SM_CXFRAME:
    case SM_CYFRAME: return FRAME;
    case SM_CXBORDER:
    case SM_CYBORDER: return 1;
    case SM_CYCAPTION:
    case SM_CYMENU: return CAPTION;
    case SM_CXICON:
    case SM_CYICON:
    case SM_CXCURSOR:
    case SM_CYCURSOR: return 32;
    case SM_CXSMICON:
    case SM_CYSMICON: return 16;
    case SM_CXVSCROLL:
    case SM_CYHSCROLL: return 16;
    case SM_MOUSEPRESENT: return 1;
    case SM_CMOUSEBUTTONS: return 3;
    case SM_SWAPBUTTON: return 0;
    }
    return 0;
}

DWORD WINAPI GetSysColor(int nIndex)
{
    switch (nIndex) {
    case COLOR_WINDOW: return RGB(255, 255, 255);
    case COLOR_WINDOWTEXT:
    case COLOR_BTNTEXT:
    case COLOR_MENUTEXT:
    case COLOR_CAPTIONTEXT: return RGB(0, 0, 0);
    case COLOR_BTNSHADOW: return RGB(128, 128, 128);
    case COLOR_BTNHIGHLIGHT: return RGB(255, 255, 255);
    case COLOR_GRAYTEXT: return RGB(128, 128, 128);
    }
    return RGB(192, 192, 192);
}

BOOL WINAPI SystemParametersInfoA(UINT, UINT, PVOID, UINT) { return FALSE; }

// Message boxes.
int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    Window *w = hWnd ? window(hWnd) : 0;
    SDL_MessageBoxButtonData buttons[3];
    int n = 0;
    auto add = [&](int id, const char *label, Uint32 flags) {
        buttons[n].flags = flags;
        buttons[n].buttonID = id;
        buttons[n].text = label;
        n++;
    };
    switch (uType & MB_TYPEMASK) {
    case MB_OKCANCEL:
        add(IDOK, "OK", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
        add(IDCANCEL, "Cancel", SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
        break;
    case MB_YESNO:
        add(IDYES, "Yes", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
        add(IDNO, "No", SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
        break;
    case MB_YESNOCANCEL:
        add(IDYES, "Yes", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
        add(IDNO, "No", 0);
        add(IDCANCEL, "Cancel", SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
        break;
    case MB_RETRYCANCEL:
        add(IDRETRY, "Retry", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
        add(IDCANCEL, "Cancel", SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
        break;
    case MB_ABORTRETRYIGNORE:
        add(IDABORT, "Abort", 0);
        add(IDRETRY, "Retry", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);
        add(IDIGNORE, "Ignore", SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
        break;
    default:
        add(IDOK, "OK", SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);
    }
    // The texts are in code page 1252; SDL wants UTF-8.
    auto utf8 = [](LPCSTR s) {
        std::string out;
        if (!s)
            return out;
        int nw = MultiByteToWideChar(CP_ACP, 0, s, -1, 0, 0);
        std::vector<WCHAR> wbuf((size_t)nw);
        MultiByteToWideChar(CP_ACP, 0, s, -1, wbuf.data(), nw);
        int nb = WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), nw, 0, 0, 0, 0);
        out.resize((size_t)nb);
        WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), nw, &out[0], nb, 0, 0);
        if (!out.empty() && out.back() == 0)
            out.pop_back();
        return out;
    };
    std::string text = utf8(lpText), title = utf8(lpCaption);
    fprintf(stderr, "MessageBox: %s: %s\n", title.c_str(), text.c_str());
    SDL_MessageBoxData data = {};
    data.flags = (uType & MB_ICONMASK) == MB_ICONHAND ? SDL_MESSAGEBOX_ERROR
                 : (uType & MB_ICONMASK) == MB_ICONEXCLAMATION ? SDL_MESSAGEBOX_WARNING
                                                                 : SDL_MESSAGEBOX_INFORMATION;
    data.window = w ? w->sdl : 0;
    data.title = title.c_str();
    data.message = text.c_str();
    data.numbuttons = n;
    data.buttons = buttons;
    int id = buttons[0].buttonID;
    if (!SDL_ShowMessageBox(&data, &id))
        log("window", "MessageBox: %s\n", SDL_GetError());
    return id;
}

int WINAPI MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    auto ansi = [](LPCWSTR s) {
        std::string out;
        if (!s)
            return out;
        int n = WideCharToMultiByte(CP_ACP, 0, s, -1, 0, 0, 0, 0);
        out.resize((size_t)n);
        WideCharToMultiByte(CP_ACP, 0, s, -1, &out[0], n, 0, 0);
        return out;
    };
    std::string t = ansi(lpText), c = ansi(lpCaption);
    return MessageBoxA(hWnd, t.c_str(), c.c_str(), uType);
}

BOOL WINAPI MessageBeep(UINT) { return TRUE; }

// Dialogs, menus, the tray and the clipboard belong to the developer tools: stubs.
INT_PTR WINAPI DialogBoxParamA(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM)
{
    PORT_STUB("DialogBoxParam");
    return -1;
}

HWND WINAPI CreateDialogParamA(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM)
{
    PORT_STUB("CreateDialogParam");
    return 0;
}

BOOL WINAPI EndDialog(HWND, INT_PTR) { return TRUE; }
BOOL WINAPI IsDialogMessageA(HWND, LPMSG) { return FALSE; }
HWND WINAPI GetDlgItem(HWND, int) { return 0; }
int WINAPI GetDlgCtrlID(HWND) { return 0; }
LRESULT WINAPI SendDlgItemMessageA(HWND, int, UINT, WPARAM, LPARAM) { return 0; }
BOOL WINAPI SetDlgItemTextA(HWND, int, LPCSTR) { return TRUE; }
UINT WINAPI GetDlgItemTextA(HWND, int, LPSTR lpString, int cchMax)
{
    if (cchMax > 0)
        lpString[0] = 0;
    return 0;
}
BOOL WINAPI SetDlgItemInt(HWND, int, UINT, BOOL) { return TRUE; }
UINT WINAPI GetDlgItemInt(HWND, int, BOOL *ok, BOOL)
{
    if (ok)
        *ok = FALSE;
    return 0;
}
BOOL WINAPI CheckDlgButton(HWND, int, UINT) { return TRUE; }
UINT WINAPI IsDlgButtonChecked(HWND, int) { return BST_UNCHECKED; }

struct MenuObject {
    int dummy;
};

HMENU WINAPI CreatePopupMenu(void) { return (HMENU) new MenuObject; }
HMENU WINAPI CreateMenu(void) { return (HMENU) new MenuObject; }
HMENU WINAPI LoadMenuA(HINSTANCE, LPCSTR) { return (HMENU) new MenuObject; }
BOOL WINAPI DestroyMenu(HMENU hMenu)
{
    delete (MenuObject *)hMenu;
    return TRUE;
}
BOOL WINAPI AppendMenuA(HMENU, UINT, UINT_PTR, LPCSTR) { return TRUE; }
DWORD WINAPI CheckMenuItem(HMENU, UINT, UINT) { return MF_UNCHECKED; }
BOOL WINAPI EnableMenuItem(HMENU, UINT, UINT) { return MF_ENABLED; }
HMENU WINAPI GetSubMenu(HMENU, int) { return 0; }
HMENU WINAPI GetSystemMenu(HWND, BOOL) { return 0; }
BOOL WINAPI GetMenuItemRect(HWND, HMENU, UINT, LPRECT r)
{
    SetRectEmpty(r);
    return FALSE;
}
BOOL WINAPI SetMenuDefaultItem(HMENU, UINT, UINT) { return TRUE; }
BOOL WINAPI SetMenu(HWND, HMENU) { return TRUE; }
HMENU WINAPI GetMenu(HWND) { return 0; }
BOOL WINAPI DrawMenuBar(HWND) { return TRUE; }
BOOL WINAPI TrackPopupMenu(HMENU, UINT, int, int, int, HWND, const RECT *) { return FALSE; }

HACCEL WINAPI LoadAcceleratorsA(HINSTANCE, LPCSTR) { return 0; }
int WINAPI TranslateAcceleratorA(HWND, HACCEL, LPMSG) { return 0; }

BOOL WINAPI Shell_NotifyIconA(DWORD, PNOTIFYICONDATAA) { return TRUE; }
HINSTANCE WINAPI ShellExecuteA(HWND, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR, LPCSTR, int)
{
    log("process", "ShellExecute(%s, %s) refused\n", lpOperation ? lpOperation : "open", lpFile ? lpFile : "-");
    return (HINSTANCE)(uintptr_t)2;  // SE_ERR_FILENOTFOUND
}
UINT WINAPI DragQueryFileA(HDROP, UINT, LPSTR, UINT) { return 0; }
void WINAPI DragFinish(HDROP) {}
void WINAPI DragAcceptFiles(HWND, BOOL) {}

BOOL WINAPI OpenClipboard(HWND) { return FALSE; }
BOOL WINAPI CloseClipboard(void) { return TRUE; }
HANDLE WINAPI GetClipboardData(UINT) { return 0; }

HIMC WINAPI ImmAssociateContext(HWND, HIMC) { return 0; }
HIMC WINAPI ImmGetContext(HWND) { return 0; }
BOOL WINAPI ImmReleaseContext(HWND, HIMC) { return TRUE; }

void WINAPI InitCommonControls(void) {}
BOOL WINAPI InitCommonControlsEx(const INITCOMMONCONTROLSEX *) { return TRUE; }
BOOL WINAPI ImageList_Destroy(HIMAGELIST) { return TRUE; }
BOOL WINAPI ImageList_Draw(HIMAGELIST, int, HDC, int, int, UINT) { return FALSE; }
HIMAGELIST WINAPI ImageList_LoadImageA(HINSTANCE, LPCSTR, int, int, COLORREF, UINT, UINT) { return 0; }
HIMAGELIST WINAPI ImageList_Create(int, int, UINT, int, int) { return 0; }
int WINAPI ImageList_GetImageCount(HIMAGELIST) { return 0; }
BOOL WINAPI ImageList_GetIconSize(HIMAGELIST, int *cx, int *cy)
{
    *cx = *cy = 0;
    return FALSE;
}

BOOL WINAPI GetOpenFileNameA(LPOPENFILENAMEA) { return FALSE; }
BOOL WINAPI GetSaveFileNameA(LPOPENFILENAMEA) { return FALSE; }
BOOL WINAPI PrintDlgA(LPPRINTDLGA) { return FALSE; }
DWORD WINAPI CommDlgExtendedError(void) { return 0; }

DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR, LPDWORD lpdwHandle)
{
    if (lpdwHandle)
        *lpdwHandle = 0;
    return 0;
}
BOOL WINAPI GetFileVersionInfoA(LPCSTR, DWORD, DWORD, LPVOID) { return FALSE; }
BOOL WINAPI VerQueryValueA(LPCVOID, LPCSTR, LPVOID *, PUINT) { return FALSE; }

HRESULT WINAPI CoInitialize(LPVOID) { return S_OK; }
HRESULT WINAPI CoInitializeEx(LPVOID, DWORD) { return S_OK; }
void WINAPI CoUninitialize(void) {}
HRESULT WINAPI CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *ppv)
{
    if (ppv)
        *ppv = 0;
    return REGDB_E_CLASSNOTREG;
}
LPVOID WINAPI CoTaskMemAlloc(SIZE_T cb) { return malloc(cb); }
void WINAPI CoTaskMemFree(LPVOID pv) { free(pv); }

}  // extern "C"
