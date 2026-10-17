// MfBmpHandler: Windows BMP reader (MfImageLib.dll 0x10001000-0x10001490).
// 0x10001000-0x100010a0 are the implicit members of the exported classes
// MfImage/MfImageParams, emitted in this object (first user).
#include "MfBmpHandler.h"
#include <string.h>

// 0x10001000 ??4MfImage@@QAEAAV0@ABV0@@Z (implicit)
// 0x10001020 ??0MfImageParams@@QAE@ABU0@@Z (implicit)
// 0x10001060 ??4MfImageParams@@QAEAAU0@ABU0@@Z (implicit)
// 0x100010a0 ??1MfImageParams@@QAE@XZ (implicit)

// 0x100010b0 ?GetExtensions@MfBmpHandler@@UAEABVMfString@@XZ (original MfBmpHandler::GetExtensions)
// 0x100010f0 _$E15 (original (atexit) destructor of the local static)
const MfString &MfBmpHandler::GetExtensions()
{
    static MfString extensions(".bmp");
    return extensions;
}

// 0x10001100 ?CanHandle@MfBmpHandler@@UAE_NW4MfImageOp@@@Z (original MfBmpHandler::CanHandle)
bool MfBmpHandler::CanHandle(MfImageOp op)
{
    return op == MfImageRead;
}

// EQUIV 0x10001110 ?Handle@MfBmpHandler@@UAE?AW4MfImageError@@W4MfImageOp@@AAUMfImageParams@@@Z: reviewed: the reload of params (edi) at the loop top instead of the bottom, plus one reload after the loop
MfImageError MfBmpHandler::Handle(MfImageOp op, MfImageParams &params)
{
    if (op != MfImageRead)
        return MfImageUnsupported;

    FILE *file = fopen(params.path, "rb");
    if (!file)
        return MfImageCantOpen;

    BITMAPFILEHEADER header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return MfImageBadFile;
    }
    if (header.bfType != 0x4d42) {
        fclose(file);
        return MfImageBadFile;
    }

    DWORD infoSize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
    BITMAPINFO *info = (BITMAPINFO *)new unsigned char[infoSize];
    if (fread(info, infoSize, 1, file) != 1) {
        fclose(file);
        delete info;
        return MfImageBadFile;
    }

    int numColors = (infoSize - sizeof(BITMAPINFOHEADER)) / sizeof(RGBQUAD);
    if (numColors > 0) {
        params.image->InitPalette(numColors, 0x808);
        int size = MfImage::PixelSize(0x808);
        for (int i = 0; i < numColors; i++)
            memcpy(params.image->m_palette + i * size, &info->bmiColors[i], size);
    }

    MfImageError result = MfImageOk;
    if (!ReadPixels(file, &info->bmiHeader, params))
        result = MfImageBadFile;
    fclose(file);
    delete info;
    return result;
}

// 0x100012b0 ?ReadPixels@MfBmpHandler@@AAE_NPAU_iobuf@@PAUtagBITMAPINFOHEADER@@AAUMfImageParams@@@Z (original MfBmpHandler::ReadPixels)
bool MfBmpHandler::ReadPixels(FILE *file, BITMAPINFOHEADER *info, MfImageParams &params)
{
    int bits = info->biBitCount;
    int format;
    switch (bits) {
    case 4:
    case 8:
        format = 0x201;
        break;
    case 24:
        format = 0x808;
        break;
    default:
        return false;
    }

    int width = info->biWidth;
    int height = abs(info->biHeight);
    bool topDown = info->biHeight < 0;
    params.image->InitPixels(width, height, format, params.alignment);

    int stride = ((bits * info->biWidth + 7) / 8 + 3) & ~3;
    unsigned char *row = new unsigned char[stride];
    int rowBytes = MfImage::PixelSize(format) * width;

    unsigned char *dst;
    int step;
    if (topDown) {
        dst = params.image->m_pixels;
        step = params.image->RowOffset();
    } else {
        dst = params.image->m_pixels + params.image->RowOffset() * (height - 1);
        step = -params.image->RowOffset();
    }

    for (int y = 0; y < height; y++) {
        if (fread(row, stride, 1, file) != 1) {
            delete row;
            return false;
        }
        if (bits == 4) {
            unsigned char *d = dst;
            for (int i = 0; i < rowBytes / 2; i++) {
                *d++ = row[i] >> 4;
                *d++ = row[i] & 0xf;
            }
        } else {
            memcpy(dst, row, rowBytes);
        }
        dst += step;
    }

    if (format == 0x201 && !(params.flags & 0x200))
        params.image->RemovePalette(0x808);
    params.image->ReorderComponents(params.flags);
    delete row;
    return true;
}


// Also matched by this code:
// piccoloconsole.dll 0x1001b3b0 ??1MfImageParams@@QAE@XZ