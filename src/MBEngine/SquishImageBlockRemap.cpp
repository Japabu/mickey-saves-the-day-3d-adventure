// SquishImageBlockRemap.cpp - converting the pixels of an image block to another depth,
// pixel format or palette. The data is either plain rows (compression 0x200) or RLE rows
// of codes of the pixel size (see SquishDecode.cpp).
#include "Squish.h"

// 0x10064e00 ?AllocBuffer@@YAPAEK@Z
unsigned char *__cdecl AllocBuffer(unsigned long size)
{
    return (unsigned char *)MBmalloc_release(size);
}

// 0x10064e10 ?LockBuffer@@YAPAEPAE@Z
unsigned char *__cdecl LockBuffer(unsigned char *ptr)
{
    return ptr;
}

// 0x10064e20 ?UnlockBuffer@@YAXPAE@Z
void __cdecl UnlockBuffer(unsigned char *)
{
}

bool RemapHiColorToIndex(unsigned long *data, unsigned char **newdata, SquishImageDescriptor &desc,
                         SquishPalette *newPalette);
bool RemapHiColorToHiColor(unsigned long *data, unsigned char **newdata, SquishImageDescriptor &desc,
                           unsigned long newPixFmt, unsigned short dummy);

// Converts the pixels of the given size to another pixel format in place.
template <class T> inline void RemapPixelsInPlace(T *data, SquishImageDescriptor &desc, unsigned long newPixFmt)
{
    SquishPixel p;
    unsigned long oldpixfmt = desc.GetPixelFmt();
    unsigned long oldrowbytes = desc.rowbytes;
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            T *srcLineStart = data;
            for (unsigned long x = desc.width; x--;) {
                p.SetPixel(*data, oldpixfmt);
                *data++ = (T)p.GetPixel(newPixFmt);
            }
            data = (T *)((unsigned char *)srcLineStart + oldrowbytes);
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            long x = desc.width;
            while (x) {
                T code = *data++;
                if (code & ((T)1 << (sizeof(T) * 8 - 1))) {
                    T count = (T)((code ^ ((T)1 << (sizeof(T) * 8 - 1))) + 1);
                    x -= count;
                    while (count--) {
                        p.SetPixel(*data, oldpixfmt);
                        *data++ = (T)p.GetPixel(newPixFmt);
                    }
                } else {
                    x -= (T)(code + 1);
                    p.SetPixel(*data, oldpixfmt);
                    *data++ = (T)p.GetPixel(newPixFmt);
                }
            }
        }
    }
    desc.SetPixelFmt(newPixFmt);
}

// Converts 16 bit pixels to indices into a palette.
inline unsigned char *RemapShortToIndex(unsigned short *data, SquishImageDescriptor &desc,
                                        SquishPalette *newPalette)
{
    long newrowbytes = SquishImageBlock::CalcBestRowbytes(desc.width, 8);
    long newdatasize = desc.height * newrowbytes;
    unsigned char *newdata = AllocBuffer(newdatasize);
    SquishPixel p;
    unsigned char *dest = LockBuffer(newdata);
    unsigned long oldrowbytes = desc.rowbytes;
    unsigned long oldpixfmt = desc.GetPixelFmt();
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            unsigned short *srcLineStart = data;
            unsigned char *destLineStart = dest;
            for (unsigned long x = desc.width; x--;) {
                p.SetPixel(*data++, oldpixfmt);
                *dest++ = (unsigned char)newPalette->GetNearestIndex(p);
            }
            data = (unsigned short *)((unsigned char *)srcLineStart + oldrowbytes);
            dest = destLineStart + newrowbytes;
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            unsigned char *destLineStart = dest;
            long x = desc.width;
            while (x) {
                unsigned short code = *data++;
                if (code & 0x8000) {
                    unsigned short count = (unsigned short)((code ^ 0x8000) + 1);
                    x -= count;
                    while (count--) {
                        p.SetPixel(*data++, oldpixfmt);
                        *dest++ = (unsigned char)newPalette->GetNearestIndex(p);
                    }
                } else {
                    unsigned short count = (unsigned short)(code + 1);
                    x -= count;
                    p.SetPixel(*data++, oldpixfmt);
                    unsigned char index = (unsigned char)newPalette->GetNearestIndex(p);
                    while (count--)
                        *dest++ = index;
                }
            }
            dest = destLineStart + newrowbytes;
        }
    }
    desc.depth = 8;
    desc.rowbytes = (unsigned short)newrowbytes;
    desc.datasize = newdatasize;
    desc.SetPalette(newPalette);
    desc.SetPixelFmt(kSquishPixFmtIndexed);
    desc.SetCompFmt(0x200);
    UnlockBuffer(newdata);
    return newdata;
}

