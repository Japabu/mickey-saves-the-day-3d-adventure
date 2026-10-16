// MfJpgHandler: JPEG reader/writer of MfImageLib (MfImageLib.dll, VC5, no PDB), on IJG libjpeg 6b.
#ifndef MFJPGHANDLER_H
#define MFJPGHANDLER_H

#include <windows.h>
#include <stdio.h>

extern "C" {
#include "jpeg/jpeglib.h"
}

// mfutil.dll
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    operator const char *() const;
private:
    char *m_str;
};

class __declspec(dllimport) MfLogger {
public:
    static void Log(int level, const MfString &title, const MfString &context, const MfString &message);
};

enum MfImageError {
    MfImageOk = 0,
    MfImageUnsupported = 1,
    MfImageCantOpen = 2,
    MfImageBadFile = 3
};

enum MfImageOp {
    MfImageRead = 0,
    MfImageWrite = 1
};

// 0x24 bytes
class __declspec(dllexport) MfImage {
public:
    MfImage();
    MfImage(const MfImage &other);
    ~MfImage();
    void InitPixels(int width, int height, int format, int alignment);
    void InitPalette(int numColors, int format);
    unsigned char *PaletteEntry(int index);
    int RowOffset();
    static int PixelSize(int format);
    MfImageError RemovePalette(int format);
    MfImageError ReorderComponents(int flags);

    int m_format;            // 0x00
    int m_width;             // 0x04
    int m_height;            // 0x08
    int m_alignment;         // 0x0c
    int m_rowOffset;         // 0x10
    unsigned char *m_pixels; // 0x14
    int m_numColors;         // 0x18
    int m_paletteFormat;     // 0x1c
    unsigned char *m_palette;// 0x20
};

// 0x14 bytes
struct __declspec(dllexport) MfImageParams {
    MfImageParams();
    MfImage *image;  // 0x00
    MfString path;   // 0x04
    int alignment;   // 0x08 (passed to InitPixels)
    int flags;       // 0x0c (passed to ReorderComponents)
    int field_10;    // 0x10
};

class MfImageHandler {
public:
    virtual const MfString &GetExtensions() = 0;
    virtual bool CanHandle(MfImageOp op) = 0;
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params) = 0;
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
