#pragma once
// muffin/include/Runtime/MBContainers.h: the STL containers on the engine's heap.
#include <list>
#include <map>
#include <string>
#include <vector>

#include "MBNew.h"

// The game's own code is no-alias code (/Oa), the library code in the headers above is not:
// with the flag on from here, CSBWorld's constructor (operator new), SBActivityNode's
// constructor (basic_string copy) and the MBRect copies, QueryInterface and member copies of
// CSBWorld, SimpleNode and CSBAnimButton all match; with it on for the whole unit or off,
// some of them do not. See NOTES.md.
#pragma optimize("a", on)

template <class T> class MBAllocator : public std::allocator<T> {
    typedef unsigned long SizeType;
    typedef T *PointerType;
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

template <class K, class T, class P> class MBMap : public std::map<K, T, P, MBAllocator<T> > {
};

template <class K, class T, class P> class MBMultimap : public std::multimap<K, T, P, MBAllocator<T> > {
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;
