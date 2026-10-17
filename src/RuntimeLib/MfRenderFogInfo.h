// MfRenderFogInfo: the fog of a space (exported struct; the implicit
// members are emitted by the first object file of RuntimeLib.dll,
// MfSkeletonNode.cpp).
#ifndef RUNTIMELIB_MFRENDERFOGINFO_H
#define RUNTIMELIB_MFRENDERFOGINFO_H
#include "MfColor.h"
#include "RuntimeLib_api.h"

struct RUNTIME_API MfRenderFogInfo {
    bool mEnabled;                                          // 0x00
    int mMode;                                              // 0x04
    MfColor mColor;                                         // 0x08
    float mDensity;                                         // 0x24
    float mStart;                                           // 0x28
    float mEnd;                                             // 0x2c
};

#endif
