// MfBmpHandler: Windows BMP reader of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFBMPHANDLER_H
#define MFBMPHANDLER_H

#include <windows.h>
#include <stdio.h>

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

class MfBmpHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
private:
    bool ReadPixels(FILE *file, BITMAPINFOHEADER *info, MfImageParams &params);
};

#endif
