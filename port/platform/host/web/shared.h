// shared.h - what the web host shares with the page (port/web/mickey.js): a block of
// 32-bit fields in the program's memory, which the page finds through
// host_web_shared() and reads and writes with Atomics. The game runs on a worker; the
// page's thread draws the frames, turns DOM events into the event ring and plays the
// sound ring. Keep the field numbers in step with mickey.js.
#ifndef PORT_HOST_WEB_SHARED_H
#define PORT_HOST_WEB_SHARED_H

#include <atomic>
#include <stdint.h>

namespace host {
namespace web {

enum Field {
    MAGIC,            // 'MICK'
    FRAME_SEQ,        // C: counts the frames presented
    FRAME_FRONT,      // C: the buffer with the newest frame, -1 before the first
    FRAME_READING,    // page: the buffer it copies now, -1 when none
    FRAME_PTR,        // C: three buffers (FRAME_PTR + i), 0xAARRGGBB pixels
    FRAME_W = FRAME_PTR + 3,
    FRAME_H = FRAME_W + 3,
    FRAME_SCALE = FRAME_H + 3,
    SMOOTH = FRAME_SCALE + 3,   // C: filtered scaling
    WINDOW_SHOWN,     // C
    CURSOR_VISIBLE,   // C: the system pointer over the canvas
    FULLSCREEN_WANT,  // C: 0/1, the page asks the browser
    FULLSCREEN_IS,    // page
    PAGE_VISIBLE,     // page: the tab can be seen
    MOUSE_X,          // page: the pointer in the game's screen, -1 when unknown
    MOUSE_Y,
    EV_HEAD,          // page: events pushed (the ring's write count)
    EV_TAIL,          // C: events taken
    SOUND_RATE,       // page: the AudioContext's rate, 0 without sound
    SOUND_RING,       // C: the ring's address (interleaved stereo float)
    SOUND_FRAMES,     // C: its size in frames
    SOUND_WRITE,      // C: frames written (wraps)
    SOUND_READ,       // page: frames played (wraps)
    GAME_W,           // C: the game's screen
    GAME_H,
    FIELDS = 40
};

// An event of the ring: 18 int32 (EVENT_INTS) per event, EVENTS of them after the fields.
enum { EVENTS = 256, EVENT_INTS = 18 };
enum EventField { E_TYPE, E_X, E_Y, E_BUTTON, E_CLICKS, E_WHEEL_MILLI, E_VK, E_SIDE_VK, E_SCANCODE, E_FLAGS, E_TEXT };
enum { E_FLAG_REPEAT = 1, E_FLAG_COMMAND = 2 };

struct Shared {
    std::atomic<int32_t> f[FIELDS];
    int32_t events[EVENTS][EVENT_INTS];
};

Shared &shared();

}  // namespace web
}  // namespace host

#endif
