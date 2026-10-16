// SquishDiff.cpp - difference images: pixels equal to the previous frame become transparent.
#include "Squish.h"

template <class T> class SquishDiff {
public:
    // dest holds the new frame, src the previous one; both use srcFmt's layout
    static void DifferenceImage(unsigned char *dest, const unsigned char *src, SquishImageDescriptor *srcFmt);
};

// 0x10062910 ?DifferenceImage@?$SquishDiff@E@@SAXPAEPBEPAVSquishImageDescriptor@@@Z
// 0x10062990 ?DifferenceImage@?$SquishDiff@G@@SAXPAEPBEPAVSquishImageDescriptor@@@Z
// 0x10062a10 ?DifferenceImage@?$SquishDiff@K@@SAXPAEPBEPAVSquishImageDescriptor@@@Z
template <class T>
inline void SquishDiff<T>::DifferenceImage(unsigned char *dest, const unsigned char *src, SquishImageDescriptor *srcFmt)
{
    const T *s = (const T *)src;
    T *d = (T *)dest;
    int width = srcFmt->GetWidth();
    int height = srcFmt->GetHeight();
    int rowbytes = srcFmt->GetRowbytes();
    if (srcFmt->TransEnabled()) {
        T transp = (T)srcFmt->GetNativeTransColor();
        for (int y = 0; y < height; y++) {
            T *destSOL = d;
            const T *srcSOL = s;
            for (int x = 0; x < width; x++) {
                if (*d == *s)
                    *d = transp;
                d++;
                s++;
            }
            s = srcSOL + rowbytes;
            d = destSOL + rowbytes;
        }
    }
}

template class SquishDiff<unsigned char>;
template class SquishDiff<unsigned short>;
template class SquishDiff<unsigned long>;

// 0x10062790 ?DifferenceImage@@YAXPAEPBEPAVSquishImageDescriptor@@@Z
void DifferenceImage(unsigned char *dest, const unsigned char *src, SquishImageDescriptor *srcFmt)
{
    if (srcFmt->GetDepth() == 8)
        SquishDiff<unsigned char>::DifferenceImage(dest, src, srcFmt);
    else if (srcFmt->GetDepth() == 16)
        SquishDiff<unsigned short>::DifferenceImage(dest, src, srcFmt);
    else if (srcFmt->GetDepth() == 32)
        SquishDiff<unsigned long>::DifferenceImage(dest, src, srcFmt);
}


// Matched by this code:
// 0x10062a90 _$E19
// 0x10062ab0 _$E18
