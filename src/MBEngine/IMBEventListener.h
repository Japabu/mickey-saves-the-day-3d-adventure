// IMBEventListener.h - receives the engine's broadcast events.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBEventListener.
#ifndef MBENGINE_IMBEVENTLISTENER_H
#define MBENGINE_IMBEVENTLISTENER_H

class IMBEventListener : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventListener; }
    virtual void ListenToEvent(long, long, void *) = 0;
};

#endif
