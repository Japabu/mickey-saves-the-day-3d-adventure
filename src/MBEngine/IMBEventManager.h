// IMBEventManager.h - the engine's event broadcaster (channels of listeners).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBEventManager.
#ifndef MBENGINE_IMBEVENTMANAGER_H
#define MBENGINE_IMBEVENTMANAGER_H

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

#endif
