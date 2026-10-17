// MBPixelFormat.h - a surface's pixel format: depth and colour masks.
#ifndef MBENGINE_MBPIXELFORMAT_H
#define MBENGINE_MBPIXELFORMAT_H

#include "MBColorMask.h"

struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};

#endif
