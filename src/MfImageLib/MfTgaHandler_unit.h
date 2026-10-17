// own copy: VC5 compiles MfTgaHandler::Write differently with the shared headers
// MfTgaHandler: Truevision TGA reader/writer of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFTGAHANDLER_UNIT_H
#define MFTGAHANDLER_UNIT_H

#include <windows.h>
#include <stdio.h>

// mfutil.dll
#include "../mfutil/MfString.h"

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

// 0x24 bytes; implicit operator= (memberwise, rep movsd)
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

// 0x14 bytes; implicit copy constructor, operator= and destructor (exported)
struct __declspec(dllexport) MfImageParams {
    MfImageParams();
    MfImage *image;  // 0x00
    MfString path;   // 0x04
    int alignment;   // 0x08 (passed to InitPixels)
    int flags;       // 0x0c (0x200: keep palette; passed to ReorderComponents)
    int field_10;    // 0x10
};

// no data, no virtual destructor (DllMain deletes with plain operator delete)
class MfImageHandler {
public:
    virtual const MfString &GetExtensions() = 0;
    virtual bool CanHandle(MfImageOp op) = 0;
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params) = 0;
};

// TGA file header as read/written field by field (natural alignment, 0x14 bytes)
struct TgaHeader {
    unsigned char idLength;      // 0x00
    unsigned char colorMapType;  // 0x01
    unsigned char imageType;     // 0x02
    unsigned short cmapStart;    // 0x04
    unsigned short cmapLength;   // 0x06
    unsigned char cmapDepth;     // 0x08
    unsigned short xOrigin;      // 0x0a
    unsigned short yOrigin;      // 0x0c
    unsigned short width;        // 0x0e
    unsigned short height;       // 0x10
    unsigned char pixelDepth;    // 0x12
    unsigned char descriptor;    // 0x13
};

class MfTgaHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
private:
    MfImageError Read(MfImageParams &params);
    MfImageError Write(MfImageParams &params);
    bool ReadPalette(FILE *file, TgaHeader *header, MfImage *image);
    bool ReadPixels(FILE *file, TgaHeader *header, MfImageParams &params);
};

#endif
