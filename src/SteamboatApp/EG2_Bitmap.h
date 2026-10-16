#pragma once
// EG2_Bitmap.h: a Windows device-independent bitmap read from a stream or wrapped around a .bmp in
// memory (EG2Bitmap.obj, declaration from SteamboatApp.pdb).
#include "EG2_Asset.h"
#include <windows.h>

class IMBStream;
template <class T> class _SS;

namespace EG2 {
class Bitmap : public EG2::Asset {
public:
    Bitmap();
    virtual ~Bitmap();
    virtual bool LoadFromResource(EG2::Resource *resource);
    bool LoadFromFile(_SS<char> fileName);
    bool Load(IMBStream &stream);
    void WrapMemory(unsigned char *pMemory);
    tagBITMAPINFOHEADER *GetBitmapInfo() { return mBitmapHeader; }
    tagRGBQUAD *GetRGBQuadArray() { return mRGBQuad; }
    void *GetBits() { return mBits; }
    HBITMAP CreateHBitmap();
    HPALETTE CreateHPalette();
private:
    unsigned short NumColors();
    bool mOwnPointers;                                  // +0x0008
    tagBITMAPINFOHEADER *mBitmapHeader;                 // +0x000c
    tagRGBQUAD *mRGBQuad;                               // +0x0010
    void *mBits;                                        // +0x0014
};
}