// Converts 16 bit pixels to 32 bit pixels of another format.
inline unsigned char *RemapShortToLong(unsigned short *data, SquishImageDescriptor &desc, unsigned long newPixFmt)
{
    long newrowbytes = SquishImageBlock::CalcBestRowbytes(desc.width, 32);
    long newdatasize;
    if (desc.GetCompFmt() == 0x200)
        newdatasize = desc.height * newrowbytes;
    else
        newdatasize = (desc.datasize * 4) >> 1;
    unsigned char *newdata = AllocBuffer(newdatasize);
    SquishPixel p;
    unsigned long *dest = (unsigned long *)LockBuffer(newdata);
    unsigned long oldrowbytes = desc.rowbytes;
    unsigned long oldpixfmt = desc.GetPixelFmt();
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            unsigned short *srcLineStart = data;
            unsigned long *destLineStart = dest;
            for (unsigned long x = desc.width; x--;) {
                p.SetPixel(*data++, oldpixfmt);
                *dest++ = p.GetPixel(newPixFmt);
            }
            data = (unsigned short *)((unsigned char *)srcLineStart + oldrowbytes);
            dest = (unsigned long *)((unsigned char *)destLineStart + newrowbytes);
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            long x = desc.width;
            while (x) {
                unsigned short code = *data++;
                if (code & 0x8000) {
                    code ^= 0x8000;
                    *dest++ = code | 0x80000000;
                    unsigned short count = (unsigned short)(code + 1);
                    x -= count;
                    while (count--) {
                        p.SetPixel(*data++, oldpixfmt);
                        *dest++ = p.GetPixel(newPixFmt);
                    }
                } else {
                    *dest++ = code;
                    x -= (unsigned short)(code + 1);
                    p.SetPixel(*data++, oldpixfmt);
                    *dest++ = p.GetPixel(newPixFmt);
                }
            }
        }
    }
    desc.datasize = newdatasize;
    desc.depth = 32;
    desc.rowbytes = (unsigned short)newrowbytes;
    desc.SetPalette(0);
    desc.SetPixelFmt(newPixFmt);
    UnlockBuffer(newdata);
    return newdata;
}

