// MfJpgHandler: JPEG reader/writer of MfImageLib (MfImageLib.dll, VC5, no PDB), on IJG libjpeg 6b.
#ifndef MFJPGHANDLER_H
#define MFJPGHANDLER_H

#include <windows.h>
#include <stdio.h>

extern "C" {
#include "jpeg/jpeglib.h"
}

#include "MfImageHandler.h"

// mfutil.dll
class MFUTIL_API MfLogger {
public:
    static void Log(int level, const MfString &title, const MfString &context, const MfString &message);
};

// 0x240 bytes (DllMain: new 0x240)
class MfJpgHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);      // folded with MfTgaHandler::CanHandle (0x10002490)
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
private:
    bool Decode(FILE *file, MfImageParams &params);

    int m_field_04;                         // 0x004 (not used here)
    struct jpeg_decompress_struct m_cinfo;  // 0x008 (0x1b0)
    struct jpeg_error_mgr m_jerr;           // 0x1b8 (0x84)
    int m_field_23c;                        // 0x23c (not used here)
};

#endif
