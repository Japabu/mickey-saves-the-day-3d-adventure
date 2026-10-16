// PiccoloRT.dll 0x1000bda0-0x1000c02a: the VM data stack.
#include <windows.h>
#include "MfScVmStack.h"

unsigned long MfScVmStack::dwPageSize;

// 0x1000bda0 ??0MfScVmStack@@QAE@KHKPAPAXPAK@Z
MfScVmStack::MfScVmStack(unsigned long reserve, int growsDown, unsigned long initial, void **base,
                         unsigned long *committed)
{
    mGrowsDown = growsDown;
    if (!dwPageSize) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        dwPageSize = info.dwPageSize;
    }
    mBase = (char *)VirtualAlloc(0, reserve, MEM_RESERVE, PAGE_READWRITE);
    MEMORY_BASIC_INFORMATION memory;
    VirtualQuery(mBase, &memory, sizeof(memory));
    mReserved = memory.RegionSize;
    mCommitted = 0;
    mTop = 0;
    if (initial > 0 && base)
        *base = Commit(initial, committed);
}

// 0x1000be30 ??1MfScVmStack@@QAE@XZ
MfScVmStack::~MfScVmStack()
{
    if (mBase)
        VirtualFree(mBase, 0, MEM_RELEASE);
}

// 0x1000be50 ?Commit@MfScVmStack@@QAEPAXKPAK@Z
void *MfScVmStack::Commit(unsigned long size, unsigned long *committed)
{
    unsigned long page = dwPageSize;
    if (size & (page - 1))
        size = (size & ~(page - 1)) + page;
    if (mGrowsDown == 1)
        size += page;
    if (mReserved - mCommitted < size) {
        if (committed)
            *committed = mReserved - mCommitted;
        return 0;
    }
    void *p;
    if (mGrowsDown == 1) {
        char *bottom;
        if (mTop) {
            VirtualAlloc(mTop, page, MEM_COMMIT, PAGE_READWRITE);
            bottom = mTop - size;
        } else
            bottom = mBase - size + mReserved;
        p = VirtualAlloc(bottom, size, MEM_COMMIT, PAGE_READWRITE);
        VirtualAlloc(bottom, dwPageSize, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD);
    } else
        p = VirtualAlloc(mBase + mCommitted, size, MEM_COMMIT, PAGE_READWRITE);
    if (!p) {
        if (committed)
            *committed = 0;
        return 0;
    }
    mCommitted += size;
    if (mGrowsDown == 1)
        mTop = (char *)p;
    else
        mTop = mBase;
    if (committed)
        *committed = size;
    return p;
}

// 0x1000bf50 ?Decommit@MfScVmStack@@QAEPAXKPAK@Z
void *MfScVmStack::Decommit(unsigned long size, unsigned long *decommitted)
{
    size &= ~(dwPageSize - 1);
    if (!size)
        goto failed;
    if (size > mCommitted)
        size = mCommitted;
    if (!VirtualFree(mGrowsDown == 1 ? mTop : mTop - size + mCommitted, size - 1, MEM_DECOMMIT)) {
failed:
        if (decommitted)
            *decommitted = 0;
        return mTop;
    }
    mCommitted -= size;
    if (mGrowsDown == 1)
        mTop += size;
    if (decommitted)
        *decommitted = size;
    return mTop;
}

// 0x1000bfd0 ?PageSize@MfScVmStack@@SAKXZ
unsigned long MfScVmStack::PageSize()
{
    if (!dwPageSize) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        dwPageSize = info.dwPageSize;
    }
    return dwPageSize;
}

// 0x1000c000 ?CommittedEnd@MfScVmStack@@QAEPADXZ
char *MfScVmStack::CommittedEnd()
{
    return mBase + mCommitted;
}

// 0x1000c010 ?ResetGuardPage@MfScVmStack@@QAEXXZ
void MfScVmStack::ResetGuardPage()
{
    VirtualAlloc(mTop, dwPageSize, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD);
}
