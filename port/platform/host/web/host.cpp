// host.cpp - the host in a browser (Emscripten).
#include "../host.h"

namespace host {

bool window_open(const char *, int, int, bool) { return false; }
void window_close() {}
void window_show(bool) {}
void window_title(const char *) {}
void window_fullscreen(bool) {}
bool window_is_fullscreen() { return false; }
void window_minimize(bool) {}
bool window_is_minimized() { return false; }
bool window_visible() { return false; }
void present(const uint32_t *, int, int, int, bool) {}
void cursor_visible(bool) {}
void cursor_move(int, int) {}
bool cursor_position(int *, int *) { return false; }
bool poll_event(Event *) { return false; }
void wait_event(int) {}
int sound_open(SoundFill, void *) { return 0; }
void sound_close() {}
void *font_create(const char *, double, bool, bool) { return 0; }
void font_release(void *) {}
void font_metrics(void *, int *ascent, int *descent, int *leading) { *ascent = *descent = *leading = 0; }
int text_width(void *, const char *, int) { return 0; }
void text_render(void *, const char *, int, uint8_t *, int, int, int, bool) {}

}  // namespace host
