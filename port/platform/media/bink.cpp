// bink.cpp - the Bink calls of the game (binkw32.dll), as CMBBinkMovie makes them, on
// FFmpeg's Bink decoder.
//
// CMBBinkMovie's loop: BinkWait says whether the next picture is due; BinkDoFrame decodes
// the current one (the loop around it skips frames while it returns 1; here it never
// does, decoding is fast enough); BinkCopyToBuffer converts it into the 16-bit surface;
// BinkNextFrame moves on, and from the last frame back to the first. FrameNum counts
// from 1. The clock starts with the first BinkDoFrame, and so does the sound, which
// plays through the audio device like the Miles sounds (BinkOpenMiles): the master
// volume applies to it.
#include "movie.h"

#include <string>

#pragma GCC visibility push(hidden)

namespace {

std::string s_error;

struct BinkMovie {
    BINK pub;
    media::Movie movie;
    HANDLE owned = 0;  // the file BinkOpen opened itself (a name, not a handle)
};

BinkMovie *bink(BINK *b) { return (BinkMovie *)b; }

long __stdcall no_sound_open(BINKSND *, unsigned long, long) { return 0; }

void reset_frame(BinkMovie *b)
{
    b->movie.rewind();
    b->pub.FrameNum = 1;
}

}  // namespace

#pragma GCC visibility push(default)

extern "C" {

BINK *__stdcall BinkOpen(const char *name, unsigned long flags)
{
    if (!name) {
        s_error = "no file";
        return 0;
    }
    HANDLE h = (HANDLE)name, owned = 0;
    if (!(flags & BINKFILEHANDLE)) {
        h = owned = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (h == INVALID_HANDLE_VALUE) {
            s_error = std::string("cannot open ") + name;
            return 0;
        }
    }
    BinkMovie *b = new BinkMovie();
    b->owned = owned;
    if (!b->movie.open(h, owned ? name : "bink movie")) {
        s_error = "not a Bink movie FFmpeg can read";
        if (owned)
            CloseHandle(owned);
        delete b;
        return 0;
    }
    AVRational r = b->movie.rate();
    b->pub.Width = (unsigned long)b->movie.width();
    b->pub.Height = (unsigned long)b->movie.height();
    b->pub.Frames = (unsigned long)(b->movie.frames() > 0 ? b->movie.frames() : 1);
    b->pub.FrameNum = 1;
    b->pub.LastFrameNum = 0;
    b->pub.FrameRate = (unsigned long)r.num;
    b->pub.FrameRateDiv = (unsigned long)r.den;
    return &b->pub;
}

char *__stdcall BinkGetError(void) { return (char *)s_error.c_str(); }

void __stdcall BinkClose(BINK *bnk)
{
    if (!bnk)
        return;
    BinkMovie *b = bink(bnk);
    HANDLE owned = b->owned;
    delete b;
    if (owned)
        CloseHandle(owned);
}

long __stdcall BinkWait(BINK *bnk)
{
    if (!bnk)
        return 0;
    BinkMovie *b = bink(bnk);
    if (!b->movie.started())
        return 0;
    return b->movie.due((long)bnk->FrameNum - 1) ? 0 : 1;
}

long __stdcall BinkDoFrame(BINK *bnk)
{
    if (!bnk)
        return 0;
    BinkMovie *b = bink(bnk);
    if (!b->movie.started())
        b->movie.start();
    b->movie.decode((long)bnk->FrameNum - 1);
    bnk->LastFrameNum = bnk->FrameNum;
    return 0;
}

void __stdcall BinkNextFrame(BINK *bnk)
{
    if (!bnk)
        return;
    if (bnk->FrameNum >= bnk->Frames)
        reset_frame(bink(bnk));
    else
        bnk->FrameNum++;
}

void __stdcall BinkGoto(BINK *bnk, unsigned long frame, long flags)
{
    (void)flags;
    if (!bnk)
        return;
    if (frame <= 1)
        reset_frame(bink(bnk));
    else
        bnk->FrameNum = frame > bnk->Frames ? bnk->Frames : frame;  // decoded on the next BinkDoFrame
}

long __stdcall BinkCopyToBuffer(BINK *bnk, void *dest, long destpitch, unsigned long destheight, unsigned long destx,
                                unsigned long desty, unsigned long flags)
{
    if (!bnk || !dest)
        return 0;
    BinkMovie *b = bink(bnk);
    if (b->movie.decoded < 0)
        return 0;
    AVPixelFormat fmt;
    switch (flags & BINKSURFACEMASK) {
    case BINKSURFACE32: fmt = AV_PIX_FMT_BGR0; break;
    case BINKSURFACE555: fmt = AV_PIX_FMT_RGB555LE; break;
    case BINKSURFACE565: fmt = AV_PIX_FMT_RGB565LE; break;
    default:
        MEDIA_LOG("BinkCopyToBuffer: surface type %lu is not implemented\n", flags & BINKSURFACEMASK);
        return 0;
    }
    int scale = (flags & 0x70000000) == BINKCOPY2XWH ? 2 : 1;
    media::blit_frame(b->movie.frame, fmt, (int)bnk->Width * scale, (int)bnk->Height * scale, (uint8_t *)dest,
                      destpitch, (long)destheight, (long)destx, (long)desty);
    return 0;
}

long __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN open, unsigned long param)
{
    (void)open;
    (void)param;  // the sound always goes to the audio device
    return 1;
}

BINKSNDOPEN __stdcall BinkOpenMiles(unsigned long param)
{
    (void)param;
    return no_sound_open;
}

}  // extern "C"
