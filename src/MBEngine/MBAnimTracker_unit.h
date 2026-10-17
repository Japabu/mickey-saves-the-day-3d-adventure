// MBAnimTracker unit declarations (MBEngine.dll)
#ifndef MBANIMTRACKER_UNIT_H
#define MBANIMTRACKER_UNIT_H

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
// own copy: an empty view: this unit declares none of the types the interface names
class IMBAnim : public IMBUnknown {
};

#include "MBAnimEventStruct.h"

#include "MBAnimTracker.h"

#endif
