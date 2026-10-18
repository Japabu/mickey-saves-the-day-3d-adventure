// SquishImageBlock unit (MBEngine.dll): one rectangle of image data, raw or RLE-compressed
#include "Squish.h"

// 0x10063d10 ??0SquishImageBlock@@QAE@XZ
SquishImageBlock::SquishImageBlock(void) : xOffset(0), yOffset(0), mData(0)
{
}

// 0x10063d40 ??1SquishImageBlock@@QAE@XZ
SquishImageBlock::~SquishImageBlock(void)
{
    Reset();
}

// 0x10063d60 ?Load@SquishImageBlock@@QAE_NPAVSquishFileFilter@@PAVSquishImageDescriptor@@@Z
bool SquishImageBlock::Load(SquishFileFilter *filter, SquishImageDescriptor *creationHints)
{
    Reset();
    if (!filter)
        return false;
    SquishImageDescriptor newDesc;
    if (!filter->Describe(&newDesc))
        return false;
    if (creationHints->GetCompFmt() != 0x200) {
        SquishImageBlock tempBlock;
        if (!tempBlock.CreateEmpty(&newDesc))
            return false;
        ImageDataPtr data(&tempBlock);
        if (!filter->Read(&tempBlock.desc, data))
            return false;
        if (!Create(&tempBlock, creationHints, 0, 0, -1, -1))
            return false;
        return true;
    }
    if (!CreateEmpty(&newDesc))
        return false;
    ImageDataPtr data(this);
    if (!filter->Read(&desc, data))
        return false;
    return true;
}

// 0x10063f20 ?Save@SquishImageBlock@@QAE_NPAVSquishFileFilter@@@Z
bool SquishImageBlock::Save(SquishFileFilter *filter)
{
    if (!filter || desc.depth < 8)
        return false;
    if (desc.GetCompFmt() == 0x200 && desc.GetPixelFmt() != kSquishPixFmtRGB565)
        return filter->Write(this);
    SquishImageBlock tempBlock;
    SquishImageDescriptor emptyHints;
    if (!tempBlock.Create(this, &emptyHints, 0, 0, -1, -1))
        return false;
    if (tempBlock.desc.GetPixelFmt() == kSquishPixFmtRGB565)
        tempBlock.Remap(16, kSquishPixFmtARGB1555, 0);
    if (!filter->Write(&tempBlock))
        return false;
    return true;
}

// 0x10064000 ?CreateEmpty@SquishImageBlock@@QAE_NPAVSquishImageDescriptor@@@Z
bool SquishImageBlock::CreateEmpty(SquishImageDescriptor *hints)
{
    Reset();
    if (!hints->width || !hints->height || !hints->depth)
        return false;
    desc.width = hints->width;
    desc.height = hints->height;
    desc.depth = hints->depth;
    desc.SetCompFmt(0x200);
    unsigned long pixfmt = hints->GetPixelFmt();
    if (pixfmt) {
        desc.SetPixelFmt(pixfmt);
    } else {
        switch (desc.depth) {
        case 1:
            desc.SetPixelFmt(0x100);
            break;
        case 8:
            desc.SetPixelFmt(kSquishPixFmtIndexed);
            break;
        case 16:
            desc.SetPixelFmt(kSquishPixFmtARGB1555);
            break;
        default:
            desc.SetPixelFmt(kSquishPixFmtBGRA8888);
            desc.depth = 32;
            break;
        }
    }
    desc.rowbytes = (unsigned short)CalcBestRowbytes(desc.width, desc.depth);
    desc.datasize = desc.height * desc.rowbytes;
    if (hints->TransEnabled())
        desc.SetTransColor(hints->GetTransColor());
    desc.SetPalette(hints->GetPalette());
    mData = (unsigned char *)MBmalloc_release(desc.datasize);
    if (desc.TransEnabled())
        Fill(desc.GetTransColor(), 0);
    else
        Fill(SquishPixel(0, 0, 0, 0), 0);
    return true;
}

