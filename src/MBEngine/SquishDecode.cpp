// SquishDecode.cpp - RLE decoding of Squish image blocks into 8, 16 and 32 bit buffers.
// An RLE row is a sequence of codes of the pixel size: a code with the top bit set is
// followed by (code & ~top) + 1 literal pixels, any other code by one pixel repeated
// code + 1 times.
#include "Squish.h"

template <class T> class SquishDecode {
public:
    enum { kLiteralRun = (T)1 << (sizeof(T) * 8 - 1) };

// EQUIV 0x1005fee0 ?RLEDecode@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z: reviewed: only the stack slot of the row counter differs (a dead parameter slot each: srcFmt in the original, src here). Was: 3 bytes: the original keeps the row counter in the srcFmt slot, ours in the src slot (96% similar)
// 0x10060ce0 ?RLEDecode@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z
// EQUIV 0x10061ac0 ?RLEDecode@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z: reviewed: the same loads of the two descriptors in another order (independent reads) and register choices. Was: load order: the original reads destFmt->rowbytes first and keeps dest in ecx (54% similar)
    static void RLEDecode(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                          const SquishImageDescriptor *srcFmt)
    {
        long destRowbytes = destFmt->GetRowbytes();
        const T *s = (const T *)src;
        T *d = (T *)dest;
        unsigned long height = srcFmt->GetHeight();
        long width = srcFmt->GetWidth();
        while (height--) {
            T *destSOL = d;
            long x = 0;
            while (x < width) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    x += count;
                    while (count--)
                        *d++ = *s++;
                } else {
                    count = (T)(code + 1);
                    x += count;
                    while (count--)
                        *d++ = *s;
                    s++;
                }
            }
            d = (T *)((unsigned char *)destSOL + destRowbytes);
        }
    }

// TODO 0x1005ffb0 ?RLEDecodeTrans@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z: transparent-run handling and slot use not reproduced yet (88% similar)
// TODO 0x10060db0 ?RLEDecodeTrans@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z: transparent-run handling and slot use not reproduced yet (94% similar)
// TODO 0x10061b60 ?RLEDecodeTrans@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2@Z: transparent-run handling and slot use not reproduced yet (89% similar)
    static void RLEDecodeTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                               const SquishImageDescriptor *srcFmt)
    {
        long destRowbytes = destFmt->GetRowbytes();
        const T *s = (const T *)src;
        T *d = (T *)dest;
        unsigned long height = srcFmt->GetHeight();
        long width = srcFmt->GetWidth();
        T trans = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        while (height--) {
            T *destSOL = d;
            long x = 0;
            while (x < width) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    x += count;
                    while (count--)
                        *d++ = *s++;
                } else {
                    count = (T)(code + 1);
                    x += count;
                    if (*s == trans)
                        d += count;
                    else {
                        while (count--)
                            *d++ = *s;
                    }
                    s++;
                }
            }
            d = (T *)((unsigned char *)destSOL + destRowbytes);
        }
    }

    // Decodes the part srcRect of the image into destRect of the destination (no scaling).
// TODO 0x100600b0 ?RLEDecodeBlock@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (88% similar)
// EQUIV 0x10060ea0 ?RLEDecodeBlock@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: reviewed: register and stack slot choices only (no instruction differs once registers, stack offsets and relocations are normalized). Was: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (73% similar)
// EQUIV 0x10061c30 ?RLEDecodeBlock@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: reviewed: the only difference is the copy of the row count into its register, lea in the original and mov here (the same move). Was: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (65% similar)
    static void RLEDecodeBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                               const SquishImageDescriptor *srcFmt, const SquishRect *destRect,
                               const SquishRect *srcRect)
    {
        SquishRect sRect = *srcRect;
        SquishRect dRect = *destRect;
        if (sRect.w > dRect.w)
            sRect.w = dRect.w;
        if (sRect.h > dRect.h)
            sRect.h = dRect.h;
        int destrowbytes = destFmt->GetRowbytes();
        int srcw = srcFmt->GetWidth();
        T *d = (T *)(dest + destRect->y * destrowbytes) + destRect->x;
        const T *s = RLESkipLines((const T *)src, srcw, sRect.y);
        long farX = sRect.x + sRect.w;
        for (unsigned long y = sRect.h; y; y--) {
            T *destSOL = d;
            long x = 0;
            while (x < srcw) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    if (x >= sRect.x && x + count < farX) {
                        x += count;
                        while (count--)
                            *d++ = *s++;
                    } else if (x + count >= sRect.x && x < farX) {
                        unsigned long pixelsToSkipNear = 0;
                        if (x < sRect.x)
                            pixelsToSkipNear = sRect.x - x;
                        x += count;
                        unsigned long pixelsToSkipFar = 0;
                        if (x > farX)
                            pixelsToSkipFar = x - farX;
                        count -= (T)(pixelsToSkipFar + pixelsToSkipNear);
                        if (pixelsToSkipNear > 0)
                            s += pixelsToSkipNear;
                        while (count--)
                            *d++ = *s++;
                        if (pixelsToSkipFar > 0)
                            s += pixelsToSkipFar;
                    } else {
                        s += count;
                        x += count;
                    }
                } else {
                    count = (T)(code + 1);
                    if (x >= sRect.x && x + count < farX) {
                        x += count;
                        while (count--)
                            *d++ = *s;
                        s++;
                    } else if (x + count >= sRect.x && x < farX) {
                        unsigned long pixelsToSkipNear = 0;
                        if (x < sRect.x)
                            pixelsToSkipNear = sRect.x - x;
                        x += count;
                        unsigned long pixelsToSkipFar = 0;
                        if (x > farX)
                            pixelsToSkipFar = x - farX;
                        count -= (T)(pixelsToSkipFar + pixelsToSkipNear);
                        while (count--)
                            *d++ = *s;
                        s++;
                    } else {
                        s++;
                        x += count;
                    }
                }
            }
            d = (T *)((unsigned char *)destSOL + destrowbytes);
        }
    }

    // As RLEDecodeBlock, leaving the transparent pixels of the repeat runs alone.
