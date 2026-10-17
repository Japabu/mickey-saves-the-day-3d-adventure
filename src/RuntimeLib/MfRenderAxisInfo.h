// MfRenderAxisInfo: how a space draws one of its three axes (exported
// struct; the implicit members are emitted by the first object file of
// RuntimeLib.dll, MfSkeletonNode.cpp).
#ifndef RUNTIMELIB_MFRENDERAXISINFO_H
#define RUNTIMELIB_MFRENDERAXISINFO_H
#include "MfColor.h"
#include "MfVector3.h"
#include "RuntimeLib_api.h"

struct RUNTIME_API MfRenderAxisInfo {
    MfVector3 mOrigin;                                      // 0x00
    MfVector3 mExtent;                                      // 0x0c
    float mLineWidth;                                       // 0x18
    MfColor mColor;                                         // 0x1c
};

#endif
