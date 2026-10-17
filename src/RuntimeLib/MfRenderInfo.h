// The display settings of a space: the three axes, the six grid planes and
// the fog (exported structs; the implicit members are emitted by the first
// object file of RuntimeLib.dll).
#ifndef RUNTIMELIB_MFRENDERINFO_H
#define RUNTIMELIB_MFRENDERINFO_H
#include "MfColor.h"
#include "MfTransform3.h"
#include "MfVector3.h"

struct __declspec(dllexport) MfRenderAxisInfo {
    MfVector3 mOrigin;                                      // 0x00
    MfVector3 mExtent;                                      // 0x0c
    float mLineWidth;                                       // 0x18
    MfColor mColor;                                         // 0x1c
};

struct __declspec(dllexport) MfRenderGridInfo {
    float mSpacing;                                         // 0x00
    int mNumLines;                                          // 0x04
    float mLineWidth;                                       // 0x08
    MfColor mColor;                                         // 0x0c
    MfTransform3 mTransform;                                // 0x28
};

struct __declspec(dllexport) MfRenderFogInfo {
    bool mEnabled;                                          // 0x00
    int mMode;                                              // 0x04
    MfColor mColor;                                         // 0x08
    float mDensity;                                         // 0x24
    float mStart;                                           // 0x28
    float mEnd;                                             // 0x2c
};

#endif
