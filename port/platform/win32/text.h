// text.h - glyph rendering for GDI text (text.cpp, on CoreText). Kept apart from the
// Windows headers: CoreServices declares names of its own (GetCurrentProcess, ...).
#ifndef PORT_WIN32_TEXT_H
#define PORT_WIN32_TEXT_H

#include <stdint.h>

namespace win32 {
namespace text {

// A font by its Windows face name, px pixels high (the character height), 0 if none.
void *font_create(const char *face, double px, bool bold, bool italic);
void font_release(void *font);
void font_metrics(void *font, int *ascent, int *descent, int *leading);
// Width in pixels of n code page 1252 characters.
int width(void *font, const char *s, int n);
// Draws the characters into an 8-bit coverage mask of w x h (top row first), with the
// baseline `descent` pixels above the bottom edge; aa: antialiased.
void render(void *font, const char *s, int n, uint8_t *mask, int w, int h, int descent, bool aa);

}  // namespace text
}  // namespace win32

#endif