// TODO 0x10064e30 ?Remap@SquishImageBlock@@QAE_NKKPAVSquishPalette@@@Z: per-format cases reconstructed, but ours is 374 bytes shorter (a format path is merged or missing) (57% similar)
// 0x10065900 ?Unlock@ImageDataPtr@@QAEXXZ
bool SquishImageBlock::Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette)
{
    if (desc.depth == newDepth && desc.GetPalette() == newPalette && desc.GetPixelFmt() == newPixFmt)
        return true;
    ImageDataPtr data(this);
    if (desc.depth == newDepth) {
        if (desc.depth == 8)
            return RemapIndexedToIndexed(newPalette);
        if (desc.depth == 16) {
            RemapPixelsInPlace((unsigned short *)(unsigned char *)data, desc, newPixFmt);
            data.Unlock();
            return true;
        }
        if (desc.depth == 32) {
            RemapPixelsInPlace((unsigned long *)(unsigned char *)data, desc, newPixFmt);
            return true;
        }
        return false;
    }
    if (desc.depth == 32) {
        if (newDepth == 8) {
            SquishImageDescriptor newDesc(desc);
            unsigned char *newdata = 0;
            if (!RemapHiColorToIndex((unsigned long *)(unsigned char *)data, &newdata, newDesc, newPalette))
                return false;
            desc = newDesc;
            data.Unlock();
            MBfree_release(mData);
            mData = newdata;
            return true;
        }
        if (newDepth == 16) {
            SquishImageDescriptor newDesc(desc);
            unsigned char *newdata = 0;
            if (!RemapHiColorToHiColor((unsigned long *)(unsigned char *)data, &newdata, newDesc, newPixFmt, 0))
                return false;
            desc = newDesc;
            data.Unlock();
            MBfree_release(mData);
            mData = newdata;
            return true;
        }
    } else if (desc.depth == 16) {
        if (newDepth == 8) {
            SquishImageDescriptor newDesc(desc);
            unsigned char *newdata = RemapShortToIndex((unsigned short *)(unsigned char *)data, newDesc, newPalette);
            if (!newdata)
                return false;
            desc = newDesc;
            data.Unlock();
            MBfree_release(mData);
            mData = newdata;
            return true;
        }
        if (newDepth == 32) {
            SquishImageDescriptor newDesc(desc);
            unsigned char *newdata = RemapShortToLong((unsigned short *)(unsigned char *)data, newDesc, newPixFmt);
            if (!newdata)
                return false;
            desc = newDesc;
            data.Unlock();
            MBfree_release(mData);
            mData = newdata;
            return true;
        }
    } else if (desc.depth == 8) {
        // indexed to hicolor is not supported
        if (newDepth == 16) {
            SquishImageDescriptor newDesc(desc);
            return false;
        }
        if (newDepth == 32) {
            SquishImageDescriptor newDesc(desc);
            return false;
        }
    }
    return false;
}

// EQUIV 0x10065930 ?RemapIndexedToIndexed@SquishImageBlock@@IAE_NPAVSquishPalette@@@Z: reviewed: register choices and the compression-format test (flags & 0xe00 == 0x200) scheduled before an independent load; same calls, conditions and stores. Was: same code shape; register allocation differs (94% similar)
bool SquishImageBlock::RemapIndexedToIndexed(SquishPalette *newPalette)
{
    if (!newPalette)
        return false;
    unsigned char translationTable[256];
    SquishPalette *oldPalette = desc.GetPalette();
    for (long i = 0; i < (long)oldPalette->GetSize(); i++)
        translationTable[i] = (unsigned char)newPalette->GetNearestIndex(oldPalette->GetPixel(i));
    ImageDataPtr data(this);
    unsigned char *p = data;
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            unsigned char *lineStart = p;
            for (unsigned long x = desc.width; x--;) {
                *p = translationTable[*p];
                p++;
            }
            p = lineStart + desc.rowbytes;
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            long x = desc.width;
            while (x) {
                unsigned char tByte = *p++;
                if (tByte & 0x80) {
                    tByte = (unsigned char)((tByte ^ 0x80) + 1);
                    x -= tByte;
                    while (tByte--) {
                        *p = translationTable[*p];
                        p++;
                    }
                } else {
                    x -= (unsigned char)(tByte + 1);
                    *p = translationTable[*p];
                    p++;
                }
            }
        }
    }
    desc.SetPalette(newPalette);
    return true;
}

