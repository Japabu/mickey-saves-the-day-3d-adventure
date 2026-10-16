// PiccoloComp.dll MfPcBufferManager.cpp (0x10002e90-0x10003080): a free list of
// source buffers.
#include "MfPcBufferManager.h"
#include "MfPcBuffer.h"
#include "MfPcCompiler.h"
#include "MfDblList.h"

MfPcBufferManager *MfPcBufferManager::s_instance;

// 0x10002e90 ??0MfPcBufferManager@@QAE@XZ
MfPcBufferManager::MfPcBufferManager()
{
    mFreeList = new MfDblList;
}

// 0x10002ed0 ??_GMfDblList@@UAEPAXI@Z (inline from mfutil header)

// 0x10002ef0 ??_GMfPcBufferManager@@UAEPAXI@Z (compiler-generated)

// 0x10002f10 ??1MfPcBufferManager@@UAE@XZ
MfPcBufferManager::~MfPcBufferManager()
{
    MfPcBufferManagerFreeListCB cb;
    mFreeList->Clear(cb);
    delete mFreeList;
}

// 0x10002f70 ??_GMfPcBufferManagerFreeListCB@MfPcBufferManager@@UAEPAXI@Z (compiler-generated)

// 0x10002f90 ?GetBuffer@MfPcBufferManager@@QAEPAVMfPcBuffer@@XZ
MfPcBuffer *MfPcBufferManager::GetBuffer()
{
    if (mFreeList) {
        if (mFreeList->GetCount() > 0)
            return (MfPcBuffer *)mFreeList->DeleteFirst();
        return new MfPcBuffer;
    }
    return 0;
}

// 0x10003010 ?ReleaseBuffer@MfPcBufferManager@@QAEXPAVMfPcBuffer@@@Z
void MfPcBufferManager::ReleaseBuffer(MfPcBuffer *buffer)
{
    mFreeList->Add(buffer);
}

// 0x10003020 ?Init@MfPcBufferManager@@SAXXZ
void MfPcBufferManager::Init()
{
    if (!s_instance)
        s_instance = new MfPcBufferManager;
    MfPcCompiler::SetBufferManager(s_instance);
}

