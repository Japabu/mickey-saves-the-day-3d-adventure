// port.h - interfaces between the parts of the platform library (Win32 layer, software
// DirectDraw/Direct3D, media). Not seen by the game's source.
#ifndef PORT_PLATFORM_PORT_H
#define PORT_PLATFORM_PORT_H

#include <windows.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Display (Win32 layer). The DirectDraw layer hands over each finished frame of the
// primary surface as 32-bit ARGB (0xAARRGGBB in memory order of a uint32 per pixel);
// the Win32 layer uploads it to the game window, scaled to fit (letterboxed). A frame
// of the sharp copies (dx.h) is scale times the size of the game's screen.
void port_present(const void *argb8888, int w, int h, int pitch, int scale);
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

// Modules: called by each library's attach stub (port/flags.py) before its DllMain.
HINSTANCE port_module_attach(const char *dllname, const void *anchor);

#ifdef __cplusplus
}
#endif

#endif
