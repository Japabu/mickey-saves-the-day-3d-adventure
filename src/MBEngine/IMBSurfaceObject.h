// IMBSurfaceObject.h - an IMBObject that draws a surface.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBSurfaceObject.
#ifndef MBENGINE_IMBSURFACEOBJECT_H
#define MBENGINE_IMBSURFACEOBJECT_H

#include "IMBObject.h"

class IMBSurface;

class IMBSurfaceObject : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurfaceObject; }
    virtual void GetSurface(IMBSurface **) = 0;
    virtual void SetSurface(IMBSurface *) = 0;
};

#endif
