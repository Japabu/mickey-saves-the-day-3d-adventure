// MfJpgHandler: JPEG reader/writer (MfImageLib.dll 0x10002ac0-0x10003040).
#include "MfJpgHandler_unit.h"

#define MF_JPEG_EXCEPTION 0xE0000001

// 0x10002ac0 ?GetExtensions@MfJpgHandler@@UAEABVMfString@@XZ (original MfJpgHandler::GetExtensions)
// 0x10002b00 _$E2 (original (atexit) destructor of the local static)
const MfString &MfJpgHandler::GetExtensions()
{
    static MfString extensions(".jpg .jpeg .jpe");
    return extensions;
}

// 0x10002490 ?CanHandle@MfJpgHandler@@UAE_NW4MfImageOp@@@Z (identical to MfTgaHandler::CanHandle, folded by the linker)
bool MfJpgHandler::CanHandle(MfImageOp op)
{
    return op == MfImageRead || op == MfImageWrite;
}

static void JpgErrorExit(j_common_ptr cinfo);
static void JpgOutputMessage(j_common_ptr cinfo);

// 0x10002b10 ?Handle@MfJpgHandler@@UAE?AW4MfImageError@@W4MfImageOp@@AAUMfImageParams@@@Z (__try/__except: filters at 0x10002c0d/0x10002de2)
MfImageError MfJpgHandler::Handle(MfImageOp op, MfImageParams &params)
{
    if (op == MfImageRead) {
        FILE *file = fopen(params.path, "rb");
        if (!file)
            return MfImageCantOpen;

        m_cinfo.err = jpeg_std_error(&m_jerr);
        m_jerr.error_exit = JpgErrorExit;
        m_jerr.output_message = JpgOutputMessage;

        MfImageError result = MfImageOk;
        __try {
            jpeg_create_decompress(&m_cinfo);
            jpeg_stdio_src(&m_cinfo, file);
            jpeg_read_header(&m_cinfo, TRUE);
            jpeg_start_decompress(&m_cinfo);
            if (!Decode(file, params))
                result = MfImageBadFile;
            jpeg_finish_decompress(&m_cinfo);
        } __except (GetExceptionCode() == MF_JPEG_EXCEPTION) {
            result = MfImageBadFile;
        }
        jpeg_destroy_decompress(&m_cinfo);
        fclose(file);
        return result;
    }
    if (op == MfImageWrite) {
        // image and pixelSize are (nearly) dead but keep their stack slots: the
        // frame size and slot layout of the original need both of them
        MfImage *image = params.image;
        int format = image->m_format;
        int pixelSize = MfImage::PixelSize(format);
        int bits = format & 0xff;
        if ((format & 0xff00) != 0x800 || bits != 8)
            return MfImageBadFile;

        FILE *file = fopen(params.path, "wb");
        if (!file)
            return MfImageCantOpen;

        struct jpeg_compress_struct cinfo;
        cinfo.err = jpeg_std_error(&m_jerr);
        m_jerr.error_exit = JpgErrorExit;
        m_jerr.output_message = JpgOutputMessage;

        MfImageError result = MfImageOk;
        __try {
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, file);
            cinfo.image_width = params.image->m_width;
            cinfo.image_height = params.image->m_height;
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            jpeg_set_defaults(&cinfo);
            jpeg_start_compress(&cinfo, TRUE);
            JSAMPROW row_pointer[1];
            int row_stride = cinfo.image_width * 3;
            while (cinfo.next_scanline < cinfo.image_height) {
                row_pointer[0] = params.image->m_pixels + row_stride * cinfo.next_scanline;
                jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            jpeg_finish_compress(&cinfo);
        } __except (GetExceptionCode() == MF_JPEG_EXCEPTION) {
            result = MfImageBadFile;
        }
        jpeg_destroy_compress((j_compress_ptr)&m_cinfo);
        fclose(file);
        return result;
    }
    return MfImageUnsupported;
}

// 0x10002e70 ?JpgErrorExit@@YAXPAUjpeg_common_struct@@@Z (original (libjpeg) error_exit callback)
static void JpgErrorExit(j_common_ptr cinfo)
{
    (*cinfo->err->output_message)(cinfo);
    RaiseException(MF_JPEG_EXCEPTION, 0, 0, NULL);
}

// 0x10002e90 ?JpgOutputMessage@@YAXPAUjpeg_common_struct@@@Z (original (libjpeg) output_message callback)
static void JpgOutputMessage(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    MfString message(buffer);
    MfString context("Decode");
    MfString title("JPEG Library Error");
    MfLogger::Log(1, title, context, message);
}

// 0x10002f70 ?Decode@MfJpgHandler@@AAE_NPAU_iobuf@@AAUMfImageParams@@@Z (original MfJpgHandler::Decode)
bool MfJpgHandler::Decode(FILE *file, MfImageParams &params)
{
    params.image->InitPixels(m_cinfo.output_width, m_cinfo.output_height, 0x408, params.alignment);
    MfImage::PixelSize(0x408);
    unsigned char *dst = params.image->m_pixels;
    int rowOffset = params.image->RowOffset();
    int rowSize = m_cinfo.output_components * m_cinfo.output_width;
    JSAMPROW buffer = new unsigned char[rowSize];
    while (m_cinfo.output_scanline < m_cinfo.output_height) {
        jpeg_read_scanlines(&m_cinfo, &buffer, 1);
        memcpy(dst, buffer, rowSize);
        dst += rowOffset;
    }
    params.image->ReorderComponents(params.flags);
    delete buffer;
    return true;
}
