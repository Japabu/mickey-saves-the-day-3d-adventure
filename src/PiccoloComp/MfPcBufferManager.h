#pragma once
#include "MfCallBack.h"
class MfDblList;
#include "MfPcBuffer.h"

// A free list of source buffers (8 bytes).
class MfPcBufferManager {
public:
    // Deletes the buffers when the free list is cleared.
    class MfPcBufferManagerFreeListCB : public MfCallBack {
    public:
        virtual int CallBack(void *p) { delete (MfPcBuffer *)p; return 1; }
    };

    MfPcBufferManager();
    virtual ~MfPcBufferManager();
    MfPcBuffer *GetBuffer();
    void ReleaseBuffer(MfPcBuffer *buffer);
    static void Init();

    static MfPcBufferManager *s_instance;

    MfDblList *mFreeList;       // +0x04
};
