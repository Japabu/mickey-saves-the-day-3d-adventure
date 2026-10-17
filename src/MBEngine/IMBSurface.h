// IMBSurface.h - a drawing surface (pixels, palette, blits).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBSurface.
#ifndef MBENGINE_IMBSURFACE_H
#define MBENGINE_IMBSURFACE_H

class MBColor;
class MBPalette;
struct MBPixelFormat;
class MBRect;

class IMBSurface : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurface; }
    virtual unsigned long GetWidth(void) = 0;
    virtual unsigned long GetHeight(void) = 0;
    virtual unsigned long GetPitch(void) = 0;
    virtual unsigned long GetSize(void) = 0;
    virtual unsigned long GetDepth(void) = 0;
    virtual MBPixelFormat GetFormat(void) = 0;
    virtual long Lock(void **, unsigned long) = 0;
    virtual long Unlock(void **) = 0;
    virtual MBColor GetPixelColor(unsigned long, unsigned long) = 0;
    virtual long SetTransparentColor(MBColor) = 0;
    virtual MBColor GetTransparentColor(void) = 0;
    virtual long SetPalette(MBPalette *) = 0;
    virtual MBPalette *GetPalette(void) = 0;
    virtual long ColorFill(MBColor, MBRect *) = 0;
    virtual long Blit(IMBSurface *, MBRect *, MBRect *, unsigned long) = 0;
};

#endif