// TODO 0x10060340 ?RLEDecodeBlockTrans@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (77% similar)
// TODO 0x10061120 ?RLEDecodeBlockTrans@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (95% similar)
// TODO 0x10061e50 ?RLEDecodeBlockTrans@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (65% similar)
    static void RLEDecodeBlockTrans(unsigned char *dest, const unsigned char *src,
                                    const SquishImageDescriptor *destFmt, const SquishImageDescriptor *srcFmt,
                                    const SquishRect *destRect, const SquishRect *srcRect)
    {
        SquishRect sRect = *srcRect;
        SquishRect dRect = *destRect;
        if (sRect.w > dRect.w)
            sRect.w = dRect.w;
        if (sRect.h > dRect.h)
            sRect.h = dRect.h;
        int destrowbytes = destFmt->GetRowbytes();
        int srcw = srcFmt->GetWidth();
        T transPixel = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        T *destPtr = (T *)(dest + destRect->y * destrowbytes) + destRect->x;
        const T *s = RLESkipLines((const T *)src, srcw, sRect.y);
        long farX = sRect.x + sRect.w;
        for (unsigned long y = sRect.h; y; y--) {
            T *destSOL = destPtr;
            long x = 0;
            while (x < srcw) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    if (x >= sRect.x && x + count < farX) {
                        x += count;
                        while (count--)
                            *destPtr++ = *s++;
                    } else if (x + count >= sRect.x && x < farX) {
                        unsigned long pixelsToSkipNear = 0;
                        if (x < sRect.x)
                            pixelsToSkipNear = sRect.x - x;
                        x += count;
                        unsigned long pixelsToSkipFar = 0;
                        if (x > farX)
                            pixelsToSkipFar = x - farX;
                        count -= (T)(pixelsToSkipFar + pixelsToSkipNear);
                        if (pixelsToSkipNear > 0)
                            s += pixelsToSkipNear;
                        while (count--)
                            *destPtr++ = *s++;
                        if (pixelsToSkipFar > 0)
                            s += pixelsToSkipFar;
                    } else {
                        s += count;
                        x += count;
                    }
                } else {
                    count = (T)(code + 1);
                    if (x >= sRect.x && x + count < farX) {
                        x += count;
                        if (*s == transPixel)
                            destPtr += count;
                        else {
                            while (count--)
                                *destPtr++ = *s;
                        }
                        s++;
                    } else if (x + count >= sRect.x && x < farX) {
                        unsigned long pixelsToSkipNear = 0;
                        if (x < sRect.x)
                            pixelsToSkipNear = sRect.x - x;
                        x += count;
                        unsigned long pixelsToSkipFar = 0;
                        if (x > farX)
                            pixelsToSkipFar = x - farX;
                        count -= (T)(pixelsToSkipFar + pixelsToSkipNear);
                        if (*s == transPixel)
                            destPtr += count;
                        else {
                            while (count--)
                                *destPtr++ = *s;
                        }
                        s++;
                    } else {
                        s++;
                        x += count;
                    }
                }
            }
            destPtr = (T *)((unsigned char *)destSOL + destrowbytes);
        }
    }

    // Decodes srcRect of the image scaled to the size of destRect: columns are repeated or
    // dropped by an accumulator per row, rows are copied or skipped by one per column.
