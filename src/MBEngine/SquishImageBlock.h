// SquishImageBlock.h - a block of a Squish image frame.
#ifndef MBENGINE_SQUISHIMAGEBLOCK_H
#define MBENGINE_SQUISHIMAGEBLOCK_H

#include "SquishImageDescriptor.h"

class SquishFile;
class SquishFileFilter;
class SquishPalette;
class SquishRect;

class SquishImageBlock {
public:
    SquishImageBlock(void);
    ~SquishImageBlock(void);
    bool Create(const SquishImageBlock *srcBlock, const SquishImageDescriptor *creationHints, long srcx, long srcy,
                long srcw, long srch);
    bool Create(const unsigned char *src, const SquishImageDescriptor *srcFmt, const SquishImageDescriptor *creationHints,
                long srcx, long srcy, long srcw, long srch);
    bool CreateEmpty(SquishImageDescriptor *hints);
    bool Load(SquishFileFilter *filter, SquishImageDescriptor *creationHints);
    bool Save(SquishFileFilter *filter);
    bool Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
    bool BlitTrans(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
    bool BlitBlock(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect, SquishRect *destRect) const;
    bool BlitBlockTrans(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect, SquishRect *destRect) const;
    bool Fill(const SquishPixel &pixel, SquishRect *r);
    bool Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette);
    bool StreamIn(SquishFile *file);
    bool StreamOut(SquishFile *file) const;
    long GetX(void) const { return xOffset; }
    long GetY(void) const { return yOffset; }
    unsigned long GetWidth(void) const { return desc.width; }
    unsigned long GetHeight(void) const { return desc.height; }
    unsigned long GetDepth(void) const { return desc.depth; }
    const unsigned char *Lock(void) const;
    unsigned char *Lock(void);
    void Unlock(const unsigned char *ptr) const;
    void Unlock(unsigned char *ptr);
    void Reset(void);
    static long __cdecl CalcBestRowbytes(unsigned short width, unsigned short depth);
protected:
    bool RemapIndexedToIndexed(SquishPalette *newPalette);
public:
    short xOffset;
    short yOffset;
    SquishImageDescriptor desc;
    unsigned char *mData;
};

#endif
