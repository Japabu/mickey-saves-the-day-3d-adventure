// MfImageHandler: interface of the image file format handlers of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFIMAGEHANDLER_H
#define MFIMAGEHANDLER_H

#ifndef MFIMAGE_API
#define MFIMAGE_API __declspec(dllimport)
#endif
#include "../mfutil/MfString.h"
#include "MfImage.h"
#include "MfImageParams.h"

// no data, no virtual destructor (DllMain deletes with plain operator delete)
class MfImageHandler {
public:
    virtual const MfString &GetExtensions() = 0;
    virtual bool CanHandle(MfImageOp op) = 0;
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params) = 0;
};

// the registered handler for path's extension that can do op, or 0
MFIMAGE_API MfImageHandler *MfFindImageHandler(MfImageOp op, const MfString &path);

#endif