// TODO 0x10065a80 ?RemapHiColorToIndex@@YA_NPAKPAPAEAAVSquishImageDescriptor@@PAVSquishPalette@@@Z: loops reconstructed; branch layout and register allocation differ (84% similar)
bool RemapHiColorToIndex(unsigned long *data, unsigned char **newdata, SquishImageDescriptor &desc,
                         SquishPalette *newPalette)
{
    long newrowbytes = SquishImageBlock::CalcBestRowbytes(desc.width, 8);
    long newdatasize = desc.height * newrowbytes;
    *newdata = AllocBuffer(newdatasize);
    SquishPixel p;
    unsigned char *dest = LockBuffer(*newdata);
    unsigned long oldpixfmt = desc.GetPixelFmt();
    unsigned long oldrowbytes = desc.rowbytes;
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            unsigned long *srcLineStart = data;
            unsigned char *destLineStart = dest;
            for (int x = desc.width; x--;) {
                p.SetPixel(*data++, oldpixfmt);
                *dest++ = (unsigned char)newPalette->GetNearestIndex(p);
            }
            data = (unsigned long *)((unsigned char *)srcLineStart + oldrowbytes);
            dest = destLineStart + newrowbytes;
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            unsigned char *destLineStart = dest;
            int x = desc.width;
            while (x) {
                unsigned long code = *data++;
                if (code & 0x80000000) {
                    unsigned long count = (code ^ 0x80000000) + 1;
                    x -= count;
                    while (count--) {
                        p.SetPixel(*data++, oldpixfmt);
                        *dest++ = (unsigned char)newPalette->GetNearestIndex(p);
                    }
                } else {
                    unsigned long count = code + 1;
                    x -= count;
                    p.SetPixel(*data++, oldpixfmt);
                    unsigned char index = (unsigned char)newPalette->GetNearestIndex(p);
                    while (count--)
                        *dest++ = index;
                }
            }
            dest = destLineStart + newrowbytes;
        }
    }
    desc.depth = 8;
    desc.rowbytes = (unsigned short)newrowbytes;
    desc.datasize = newdatasize;
    desc.SetPalette(newPalette);
    desc.SetPixelFmt(kSquishPixFmtIndexed);
    desc.SetCompFmt(0x200);
    UnlockBuffer(*newdata);
    return true;
}

// TODO 0x10065cb0 ?RemapHiColorToHiColor@@YA_NPAKPAPAEAAVSquishImageDescriptor@@KG@Z: loops reconstructed; branch layout and register allocation differ (91% similar)
bool RemapHiColorToHiColor(unsigned long *data, unsigned char **newdata, SquishImageDescriptor &desc,
                           unsigned long newPixFmt, unsigned short dummy)
{
    long newrowbytes = SquishImageBlock::CalcBestRowbytes(desc.width, 16);
    long newdatasize;
    if (desc.GetCompFmt() == 0x200)
        newdatasize = desc.height * newrowbytes;
    else
        newdatasize = (desc.datasize * 2) >> 2;
    *newdata = AllocBuffer(newdatasize);
    SquishPixel p;
    unsigned short *dest = (unsigned short *)LockBuffer(*newdata);
    unsigned long oldpixfmt = desc.GetPixelFmt();
    unsigned long oldrowbytes = desc.rowbytes;
    if (desc.GetCompFmt() == 0x200) {
        for (unsigned long y = desc.height; y--;) {
            unsigned long *srcLineStart = data;
            unsigned short *destLineStart = dest;
            for (int x = desc.width; x--;) {
                p.SetPixel(*data++, oldpixfmt);
                *dest++ = (unsigned short)p.GetPixel(newPixFmt);
            }
            data = (unsigned long *)((unsigned char *)srcLineStart + oldrowbytes);
            dest = (unsigned short *)((unsigned char *)destLineStart + newrowbytes);
        }
    } else {
        for (unsigned long y = desc.height; y--;) {
            int x = desc.width;
            while (x) {
                unsigned long code = *data++;
                if (code & 0x80000000) {
                    code ^= 0x80000000;
                    *dest++ = (unsigned short)(code | 0x8000);
                    unsigned long count = code + 1;
                    x -= count;
                    while (count--) {
                        p.SetPixel(*data++, oldpixfmt);
                        *dest++ = (unsigned short)p.GetPixel(newPixFmt);
                    }
                } else {
                    *dest++ = (unsigned short)code;
                    x -= code + 1;
                    p.SetPixel(*data++, oldpixfmt);
                    *dest++ = (unsigned short)p.GetPixel(newPixFmt);
                }
            }
        }
    }
    desc.depth = 16;
    desc.rowbytes = (unsigned short)newrowbytes;
    desc.datasize = newdatasize;
    desc.SetPalette(0);
    desc.SetPixelFmt(newPixFmt);
    UnlockBuffer(*newdata);
    return true;
}
