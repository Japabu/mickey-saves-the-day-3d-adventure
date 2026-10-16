// rad.h - the Miles Sound System (mss32.dll), Bink (binkw32.dll) and Smacker
// (smackw32.dll) functions the game calls, declared exactly as the game's own source
// declares them: MBEngine/MBMilesSoundEngine.h, CMBMilesSoundManager.h,
// MBMilesStreamingSoundEngine.h, CMBBinkMovie.h, CMBSmackerMovie.h and SteamboatApp/mss.h.
// The structures the game reads fields of (AUDIO_TYPE, BINK, SmackTag) have the game's
// layout, its long being the 32-bit int here (port/rewrite.py), and the Bink sound-system
// parameter that carries the Miles driver pointer is pointer-sized (port/edits/MBEngine.py).
// Clang rejects a conflicting redeclaration of an extern "C" function, so including this
// after those headers in one unit checks the two agree.
#ifndef PORT_MEDIA_RAD_H
#define PORT_MEDIA_RAD_H

#include <windows.h>

// ---- Miles ----
struct _DIG_DRIVER;
struct _MDI_DRIVER;
struct _DLSDEVICE;
struct _SAMPLE;
typedef _SAMPLE *HSAMPLE;
struct _STREAM;
typedef _STREAM *HSTREAM;
struct _DLSFILEID;
struct AUDIO_TYPE {
    const unsigned int *data;
    int size;
    int type;
    void *handle;
    int status;
    void *next;
    int speed;
    int volume;
    int extravol;
    float rlevel;
    float rrtime;
    float rdtime;
    _DLSFILEID *dlsid;
    void *dlsmem;
    void *dlsmemunc;
    int milliseconds;
    int length;
    int userdata;
};
typedef AUDIO_TYPE *HAUDIO;

extern "C" {
int __stdcall AIL_quick_startup(int use_digital, int use_MIDI, unsigned int output_rate, int output_bits,
                                 int output_channels);
void __stdcall AIL_quick_shutdown(void);
void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi, _DLSDEVICE **pdls);
int __stdcall AIL_set_preference(unsigned int number, int value);
HWND__ *__stdcall AIL_HWND(void);
int __stdcall AIL_set_DirectSound_HWND(_DIG_DRIVER *dig, HWND__ *wnd);
void __stdcall AIL_set_digital_master_volume(_DIG_DRIVER *dig, int master_volume);
int __stdcall AIL_digital_master_volume(_DIG_DRIVER *dig);
char *__stdcall AIL_last_error();

HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned int size);
int __stdcall AIL_quick_play(HAUDIO audio, unsigned int loop_count);
void __stdcall AIL_quick_halt(HAUDIO audio);
int __stdcall AIL_quick_status(HAUDIO audio);
void __stdcall AIL_quick_unload(HAUDIO audio);
void __stdcall AIL_quick_set_volume(HAUDIO audio, int volume, int extravol);
void __stdcall AIL_quick_set_speed(HAUDIO audio, int speed);
int __stdcall AIL_quick_ms_position(HAUDIO audio);
void __stdcall AIL_quick_set_ms_position(HAUDIO audio, int milliseconds);
unsigned int __stdcall AIL_sample_playback_rate(HSAMPLE S);
int __stdcall AIL_sample_loop_count(HSAMPLE S);

HSTREAM __stdcall AIL_open_stream(_DIG_DRIVER *dig, const char *filename, int stream_mem);
void __stdcall AIL_close_stream(HSTREAM stream);
void __stdcall AIL_start_stream(HSTREAM stream);
void __stdcall AIL_pause_stream(HSTREAM stream, int onoff);
int __stdcall AIL_stream_status(HSTREAM stream);
void __stdcall AIL_set_stream_volume(HSTREAM stream, int volume);
void __stdcall AIL_set_stream_loop_count(HSTREAM stream, int count);
int __stdcall AIL_auto_service_stream(HSTREAM stream, int onoff);
}

// Miles' status values, as the game tests them.
enum {
    QSTAT_DONE = 1,     // AIL_quick_status: finished or halted
    QSTAT_LOADED = 2,   // loaded, never played
    QSTAT_PLAYING = 3,
    SMP_DONE = 2,       // AIL_stream_status
    SMP_PLAYING = 4,
    SMP_STOPPED = 8,    // paused
};

