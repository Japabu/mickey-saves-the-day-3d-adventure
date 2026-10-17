// IMBWinDC.h - a surface that hands out a Windows device context.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBWinDC.
#ifndef MBENGINE_IMBWINDC_H
#define MBENGINE_IMBWINDC_H

struct HDC__;

class IMBWinDC : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWinDC; }
    virtual long GetDC(HDC__ **, unsigned long) = 0;
    virtual long ReleaseDC(HDC__ *) = 0;
};

#endif