// EQUIV 0x10064170 ?Create@SquishImageBlock@@QAE_NPBV1@PBVSquishImageDescriptor@@JJJJ@Z: reviewed: with the 0x400 test reading creationHints (as the original), only register choices and two spills of srcw/srch to their own slots differ. Was: flow matches; the original tests the TransEnabled bits in ecx/edx (ours eax/ecx), takes the pixel format in eax and spills srcw/srch to their own slots before the SquishMin calls; the 0x400 test reads creationHints' format like the original (81% similar)
bool SquishImageBlock::Create(const SquishImageBlock *srcBlock, const SquishImageDescriptor *creationHints, long srcx,
                              long srcy, long srcw, long srch)
{
    Reset();
    if (srcw == -1)
        srcw = srcBlock->desc.width;
    if (srch == -1)
        srch = srcBlock->desc.height;
    ImageDataPtr src((SquishImageBlock *)srcBlock);
    const SquishImageDescriptor *srcFmt = &srcBlock->desc;
    desc.width = creationHints->width ? creationHints->width : srcw;
    desc.height = creationHints->height ? creationHints->height : srch;
    desc.depth = creationHints->depth ? creationHints->depth : srcFmt->depth;
    desc.rowbytes = creationHints->rowbytes ? creationHints->rowbytes : CalcBestRowbytes(desc.width, desc.depth);
    desc.SetPalette(creationHints->GetPalette() ? creationHints->GetPalette() : srcFmt->GetPalette());
    if (creationHints->TransEnabled())
        desc.SetTransColor(creationHints->GetTransColor());
    else if (srcFmt->TransEnabled())
        desc.SetTransColor(srcFmt->GetTransColor());
    desc.SetPixelFmt(srcFmt->GetPixelFmt());
    desc.SetCompFmt(creationHints->GetCompFmt());
    if (desc.GetCompFmt() == 0x200) {
        desc.datasize = desc.rowbytes * desc.height;
        mData = (unsigned char *)MBmalloc_release(desc.datasize);
        ImageDataPtr data(this);
        long w = SquishMin((long)srcw, (long)desc.width);
        long h = SquishMin((long)srch, (long)desc.height);
        srcBlock->BlitBlock(data, &desc, &SquishRect(srcx, srcy, w, h), &SquishRect(0, 0, w, h));
    } else if (creationHints->GetCompFmt() == 0x400) {
        if (creationHints->datasize) {
            desc.datasize = creationHints->datasize;
            mData = (unsigned char *)MBmalloc_release(desc.datasize);
        }
        ImageDataPtr data(this);
        unsigned char *dataPtr = data;
        if (desc.TransEnabled())
            RLEEncodeBlockTrans(&dataPtr, src, &desc, srcFmt, srcx, srcy, srcw, srch);
        else
            RLEEncodeBlock(&dataPtr, src, &desc, srcFmt, srcx, srcy, srcw, srch);
    }
    return true;
}

// EQUIV 0x10064400 ?Create@SquishImageBlock@@QAE_NPBEPBVSquishImageDescriptor@@1JJJJ@Z: reviewed: with the 0x400 test reading creationHints (as the original), only register choices, slots and the order of two independent loads differ. Was: same TransEnabled/pixel-format register choice as the other Create; the 0x400 test reads creationHints' format like the original (91% similar)
bool SquishImageBlock::Create(const unsigned char *src, const SquishImageDescriptor *srcFmt,
                              const SquishImageDescriptor *creationHints, long srcx, long srcy, long srcw, long srch)
{
    Reset();
    desc.width = creationHints->width ? creationHints->width : srcw;
    desc.height = creationHints->height ? creationHints->height : srch;
    desc.depth = creationHints->depth ? creationHints->depth : srcFmt->depth;
    desc.rowbytes = creationHints->rowbytes ? creationHints->rowbytes : CalcBestRowbytes(desc.width, desc.depth);
    desc.SetPalette(creationHints->GetPalette() ? creationHints->GetPalette() : srcFmt->GetPalette());
    if (creationHints->TransEnabled())
        desc.SetTransColor(creationHints->GetTransColor());
    else if (srcFmt->TransEnabled())
        desc.SetTransColor(srcFmt->GetTransColor());
    desc.SetPixelFmt(srcFmt->GetPixelFmt());
    desc.SetCompFmt(creationHints->GetCompFmt());
    if (desc.GetCompFmt() == 0x200) {
        desc.datasize = desc.height * desc.rowbytes;
        mData = (unsigned char *)MBmalloc_release(desc.datasize);
        ImageDataPtr data(this);
        unsigned long srcDepth = srcFmt->depth;
        unsigned long srcRowbytes = srcFmt->rowbytes;
        unsigned long bytes2copy = (long)(srcDepth * srcw) / 8;
        const unsigned char *s = src + (long)(srcDepth * srcx) / 8 + srcy * srcRowbytes;
        unsigned char *d = data;
        for (long y = 0; y < srch; y++) {
            memcpy(d, s, bytes2copy);
            s += srcRowbytes;
            d += desc.rowbytes;
        }
    } else if (creationHints->GetCompFmt() == 0x400) {
        if (creationHints->datasize) {
            desc.datasize = creationHints->datasize;
            mData = (unsigned char *)MBmalloc_release(desc.datasize);
        }
        ImageDataPtr data(this);
        unsigned char *dataPtr = data;
        if (desc.TransEnabled())
            RLEEncodeBlockTrans(&dataPtr, src, &desc, srcFmt, srcx, srcy, srcw, srch);
        else
            RLEEncodeBlock(&dataPtr, src, &desc, srcFmt, srcx, srcy, srcw, srch);
    }
    return true;
}

