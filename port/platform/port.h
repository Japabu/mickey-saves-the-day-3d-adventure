// port.h - interfaces between the parts of the platform library (Win32 layer, software
// DirectDraw/Direct3D, media). Not seen by the game's source.
#ifndef PORT_PLATFORM_PORT_H
#define PORT_PLATFORM_PORT_H

#include <windows.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Display. The DirectDraw layer says when the primary surface changed (any thread, any
// number of times a frame: cheap); the Win32 layer, when it shows a frame (at most once
// per display refresh, on the main thread), asks for its pixels: 32-bit ARGB
// (0xAARRGGBB per uint32), w x h, valid until the next call, scale times the game's
// screen for the sharp copies (dx.h). 0 when there is nothing to show.
void port_frame_changed(void);
int port_frame_pixels(const uint32_t **argb, int *w, int *h, int *scale);
// The options panel (win32/options.cpp): the volume in percent of a channel of the
// game's sound manager and of its background music, a counter that changes with them,
// and the other port.ini settings (fallback when unset). port_scale: how many times
// the game's screen the 3D is drawn at (MICKEY_SCALE, else port.ini's scale, else 3).
int port_sound_gain(int channel);
int port_music_gain(void);
unsigned port_sound_gain_version(void);
int port_setting(const char *key, int fallback);
int port_scale(void);
void port_set_scale(int scale);  // takes effect at once (the sharp copies are made again)
// Marks a Miles stream as the background music (its volume follows the music slider).
void port_music_stream(void *stream);
// Blocks until the next display refresh (IDirectDraw::WaitForVerticalBlank).
void port_wait_vblank(void);
// The game's choice of full screen (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) or a window
// (DDSCL_NORMAL), from SetCooperativeLevel. MICKEY_WINDOWED=1 keeps a window anyway.
void port_set_fullscreen(int fullscreen);

// GDI on memory the caller owns (DirectDraw surfaces' GetDC). The DC draws straight into
// pixels: w x h, pitch bytes per row, bpp 8 (palette is the surface's 256 entries), 16
// (RGB 565), 24 or 32 (BGR order as in DIBs). Release it before the pixels go away.
HDC port_gdi_surface_dc(void *pixels, int w, int h, int pitch, int bpp, const PALETTEENTRY *palette);
void port_gdi_release_surface_dc(HDC hdc);
// The display's bits per pixel (DirectDraw layer): the current DirectDraw mode, so
// GetDeviceCaps(BITSPIXEL) agrees with it.
int port_display_bpp(void);

// The C runtime's file functions on the game's virtual file system (win32/crtfs.cpp):
// game paths (C:\..., backslashes, any case, relative to the game's current directory)
// resolve to the CD or the overlay; host paths (starting with '/') pass through.
// port_host_path gives a host path for a game path (for_write 0: an existing file, a CD
// file inside the ISO is copied to a cache first; 1: the overlay's path; 2: the overlay's
// path with the CD's contents copied there first).
struct stat;
FILE *port_fopen(const char *path, const char *mode);
FILE *port_freopen(const char *path, const char *mode, FILE *stream);
int port_open(const char *path, int flags, int mode);
int port_stat(const char *path, struct stat *st);
int port_access(const char *path, int mode);
int port_remove(const char *path);
int port_rename(const char *from, const char *to);
int port_mkdir(const char *path);
int port_rmdir(const char *path);
int port_chdir(const char *path);
char *port_getcwd(char *buf, size_t size);
int port_host_path(const char *path, int for_write, char *out, size_t outsize);

#ifdef __cplusplus
}
#endif

#endif
