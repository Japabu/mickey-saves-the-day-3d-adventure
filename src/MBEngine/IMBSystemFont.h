// IMBSystemFont.h - a font made from a system (Windows) font.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBSystemFont.
#ifndef MBENGINE_IMBSYSTEMFONT_H
#define MBENGINE_IMBSYSTEMFONT_H

#include "IMBFont.h"

class IMBSurface;
class MBColor;

class IMBSystemFont : public IMBFont {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSystemFont; }
    virtual long LoadFont(IMBSurface *, const char *, short, const MBColor &, unsigned long) = 0; // +0x24
};

#endif
