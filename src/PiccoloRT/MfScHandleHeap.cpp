// PiccoloRT.dll 0x1000ba40-0x1000bbf6: MfScHandleHeap, the pools data handles come from.
#include "MfScHandleHeap.h"
#include "MfScDataHandle.h"
#include "MfScVmStack.h"

// 0x1000ba40 ??0MfScHandleHeap@@QAE@XZ
MfScHandleHeap::MfScHandleHeap()
    : m_free(0), m_nextUnused(0), m_stack(0), m_numFree(0), m_numHandles(0)
{
    unsigned long committed;
    m_stack = new MfScVmStack(MfScVmStack::PageSize() * 512, 0, MfScVmStack::PageSize() * 8,
                              (void **)&m_nextUnused, &committed);
    if (m_nextUnused)
        m_numHandles = m_numFree = committed / sizeof(MfScDataHandle);
}

// 0x1000baf0 ??1MfScHandleHeap@@QAE@XZ
MfScHandleHeap::~MfScHandleHeap()
{
    if (m_stack)
        delete m_stack;
    m_stack = 0;
    m_free = 0;
    m_nextUnused = 0;
    m_numFree = 0;
    m_numHandles = 0;
}

// 0x1000bb60 ?GrowHeap@MfScHandleHeap@@QAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfScHandleHeap::GrowHeap()
{
    unsigned long committed;
    Lock();
    m_nextUnused = (MfScDataHandle *)m_stack->Commit(MfScVmStack::PageSize() * 8, &committed);
    if (m_nextUnused) {
        m_numFree += committed / sizeof(MfScDataHandle);
        m_numHandles += committed / sizeof(MfScDataHandle);
    }
    Unlock();
    return m_nextUnused;
}

// 0x1000bbc0 ?Contains@MfScHandleHeap@@QAEHPAVMfScDataHandle@@@Z
int MfScHandleHeap::Contains(MfScDataHandle *handle)
{
    MfScDataHandle *first = (MfScDataHandle *)m_stack->Base();
    MfScDataHandle *end = (MfScDataHandle *)m_stack->CommittedEnd();
    if (!end)
        end = (MfScDataHandle *)m_stack->CommittedEnd();
    if (handle >= first && handle < end)
        return 1;
    return 0;
}