// 0x10064660 ?Fill@SquishImageBlock@@QAE_NABUSquishPixel@@PAVSquishRect@@@Z
bool SquishImageBlock::Fill(const SquishPixel &pixel, SquishRect *r)
{
    if (desc.GetCompFmt() != 0x200 || desc.depth < 8)
        return false;
    unsigned long fill = desc.GetPalette() ? desc.GetPalette()->GetIndex(pixel) : pixel.GetPixel(desc.GetPixelFmt());
    unsigned char *data = Lock();
    if (!r)
        SquishFill(data, fill, desc.width, desc.height, desc.depth, desc.rowbytes);
    else
        SquishFill(data + r->x * (desc.depth >> 3) + r->y * desc.rowbytes, fill, r->w, r->h, desc.depth,
                   desc.rowbytes);
    Unlock(data);
    return true;
}

// 0x10064740 ?Blit@SquishImageBlock@@QBE_NPAEPBVSquishImageDescriptor@@@Z
bool SquishImageBlock::Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const
{
    ImageDataPtr src((SquishImageBlock *)this);
    if (!src)
        return false;
    if (destFmt->depth != desc.depth)
        return false;
    switch (desc.GetCompFmt()) {
    case 0x400:
        RLEBlit(dest, src, destFmt, &desc);
        break;
    case 0x200:
        SquishBlit(dest, src, destFmt, &desc);
        break;
    }
    return true;
}

// TODO 0x100647e0 ?BlitBlock@SquishImageBlock@@QBE_NPAEPBVSquishImageDescriptor@@PAVSquishRect@@2@Z: flow matches; the original keeps the locked data in edi (all four registers pushed up front), ours in ebx with edi pushed late (70% similar)
bool SquishImageBlock::BlitBlock(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect,
                                 SquishRect *destRect) const
{
    ImageDataPtr src((SquishImageBlock *)this);
    if (!src)
        return false;
    if (destFmt->depth != desc.depth)
        return false;
    switch (desc.GetCompFmt()) {
    case 0x400:
        if (srcRect->Width() == destRect->Width() && srcRect->Height() == destRect->Height())
            RLEBlitBlock(dest, src, destFmt, &desc, destRect, srcRect);
        else
            RLEBlitScaledBlock(dest, src, destFmt, &desc, destRect, srcRect);
        break;
    case 0x200:
        if (srcRect->Width() != destRect->Width() || srcRect->Height() != destRect->Height()) {
            srcRect->w = destRect->w = SquishMin(destRect->Width(), srcRect->Width());
            srcRect->h = destRect->h = SquishMin(destRect->Height(), srcRect->Height());
        }
        SquishBlitBlock(dest, src, destFmt, &desc, destRect, srcRect);
        break;
    }
    return true;
}

// 0x10064940 ?BlitTrans@SquishImageBlock@@QBE_NPAEPBVSquishImageDescriptor@@@Z
bool SquishImageBlock::BlitTrans(unsigned char *dest, const SquishImageDescriptor *destFmt) const
{
    if (!desc.TransEnabled())
        return Blit(dest, destFmt);
    ImageDataPtr src((SquishImageBlock *)this);
    if (!src)
        return false;
    if (destFmt->depth != desc.depth)
        return false;
    bool returnVal = false;
    switch (desc.GetCompFmt()) {
    case 0x400:
        RLEBlitTrans(dest, src, destFmt, &desc);
        returnVal = true;
        break;
    case 0x200:
        SquishBlitTrans(dest, src, destFmt, &desc);
        returnVal = true;
        break;
    }
    return returnVal;
}

