// MfTgaHandler: Truevision TGA reader/writer of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFTGAHANDLER_H
#define MFTGAHANDLER_H

#include <windows.h>
#include <stdio.h>
#include "MfImageHandler.h"

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
