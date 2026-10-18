// SquishBlit.cpp - fills and rectangle copies between Squish image buffers.
#include "Squish.h"

template <class T> class SquishBlitter {
public:
    static void Fill(unsigned char *dest, T fill, int width, int height, int rowbytes)
    {
        while (height--) {
            T *d = (T *)dest;
            int x = width;
            while (x--)
                *d++ = fill;
            dest += rowbytes;
        }
    }

    static void Blit(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                     const SquishImageDescriptor *srcFmt, unsigned long bytes, unsigned long height)
    {
        int srcRowbytes = srcFmt->GetRowbytes();
        int destRowbytes = destFmt->GetRowbytes();
        while (height--) {
            MBmemcpy(dest, src, bytes);
            src += srcRowbytes;
            dest += destRowbytes;
        }
    }

    // Copies the pixels that are not the transparent colour.
    static void BlitTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                          const SquishImageDescriptor *srcFmt, unsigned long bytes, unsigned long height)
    {
        T trans = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        int srcRowbytes = srcFmt->GetRowbytes();
        int destRowbytes = destFmt->GetRowbytes();
        unsigned long pixels = bytes / sizeof(T);
        while (height--) {
            const T *s = (const T *)src;
            T *d = (T *)dest;
            for (unsigned long x = pixels; x; x--) {
                if (*s != trans)
                    *d = *s;
                d++;
                s++;
            }
            src += srcRowbytes;
            dest += destRowbytes;
        }
    }

    static void BlitBlock(unsigned char *dest, const unsigned char *src, int destRowbytes, int srcRowbytes,
                          unsigned long bytes, unsigned long lines2copy)
    {
        while (lines2copy--) {
            MBmemcpy(dest, src, bytes);
            src += srcRowbytes;
            dest += destRowbytes;
        }
    }

    static void BlitBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *srcFmt,
                               int destRowbytes, int srcRowbytes, unsigned long bytes, unsigned long lines2copy)
    {
        T trans = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        unsigned long pixels = bytes / sizeof(T);
        while (lines2copy--) {
            const T *s = (const T *)src;
            T *d = (T *)dest;
            for (unsigned long x = pixels; x; x--) {
                if (*s != trans)
                    *d = *s;
                d++;
                s++;
            }
            src += srcRowbytes;
            dest += destRowbytes;
        }
    }
};

// 0x1005e5d0 ?SquishFill@@YAXPAEKHHHH@Z
void SquishFill(unsigned char *dest, unsigned long fill, int width, int height, int depth, int rowbytes)
{
    if (depth == 8)
        SquishBlitter<unsigned char>::Fill(dest, (unsigned char)fill, width, height, rowbytes);
    else if (depth == 16)
        SquishBlitter<unsigned short>::Fill(dest, (unsigned short)fill, width, height, rowbytes);
    else if (depth == 32)
        SquishBlitter<unsigned long>::Fill(dest, fill, width, height, rowbytes);
}

// EQUIV 0x1005e6b0 ?SquishBlit@@YAXPAEPBEPBVSquishImageDescriptor@@2@Z: reviewed: register and stack-slot choice only
void SquishBlit(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt, const SquishImageDescriptor *srcFmt)
{
    unsigned long depth = srcFmt->GetDepth();
    unsigned long bytes = (depth >> 3) * SquishMin((unsigned long)destFmt->GetWidth(), (unsigned long)srcFmt->GetWidth());
    unsigned long height = SquishMin((unsigned long)destFmt->GetHeight(), (unsigned long)srcFmt->GetHeight());
    if (depth == 8)
        SquishBlitter<unsigned char>::Blit(dest, src, destFmt, srcFmt, bytes, height);
    else if (depth == 16)
        SquishBlitter<unsigned short>::Blit(dest, src, destFmt, srcFmt, bytes, height);
    else if (depth == 32)
        SquishBlitter<unsigned long>::Blit(dest, src, destFmt, srcFmt, bytes, height);
}

