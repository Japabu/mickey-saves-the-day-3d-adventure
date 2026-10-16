// MfImage: in-memory image of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFIMAGE_H
#define MFIMAGE_H

// return codes; 3 is what the conversion functions return for unsupported formats
enum MfImageError {
    MfImageOk = 0,
    MfImageUnsupported = 1,
    MfImageCantOpen = 2,
    MfImageBadFile = 3
};

// format: low byte = pixel kind (1: 8 bit, 2/4: 16 bit, 8: 24 bit, 0x10: 32 bit),
// 0xff00 = component order/kind (0x200 palettized, 0x400/0x800 the two RGB orders),
// 16 bit sub-formats: 2 = 555, 0x40000 = 1555, 0x20000 = 565, 0x80000 = 4444,
// 0x10000 = has alpha.
// 0x24 bytes; implicit operator= (memberwise, rep movsd)
class __declspec(dllexport) MfImage {
public:
    MfImage();
    MfImage(const MfImage &other);
    ~MfImage();
    static int PixelSize(int format);
    void InitPixels(int width, int height, int format, int alignment);
    void InitPalette(int numColors, int format);
    unsigned char *PaletteEntry(int index);
    int RowOffset();
    void SampleBilinear(float x, float y, unsigned char *out, int count) const;
    void SampleNearest(float x, float y, unsigned char *out, int count) const;
    void Resize(const MfImage &src, int width, int height);
    MfImageError ReorderComponents(int flags);
    MfImageError RemovePalette(int format);
    MfImageError ChangePixelFormat(int format);
    MfImageError CopyAlpha(const MfImage &src);

    int m_format;            // 0x00
    int m_width;             // 0x04
    int m_height;            // 0x08
    int m_alignment;         // 0x0c
    int m_rowOffset;         // 0x10 bytes per row
    unsigned char *m_pixels; // 0x14
    int m_numColors;         // 0x18
    int m_paletteFormat;     // 0x1c
    unsigned char *m_palette;// 0x20
};

#endif
