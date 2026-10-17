// IMBFont.h - draws and measures text.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBFont.
#ifndef MBENGINE_IMBFONT_H
#define MBENGINE_IMBFONT_H

class IMBSurface;
class MBRect;
class MBSize;

class IMBFont : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFont; }
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual long DrawText(IMBSurface *, long, long, const unsigned short *const, unsigned long) = 0; // +0x18
    virtual long DrawText(IMBSurface *, MBRect &, const unsigned short *const, unsigned long) = 0;    // +0x14
    virtual long DrawText(IMBSurface *, long, long, const char *const, unsigned long) = 0;           // +0x10
    virtual long DrawText(IMBSurface *, MBRect &, const char *const, unsigned long) = 0;              // +0x0c
    virtual long GetTextSize(IMBSurface *, const unsigned short *, unsigned long, MBSize &) = 0;
    virtual long GetTextSize(IMBSurface *, const char *, unsigned long, MBSize &) = 0;
};

#endif
