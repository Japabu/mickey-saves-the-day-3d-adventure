// SquishEncode.cpp - RLE encoding of a block of an 8, 16 or 32 bit image (the code format
// SquishDecode.cpp reads): a run of equal pixels is written as (count - 1) and the pixel,
// a run of different pixels as ((count - 1) | top bit) and the pixels.
#include "Squish.h"

extern "C" __declspec(dllimport) void *__cdecl MBrealloc_release(void *p, unsigned int size);

template <class T> class SquishEncode {
public:
    enum { kLiteralRun = (T)1 << (sizeof(T) * 8 - 1), kMaxRun = kLiteralRun };

// TODO 0x10063000 ?RLEEncodeBlock@?$SquishEncode@E@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (23% similar)
// TODO 0x10063440 ?RLEEncodeBlock@?$SquishEncode@G@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (45% similar)
// TODO 0x100638b0 ?RLEEncodeBlock@?$SquishEncode@K@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (48% similar)
    static void RLEEncodeBlock(unsigned char **destHdl, const unsigned char *src, SquishImageDescriptor *destFmt,
                               const SquishImageDescriptor *srcFmt, long srcx, long srcy, long srcw, long srch)
    {
        unsigned char *dest = *destHdl;
        if (!dest) {
            dest = (unsigned char *)MBmalloc_release(srcFmt->GetRowbytes() * srcFmt->GetHeight() * 2);
            if (!dest)
                return;
            *destHdl = dest;
        }
        T *runBuf = (T *)MBmalloc_release((kMaxRun + 1) * sizeof(T));
        int srcrowbytes = srcFmt->GetRowbytes();
        const T *srcPtr = (const T *)(src + srcrowbytes * srcy) + srcx;
        T *destPtr = (T *)dest;
        for (long y = srch; y; y--) {
            long x = 0;
            do {
                T pixel = srcPtr[x++];
                T runLength = 1;
                T nextPixel;
                if (x < srcw) {
                    // a run of equal pixels
                    while ((nextPixel = srcPtr[x]) == pixel && runLength < kMaxRun) {
                        runLength++;
                        x++;
                        if (x >= srcw)
                            break;
                    }
                    if (runLength <= 1 && x < srcw) {
                        // a run of different pixels
                        runBuf[runLength] = pixel;
                        x++;
                        pixel = nextPixel;
                        while (x < srcw) {
                            nextPixel = srcPtr[x];
                            if (pixel == nextPixel || runLength >= kMaxRun)
                                break;
                            runLength++;
                            runBuf[runLength] = pixel;
                            x++;
                            pixel = nextPixel;
                        }
                        if (x >= srcw) {
                            runLength++;
                            runBuf[runLength] = pixel;
                        }
                        runBuf[0] = (T)((runLength - 1) | kLiteralRun);
                        MBmemcpy(destPtr, runBuf, (runLength + 1) * sizeof(T));
                        destPtr += runLength + 1;
                        if (x < srcw)
                            x--;
                        continue;
                    }
                }
                *destPtr++ = (T)(runLength - 1);
                *destPtr++ = pixel;
            } while (x < srcw);
            srcPtr = (const T *)((const unsigned char *)srcPtr + srcrowbytes);
        }
        destFmt->datasize = (unsigned char *)destPtr - dest;
        *destHdl = (unsigned char *)MBrealloc_release(dest, destFmt->datasize);
        if (runBuf)
            MBfree_release(runBuf);
    }

    // As RLEEncodeBlock; transparent pixels always go into runs of equal pixels.
// TODO 0x10063200 ?RLEEncodeBlockTrans@?$SquishEncode@E@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (29% similar)
// TODO 0x10063640 ?RLEEncodeBlockTrans@?$SquishEncode@G@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (27% similar)
// TODO 0x10063aa0 ?RLEEncodeBlockTrans@?$SquishEncode@K@@SAXPAPAEPBEPAVSquishImageDescriptor@@PBV2@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (38% similar)
    static void RLEEncodeBlockTrans(unsigned char **destHdl, const unsigned char *src,
                                    SquishImageDescriptor *destFmt, const SquishImageDescriptor *srcFmt, long srcx,
                                    long srcy, long srcw, long srch)
    {
        unsigned char *dest = *destHdl;
        if (!dest) {
            dest = (unsigned char *)MBmalloc_release(srcFmt->GetRowbytes() * srcFmt->GetHeight() * 2);
            if (!dest)
                return;
            *destHdl = dest;
        }
        T *runBuf = (T *)MBmalloc_release((kMaxRun + 1) * sizeof(T));
        int srcrowbytes = srcFmt->GetRowbytes();
        const T *srcPtr = (const T *)(src + srcrowbytes * srcy) + srcx;
        T *destPtr = (T *)dest;
        T nextPixel = 0;
        T transPixel = srcFmt->TransEnabled() ? (T)srcFmt->GetNativeTransColor() : (T)~0;
        for (long y = srch; y; y--) {
            long x = 0;
            do {
                T pixel = srcPtr[x++];
                T runLength = 1;
                if (x < srcw) {
                    while ((nextPixel = srcPtr[x]) == pixel && runLength < kMaxRun) {
                        runLength++;
                        x++;
                        if (x >= srcw)
                            break;
                    }
                }
                if (runLength <= 1 && pixel != transPixel && x < srcw) {
                    runBuf[runLength] = pixel;
                    x++;
                    pixel = nextPixel;
                    while (x < srcw) {
                        nextPixel = srcPtr[x];
                        if (pixel == nextPixel || pixel == transPixel || runLength >= kMaxRun)
                            break;
                        runLength++;
                        runBuf[runLength] = pixel;
                        x++;
                        pixel = nextPixel;
                    }
                    if (x >= srcw && pixel != transPixel) {
                        runLength++;
                        runBuf[runLength] = pixel;
                    }
                    runBuf[0] = (T)((runLength - 1) | kLiteralRun);
                    MBmemcpy(destPtr, runBuf, (runLength + 1) * sizeof(T));
                    destPtr += runLength + 1;
                    if (x < srcw)
                        x--;
                    continue;
                }
                *destPtr++ = (T)(runLength - 1);
                *destPtr++ = pixel;
            } while (x < srcw);
            srcPtr = (const T *)((const unsigned char *)srcPtr + srcrowbytes);
        }
        destFmt->datasize = (unsigned char *)destPtr - dest;
        *destHdl = (unsigned char *)MBrealloc_release(dest, destFmt->datasize);
        if (runBuf)
            MBfree_release(runBuf);
    }
};