// TODO 0x1005e7f0 ?SquishBlitTrans@@YAXPAEPBEPBVSquishImageDescriptor@@2@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (75% similar)
void SquishBlitTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                     const SquishImageDescriptor *srcFmt)
{
    unsigned long depth = srcFmt->GetDepth();
    unsigned long bytes = (depth >> 3) * SquishMin((unsigned long)destFmt->GetWidth(), (unsigned long)srcFmt->GetWidth());
    unsigned long height = SquishMin((unsigned long)destFmt->GetHeight(), (unsigned long)srcFmt->GetHeight());
    if (depth == 8)
        SquishBlitter<unsigned char>::BlitTrans(dest, src, destFmt, srcFmt, bytes, height);
    else if (depth == 16)
        SquishBlitter<unsigned short>::BlitTrans(dest, src, destFmt, srcFmt, bytes, height);
    else if (depth == 32)
        SquishBlitter<unsigned long>::BlitTrans(dest, src, destFmt, srcFmt, bytes, height);
}

// TODO 0x1005e9f0 ?SquishBlitBlock@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (75% similar)
void SquishBlitBlock(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                     const SquishImageDescriptor *srcFmt, const SquishRect *destRect, const SquishRect *srcRect)
{
    int destRowbytes = destFmt->GetRowbytes();
    dest += destRect->x * (destFmt->GetDepth() >> 3) + destRect->y * destRowbytes;
    unsigned long depth = srcFmt->GetDepth();
    int srcRowbytes = srcFmt->GetRowbytes();
    src += srcRect->x * (depth >> 3) + srcRect->y * srcRowbytes;
    unsigned long bytes = (depth >> 3) * SquishMin((unsigned long)destRect->w, (unsigned long)srcRect->w);
    unsigned long lines2copy = SquishMin((unsigned long)destRect->h, (unsigned long)srcRect->h);
    if (depth == 8)
        SquishBlitter<unsigned char>::BlitBlock(dest, src, destRowbytes, srcRowbytes, bytes, lines2copy);
    else if (depth == 16)
        SquishBlitter<unsigned short>::BlitBlock(dest, src, destRowbytes, srcRowbytes, bytes, lines2copy);
    else if (depth == 32)
        SquishBlitter<unsigned long>::BlitBlock(dest, src, destRowbytes, srcRowbytes, bytes, lines2copy);
}

// TODO 0x1005eb70 ?SquishBlitBlockTrans@@YAXPAEPBEPBVSquishImageDescriptor@@2PBVSquishRect@@3@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (70% similar)
void SquishBlitBlockTrans(unsigned char *dest, const unsigned char *src, const SquishImageDescriptor *destFmt,
                          const SquishImageDescriptor *srcFmt, const SquishRect *destRect,
                          const SquishRect *srcRect)
{
    int destRowbytes = destFmt->GetRowbytes();
    dest += destRect->y * destRowbytes + destRect->x * (destFmt->GetDepth() >> 3);
    unsigned long depth = srcFmt->GetDepth();
    int srcRowbytes = srcFmt->GetRowbytes();
    src += srcRect->y * srcRowbytes + srcRect->x * (depth >> 3);
    unsigned long bytes = (depth >> 3) * SquishMin((unsigned long)destRect->w, (unsigned long)srcRect->w);
    unsigned long lines2copy = SquishMin((unsigned long)destRect->h, (unsigned long)srcRect->h);
    if (depth == 8)
        SquishBlitter<unsigned char>::BlitBlockTrans(dest, src, srcFmt, destRowbytes, srcRowbytes, bytes, lines2copy);
    else if (depth == 16)
        SquishBlitter<unsigned short>::BlitBlockTrans(dest, src, srcFmt, destRowbytes, srcRowbytes, bytes,
                                                       lines2copy);
    else if (depth == 32)
        SquishBlitter<unsigned long>::BlitBlockTrans(dest, src, srcFmt, destRowbytes, srcRowbytes, bytes,
                                                      lines2copy);
}


// Matched by this code:
// 0x1005ed60 _$E19
// 0x1005ed80 _$E18
