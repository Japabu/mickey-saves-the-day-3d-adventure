// MfImage: in-memory image (MfImageLib.dll 0x10001490-0x10002200, VC5 /O2 /MD: floor is called through the MSVCRT import).
#include <math.h>
#include <string.h>
#include "MfImage.h"

// 0x10001490 ??0MfImage@@QAE@XZ
MfImage::MfImage()
    : m_format(0), m_width(0), m_height(0), m_alignment(0), m_rowOffset(0),
      m_pixels(0), m_numColors(0), m_paletteFormat(0), m_palette(0)
{
}

// 0x100014b0 ??0MfImage@@QAE@ABV0@@Z
MfImage::MfImage(const MfImage &other)
    : m_format(0), m_width(0), m_height(0), m_alignment(0), m_rowOffset(0),
      m_pixels(0), m_numColors(0), m_paletteFormat(0), m_palette(0)
{
    if (other.m_format != 0) {
        InitPixels(other.m_width, other.m_height, other.m_format, other.m_alignment);
        memcpy(m_pixels, other.m_pixels, m_rowOffset * m_height);
    }
    if (other.m_paletteFormat != 0) {
        InitPalette(other.m_numColors, other.m_paletteFormat);
        memcpy(m_palette, other.m_palette, PixelSize(m_paletteFormat) * m_numColors);
    }
}

// 0x10001550 ??1MfImage@@QAE@XZ
MfImage::~MfImage()
{
    delete m_pixels;
    delete m_palette;
}

// 0x10001570 ?PixelSize@MfImage@@SAHH@Z
int MfImage::PixelSize(int format)
{
    switch (format & 0xff) {
    case 1:
        return 1;
    case 2:
        return 2;
    case 4:
        return 2;
    case 8:
        return 3;
    case 0x10:
        return 4;
    }
    return 0;
}

// 0x100015e0 ?InitPixels@MfImage@@QAEXHHHH@Z
void MfImage::InitPixels(int width, int height, int format, int alignment)
{
    delete m_pixels;
    m_width = width;
    m_height = height;
    m_format = format;
    m_alignment = alignment;
    m_rowOffset = PixelSize(format) * width;
    if (alignment != 0) {
        m_rowOffset = m_rowOffset + alignment - 1;
        m_rowOffset -= m_rowOffset % alignment;
    }
    m_pixels = new unsigned char[height * m_rowOffset];
}

// 0x10001650 ?InitPalette@MfImage@@QAEXHH@Z
void MfImage::InitPalette(int numColors, int format)
{
    delete m_palette;
    m_palette = 0;
    m_numColors = numColors;
    m_paletteFormat = format;
    if (numColors > 0)
        m_palette = new unsigned char[PixelSize(format) * m_numColors];
}

// 0x100016a0 ?PaletteEntry@MfImage@@QAEPAEH@Z
unsigned char *MfImage::PaletteEntry(int index)
{
    return m_palette + PixelSize(m_paletteFormat) * index;
}

// 0x100016c0 ?RowOffset@MfImage@@QAEHXZ
int MfImage::RowOffset()
{
    return m_rowOffset;
}

// TODO 0x100016d0 ?SampleBilinear@MfImage@@QBEXMMPAEH@Z: x87 code: original interleaves the 4-term bilinear sum with fxch (keeps 4 products on the FP stack, spills this); compiled sums sequentially without fxch, tried 10+ term orders/parenthesizations
void MfImage::SampleBilinear(float x, float y, unsigned char *out, int count) const
{
    int pixelSize = PixelSize(m_format);
    if (count == 0)
        count = pixelSize;
    int ix = (int)x;
    int iy = (int)y;
    x -= ix;
    y -= iy;
    const unsigned char *p = m_pixels + iy * m_rowOffset + ix * pixelSize;
    float x1 = 1.0f - x;
    float y1 = 1.0f - y;
    const unsigned char *end = p + count;
    if (x <= 0.001f && y <= 0.001f) {
        while (p < end)
            *out++ = *p++;
    } else if (x <= 0.001f) {
        while (p < end) {
            *out++ = (unsigned char)(p[m_rowOffset] * y + *p * y1);
            p++;
        }
    } else if (y <= 0.001f) {
        while (p < end) {
            *out++ = (unsigned char)(p[pixelSize] * x + *p * x1);
            p++;
        }
    } else {
        while (p < end) {
            const unsigned char *q = p + m_rowOffset;
            *out++ = (unsigned char)(q[pixelSize] * y * x + p[pixelSize] * y1 * x +
                                     *p * y1 * x1 + *q * y * x1);
            p++;
        }
    }
}

