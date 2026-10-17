// host.h - what the port needs from the system it runs on.
//
// The platform (Win32, DirectX and the media APIs for the game) is the same on every
// system. It assumes the C and C++ standard libraries and POSIX threads and files; all
// else comes through this interface. A system is supported by implementing it, in
// backends under port/platform/host/<backend>/ (the build picks the backends of a
// target, see port/CMakeLists.txt):
//   sdl       the window, input and sound on SDL3 (macOS, Linux, Windows)
//   coretext  fonts through CoreText (macOS)
//   posix     crash reports from signals (macOS, Linux)
//   web       all of it in a browser (Emscripten)
//
// Unless a function says otherwise, the platform calls it on the game's main thread (the
// thread that runs WinMain), which is also where the host delivers events.
#ifndef PORT_HOST_H
#define PORT_HOST_H

#include <stdint.h>

namespace host {

// ---- Start
// Called first in main, before the game reads a file or a setting: a host prepares what
// the platform expects (the web host attaches the browser's storage and names the CD and
// the data directory in MICKEY_CD and MICKEY_DATA).
void startup();

// ---- The window
// The game has one window. Its picture is the game's screen (w x h, 640 x 480), scaled to
// fit the window or the screen and letterboxed; positions in events and of the cursor
// are in the game's screen.

// Opens the window, hidden; false if there is none to be had.
bool window_open(const char *title_utf8, int w, int h, bool fullscreen);
void window_close();
void window_show(bool shown);           // shows and raises, or hides
void window_title(const char *title_utf8);
void window_fullscreen(bool on);
bool window_is_fullscreen();
void window_minimize(bool minimized);   // false: restores
bool window_is_minimized();
// Whether a frame would be seen now (shown, not minimized, not covered).
bool window_visible();

// Shows a frame: w x h pixels of 0xAARRGGBB (alpha ignored), `scale` times the game's
// screen in each direction; smooth: filtered scaling instead of sharp pixels. The pixels
// are only read during the call.
void present(const uint32_t *argb, int w, int h, int scale, bool smooth);

// The system's mouse pointer over the window: hidden while the game draws its own.
void cursor_visible(bool visible);
void cursor_move(int x, int y);
bool cursor_position(int *x, int *y);   // false when unknown

// ---- Events
struct Event {
    enum Type {
        Close,          // the window's close button, or the system asks the game to quit
        FocusIn,
        FocusOut,
        Minimized,
        Restored,
        Exposed,        // the window must be drawn again
        MouseMove,
        MouseDown,
        MouseUp,
        Wheel,
        KeyDown,
        KeyUp,
        Text,           // characters typed
    } type;
    int x, y;           // mouse events: the position in the game's screen
    int button;         // MouseDown/Up: 0 left, 1 right, 2 middle
    int clicks;         // MouseDown: 2 for the second press of a double click
    float wheel;        // Wheel: notches, positive away from the user
    int vk;             // keys: the Windows virtual-key code (VK_SHIFT, 'A', ...), 0 if none
    int side_vk;        // keys: VK_LSHIFT, VK_RCONTROL, ... for a modifier, else 0
    int scancode;       // keys: the key's scan code (the low byte goes into lParam)
    bool repeat;        // KeyDown: held down
    bool command;       // keys: the system's shortcut modifier is held (Cmd on macOS, else Ctrl)
    char text[32];      // Text: UTF-8
};
// The key codes a host reports: Windows' virtual-key codes. Letters and digits are their
// upper-case ASCII codes ('A', '0'); the others:
enum Key {
    KeyBack = 0x08, KeyTab = 0x09, KeyReturn = 0x0D, KeyShift = 0x10, KeyControl = 0x11, KeyAlt = 0x12,
    KeyPause = 0x13, KeyCapsLock = 0x14, KeyEscape = 0x1B, KeySpace = 0x20, KeyPageUp = 0x21, KeyPageDown = 0x22,
    KeyEnd = 0x23, KeyHome = 0x24, KeyLeft = 0x25, KeyUp = 0x26, KeyRight = 0x27, KeyDown = 0x28,
    KeyPrintScreen = 0x2C, KeyInsert = 0x2D, KeyDelete = 0x2E, KeyLeftWin = 0x5B, KeyRightWin = 0x5C,
    KeyNumpad0 = 0x60, KeyMultiply = 0x6A, KeyAdd = 0x6B, KeySubtract = 0x6D, KeyDecimal = 0x6E, KeyDivide = 0x6F,
    KeyF1 = 0x70, KeyF10 = 0x79, KeyNumLock = 0x90, KeyScrollLock = 0x91,
    KeyLeftShift = 0xA0, KeyRightShift = 0xA1, KeyLeftControl = 0xA2, KeyRightControl = 0xA3, KeyLeftAlt = 0xA4,
    KeyRightAlt = 0xA5,
    KeySemicolon = 0xBA, KeyEquals = 0xBB, KeyComma = 0xBC, KeyMinus = 0xBD, KeyPeriod = 0xBE, KeySlash = 0xBF,
    KeyGrave = 0xC0, KeyLeftBracket = 0xDB, KeyBackslash = 0xDC, KeyRightBracket = 0xDD, KeyApostrophe = 0xDE,
};

// The next event; false when there is none.
bool poll_event(Event *e);
// Waits until there is an event, or ms milliseconds.
void wait_event(int ms);

// ---- Sound
// The output: the host calls fill on a thread of its own for `frames` frames of
// interleaved stereo float samples (-1..1); fill must not block. The result is the rate
// the samples play at (Hz), 0 if there is no sound output.
typedef void (*SoundFill)(float *out, int frames, void *user);
int sound_open(SoundFill fill, void *user);
void sound_close();

// ---- Text
// Fonts by their Windows face names (Arial, Times New Roman, ...); px is the character
// height in pixels. Any thread.
void *font_create(const char *face, double px, bool bold, bool italic);  // 0 if none
void font_release(void *font);
void font_metrics(void *font, int *ascent, int *descent, int *leading);
// Width in pixels of n code page 1252 characters.
int text_width(void *font, const char *s, int n);
// Draws n code page 1252 characters into an 8-bit coverage mask of w x h (top row
// first), the baseline `descent` pixels above the bottom edge; aa: antialiased.
void text_render(void *font, const char *s, int n, uint8_t *mask, int w, int h, int descent, bool aa);

}  // namespace host

#endif
