#pragma once

// The data stack of an MfScVmEnv: address space reserved with VirtualAlloc,
// committed page by page. A stack that grows down keeps a guard page below
// its committed part (a stack overflow touches it).
class MfScVmStack {
public:
    // reserves reserve bytes; commits initial bytes and stores their address
    // in *base (the size committed in *committed)
    MfScVmStack(unsigned long reserve, int growsDown, unsigned long initial, void **base,
                unsigned long *committed);
    ~MfScVmStack();
    // commits size more bytes (rounded up to pages); returns their address
    void *Commit(unsigned long size, unsigned long *committed);
    // gives size bytes back (rounded down to pages); returns the new top
    void *Decommit(unsigned long size, unsigned long *decommitted);
    // the end of the committed part of a stack that grows up
    char *CommittedEnd();
    char *Base() { return mBase; }
    // arms the guard page again after a stack overflow
    void ResetGuardPage();

    static unsigned long PageSize();

private:
    char *mBase;                    // +0x00
    unsigned long mReserved;        // +0x04
    unsigned long mCommitted;       // +0x08
    char *mTop;                     // +0x0c the lowest committed address of a stack growing down
    int mGrowsDown;                 // +0x10

    static unsigned long dwPageSize;
};