// 0x10001900 ?SampleNearest@MfImage@@QBEXMMPAEH@Z
void MfImage::SampleNearest(float x, float y, unsigned char *out, int count) const
{
    int pixelSize = PixelSize(m_format);
    if (count == 0)
        count = pixelSize;
    int ix = (int)floor(x + 0.5f);
    int iy = (int)floor(y + 0.5f);
    const unsigned char *p = m_pixels + ix * pixelSize + iy * m_rowOffset;
    const unsigned char *end = p + count;
    while (p < end)
        *out++ = *p++;
}

// 0x10001990 ?Resize@MfImage@@QAEXABV1@HH@Z
void MfImage::Resize(const MfImage &src, int width, int height)
{
    float dx, dy;
    InitPixels(width, height, src.m_format, src.m_alignment);
    int pixelSize = PixelSize(m_format);
    unsigned char *row = m_pixels;
    unsigned char *rowEnd = m_pixels + pixelSize * m_width;
    unsigned char *end = m_pixels + m_rowOffset * m_height;
    dx = (float)(src.m_width - 1) / m_width;
    dy = (float)(src.m_height - 1) / m_height;
    float sy = 0;
    for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, sy += dy) {
        float sx = 0;
        for (unsigned char *p = row; p < rowEnd; p += pixelSize, sx += dx)
            src.SampleBilinear(sx, sy, p, 0);
    }
}

// EQUIV 0x10001a90 ?ReorderComponents@MfImage@@QAE?AW4MfImageError@@H@Z: reviewed: m_format reloaded after the PixelSize call instead of before it (PixelSize is pure)
MfImageError MfImage::ReorderComponents(int flags)
{
    int pixelSize, order, current;
    order = flags & 0xff00;
    pixelSize = PixelSize(m_format);
    current = m_format & 0xff00;
    if (order & current)
        return MfImageOk;
    if (((current == 0x400 && order == 0x800) || (current == 0x800 && order == 0x400)) &&
        pixelSize >= 3) {
        unsigned char *rowEnd = m_pixels + m_width * pixelSize;
        unsigned char *end = m_pixels + m_height * m_rowOffset;
        for (unsigned char *row = m_pixels; row < end; row += m_rowOffset, rowEnd += m_rowOffset) {
            for (unsigned char *p = row; p < rowEnd; p += pixelSize) {
                unsigned char t = p[0];
                p[0] = p[2];
                p[2] = t;
            }
        }
        m_format = (m_format & ~0xff00) | order;
        return MfImageOk;
    }
    return MfImageBadFile;
}

// EQUIV 0x10001b50 ?RemovePalette@MfImage@@QAE?AW4MfImageError@@H@Z: reviewed: entry address in the palette-copy loop computed in another order (idx*entrySize + palette)
MfImageError MfImage::RemovePalette(int format)
{
    if ((m_format & 0xff00) == 0x200 && PixelSize(m_format) == 1) {
        int order = format & 0xff00;
        if (order == 0x400 || order == 0x800) {
            int pixelSize = PixelSize(format);
            if (PixelSize(format) >= 3) {
                unsigned char *oldPixels = m_pixels;
                int oldRowOffset = m_rowOffset;
                m_pixels = 0;
                InitPixels(m_width, m_height, format, m_alignment);
                unsigned char *row = m_pixels;
                unsigned char *rowEnd = m_pixels + pixelSize * m_width;
                unsigned char *end = m_pixels + m_height * m_rowOffset;
                unsigned char *srcRow = oldPixels;
                int entrySize = PixelSize(m_paletteFormat);
                if ((m_paletteFormat & 0xff00) == order) {
                    for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, srcRow += oldRowOffset) {
                        unsigned char *s = srcRow;
                        for (unsigned char *p = row; p < rowEnd; p += pixelSize, s++) {
                            unsigned char *e = m_palette + entrySize * *s;
                            p[0] = e[0];
                            p[1] = e[1];
                            p[2] = e[2];
                        }
                    }
                } else {
                    for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, srcRow += oldRowOffset) {
                        unsigned char *s = srcRow;
                        for (unsigned char *p = row; p < rowEnd; p += pixelSize, s++) {
                            unsigned char *e = m_palette + entrySize * *s;
                            p[0] = e[2];
                            p[1] = e[1];
                            p[2] = e[0];
                        }
                    }
                }
                delete oldPixels;
                delete m_palette;
                m_palette = 0;
                m_paletteFormat = 0;
                return MfImageOk;
            }
        }
    }
    return MfImageBadFile;
}

