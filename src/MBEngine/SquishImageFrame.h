// SquishImageFrame.h - a Squish image frame: a list of blocks.
// Declared first by the includer (still one copy per unit in wave 1): MBVector.
#ifndef MBENGINE_SQUISHIMAGEFRAME_H
#define MBENGINE_SQUISHIMAGEFRAME_H

class SquishFile;
class SquishImageBlock;
class SquishImageDescriptor;
class SquishPalette;
class SquishRect;

class SquishImageFrame {
public:
    SquishImageFrame(void);
    ~SquishImageFrame(void);
    unsigned long GetWidth(void) const { return mWidth; }
    unsigned long GetHeight(void) const { return mHeight; }
    unsigned long GetDepth(void) const { return mDepth; }
    unsigned long GetNBlocks(void) const { return mBlocks.size(); }
    bool StreamIn(SquishFile *file);
    bool StreamOut(SquishFile *file) const;
    bool AddBlock(SquishImageBlock *block);
    bool DeleteBlock(unsigned int index);
    bool DeleteBlock(SquishImageBlock *block);
    SquishImageBlock *GetBlock(unsigned int index) const;
    void Reset(void);
    void MoveBlockOffsets(short mx, short my);
    bool Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
    bool BlitTrans(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
protected:
    bool BlitBlock(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect, SquishRect *destRect, bool blitTrans) const;
public:
    bool BlitBlock(unsigned char *d, const SquishImageDescriptor *f, SquishRect *s, SquishRect *r) const { return BlitBlock(d, f, s, r, false); }
    bool BlitBlockTrans(unsigned char *d, const SquishImageDescriptor *f, SquishRect *s, SquishRect *r) const { return BlitBlock(d, f, s, r, true); }
    bool Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette);
    MBVector<SquishImageBlock *> mBlocks;
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
};

#endif
