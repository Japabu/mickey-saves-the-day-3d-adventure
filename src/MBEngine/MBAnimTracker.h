// MBAnimTracker.h - tracks the running animations by ID for the script events.
// Declared first by the includer (still one copy per unit in wave 1): MBGUID, MBMap.
#ifndef MBENGINE_MBANIMTRACKER_H
#define MBENGINE_MBANIMTRACKER_H

#include "IMBEventListener.h"

class IMBAnim;

class MBAnimTracker : public IMBEventListener {
public:
    MBAnimTracker(void);
    ~MBAnimTracker(void);
    void SetupTracker(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual IMBAnim *FindAnimation(unsigned long id);
protected:
    virtual void AddAnimation(IMBAnim *anim, unsigned long id);
    virtual void RemoveAnimation(IMBAnim *anim, unsigned long id);
    typedef MBMap<unsigned long, IMBAnim *, std::less<unsigned long> > AnimList;
    AnimList mAnimList;
    bool mAlreadyListening;
};

#endif
