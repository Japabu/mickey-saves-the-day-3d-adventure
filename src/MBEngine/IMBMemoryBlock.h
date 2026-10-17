// IMBMemoryBlock.h - a growable block of memory.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBMemoryBlock.
#ifndef MBENGINE_IMBMEMORYBLOCK_H
#define MBENGINE_IMBMEMORYBLOCK_H

class IMBMemoryBlock : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMemoryBlock; }
    virtual bool Allocate(unsigned long) = 0;
    virtual bool Grow(unsigned long) = 0;
    virtual unsigned char *GetData(void) = 0;
    virtual unsigned long GetSize(void) = 0;
};

#endif
