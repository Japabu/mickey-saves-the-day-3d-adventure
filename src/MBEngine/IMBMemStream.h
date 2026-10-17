// IMBMemStream.h - a stream over an IMBMemoryBlock.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBComPtr, MBGUID, MBIID_IMBMemStream.
#ifndef MBENGINE_IMBMEMSTREAM_H
#define MBENGINE_IMBMEMSTREAM_H

class IMBMemoryBlock;

class IMBMemStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMemStream; }
    virtual MBComPtr<IMBMemoryBlock> GetMemoryBlock(void) = 0;
    virtual void SetMemoryBlock(const MBComPtr<IMBMemoryBlock> &) = 0;
};

#endif
