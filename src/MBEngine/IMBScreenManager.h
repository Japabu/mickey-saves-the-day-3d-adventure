// IMBScreenManager.h - creates and finds the screens.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBScreenManager.
#ifndef MBENGINE_IMBSCREENMANAGER_H
#define MBENGINE_IMBSCREENMANAGER_H

class IMBScreen;
class MBPoint;
struct MBScreenAttributes;

class IMBScreenManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreenManager; }
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
    virtual void GetScreenByID(long, IMBScreen **) = 0;
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0;
};

#endif
