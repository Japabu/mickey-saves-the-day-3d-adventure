// IMBDirectXScreen.h - a DirectDraw/Direct3D screen.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBDDEnumCallback, MBGUID, MBIID_IMBDirectXScreen.
#ifndef MBENGINE_IMBDIRECTXSCREEN_H
#define MBENGINE_IMBDIRECTXSCREEN_H

#include "IMBWinScreen.h"

struct IDirectDraw7;
struct IDirectDrawSurface7;
struct _GUID;
struct tagRECT;

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
    virtual bool IsFullscreen(void) = 0;
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback) = 0;
    virtual IDirectDraw7 *GetDirectDraw7(void) = 0;
    virtual _GUID *GetDeviceGUID(void) = 0;
    virtual IDirectDrawSurface7 *GetBackBuffer(void) = 0;
    virtual IDirectDrawSurface7 *GetFrontBuffer(void) = 0;
    virtual bool IsInGDIMode(void) = 0;
    virtual void GDIModeUpdateRect(tagRECT &) = 0;
};

#endif