// 0x10001d20 ?ChangePixelFormat@MfImage@@QAE?AW4MfImageError@@H@Z
MfImageError MfImage::ChangePixelFormat(int format)
{
    if (format == m_format)
        return MfImageOk;
    int oldSize = PixelSize(m_format);
    int newSize = PixelSize(format);
    if (oldSize < 2 || newSize < 2)
        return MfImageBadFile;
    unsigned char *oldPixels = m_pixels;
    int oldRowOffset = m_rowOffset;
    int oldFormat = m_format;
    m_pixels = 0;
    InitPixels(m_width, m_height, format, m_alignment);
    unsigned char *row = m_pixels;
    unsigned char *rowEnd = m_pixels + newSize * m_width;
    unsigned char *end = m_pixels + m_height * m_rowOffset;
    unsigned char *srcRow = oldPixels;
    unsigned char c[4];
    for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, srcRow += oldRowOffset) {
        unsigned char *s = srcRow;
        for (unsigned char *p = row; p < rowEnd; p += newSize, s += oldSize) {
            switch (oldSize) {
            case 3:
                c[0] = s[0];
                c[1] = s[1];
                c[2] = s[2];
                c[3] = 0xff;
                break;
            case 4:
                c[0] = s[0];
                c[1] = s[1];
                c[2] = s[2];
                c[3] = s[3];
                break;
            default: {
                short v = *(short *)s;
                if (oldFormat & 2) {
                    c[0] = (unsigned char)(v << 3);
                    c[1] = (unsigned char)((unsigned char)(v >> 5) << 3);
                    c[2] = (unsigned char)((unsigned char)(v >> 10) << 3);
                    c[3] = 0xff;
                } else if (oldFormat & 0x40000) {
                    c[0] = (unsigned char)(v << 3);
                    c[1] = (unsigned char)((unsigned char)(v >> 5) << 3);
                    c[2] = (unsigned char)((unsigned char)(v >> 10) << 3);
                    c[3] = (unsigned char)(v >> 15);
                } else if (oldFormat & 0x20000) {
                    c[0] = (unsigned char)(v << 3);
                    c[1] = (unsigned char)((unsigned char)(v >> 5) << 2);
                    c[2] = (unsigned char)((unsigned char)(v >> 11) << 3);
                    c[3] = 0xff;
                } else if (oldFormat & 0x80000) {
                    c[0] = (unsigned char)(v << 4);
                    c[1] = (unsigned char)((unsigned char)(v >> 4) << 4);
                    c[2] = (unsigned char)((unsigned char)(v >> 8) << 4);
                    c[3] = (unsigned char)((unsigned char)(v >> 12) << 4);
                }
                break;
            }
            }
            switch (newSize) {
            case 4:
                p[3] = c[3];
            case 3:
                p[2] = c[2];
                p[1] = c[1];
                p[0] = c[0];
                break;
            default: {
                unsigned short v;
                if (m_format & 2)
                    v = (unsigned short)((((unsigned char)(c[2] >> 3) << 5 | (unsigned char)(c[1] >> 3)) << 5) | (unsigned char)(c[0] >> 3));
                else if (m_format & 0x40000)
                    v = (unsigned short)((((((unsigned char)(c[3] >> 7) << 5) | (unsigned char)(c[2] >> 3)) << 5 | (unsigned char)(c[1] >> 3)) << 5) | (unsigned char)(c[0] >> 3));
                else if (m_format & 0x20000)
                    v = (unsigned short)((((unsigned char)(c[2] >> 3) << 6 | (unsigned char)(c[1] >> 2)) << 5) | (unsigned char)(c[0] >> 3));
                else if (m_format & 0x80000)
                    v = (unsigned short)((((((unsigned char)(c[3] >> 4) << 4) | (unsigned char)(c[2] >> 4)) << 4 | (unsigned char)(c[1] >> 4)) << 4) | (unsigned char)(c[0] >> 4));
                *(unsigned short *)p = v;
                break;
            }
            }
        }
    }
    delete oldPixels;
    return MfImageOk;
}

// 0x10002090 ?CopyAlpha@MfImage@@QAE?AW4MfImageError@@ABV1@@Z
MfImageError MfImage::CopyAlpha(const MfImage &src)
{
    int kind = m_format & 0xff;
    if ((m_format & 0x10000) && kind == 0x10) {
        int pixelSize = PixelSize(m_format);
        unsigned char *row = m_pixels + 3;
        unsigned char *rowEnd = m_pixels + pixelSize * m_width;
        unsigned char *end = row + m_rowOffset * m_height;
        if (src.m_width == m_width && src.m_height == m_height) {
            const unsigned char *srcRow = src.m_pixels;
            int srcSize = PixelSize(src.m_format);
            for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, srcRow += src.m_rowOffset) {
                unsigned char *p;
                const unsigned char *s;
                for (p = row, s = srcRow; p < rowEnd; p += pixelSize, s += srcSize)
                    *p = *s;
            }
            return MfImageOk;
        }
        float dx = (float)(src.m_width - 1) / (m_width - 1);
        float dy = (float)(src.m_height - 1) / (m_height - 1);
        float sy = 0;
        for (; row < end; row += m_rowOffset, rowEnd += m_rowOffset, sy += dy) {
            float sx = 0;
            for (unsigned char *p = row; p < rowEnd; p += pixelSize, sx += dx)
                src.SampleBilinear(sx, sy, p, 1);
        }
        return MfImageOk;
    }
    return MfImageBadFile;
}
