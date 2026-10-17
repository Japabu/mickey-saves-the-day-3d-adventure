// IMBMemorySurface.h - a surface in plain memory.
#ifndef MBENGINE_IMBMEMORYSURFACE_H
#define MBENGINE_IMBMEMORYSURFACE_H

#include "IMBSurface.h"

struct MBPixelFormat;

class IMBMemorySurface : public IMBSurface {
public:
    virtual long CreateEmpty(unsigned long, unsigned long, MBPixelFormat) = 0;
};

#endif
