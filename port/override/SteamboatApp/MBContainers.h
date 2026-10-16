// PORT override: MBVector with pointer iterators (VC6 vector iterators are T *); also carries port/rewrite.py's MBAllocator and MBMap changes, since an override replaces the rewritten copy.
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
    MBAllocator() {} template <class U> MBAllocator(const MBAllocator<U> &) {} template <class U> struct rebind { typedef MBAllocator<U> other; }; T *allocate(size_t n) { return (T *)MBmalloc_release(n * sizeof(T)); } };

// The game walks MBVectors with plain pointers (VC6's vector iterator is T *): the port's
// MBVector hands out pointers too and takes them back in insert and erase.
template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
    typedef std::vector<T, MBAllocator<T> > Base;
    typename Base::iterator at(T *p) { return Base::begin() + (p - begin()); }
public:
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef typename Base::size_type size_type;
    T *begin() { return Base::empty() ? 0 : Base::data(); }
    T *end() { return Base::empty() ? 0 : Base::data() + Base::size(); }
    const T *begin() const { return Base::empty() ? 0 : Base::data(); }
    const T *end() const { return Base::empty() ? 0 : Base::data() + Base::size(); }
    T *erase(T *p) { size_type i = p - begin(); Base::erase(at(p)); return begin() + i; }
    T *erase(T *first, T *last)
    {
        size_type i = first - begin();
        Base::erase(at(first), at(last));
        return begin() + i;
    }
    T *insert(T *p, const T &v) { size_type i = p - begin(); Base::insert(at(p), v); return begin() + i; }
    void insert(T *p, size_type n, const T &v) { Base::insert(at(p), n, v); }
    template <class I> void insert(T *p, I first, I last) { Base::insert(at(p), first, last); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

template <class K, class T, class P> class MBMap : public std::map<K, T, P, MBAllocator<std::pair<const K, T> > > {
};

template <class K, class T, class P> class MBMultimap : public std::multimap<K, T, P, MBAllocator<std::pair<const K, T> > > {
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;
