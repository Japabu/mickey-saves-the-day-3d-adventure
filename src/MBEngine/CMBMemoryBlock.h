// CMBMemoryBlock.h - declarations for MBEngine.dll CMBMemoryBlock.obj
#ifndef CMBMEMORYBLOCK_H
#define CMBMEMORYBLOCK_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) void *MBmemcpy(void *dst, const void *src, size_t n);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBMemoryBlock =
    { 0x7944d1b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

template <class T> inline const T &MBmin(const T &a, const T &b) { return a < b ? a : b; }


#include "IMBMemoryBlock.h"

class CMBMemoryBlock : public IMBMemoryBlock {
public:
    CMBMemoryBlock(void);
    ~CMBMemoryBlock(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool Allocate(unsigned long size);
    virtual bool Grow(unsigned long newSize);
    virtual unsigned char *GetData(void);
    virtual unsigned long GetSize(void);
private:
    unsigned char *mPtr;
    unsigned long mSize;
};

IMBUnknown *MakeCMBMemoryBlock(void);

#endif