// TODO 0x10064a00 ?BlitBlockTrans@SquishImageBlock@@QBE_NPAEPBVSquishImageDescriptor@@PAVSquishRect@@2@Z: same data-register choice as BlitBlock (73% similar)
bool SquishImageBlock::BlitBlockTrans(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect,
                                      SquishRect *destRect) const
{
    if (!desc.TransEnabled())
        return BlitBlock(dest, destFmt, srcRect, destRect);
    ImageDataPtr src((SquishImageBlock *)this);
    if (!src)
        return false;
    if (destFmt->depth != desc.depth)
        return false;
    bool returnVal = false;
    switch (desc.GetCompFmt()) {
    case 0x400:
        if (srcRect->Width() == destRect->Width() && srcRect->Height() == destRect->Height())
            RLEBlitBlockTrans(dest, src, destFmt, &desc, destRect, srcRect);
        else
            RLEBlitScaledBlockTrans(dest, src, destFmt, &desc, destRect, srcRect);
        returnVal = true;
        break;
    case 0x200:
        if (srcRect->Width() != destRect->Width() || srcRect->Height() != destRect->Height()) {
            srcRect->w = destRect->w = SquishMin(destRect->Width(), srcRect->Width());
            srcRect->h = destRect->h = SquishMin(destRect->Height(), srcRect->Height());
        }
        SquishBlitBlockTrans(dest, src, destFmt, &desc, destRect, srcRect);
        break;
    }
    return returnVal;
}

// 0x10064bb0 ?StreamIn@SquishImageBlock@@QAE_NPAVSquishFile@@@Z
bool SquishImageBlock::StreamIn(SquishFile *file)
{
    Reset();
    if (!desc.StreamIn(file))
        return false;
    if (!file->ReadBytes(&xOffset, 2))
        return false;
    if (!file->ReadBytes(&yOffset, 2))
        return false;
    mData = (unsigned char *)MBmalloc_release(desc.datasize);
    ImageDataPtr data(this);
    if (!data)
        return false;
    if (!file->ReadBytes(data, desc.datasize))
        return false;
    return true;
}

// 0x10064c60 ?StreamOut@SquishImageBlock@@QBE_NPAVSquishFile@@@Z
bool SquishImageBlock::StreamOut(SquishFile *file) const
{
    if (!desc.StreamOut(file))
        return false;
    if (!file->WriteBytes(&xOffset, 2))
        return false;
    if (!file->WriteBytes(&yOffset, 2))
        return false;
    ImageDataPtr data((SquishImageBlock *)this);
    if (!file->WriteBytes(data, desc.datasize))
        return false;
    return true;
}

// 0x10064cf0 ?Reset@SquishImageBlock@@QAEXXZ
void SquishImageBlock::Reset(void)
{
    desc.Reset();
    yOffset = 0;
    xOffset = 0;
    if (mData)
        delete[] mData;
    mData = 0;
}

// 0x10064d20 ?Lock@SquishImageBlock@@QAEPAEXZ
unsigned char *SquishImageBlock::Lock(void)
{
    return mData;
}

// 0x10064d30 ?Lock@SquishImageBlock@@QBEPBEXZ
const unsigned char *SquishImageBlock::Lock(void) const
{
    const unsigned char *p = ((SquishImageBlock *)this)->Lock();
    return p;
}

// 0x10064d40 ?Unlock@SquishImageBlock@@QAEXPAE@Z
void SquishImageBlock::Unlock(unsigned char *ptr)
{
}

// 0x10064d50 ?Unlock@SquishImageBlock@@QBEXPBE@Z
void SquishImageBlock::Unlock(const unsigned char *ptr) const
{
    ((SquishImageBlock *)this)->Unlock((unsigned char *)ptr);
}

// 0x10064d60 ?CalcBestRowbytes@SquishImageBlock@@SAJGG@Z
long SquishImageBlock::CalcBestRowbytes(unsigned short width, unsigned short depth)
{
    if (depth >= 8)
        return ((width * depth) / 8 + 7) / 8 * 8;
    if (depth == 1)
        return (width + 7) / 8;
    return (width * depth) / 8;
}
