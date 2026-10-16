// rad.h - the Miles Sound System (mss32.dll), Bink (binkw32.dll) and Smacker
// (smackw32.dll) functions the game calls, declared exactly as the game's own source
// declares them: MBEngine/MBMilesSoundEngine.h, CMBMilesSoundManager.h,
// MBMilesStreamingSoundEngine.h, CMBBinkMovie.h, CMBSmackerMovie.h and SteamboatApp/mss.h.
// The structures the game reads fields of (AUDIO_TYPE, BINK, SmackTag) have the game's
// layout, `long` included, so both sides agree on the 64-bit target too. (Clang rejects a
// conflicting redeclaration of an extern "C" function, so including this after those
// headers in one unit checks the two agree.)
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
    const unsigned long *data;
    long size;
    long type;
    void *handle;
    long status;
    void *next;
    long speed;
    long volume;
    long extravol;
    float rlevel;
    float rrtime;
    float rdtime;
    _DLSFILEID *dlsid;
    void *dlsmem;
    void *dlsmemunc;
    long milliseconds;
    long length;
    long userdata;
};
typedef AUDIO_TYPE *HAUDIO;

extern "C" {
long __stdcall AIL_quick_startup(long use_digital, long use_MIDI, unsigned long output_rate, long output_bits,
                                 long output_channels);
void __stdcall AIL_quick_shutdown(void);
void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi, _DLSDEVICE **pdls);
long __stdcall AIL_set_preference(unsigned long number, long value);
HWND__ *__stdcall AIL_HWND(void);
long __stdcall AIL_set_DirectSound_HWND(_DIG_DRIVER *dig, HWND__ *wnd);
void __stdcall AIL_set_digital_master_volume(_DIG_DRIVER *dig, long master_volume);
long __stdcall AIL_digital_master_volume(_DIG_DRIVER *dig);
char *__stdcall AIL_last_error();

HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned long size);
long __stdcall AIL_quick_play(HAUDIO audio, unsigned long loop_count);
void __stdcall AIL_quick_halt(HAUDIO audio);
long __stdcall AIL_quick_status(HAUDIO audio);
void __stdcall AIL_quick_unload(HAUDIO audio);
void __stdcall AIL_quick_set_volume(HAUDIO audio, long volume, long extravol);
void __stdcall AIL_quick_set_speed(HAUDIO audio, long speed);
long __stdcall AIL_quick_ms_position(HAUDIO audio);
void __stdcall AIL_quick_set_ms_position(HAUDIO audio, long milliseconds);
unsigned long __stdcall AIL_sample_playback_rate(HSAMPLE S);
long __stdcall AIL_sample_loop_count(HSAMPLE S);

HSTREAM __stdcall AIL_open_stream(_DIG_DRIVER *dig, const char *filename, long stream_mem);
void __stdcall AIL_close_stream(HSTREAM stream);
void __stdcall AIL_start_stream(HSTREAM stream);
void __stdcall AIL_pause_stream(HSTREAM stream, long onoff);
long __stdcall AIL_stream_status(HSTREAM stream);
void __stdcall AIL_set_stream_volume(HSTREAM stream, long volume);
void __stdcall AIL_set_stream_loop_count(HSTREAM stream, long count);
long __stdcall AIL_auto_service_stream(HSTREAM stream, long onoff);
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
    unsigned long Width;        // +0x00
    unsigned long Height;       // +0x04
    unsigned long Frames;       // +0x08
    unsigned long FrameNum;     // +0x0c
    unsigned long LastFrameNum; // +0x10
    unsigned long FrameRate;    // +0x14
    unsigned long FrameRateDiv; // +0x18
};
typedef long(__stdcall *BINKSNDOPEN)(BINKSND *, unsigned long, long);
typedef BINKSNDOPEN(__stdcall *BINKSNDSYSOPEN)(unsigned long);

extern "C" {
BINK *__stdcall BinkOpen(const char *name, unsigned long flags);
char *__stdcall BinkGetError(void);
void __stdcall BinkClose(BINK *bnk);
long __stdcall BinkWait(BINK *bnk);
void __stdcall BinkNextFrame(BINK *bnk);
long __stdcall BinkDoFrame(BINK *bnk);
long __stdcall BinkCopyToBuffer(BINK *bnk, void *dest, long destpitch, unsigned long destheight, unsigned long destx,
                                unsigned long desty, unsigned long flags);
void __stdcall BinkGoto(BINK *bnk, unsigned long frame, long flags);
long __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN open, unsigned long param);
BINKSNDOPEN __stdcall BinkOpenMiles(unsigned long param);
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
    unsigned long Version;
    unsigned long Width;
    unsigned long Height;
    unsigned long Frames;
    unsigned long MSPerFrame;
    unsigned long SmackerType;
    unsigned long LargestInTrack[7];
    unsigned long tablesize;
    unsigned long codesize;
    unsigned long absize;
    unsigned long detailsize;
    unsigned long typesize;
    unsigned long TrackType[7];
    unsigned long extra;
    unsigned long NewPalette;
    unsigned char Palette[772];
    unsigned long PalType;
    unsigned long FrameNum;
    unsigned long FrameSize;
    unsigned long SndSize;
    // remaining fields not used here (size 0x3b0)
};
struct _SMACKBLIT;

extern "C" {
SmackTag *__stdcall SmackOpen(const char *name, unsigned long flags, unsigned long extrabuf);
void __stdcall SmackClose(SmackTag *smk);
unsigned long __stdcall SmackDoFrame(SmackTag *smk);
void __stdcall SmackNextFrame(SmackTag *smk);
unsigned long __stdcall SmackWait(SmackTag *smk);
void __stdcall SmackGoto(SmackTag *smk, unsigned long frame);
void __stdcall SmackToBuffer(SmackTag *smk, unsigned long left, unsigned long top, unsigned long pitch,
                             unsigned long destheight, void *buf, unsigned long flags);
unsigned char __stdcall SmackSoundUseMSS(void *dd);
unsigned long __stdcall SmackUseMMX(unsigned long flag);
_SMACKBLIT *__stdcall SmackBlitOpen(unsigned long flags);
void __stdcall SmackBlitClose(_SMACKBLIT *sblit);
void __stdcall SmackBlitSetPalette(_SMACKBLIT *sblit, void *palette, unsigned long paltype);
void __stdcall SmackBlit(_SMACKBLIT *sblit, void *dest, unsigned long destpitch, unsigned long destx,
                         unsigned long desty, void *src, unsigned long srcpitch, unsigned long srcx,
                         unsigned long srcy, unsigned long srcw, unsigned long srch);
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
