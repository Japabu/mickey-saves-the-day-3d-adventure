#pragma once

// The texts of the tokens of a compilation, one after the other (0xc bytes).
struct MfPcStringPool {
    MfPcStringPool(int length)
    {
        mSize = length == 0 ? 10 : length * 2 + 4;
        mNext = mBase = new char[mSize];
    }
    ~MfPcStringPool() { delete mBase; }
    char *Allocate(int size)
    {
        mNext += size;
        return mNext - size;
    }
    char *mBase;                // +0x00
    char *mNext;                // +0x04
    int mSize;                  // +0x08
};
