// IMBDirectDrawSurface.h - a surface over a DirectDraw surface.
#ifndef MBENGINE_IMBDIRECTDRAWSURFACE_H
#define MBENGINE_IMBDIRECTDRAWSURFACE_H

#include "IMBSurface.h"

struct IDirectDrawSurface7;

class IMBDirectDrawSurface : public IMBSurface {
public:
    virtual void SetSurface(IDirectDrawSurface7 *) = 0;
    virtual void GetSurface(IDirectDrawSurface7 **) = 0;
    virtual void SetSurfaceCopy(IDirectDrawSurface7 *) = 0;
    virtual void GetSurfaceCopy(IDirectDrawSurface7 **) = 0;
    virtual void QuerySurfaceInfo(void) = 0;
};

#endif
