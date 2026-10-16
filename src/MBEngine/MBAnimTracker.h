// MBAnimTracker unit declarations (MBEngine.dll)
#ifndef MBANIMTRACKER_H
#define MBANIMTRACKER_H

#include <string> // evidence: $E21/$E20 = <string> baseline 19/18 + 2 for the map's _Nil/_Nilrefs
#include <map>
#include "CMBKeyboardDispatcher.h" // MBGUID, MBIID_* (same .rdata order), IMBEventManager/Listener, MBComPtr

// No construct/destroy: the original instantiates std::_Construct for the map's pair
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

// really IMBObject-derived; only the IMBUnknown part is used here
class IMBAnim : public IMBUnknown {
};

struct MBAnimEventStruct {
    IMBAnim *anim;
    unsigned long id;
    long sequence;
    long frame;
};

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
