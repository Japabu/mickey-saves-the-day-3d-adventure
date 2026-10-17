// MfTgaHandler: Truevision TGA reader/writer (MfImageLib.dll 0x10002440-0x10002ac0).
#include "MfTgaHandler_unit.h"

// 0x10002440 ?GetExtensions@MfTgaHandler@@UAEABVMfString@@XZ (original MfTgaHandler::GetExtensions)
// 0x10002480 _$E15 (original (atexit) destructor of the local static)
const MfString &MfTgaHandler::GetExtensions()
{
    static MfString extensions(".tga .vda .icb .vst");
    return extensions;
}

// 0x10002490 ?CanHandle@MfTgaHandler@@UAE_NW4MfImageOp@@@Z (original MfTgaHandler::CanHandle)
bool MfTgaHandler::CanHandle(MfImageOp op)
{
    return op == MfImageRead || op == MfImageWrite;
}

// 0x100024b0 ?Handle@MfTgaHandler@@UAE?AW4MfImageError@@W4MfImageOp@@AAUMfImageParams@@@Z (original MfTgaHandler::Handle)
MfImageError MfTgaHandler::Handle(MfImageOp op, MfImageParams &params)
{
    if (op == MfImageRead)
        return Read(params);
    if (op == MfImageWrite)
        return Write(params);
    return MfImageUnsupported;
}

// 0x100024e0 ?Read@MfTgaHandler@@AAE?AW4MfImageError@@AAUMfImageParams@@@Z (original MfTgaHandler::Read)
MfImageError MfTgaHandler::Read(MfImageParams &params)
{
    FILE *file = fopen(params.path, "rb");
    if (!file)
        return MfImageCantOpen;

    TgaHeader header;
    fread(&header.idLength, 1, 1, file);
    fread(&header.colorMapType, 1, 1, file);
    fread(&header.imageType, 1, 1, file);
    fread(&header.cmapStart, 2, 1, file);
    fread(&header.cmapLength, 2, 1, file);
    fread(&header.cmapDepth, 1, 1, file);
    fread(&header.xOrigin, 2, 1, file);
    fread(&header.yOrigin, 2, 1, file);
    fread(&header.width, 2, 1, file);
    fread(&header.height, 2, 1, file);
    fread(&header.pixelDepth, 1, 1, file);
    if (fread(&header.descriptor, 1, 1, file) != 1) {
        fclose(file);
        return MfImageBadFile;
    }

    if (header.idLength > 0) {
        if (fseek(file, header.idLength, SEEK_CUR) != 0) {
            fclose(file);
            return MfImageBadFile;
        }
    }

    if (header.colorMapType == 1) {
        if (!ReadPalette(file, &header, params.image)) {
            fclose(file);
            return MfImageBadFile;
        }
    }

    if (!ReadPixels(file, &header, params)) {
        fclose(file);
        return MfImageBadFile;
    }
    fclose(file);
    return MfImageOk;
}

