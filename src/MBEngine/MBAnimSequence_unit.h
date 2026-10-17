// MBAnimSequence unit declarations (MBEngine.dll)
#ifndef MBANIMSEQUENCE_UNIT_H
#define MBANIMSEQUENCE_UNIT_H

// MBAnimFrame declarations copied from MBAnimFrame.h, except MBAllocator: the original
// has no construct/destroy (PDB lists none; AddFrame calls std::_Construct)

#include <string>
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long __cdecl MBHashString(const char *);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

#include "MBPointStruct.h"

#include "../MBEngine/MBPoint.h"

class IMBScriptData;

#include "MBFrameProperty.h"

#include "MBAnimFrame.h"


#ifndef MBVECTOR_DEFINED
#define MBVECTOR_DEFINED
template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};
#endif

#include "MBSequenceProperty.h"

#include "MBAnimSequence.h"

#endif
