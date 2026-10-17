// IMBViewManager.h - the list of views.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBViewManager.
#ifndef MBENGINE_IMBVIEWMANAGER_H
#define MBENGINE_IMBVIEWMANAGER_H

class IMBScreen;
class IMBView;
class MBPoint;

class IMBViewManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBViewManager; }
    virtual void AddView(IMBView *) = 0;
    virtual void RemoveView(IMBView *) = 0;
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0;
};

#endif
