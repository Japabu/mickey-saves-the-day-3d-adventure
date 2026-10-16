// MBMovieTracker unit declarations (MBEngine.dll)
#ifndef MBMOVIETRACKER_H
#define MBMOVIETRACKER_H

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

// really IMBResource-derived; only the IMBUnknown part is used here
class IMBMovie : public IMBUnknown {
};

struct MBMovieEventStruct {
    IMBMovie *movie;
    unsigned long id;
};

class MBMovieTracker : public IMBEventListener {
public:
    MBMovieTracker(void);
    ~MBMovieTracker(void);
    void SetupTracker(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual IMBMovie *FindMovie(unsigned long id);
protected:
    virtual void AddMovie(IMBMovie *movie, unsigned long id);
    virtual void RemoveMovie(IMBMovie *movie, unsigned long id);
    typedef MBMap<unsigned long, IMBMovie *, std::less<unsigned long> > MovieList;
    MovieList mMovieList;
    bool mAlreadyListening;
};

#endif
