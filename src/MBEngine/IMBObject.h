// IMBObject.h - an object placed in a world (position, bounds, drawing, parent group).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBObject.
#ifndef MBENGINE_IMBOBJECT_H
#define MBENGINE_IMBOBJECT_H

class IMBObjectGroup;
class IMBSurface;
class MBPoint3;
class MBRect;

class IMBObject : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBObject; }
    virtual MBPoint3 GetOffset(void) const = 0;
    virtual MBPoint3 GetPosition(void) const = 0;
    virtual void Place(const MBPoint3 &) = 0;
    virtual void Move(const MBPoint3 &) = 0;
    virtual MBRect GetBounds(void) const = 0;
    virtual bool IsVisible(void) const = 0;
    virtual long Show(void) = 0;
    virtual long Hide(void) = 0;
    virtual bool IsDirty(void) const = 0;
    virtual void SetDirty(void) = 0;
    virtual void ClearDirty(void) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) const = 0;
    virtual void GetParent(IMBObjectGroup **) const = 0;
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *) = 0;
};

#endif
