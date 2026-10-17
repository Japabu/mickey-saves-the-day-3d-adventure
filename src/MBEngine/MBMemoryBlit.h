// MBMemoryBlit.h - declarations for MBEngine.dll MBMemoryBlit.obj: blits and fills between raw pixel buffers
#ifndef MBMEMORYBLIT_H
#define MBMEMORYBLIT_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) void *MBmemcpy(void *dst, const void *src, size_t n);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// PDB ldata order in MBMemoryBlit.obj (0x10090100..0x10090120)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

#include "../MBEngine/MBColor.h"

const MBColor kWhite(255, 255, 255, 0);
const MBColor kBlack(0, 0, 0, 0);
const MBColor kRed(255, 0, 0, 0);
const MBColor kGreen(0, 255, 0, 0);
const MBColor kBlue(0, 0, 255, 0);
const MBColor kYellow(255, 255, 0, 0);
const MBColor kCyan(0, 255, 255, 0);
const MBColor kMagenta(255, 0, 255, 0);
const MBColor kGrey25(64, 64, 64, 0);
const MBColor kGrey50(128, 128, 128, 0);
const MBColor kGrey75(192, 192, 192, 0);

// own copy: a plain struct without constructors
class MBRect {
public:
    long left;
    long top;
    long right;
    long bottom;
};

bool MBBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
            MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth);
bool MBBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                 MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                 unsigned long trans_value);
bool MBFill(void *dest, unsigned long width, unsigned long height, unsigned long pitch,
            unsigned long depth, unsigned long fill_value);
bool MBStretchBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                   MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth);
bool MBStretchBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                        MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                        unsigned long trans_value);

#endif
