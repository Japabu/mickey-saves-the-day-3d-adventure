// IMBBitmapFont.h - a font loaded from a bitmap font resource.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBBitmapFont.
#ifndef MBENGINE_IMBBITMAPFONT_H
#define MBENGINE_IMBBITMAPFONT_H

#include "IMBFont.h"

class IMBStream;

class IMBBitmapFont : public IMBFont {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBBitmapFont; }
    virtual long LoadFont(unsigned long, IMBStream *) = 0;
    virtual long LoadCharacterMap(unsigned long, IMBStream *) = 0;
};

#endif
