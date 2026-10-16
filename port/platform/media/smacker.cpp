// smacker.cpp - the Smacker calls of the game (smackw32.dll), as CMBSmackerMovie makes
// them, on FFmpeg's Smacker decoder. (The game's CD has no Smacker movie; the engine
// supports them, so the port does too.)
//
// CMBSmackerMovie's loop: SmackWait says whether the next picture is due; it names the
// destination with SmackToBuffer (8-bit for the doubling blitter, or 16-bit 555/565);
// SmackDoFrame puts the current picture there; SmackNextFrame moves on, from the last
// frame back to frame 0. FrameNum counts from 0. NewPalette and Palette describe the
// current frame before SmackDoFrame (the engine hands them to SmackBlitSetPalette
// first), so each frame is decoded when it becomes current. Palette entries are
// 8-bit R, G, B, here and in SmackBlitSetPalette.
#include "movie.h"

#include <string.h>

#pragma GCC visibility push(hidden)

namespace {

struct SmackMovie {
    SmackTag pub;
    media::Movie movie;
    HANDLE owned = 0;
    // SmackToBuffer's destination
    uint8_t *buf = 0;
    unsigned long left = 0, top = 0, pitch = 0, rows = 0, flags = 0;
};

struct Blitter {
    unsigned long flags;
    uint16_t pal16[256];
};

SmackMovie *smack(SmackTag *s) { return (SmackMovie *)s; }

uint16_t pack16(const uint8_t *rgb, bool is565)
{
    if (is565)
        return (uint16_t)(((rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | (rgb[2] >> 3));
    return (uint16_t)(((rgb[0] >> 3) << 10) | ((rgb[1] >> 3) << 5) | (rgb[2] >> 3));
}

// Decodes the frame FrameNum names and takes over its palette.
void make_current(SmackMovie *s)
{
    if (!s->movie.decode((long)s->pub.FrameNum))
        return;
    AVFrame *f = s->movie.frame;
    if (f->format != AV_PIX_FMT_PAL8 || !f->data[1])
        return;
    uint8_t pal[768];
    const uint32_t *argb = (const uint32_t *)f->data[1];
    for (int i = 0; i < 256; i++) {
        pal[i * 3] = (uint8_t)(argb[i] >> 16);
        pal[i * 3 + 1] = (uint8_t)(argb[i] >> 8);
        pal[i * 3 + 2] = (uint8_t)argb[i];
    }
    s->pub.NewPalette = memcmp(pal, s->pub.Palette, sizeof pal) != 0;
    memcpy(s->pub.Palette, pal, sizeof pal);
}

void restart(SmackMovie *s)
{
    s->movie.rewind();
    s->pub.FrameNum = 0;
    make_current(s);
}

}  // namespace

#pragma GCC visibility push(default)

extern "C" {

SmackTag *__stdcall SmackOpen(const char *name, unsigned long flags, unsigned long extrabuf)
{
    (void)extrabuf;
    if (!name)
        return 0;
    HANDLE h = (HANDLE)name, owned = 0;
    if (!(flags & SMACKFILEHANDLE)) {
        h = owned = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (h == INVALID_HANDLE_VALUE)
            return 0;
    }
    SmackMovie *s = new SmackMovie();
    memset(&s->pub, 0, sizeof s->pub);
    s->owned = owned;
    if (!s->movie.open(h, owned ? name : "smacker movie")) {
        if (owned)
            CloseHandle(owned);
        delete s;
        return 0;
    }
    AVRational r = s->movie.rate();
    s->pub.Version = 0x324b4d53;  // "SMK2"
    s->pub.Width = (unsigned long)s->movie.width();
    s->pub.Height = (unsigned long)s->movie.height();
    s->pub.Frames = (unsigned long)(s->movie.frames() > 0 ? s->movie.frames() : 1);
    s->pub.MSPerFrame = (unsigned long)(1000LL * r.den / r.num);
    s->pub.FrameNum = 0;
    make_current(s);
    s->pub.NewPalette = 1;
    return &s->pub;
}

void __stdcall SmackClose(SmackTag *smk)
{
    if (!smk)
        return;
    SmackMovie *s = smack(smk);
    HANDLE owned = s->owned;
    delete s;
    if (owned)
        CloseHandle(owned);
}

unsigned long __stdcall SmackWait(SmackTag *smk)
{
    if (!smk || !smack(smk)->movie.started())
        return 0;
    return smack(smk)->movie.due((long)smk->FrameNum) ? 0 : 1;
}

void __stdcall SmackToBuffer(SmackTag *smk, unsigned long left, unsigned long top, unsigned long pitch,
                             unsigned long destheight, void *buf, unsigned long flags)
{
    if (!smk)
        return;
    SmackMovie *s = smack(smk);
    s->buf = (uint8_t *)buf;
    s->left = left;
    s->top = top;
    s->pitch = pitch;
    s->rows = destheight;
    s->flags = flags;
}

unsigned long __stdcall SmackDoFrame(SmackTag *smk)
{
    if (!smk)
        return 0;
    SmackMovie *s = smack(smk);
    if (!s->movie.started())
        s->movie.start();
    AVFrame *f = s->movie.frame;
    if (!s->buf || s->movie.decoded < 0 || f->format != AV_PIX_FMT_PAL8)
        return 0;
    unsigned long kind = s->flags & SMACKBUFFERMASK;
    uint16_t pal16[256];
    if (kind)
        for (int i = 0; i < 256; i++)
            pal16[i] = pack16(&smk->Palette[i * 3], kind == SMACKBUFFER565);
    long bpp = kind ? 2 : 1;
    for (long y = 0; y < f->height && s->top + y < s->rows; y++) {
        const uint8_t *src = f->data[0] + y * f->linesize[0];
        uint8_t *dst = s->buf + (s->top + y) * s->pitch + s->left * bpp;
        long n = f->width;
        if ((long)(s->left + n) * bpp > (long)s->pitch)
            n = (long)s->pitch / bpp - (long)s->left;
        if (n <= 0)
            break;
        if (!kind)
            memcpy(dst, src, (size_t)n);
        else
            for (long x = 0; x < n; x++)
                ((uint16_t *)dst)[x] = pal16[src[x]];
    }
    smk->NewPalette = 0;
    return 0;
}

void __stdcall SmackNextFrame(SmackTag *smk)
{
    if (!smk)
        return;
    SmackMovie *s = smack(smk);
    if (smk->FrameNum + 1 >= smk->Frames) {
        restart(s);
        return;
    }
    smk->FrameNum++;
    make_current(s);
}

void __stdcall SmackGoto(SmackTag *smk, unsigned long frame)
{
    if (!smk)
        return;
    SmackMovie *s = smack(smk);
    if (frame == 0) {
        restart(s);
        return;
    }
    smk->FrameNum = frame < smk->Frames ? frame : smk->Frames - 1;
    make_current(s);
}

unsigned char __stdcall SmackSoundUseMSS(void *dd)
{
    (void)dd;  // the sound always goes to the audio device
    return 1;
}

unsigned long __stdcall SmackUseMMX(unsigned long flag)
{
    (void)flag;
    return 0;
}

_SMACKBLIT *__stdcall SmackBlitOpen(unsigned long flags)
{
    Blitter *b = new Blitter();
    b->flags = flags;
    memset(b->pal16, 0, sizeof b->pal16);
    return (_SMACKBLIT *)b;
}

void __stdcall SmackBlitClose(_SMACKBLIT *sblit) { delete (Blitter *)sblit; }

void __stdcall SmackBlitSetPalette(_SMACKBLIT *sblit, void *palette, unsigned long paltype)
{
    (void)paltype;
    if (!sblit || !palette)
        return;
    Blitter *b = (Blitter *)sblit;
    for (int i = 0; i < 256; i++)
        b->pal16[i] = pack16((const uint8_t *)palette + i * 3, (b->flags & SMACKBUFFERMASK) == SMACKBUFFER565);
}

void __stdcall SmackBlit(_SMACKBLIT *sblit, void *dest, unsigned long destpitch, unsigned long destx,
                         unsigned long desty, void *src, unsigned long srcpitch, unsigned long srcx,
                         unsigned long srcy, unsigned long srcw, unsigned long srch)
{
    if (!sblit || !dest || !src)
        return;
    Blitter *b = (Blitter *)sblit;
    unsigned long scale = (b->flags & (SMACKBLIT2X | SMACKBLIT2XSMOOTH)) ? 2 : 1;
    for (unsigned long y = 0; y < srch * scale; y++) {
        const uint8_t *s = (const uint8_t *)src + (srcy + y / scale) * srcpitch + srcx;
        uint16_t *d = (uint16_t *)((uint8_t *)dest + (desty + y) * destpitch) + destx;
        for (unsigned long x = 0; x < srcw * scale; x++)
            d[x] = b->pal16[s[x / scale]];
    }
}

}  // extern "C"