// ---- Bink ----
struct BINKSND;
struct BINK {
    unsigned int Width;        // +0x00
    unsigned int Height;       // +0x04
    unsigned int Frames;       // +0x08
    unsigned int FrameNum;     // +0x0c
    unsigned int LastFrameNum; // +0x10
    unsigned int FrameRate;    // +0x14
    unsigned int FrameRateDiv; // +0x18
};
typedef int(__stdcall *BINKSNDOPEN)(BINKSND *, unsigned int, int);
typedef BINKSNDOPEN(__stdcall *BINKSNDSYSOPEN)(UINT_PTR);

extern "C" {
BINK *__stdcall BinkOpen(const char *name, unsigned int flags);
char *__stdcall BinkGetError(void);
void __stdcall BinkClose(BINK *bnk);
int __stdcall BinkWait(BINK *bnk);
void __stdcall BinkNextFrame(BINK *bnk);
int __stdcall BinkDoFrame(BINK *bnk);
int __stdcall BinkCopyToBuffer(BINK *bnk, void *dest, int destpitch, unsigned int destheight, unsigned int destx,
                                unsigned int desty, unsigned int flags);
void __stdcall BinkGoto(BINK *bnk, unsigned int frame, int flags);
int __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN open, UINT_PTR param);
BINKSNDOPEN __stdcall BinkOpenMiles(UINT_PTR param);
}

// The flag values the game passes.
enum {
    BINKFILEHANDLE = 0x00800000,  // BinkOpen: `name` is a Win32 file handle, read from its position
    BINKCOPY2XWH = 0x40000000,    // BinkCopyToBuffer: double width and height
    BINKSURFACEMASK = 0x0000000f, // BinkCopyToBuffer: the destination format
    BINKSURFACE32 = 2,            // 32-bit, B G R x in memory
    BINKSURFACE555 = 3,
    BINKSURFACE565 = 4,
};

// ---- Smacker ----
struct SmackTag {
    unsigned int Version;
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int MSPerFrame;
    unsigned int SmackerType;
    unsigned int LargestInTrack[7];
    unsigned int tablesize;
    unsigned int codesize;
    unsigned int absize;
    unsigned int detailsize;
    unsigned int typesize;
    unsigned int TrackType[7];
    unsigned int extra;
    unsigned int NewPalette;
    unsigned char Palette[772];
    unsigned int PalType;
    unsigned int FrameNum;
    unsigned int FrameSize;
    unsigned int SndSize;
    // remaining fields not used here (size 0x3b0)
};
struct _SMACKBLIT;

extern "C" {
SmackTag *__stdcall SmackOpen(const char *name, unsigned int flags, unsigned int extrabuf);
void __stdcall SmackClose(SmackTag *smk);
unsigned int __stdcall SmackDoFrame(SmackTag *smk);
void __stdcall SmackNextFrame(SmackTag *smk);
unsigned int __stdcall SmackWait(SmackTag *smk);
void __stdcall SmackGoto(SmackTag *smk, unsigned int frame);
void __stdcall SmackToBuffer(SmackTag *smk, unsigned int left, unsigned int top, unsigned int pitch,
                             unsigned int destheight, void *buf, unsigned int flags);
unsigned char __stdcall SmackSoundUseMSS(void *dd);
unsigned int __stdcall SmackUseMMX(unsigned int flag);
_SMACKBLIT *__stdcall SmackBlitOpen(unsigned int flags);
void __stdcall SmackBlitClose(_SMACKBLIT *sblit);
void __stdcall SmackBlitSetPalette(_SMACKBLIT *sblit, void *palette, unsigned int paltype);
void __stdcall SmackBlit(_SMACKBLIT *sblit, void *dest, unsigned int destpitch, unsigned int destx,
                         unsigned int desty, void *src, unsigned int srcpitch, unsigned int srcx,
                         unsigned int srcy, unsigned int srcw, unsigned int srch);
}

enum {
    SMACKFILEHANDLE = 0x1000,     // SmackOpen: `name` is a Win32 file handle
    SMACKBUFFER555 = 0x80000000,  // SmackToBuffer / SmackBlitOpen: 16-bit 555 output
    SMACKBUFFER565 = 0xc0000000,  // 16-bit 565 output
    SMACKBUFFERMASK = 0xc0000000,
    SMACKBLIT2X = 2,              // SmackBlitOpen: double width and height
    SMACKBLIT2XSMOOTH = 4,
};

#endif
