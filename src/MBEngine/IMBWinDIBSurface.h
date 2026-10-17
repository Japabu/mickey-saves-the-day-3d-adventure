// IMBWinDIBSurface.h - a surface over a Windows DIB section.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID.
#ifndef MBENGINE_IMBWINDIBSURFACE_H
#define MBENGINE_IMBWINDIBSURFACE_H

struct HBITMAP__;
struct MBPixelFormat;
struct tagBITMAPINFO;

class IMBWinDIBSurface : public IMBUnknown {
public:
    virtual long CreateEmpty(unsigned long, unsigned long, MBPixelFormat) = 0;
    virtual HBITMAP__ *GetBitmapHandle(void) = 0;
    virtual tagBITMAPINFO *GetBitmapInfo(void) = 0;
};

#endif