// TODO 0x10062ac0 ?RLEEncodeBlock@@YAXPAPAEPBEPAVSquishImageDescriptor@@PBV1@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (28% similar)
void RLEEncodeBlock(unsigned char **dest, const unsigned char *src, SquishImageDescriptor *destFmt,
                    const SquishImageDescriptor *srcFmt, long srcx, long srcy, long srcw, long srch)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishEncode<unsigned char>::RLEEncodeBlock(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
    else if (depth == 16)
        SquishEncode<unsigned short>::RLEEncodeBlock(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
    else if (depth == 32)
        SquishEncode<unsigned long>::RLEEncodeBlock(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
}

// TODO 0x10062d40 ?RLEEncodeBlockTrans@@YAXPAPAEPBEPAVSquishImageDescriptor@@PBV1@JJJJ@Z: row and run loops reconstructed (size within 10%); branch layout and register allocation differ (36% similar)
void RLEEncodeBlockTrans(unsigned char **dest, const unsigned char *src, SquishImageDescriptor *destFmt,
                         const SquishImageDescriptor *srcFmt, long srcx, long srcy, long srcw, long srch)
{
    unsigned short depth = srcFmt->GetDepth();
    if (depth == 8)
        SquishEncode<unsigned char>::RLEEncodeBlockTrans(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
    else if (depth == 16)
        SquishEncode<unsigned short>::RLEEncodeBlockTrans(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
    else if (depth == 32)
        SquishEncode<unsigned long>::RLEEncodeBlockTrans(dest, src, destFmt, srcFmt, srcx, srcy, srcw, srch);
}

template class SquishEncode<unsigned char>;
template class SquishEncode<unsigned short>;
template class SquishEncode<unsigned long>;

