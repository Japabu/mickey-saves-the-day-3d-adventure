// MfBmpHandler: Windows BMP reader of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFBMPHANDLER_H
#define MFBMPHANDLER_H

#include <windows.h>
#include <stdio.h>
#include "MfImageHandler.h"

class MfBmpHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
private:
    bool ReadPixels(FILE *file, BITMAPINFOHEADER *info, MfImageParams &params);
};

#endif
