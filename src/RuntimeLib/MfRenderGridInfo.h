// MfRenderGridInfo: how a space draws one of its six grid planes (exported
// struct; the implicit members are emitted by the first object file of
// RuntimeLib.dll, MfSkeletonNode.cpp).
#ifndef RUNTIMELIB_MFRENDERGRIDINFO_H
#define RUNTIMELIB_MFRENDERGRIDINFO_H
#include "MfColor.h"
#include "MfTransform3.h"
#include "RuntimeLib_api.h"

struct RUNTIME_API MfRenderGridInfo {
    float mSpacing;                                         // 0x00
    int mNumLines;                                          // 0x04
    float mLineWidth;                                       // 0x08
    MfColor mColor;                                         // 0x0c
    MfTransform3 mTransform;                                // 0x28
};

#endif
