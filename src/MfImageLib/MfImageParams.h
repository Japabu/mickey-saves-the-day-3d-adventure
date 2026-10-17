// MfImageParams: the arguments of one image handler call (MfImageLib.dll, VC5, no PDB).
#ifndef MFIMAGEPARAMS_H
#define MFIMAGEPARAMS_H

#include "MfImageLib_api.h"
#include "MfImageLib_mfutil.h"

class MfImage;

// 0x14 bytes; implicit copy constructor, operator= and destructor (exported).
// The constructor is an exported inline (0x10002250); its body is in MfImageHandler.cpp
// only: an exported inline body in this header was emitted into every object and
// changed MfTgaHandler::Write's register allocation.
struct MFIMAGE_API MfImageParams {
    MfImageParams();
    MfImage *image;  // 0x00
    MfString path;   // 0x04
    int alignment;   // 0x08 (passed to InitPixels)
    int flags;       // 0x0c (0x200: keep palette; passed to ReorderComponents)
    int field_10;    // 0x10
};

#endif
