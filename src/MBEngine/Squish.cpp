// Squish unit (MBEngine.dll): SquishPixel conversions and SquishImageDescriptor
#include "Squish.h"

// 0x1005dd20 ?GetPixel@SquishPixel@@QBEKK@Z
unsigned long SquishPixel::GetPixel(unsigned long format) const
{
    unsigned long result = 0;
    switch (format) {
    case kSquishPixFmtIndexed:
        result = Pixel.pixel[0];
        break;
    case kSquishPixFmtRGBA5551:
        result = ((Pixel.pixel[2] >> 3) << 11) | ((Pixel.pixel[1] >> 3) << 6) | (((unsigned long)Pixel.pixel[0] >> 3) << 1) | (Pixel.pixel[3] >> 7);
        break;
    case kSquishPixFmtRGB565:
        result = ((Pixel.pixel[2] >> 3) << 11) | ((Pixel.pixel[1] >> 2) << 5) | (Pixel.pixel[0] >> 3);
        break;
    case kSquishPixFmtBGRA5551:
        result = ((Pixel.pixel[0] >> 3) << 11) | ((Pixel.pixel[1] >> 3) << 6) | (((unsigned long)Pixel.pixel[2] >> 3) << 1) | (Pixel.pixel[3] >> 7);
        break;
    case kSquishPixFmtBGR565:
        result = ((Pixel.pixel[0] >> 3) << 11) | ((Pixel.pixel[1] >> 2) << 5) | (Pixel.pixel[2] >> 3);
        break;
    case kSquishPixFmtARGB1555:
        result = ((Pixel.pixel[2] >> 3) << 10) | ((Pixel.pixel[3] >> 7) << 11) | ((Pixel.pixel[1] >> 3) << 5) | (Pixel.pixel[0] >> 3);
        break;
    case kSquishPixFmtARGB8888:
        result = (Pixel.pixel[3] << 24) | (Pixel.pixel[2] << 16) | (Pixel.pixel[1] << 8) | Pixel.pixel[0];
        break;
    case kSquishPixFmtBGRA8888:
        result = Pixel.bgra32;
        break;
    }
    return result;
}

// EQUIV 0x1005dee0 ?SetPixel@SquishPixel@@QAEXKK@Z: reviewed: the ARGB1555 case inserts (pixel >> 7) & 7 into the low bits of the byte with the operations scheduled in another order (original: (s ^ v) after the shift, VC6: v ^ s first; both give (s & 7) | (v & ~7)); the rest is the same. Was: ARGB1555 case: shift of pixel>>7 scheduled before the [0] store in original; 3 compiles (91% similar)
void SquishPixel::SetPixel(unsigned long pixel, unsigned long format)
{
    switch (format) {
    case kSquishPixFmtIndexed:
        Pixel.pixel[0] = (unsigned char)pixel;
        break;
    case kSquishPixFmtRGBA5551:
        Pixel.pixel[0] = ((pixel & 0x3e) << 2) + ((pixel >> 3) & 7);
        Pixel.pixel[1] = ((pixel >> 3) & 0xf8) | ((pixel >> 8) & 7);
        Pixel.pixel[2] = ((pixel >> 8) & 0xf8) | ((pixel >> 13) & 7);
        Pixel.pixel[3] = pixel & 1;
        break;
    case kSquishPixFmtRGB565:
        Pixel.pixel[3] = 0;
        Pixel.pixel[0] = ((pixel & 0x1f) << 3) + (pixel & 7);
        Pixel.pixel[1] = ((pixel >> 3) & 0xfc) | ((pixel >> 9) & 3);
        Pixel.pixel[2] = ((pixel >> 8) & 0xf8) | ((pixel >> 13) & 7);
        break;
    case kSquishPixFmtBGRA5551:
        Pixel.pixel[2] = ((pixel & 0x3e) << 2) + ((pixel >> 3) & 7);
        Pixel.pixel[1] = ((pixel >> 3) & 0xf8) | ((pixel >> 8) & 7);
        Pixel.pixel[0] = ((pixel >> 8) & 0xf8) | ((pixel >> 13) & 7);
        Pixel.pixel[3] = pixel & 1;
        break;
    case kSquishPixFmtBGR565:
        Pixel.pixel[3] = 0;
        Pixel.pixel[2] = ((pixel & 0x1f) << 3) + (pixel & 7);
        Pixel.pixel[1] = ((pixel >> 3) & 0xfc) | ((pixel >> 9) & 3);
        Pixel.pixel[0] = ((pixel >> 8) & 0xf8) | ((pixel >> 13) & 7);
        break;
    case kSquishPixFmtARGB1555:
        Pixel.pixel[0] = ((pixel & 0x1f) << 3) + ((pixel >> 2) & 7);
        Pixel.pixel[1] = ((pixel >> 7) & 7) | ((pixel >> 2) & 0xf8);
        Pixel.pixel[2] = ((pixel >> 7) & 0xf8) | ((pixel >> 12) & 7);
        Pixel.pixel[3] = (pixel >> 15) & 1;
        break;
    case kSquishPixFmtARGB8888:
        Pixel.pixel[3] = (unsigned char)(pixel >> 24);
        Pixel.pixel[2] = (unsigned char)(pixel >> 16);
        Pixel.pixel[1] = (unsigned char)(pixel >> 8);
        Pixel.pixel[0] = (unsigned char)pixel;
        break;
    case kSquishPixFmtBGRA8888:
        Pixel.bgra32 = pixel;
        break;
    }
}