// TODO 0x10060660 ?RLEDecodeScaledBlock@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (24% similar)
// TODO 0x100613f0 ?RLEDecodeScaledBlock@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (82% similar)
// TODO 0x100620c0 ?RLEDecodeScaledBlock@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (84% similar)
    static void RLEDecodeScaledBlock(unsigned char *dest, const unsigned char *src,
                                     const SquishImageDescriptor *destFmt, const SquishImageDescriptor *srcFmt,
                                     const SquishRect *destRect, const SquishRect *srcRect)
    {
        const int destrowbytes = destFmt->GetRowbytes();
        const long lSrcW = srcFmt->GetWidth();
        T *d = (T *)(dest + destRect->y * destrowbytes) + destRect->x;
        const T *s = RLESkipLines((const T *)src, lSrcW, srcRect->y);
        float fDestXInc = (float)destRect->w / srcRect->w;
        long lFarXBound = srcRect->x + srcRect->w;
        float fDestYInc = (float)destRect->h / srcRect->h;
        long lSrcY = srcRect->h;
        long lDestY = destRect->h;
        float fYAccum = 0.0f;
        T *destSOL = d;
        while (lSrcY--) {
            long x = 0;
            float fXAccum = 0.0f;
            long destX = 0;
            while (x < lSrcW) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    while (count--) {
                        if (x < lFarXBound && x >= srcRect->x) {
                            fXAccum += fDestXInc;
                            while (fXAccum >= 1.0f) {
                                fXAccum -= 1.0f;
                                d[destX++] = *s;
                            }
                        }
                        s++;
                        x++;
                    }
                } else {
                    count = (T)(code + 1);
                    while (count--) {
                        if (x < lFarXBound && x >= srcRect->x) {
                            fXAccum += fDestXInc;
                            while (fXAccum >= 1.0f) {
                                fXAccum -= 1.0f;
                                d[destX++] = *s;
                            }
                        }
                        x++;
                    }
                    s++;
                }
            }
            fYAccum += fDestYInc - 1.0f;
            d = (T *)((unsigned char *)d + destrowbytes);
            if (--lDestY == 0)
                return;
            if (lSrcY == 0)
                return;
            while (fYAccum >= 1.0f) {
                MBmemcpy(d, destSOL, destrowbytes);
                fYAccum -= 1.0f;
                d = (T *)((unsigned char *)d + destrowbytes);
                if (--lDestY == 0)
                    return;
            }
            destSOL = d;
            while (fYAccum < 0.0f && lSrcY) {
                s = RLESkipLines(s, lSrcW, 1);
                fYAccum += fDestYInc;
                lSrcY--;
            }
        }
    }

    // As RLEDecodeScaledBlock, leaving the transparent pixels alone.
// TODO 0x10060930 ?RLEDecodeScaledBlockTrans@?$SquishDecode@E@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (77% similar)
// TODO 0x100616f0 ?RLEDecodeScaledBlockTrans@?$SquishDecode@G@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (81% similar)
// TODO 0x100623a0 ?RLEDecodeScaledBlockTrans@?$SquishDecode@K@@SAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (79% similar)
    static void RLEDecodeScaledBlockTrans(unsigned char *dest, const unsigned char *src,
                                          const SquishImageDescriptor *destFmt,
                                          const SquishImageDescriptor *srcFmt, const SquishRect *destRect,
                                          const SquishRect *srcRect)
    {
        const int destrowbytes = destFmt->GetRowbytes();
        const long lSrcW = srcFmt->GetWidth();
        T transPixel = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        T *d = (T *)(dest + destRect->y * destrowbytes) + destRect->x;
        const T *s = RLESkipLines((const T *)src, lSrcW, srcRect->y);
        float fDestXInc = (float)destRect->w / srcRect->w;
        long lFarXBound = srcRect->x + srcRect->w;
        float fDestYInc = (float)destRect->h / srcRect->h;
        long lSrcY = srcRect->h;
        long lDestY = destRect->h;
        float fYAccum = 0.0f;
        T *destSOL = d;
        while (lSrcY--) {
            long x = 0;
            float fXAccum = 0.0f;
            long destX = 0;
            while (x < lSrcW) {
                T code = *s++;
                T count;
                if (code & (T)kLiteralRun) {
                    count = (T)((code & (T)~kLiteralRun) + 1);
                    while (count--) {
                        if (x < lFarXBound && x >= srcRect->x) {
                            fXAccum += fDestXInc;
                            while (fXAccum >= 1.0f) {
                                fXAccum -= 1.0f;
                                if (*s != transPixel)
                                    d[destX] = *s;
                                destX++;
                            }
                        }
                        s++;
                        x++;
                    }
                } else {
                    count = (T)(code + 1);
                    while (count--) {
                        if (x < lFarXBound && x >= srcRect->x) {
                            fXAccum += fDestXInc;
                            while (fXAccum >= 1.0f) {
                                fXAccum -= 1.0f;
                                if (*s != transPixel)
                                    d[destX] = *s;
                                destX++;
                            }
                        }
                        x++;
                    }
                    s++;
                }
            }
            fYAccum += fDestYInc - 1.0f;
            d = (T *)((unsigned char *)d + destrowbytes);
            if (--lDestY == 0)
                return;
            if (lSrcY == 0)
                return;
            while (fYAccum >= 1.0f) {
                MBmemcpy(d, destSOL, destrowbytes);
                fYAccum -= 1.0f;
                d = (T *)((unsigned char *)d + destrowbytes);
                if (--lDestY == 0)
                    return;
            }
            destSOL = d;
            while (fYAccum < 0.0f && lSrcY) {
                s = RLESkipLines(s, lSrcW, 1);
                fYAccum += fDestYInc;
                lSrcY--;
            }
        }
    }

