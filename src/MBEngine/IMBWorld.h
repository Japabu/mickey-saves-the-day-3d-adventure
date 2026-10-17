// IMBWorld.h - the world: a root object group with bounds.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBWorld.
#ifndef MBENGINE_IMBWORLD_H
#define MBENGINE_IMBWORLD_H

#include "IMBObjectGroup.h"

class IMBSurface;
class MBRect;

class IMBWorld : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWorld; }
    virtual MBRect GetBounds(void) const = 0;
    virtual void SetBounds(const MBRect &) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0;
    virtual IMBObjectGroup &GetRoot(void) = 0;
    virtual const IMBObjectGroup &GetConstRoot(void) const = 0;
    void AddObject(IMBObject *object) { GetRoot().AddObject(object); }
    void RemoveObject(IMBObject *object) { GetRoot().RemoveObject(object); }
    void GetBackground(IMBObject **background) const { GetConstRoot().GetBackground(background); }
    void SetBackground(IMBObject *background) { GetRoot().SetBackground(background); }
};

#endif