// 0x1005e160 ??0SquishImageDescriptor@@QAE@XZ
SquishImageDescriptor::SquishImageDescriptor(void)
    : width(0), height(0), depth(0), rowbytes(0), datasize(0), flags(0),
      transColor(0xffffffff), nativeTransColor(0xffffffff), palette(0)
{
    SetCompFmt(0x200);
}

// 0x1005e1a0 ??1SquishImageDescriptor@@QAE@XZ
SquishImageDescriptor::~SquishImageDescriptor(void)
{
    Reset();
}

// 0x1005e1b0 ??0SquishImageDescriptor@@QAE@ABV0@@Z
SquishImageDescriptor::SquishImageDescriptor(const SquishImageDescriptor &rhs)
{
    MBmemcpy(this, &rhs, sizeof(SquishImageDescriptor));
    if (palette)
        palette->AddRef();
}

// 0x1005e1e0 ??4SquishImageDescriptor@@QAEAAV0@ABV0@@Z
SquishImageDescriptor &SquishImageDescriptor::operator=(const SquishImageDescriptor &rhs)
{
    if (&rhs != this) {
        Reset();
        MBmemcpy(this, &rhs, sizeof(SquishImageDescriptor));
        if (palette)
            palette->AddRef();
    }
    return *this;
}

// 0x1005e210 ?Reset@SquishImageDescriptor@@QAEXXZ
void SquishImageDescriptor::Reset(void)
{
    SetTransOff();
    SetPalette(0);
    flags = 0;
    datasize = 0;
    rowbytes = 0;
    depth = 0;
    height = 0;
    width = 0;
}

// 0x1005e240 ?SetPalette@SquishImageDescriptor@@QAEXPAVSquishPalette@@@Z
void SquishImageDescriptor::SetPalette(SquishPalette *pal)
{
    if (palette)
        palette->Release();
    palette = pal;
    if (pal) {
        flags |= kSquishFlagPalette;
        pal->AddRef();
    } else {
        flags &= ~kSquishFlagPalette;
    }
    if (TransEnabled())
        SetTransColor(transColor);
}

// 0x1005e290 ?SetPixelFmt@SquishImageDescriptor@@QAEXK@Z
void SquishImageDescriptor::SetPixelFmt(unsigned long pixfmt)
{
    flags = (flags & ~kSquishPixFmtMask) | pixfmt;
    if (TransEnabled())
        SetTransColor(transColor);
}

// 0x1005e2c0 ?SetTransColor@SquishImageDescriptor@@QAEXUSquishPixel@@@Z
void SquishImageDescriptor::SetTransColor(SquishPixel trans)
{
    transColor = trans;
    if (palette && GetPixelFmt() == kSquishPixFmtIndexed)
        nativeTransColor = palette->GetIndex(trans);
    else
        nativeTransColor = transColor.GetPixel(GetPixelFmt());
    flags |= kSquishFlagTrans;
}

// 0x1005e320 ?SetTransOff@SquishImageDescriptor@@QAEXXZ
void SquishImageDescriptor::SetTransOff(void)
{
    transColor.Pixel.bgra32 = 0xffffffff;
    nativeTransColor = 0xffffffff;
    flags &= ~kSquishFlagTrans;
}

// 0x1005e340 ?StreamIn@SquishImageDescriptor@@QAE_NPAVSquishFile@@@Z
bool SquishImageDescriptor::StreamIn(SquishFile *file)
{
    Reset();
    if (!file->ReadBytes(&width, 2))
        return false;
    if (!file->ReadBytes(&height, 2))
        return false;
    if (!file->ReadBytes(&depth, 2))
        return false;
    if (!file->ReadBytes(&rowbytes, 2))
        return false;
    if (!file->ReadBytes(&datasize, 4))
        return false;
    if (!file->ReadBytes(&transColor.Pixel.pixel[0], 1))
        return false;
    if (!file->ReadBytes(&transColor.Pixel.pixel[1], 1))
        return false;
    if (!file->ReadBytes(&transColor.Pixel.pixel[2], 1))
        return false;
    if (!file->ReadBytes(&transColor.Pixel.pixel[3], 1))
        return false;
    if (!file->ReadBytes(&flags, 4))
        return false;
    if (HasPalette()) {
        SetPalette(SquishPalette::StreamIn(file));
        if (!palette)
            return false;
    }
    if (TransEnabled())
        SetTransColor(transColor);
    return true;
}

// 0x1005e490 ?StreamOut@SquishImageDescriptor@@QBE_NPAVSquishFile@@@Z
bool SquishImageDescriptor::StreamOut(SquishFile *file) const
{
    if (!file->WriteBytes(&width, 2))
        return false;
    if (!file->WriteBytes(&height, 2))
        return false;
    if (!file->WriteBytes(&depth, 2))
        return false;
    if (!file->WriteBytes(&rowbytes, 2))
        return false;
    if (!file->WriteBytes(&datasize, 4))
        return false;
    if (!file->WriteBytes(&transColor.Pixel.pixel[0], 1))
        return false;
    if (!file->WriteBytes(&transColor.Pixel.pixel[1], 1))
        return false;
    if (!file->WriteBytes(&transColor.Pixel.pixel[2], 1))
        return false;
    if (!file->WriteBytes(&transColor.Pixel.pixel[3], 1))
        return false;
    if (!file->WriteBytes(&flags, 4))
        return false;
    if (HasPalette()) {
        if (!palette->StreamOut(file))
            return false;
    }
    return true;
}


// Matched by this code:
// 0x1005e5a0 _$E19
// 0x1005e5c0 _$E18
