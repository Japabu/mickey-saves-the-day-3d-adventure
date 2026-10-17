// IMBObjectGroup.h - an IMBObject that holds other objects and a background.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBObjectGroup.
#ifndef MBENGINE_IMBOBJECTGROUP_H
#define MBENGINE_IMBOBJECTGROUP_H

#include "IMBObject.h"

class IMBObjectGroup : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBObjectGroup; }
    virtual void GetBackground(IMBObject **) const = 0;
    virtual void SetBackground(IMBObject *) = 0;
    virtual void AddObject(IMBObject *) = 0;
    virtual void RemoveObject(IMBObject *) = 0;
    virtual void ObjectMoved(IMBObject *) = 0;
};

#endif
