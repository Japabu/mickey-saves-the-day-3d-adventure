// IMBScreen.h - a screen: surfaces, back buffer, flipping.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBScreen.
#ifndef MBENGINE_IMBSCREEN_H
#define MBENGINE_IMBSCREEN_H

#include "MBScreenState.h"

class IMBSurface;
class MBPoint;
class MBRect;

class IMBScreen : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreen; }
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual bool CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
    virtual void Flip(void) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual MBRect GetGlobalBounds(void) = 0;
    virtual MBPoint GlobalToScreenPoint(const MBPoint &) = 0;
};

#endif