// 0x100026a0 ?Write@MfTgaHandler@@AAE?AW4MfImageError@@AAUMfImageParams@@@Z (original MfTgaHandler::Write)
MfImageError MfTgaHandler::Write(MfImageParams &params)
{
    int format = params.image->m_format;
    int pixelSize = MfImage::PixelSize(format);
    int bits = format & 0xff;
    if ((format & 0xff00) != 0x800 || (bits != 8 && bits != 16))
        return MfImageBadFile;

    FILE *file = fopen(params.path, "wb");
    if (!file)
        return MfImageCantOpen;

    TgaHeader header;
    header.idLength = 0;
    header.colorMapType = 0;
    header.cmapStart = 0;
    header.cmapLength = 0;
    header.cmapDepth = 0;
    header.yOrigin = 0;
    header.xOrigin = 0;
    header.imageType = 2;
    header.width = params.image->m_width;
    header.height = params.image->m_height;
    header.pixelDepth = pixelSize << 3;
    header.descriptor = ((unsigned)format >> 1) & 8;

    fwrite(&header.idLength, 1, 1, file);
    fwrite(&header.colorMapType, 1, 1, file);
    fwrite(&header.imageType, 1, 1, file);
    fwrite(&header.cmapStart, 2, 1, file);
    fwrite(&header.cmapLength, 2, 1, file);
    fwrite(&header.cmapDepth, 1, 1, file);
    fwrite(&header.xOrigin, 2, 1, file);
    fwrite(&header.yOrigin, 2, 1, file);
    fwrite(&header.width, 2, 1, file);
    fwrite(&header.height, 2, 1, file);
    fwrite(&header.pixelDepth, 1, 1, file);
    if (fwrite(&header.descriptor, 1, 1, file) != 1) {
        fclose(file);
        return MfImageBadFile;
    }

    MfImage *image = params.image;
    int rowBytes = image->m_width * pixelSize;
    int step = image->m_rowOffset;
    unsigned char *row;
    unsigned char *end;
    if (image->m_format & 0x1000000) {
        row = image->m_pixels;
        end = image->m_pixels + header.height * step;
    } else {
        row = image->m_pixels + (header.height - 1) * step;
        end = image->m_pixels - step;
        step = -step;
    }
    for (; row != end; row += step) {
        if (fwrite(row, rowBytes, 1, file) != 1) {
            fclose(file);
            return MfImageBadFile;
        }
    }
    fclose(file);
    return MfImageOk;
}

// 0x100028e0 ?FormatFromDepth@@YAHH@Z (format for a TGA bit depth)
// Not static: a static function would be emitted after its first user
// (ReadPalette); the original has it here, before ReadPalette.
int FormatFromDepth(int depth)
{
    switch (depth) {
    case 15:
        return 0x802;
    case 16:
        return 0x50804;
    case 24:
        return 0x808;
    case 32:
        return 0x10810;
    }
    return 0;
}

// 0x10002940 ?ReadPalette@MfTgaHandler@@AAE_NPAU_iobuf@@PAUTgaHeader@@PAVMfImage@@@Z (original MfTgaHandler::ReadPalette)
bool MfTgaHandler::ReadPalette(FILE *file, TgaHeader *header, MfImage *image)
{
    int format = FormatFromDepth(header->cmapDepth);
    if (!format)
        return false;
    image->InitPalette(header->cmapLength, format);
    int size = MfImage::PixelSize(format) * header->cmapLength;
    return fread(image->m_palette, size, 1, file) == 1 ? true : false;
}

// 0x100029b0 ?ReadPixels@MfTgaHandler@@AAE_NPAU_iobuf@@PAUTgaHeader@@AAUMfImageParams@@@Z (original MfTgaHandler::ReadPixels)
bool MfTgaHandler::ReadPixels(FILE *file, TgaHeader *header, MfImageParams &params)
{
    int rle = header->imageType & 8;
    int type = header->imageType & 7;
    if (type == 0 || type == 3 || rle)
        return false;

    int format = FormatFromDepth(header->pixelDepth);
    if (!format)
        return false;

    params.image->InitPixels(header->width, header->height, format, params.alignment);
    int rowOffset = params.image->RowOffset();
    int rowBytes = MfImage::PixelSize(format) * header->width;

    for (unsigned char *row = params.image->m_pixels + (header->height - 1) * rowOffset;
         row >= params.image->m_pixels; row -= rowOffset) {
        if (fread(row, rowBytes, 1, file) != 1)
            return false;
    }

    if (header->pixelDepth <= 16) {
        unsigned short *p = (unsigned short *)params.image->m_pixels;
        unsigned short *end = p + rowOffset / sizeof(unsigned short) * header->height;
        for (; p < end; p++)
            *p ^= 0x8000;
    }

    params.image->ReorderComponents(params.flags);
    return true;
}
