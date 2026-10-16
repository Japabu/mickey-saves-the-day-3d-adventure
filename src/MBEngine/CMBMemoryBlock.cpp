// MBEngine.dll .\MBEngineRelease\CMBMemoryBlock.obj
#include "CMBMemoryBlock.h"

// 0x1004b440 ?MakeCMBMemoryBlock@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBMemoryBlock(void)
{
    IMBUnknown *p = new CMBMemoryBlock;
    if (p)
        p->AddRef();
    return p;
}

// 0x1004b470 ??0CMBMemoryBlock@@QAE@XZ
CMBMemoryBlock::CMBMemoryBlock(void)
    : mPtr(0), mSize(0)
{
    _mRefCnt = 0;
}

// 0x1004b490 ?QueryInterface@CMBMemoryBlock@@UAEJABUMBGUID@@PAPAX@Z
long CMBMemoryBlock::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBMemoryBlock)
        p = (IMBMemoryBlock *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1004b510 ?AddRef@CMBMemoryBlock@@UAEKXZ
unsigned long CMBMemoryBlock::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1004b520 ?Release@CMBMemoryBlock@@UAEKXZ
unsigned long CMBMemoryBlock::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1004b550 ??1CMBMemoryBlock@@QAE@XZ
CMBMemoryBlock::~CMBMemoryBlock(void)
{
    if (mPtr)
        MBfree_release(mPtr);
}

// 0x1004b570 ?Grow@CMBMemoryBlock@@UAE_NK@Z
bool CMBMemoryBlock::Grow(unsigned long newSize)
{
    if (!mPtr)
        return Allocate(newSize);
    unsigned char *newPtr = (unsigned char *)MBmalloc_release(newSize);
    if (!newPtr)
        return false;
    MBmemcpy(newPtr, mPtr, MBmin(mSize, newSize));
    if (mPtr)
        MBfree_release(mPtr);
    mSize = newSize;
    mPtr = newPtr;
    return true;
}

// 0x1004b5f0 ?Allocate@CMBMemoryBlock@@UAE_NK@Z
bool CMBMemoryBlock::Allocate(unsigned long size)
{
    if (mPtr)
        return false;
    mPtr = (unsigned char *)MBmalloc_release(size);
    if (mPtr)
        mSize = size;
    return mPtr != 0;
}

// 0x1004b630 ?GetData@CMBMemoryBlock@@UAEPAEXZ
unsigned char *CMBMemoryBlock::GetData(void)
{
    return mPtr;
}

// 0x1004b640 ?GetSize@CMBMemoryBlock@@UAEKXZ
unsigned long CMBMemoryBlock::GetSize(void)
{
    return mSize;
}
