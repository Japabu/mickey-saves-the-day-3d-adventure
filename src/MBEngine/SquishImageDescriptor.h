// SquishImageDescriptor.h - the size, depth, format and palette of Squish image data.
#ifndef MBENGINE_SQUISHIMAGEDESCRIPTOR_H
#define MBENGINE_SQUISHIMAGEDESCRIPTOR_H

#include "SquishPixel.h"

class SquishFile;
class SquishPalette;

class SquishImageDescriptor {
public:
    unsigned short width;
    unsigned short height;
    unsigned short depth;
    unsigned short rowbytes;
    unsigned long datasize;
    unsigned long flags;
protected:
    SquishPixel transColor;
    unsigned long nativeTransColor;
    SquishPalette *palette;
public:
    SquishImageDescriptor(const SquishImageDescriptor &rhs);
    SquishImageDescriptor(void);
    ~SquishImageDescriptor(void);
    SquishImageDescriptor &operator=(const SquishImageDescriptor &rhs);
    unsigned short GetWidth(void) const { return width; }
    unsigned short GetHeight(void) const { return height; }
    unsigned short GetDepth(void) const { return depth; }
    unsigned short GetRowbytes(void) const { return rowbytes; }
    void Reset(void);
    bool StreamIn(SquishFile *file);
    bool StreamOut(SquishFile *file) const;
    bool HasPalette(void) const { return (flags >> 13) & 1; }
    void SetPalette(SquishPalette *pal);
    SquishPalette *GetPalette(void) const { return palette; }
    void SetTransOff(void);
    void SetTransColor(SquishPixel trans);
    SquishPixel GetTransColor(void) const { return SquishPixel(TransEnabled() ? transColor.Pixel.bgra32 : 0xffffffff); }
    unsigned long GetNativeTransColor(void) const { return nativeTransColor; }
    bool TransEnabled(void) const { return (flags >> 12) & 1; }
    void SetPixelFmt(unsigned long pixfmt);
    unsigned long GetPixelFmt(void) const { return flags & kSquishPixFmtMask; }
    void SetCompFmt(unsigned long f) { flags &= ~kSquishCompFmtMask; flags |= f; }
    unsigned long GetCompFmt(void) const { return flags & kSquishCompFmtMask; }
};

#endif