// EQUIV 0x10060c80 ?RLESkipLines@?$SquishDecode@E@@SAPBEPBEKK@Z: reviewed: 97% by bytes: the only difference is the copy of lines into the counter register, lea edi, [eax] in the original and mov edi, eax here (the same move)
// EQUIV 0x10061a60 ?RLESkipLines@?$SquishDecode@G@@SAPBGPBGKK@Z: reviewed: 97% by bytes: the only difference is the copy of lines into the counter register, lea edi, [eax] in the original and mov edi, eax here (the same move)
// EQUIV 0x100626e0 ?RLESkipLines@?$SquishDecode@K@@SAPBKPBKKK@Z: reviewed: 97% by bytes: the only difference is the copy of lines into the counter register, lea edi, [eax] in the original and mov edi, eax here (the same move)
    static const T *RLESkipLines(const T *src, unsigned long width, unsigned long lines)
    {
        if (lines == 0)
            return src;
        for (unsigned long y = lines; y; y--) {
            unsigned long x = 0;
            while (x < width) {
                T code = *src++;
                if (code & (T)kLiteralRun) {
                    T count = (T)((code & (T)~kLiteralRun) + 1);
                    src += count;
                    x += count;
                } else {
                    x += (T)(code + 1);
                    src++;
                }
            }
        }
        return src;
    }
};

// EQUIV 0x1005ed90 ?RLEBlit@@YAXPAEPBEPBVSquishImageDescriptor@@2@Z: reviewed: register choice and the load order of independent fields only
void RLEBlit(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
             const SquishImageDescriptor *srcFmt)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecode(dest, src, destFmt, srcFmt);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecode(dest, src, destFmt, srcFmt);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecode(dest, src, destFmt, srcFmt);
}

// EQUIV 0x1005efe0 ?RLEBlitTrans@@YAXPAEPBEPBVSquishImageDescriptor@@2@Z: reviewed: register choice only (the transparent value stays in a register instead of a stack slot) and the loop-counter form
void RLEBlitTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                  const SquishImageDescriptor *srcFmt)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecodeTrans(dest, src, destFmt, srcFmt);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecodeTrans(dest, src, destFmt, srcFmt);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecodeTrans(dest, src, destFmt, srcFmt);
}

// TODO 0x1005f2a0 ?RLEBlitBlock@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (83% similar)
void RLEBlitBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                  const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecodeBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecodeBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecodeBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
}

// TODO 0x1005f560 ?RLEBlitBlockTrans@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (73% similar)
void RLEBlitBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                       const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecodeBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecodeBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecodeBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
}

// TODO 0x1005f890 ?RLEBlitScaledBlock@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (85% similar)
void RLEBlitScaledBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                        const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecodeScaledBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecodeScaledBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecodeScaledBlock(dest, src, destFmt, srcFmt, destRect, srcRect);
}

// TODO 0x1005fb90 ?RLEBlitScaledBlockTrans@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (74% similar)
void RLEBlitScaledBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                             const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishDecode<unsigned char>::RLEDecodeScaledBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 16)
        SquishDecode<unsigned short>::RLEDecodeScaledBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
    else if (depth == 32)
        SquishDecode<unsigned long>::RLEDecodeScaledBlockTrans(dest, src, destFmt, srcFmt, destRect, srcRect);
}

template class SquishDecode<unsigned char>;
template class SquishDecode<unsigned short>;
template class SquishDecode<unsigned long>;

