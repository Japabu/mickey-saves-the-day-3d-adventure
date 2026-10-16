// CMBEventManager.h - declarations for MBEngine.dll CMBEventManager.obj
#ifndef CMBEVENTMANAGER_H
#define CMBEVENTMANAGER_H

#include <string> // evidence: original TU has $E21/$E22 for std::ctype<unsigned short>::id
#include <map>
#include "CMBKeyboardDispatcher.h"

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
    void construct(T *p, const T &v) { new ((void *)p) T(v); }
    void destroy(T *p) { p->~T(); }
};

template <class K, class V, class Pr> class MBMultimap : public std::multimap<K, V, Pr, MBAllocator<V> > {
};

typedef MBMultimap<long, IMBEventListener *, std::less<long> > MBListenerMap;

class CMBEventManager : public IMBEventManager {
public:
    CMBEventManager(void);
    ~CMBEventManager(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void BroadcastEvent(long channel, long event, void *data);
    virtual void AddListener(long channel, IMBEventListener *listener);
    virtual void RemoveListener(long channel, IMBEventListener *listener);
    virtual void RemoveListener(IMBEventListener *listener);
protected:
    void RemoveListener(IMBEventListener *listener, MBListenerMap::iterator start, MBListenerMap::iterator end);
    MBListenerMap mListeners;
};

IMBUnknown *MakeCMBEventManager(void);

#endif
