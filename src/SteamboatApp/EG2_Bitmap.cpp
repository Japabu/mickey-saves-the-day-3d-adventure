#include "EG2_Bitmap.h"
#include "MBInterfaces.h"
#include "StdString.h"
#include <stdlib.h>
#include <locale>

// library code: no /Oa (MBInterfaces.h turns it on for the game code)
#pragma optimize("a", off)

static const MBGUID MBIID_CMBWinFile = { 0x89b39ce0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

namespace EG2 {

// 0x101127f0 ??0Bitmap@EG2@@QAE@XZ
Bitmap::Bitmap() : mOwnPointers(true), mBitmapHeader(0), mRGBQuad(0), mBits(0)
{
}

// 0x10112850 ??1Bitmap@EG2@@UAE@XZ
Bitmap::~Bitmap()
{
    if (mOwnPointers) {
        free(mBitmapHeader);
        free(mBits);
    }
}

// 0x101128a0 ?LoadFromFile@Bitmap@EG2@@QAE_NV?$_SS@D@@@Z
bool Bitmap::LoadFromFile(_SS<char> fileName)
{
    IMBUnknown *fileObject;
    IMBFile *file;
    IMBUnknown *fileSpecObject;
    IMBFileSpec *fileSpec;
    IMBStream *stream;
    MBComCreateInstance(0, MBIID_CMBWinFile, &fileObject);
    fileObject->QueryInterface(MBIID_IMBFile, (void **)&file);
    MBComCreateInstance(0, MBIID_IMBFileSpec, &fileSpecObject);
    fileSpecObject->QueryInterface(MBIID_IMBFileSpec, (void **)&fileSpec);
    fileSpec->SetPath(fileName.c_str());
    file->Open(fileSpec, 1);
    file->QueryInterface(MBIID_IMBStream, (void **)&stream);
    bool result = Load(*stream);
    file->Close();
    fileObject->Release();
    file->Release();
    fileSpecObject->Release();
    fileSpec->Release();
    stream->Release();
    return result;
}

// EQUIV 0x101129d0 ?WrapMemory@Bitmap@EG2@@QAEXPAE@Z: reviewed: the original reloads mBitmapHeader for the colour table and stores mBits inside both branches
void Bitmap::WrapMemory(unsigned char *pMemory)
{
    // a .bmp file image: file header, info header (and colours), bits
    mBitmapHeader = (BITMAPINFOHEADER *)(pMemory + sizeof(BITMAPFILEHEADER));
    unsigned long nInfoSize = ((BITMAPFILEHEADER *)pMemory)->bfOffBits - sizeof(BITMAPFILEHEADER);
    mBits = (unsigned char *)mBitmapHeader + nInfoSize;
    if (nInfoSize > sizeof(BITMAPINFOHEADER))
        mRGBQuad = (RGBQUAD *)((unsigned char *)mBitmapHeader + sizeof(BITMAPINFOHEADER));
    else
        mRGBQuad = 0;
    mOwnPointers = false;
}

// 0x10112a10 ?CreateHBitmap@Bitmap@EG2@@QAEPAUHBITMAP__@@XZ
HBITMAP Bitmap::CreateHBitmap()
{
    HDC hdc = GetWindowDC(GetDesktopWindow());
    HBITMAP hBitmap = CreateDIBitmap(hdc, mBitmapHeader, CBM_INIT, mBits, (BITMAPINFO *)mBitmapHeader, DIB_RGB_COLORS);
    ReleaseDC(GetDesktopWindow(), hdc);
    return hBitmap;
}

// 0x10112a50 ?CreateHPalette@Bitmap@EG2@@QAEPAUHPALETTE__@@XZ
HPALETTE Bitmap::CreateHPalette()
{
    BITMAPINFO *pInfo = (BITMAPINFO *)mBitmapHeader;
    unsigned short nColors = NumColors();
    HPALETTE hPalette = 0;
    if (nColors) {
        LOGPALETTE *pPalette = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));
        pPalette->palVersion = 0x300;
        pPalette->palNumEntries = nColors;
        for (int i = 0; i < nColors; i++) {
            pPalette->palPalEntry[i].peRed = pInfo->bmiColors[i].rgbRed;
            pPalette->palPalEntry[i].peGreen = pInfo->bmiColors[i].rgbGreen;
            pPalette->palPalEntry[i].peBlue = pInfo->bmiColors[i].rgbBlue;
            pPalette->palPalEntry[i].peFlags = 0;
        }
        hPalette = CreatePalette(pPalette);
        free(pPalette);
    }
    return hPalette;
}

// 0x10112ad0 ?Load@Bitmap@EG2@@QAE_NAAVIMBStream@@@Z
bool Bitmap::Load(IMBStream &stream)
{
    BITMAPFILEHEADER fileHeader;
    unsigned long nSize = sizeof(BITMAPFILEHEADER);
    stream.Read(&fileHeader, nSize);
    unsigned long nInfoSize = fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    mBitmapHeader = (BITMAPINFOHEADER *)malloc(nInfoSize);
    stream.Read(mBitmapHeader, nInfoSize);
    if (nInfoSize > sizeof(BITMAPINFOHEADER))
        mRGBQuad = (RGBQUAD *)((unsigned char *)mBitmapHeader + sizeof(BITMAPINFOHEADER));
    else
        mRGBQuad = 0;
    unsigned long nBitsSize = fileHeader.bfSize - fileHeader.bfOffBits;
    mBits = malloc(nBitsSize);
    stream.Read(mBits, nBitsSize);
    return true;
}

// 0x10112b70 ?NumColors@Bitmap@EG2@@AAEGXZ
unsigned short Bitmap::NumColors()
{
    if (mBitmapHeader->biClrUsed)
        return (unsigned short)mBitmapHeader->biClrUsed;
    switch (mBitmapHeader->biBitCount) {
    case 1:
        return 2;
    case 4:
        return 16;
    case 8:
        return 256;
    }
    return 0;
}

}

// Template and inline instances this unit also emits:
// 0x10112820 ??_GBitmap@EG2@@UAEPAXI@Z
// static initializers of the locale ids (<locale>; the original has no names for them)
// 0x10112bb0 _$E24
// 0x10112be0 _$E20
// 0x10112bf0 _$E23
