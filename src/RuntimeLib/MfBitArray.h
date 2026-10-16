// MfBitArray and MfIntArray: two-dimensional arrays of bits (streamable) and of
// 16-bit counts, used by the locomotion planner's grids. RuntimeLib's own
// classes (RTTI names, unit 10055d60-100560d0); the member names are invented.
#ifndef RUNTIMELIB_MFBITARRAY_H
#define RUNTIMELIB_MFBITARRAY_H
#include "MfStreamObject.h"

class MfBitArray : public MfStreamObject {
public:
    enum { STREAM_TYPE = 'BitA' };
    MfBitArray();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfBitArray();
    void SetSize(int width, int height);
    void Clear();
    // sets or clears bit (0-7) of byte (defined with the planner grid,
    // MfBfp.cpp)
    void SetBit(int bit, unsigned char *byte, int on);
    void Set(int x, int y)
    {
        int index = mWidth * y + x;
        SetBit(index % 8, &mBits[index / 8], 1);
    }
    int Get(int x, int y)
    {
        int index = mWidth * y + x;
        unsigned char byte = mBits[index / 8];
        int set;
        switch (index % 8) {
        case 0: set = byte & 0x01; break;
        case 1: set = byte & 0x02; break;
        case 2: set = byte & 0x04; break;
        case 3: set = byte & 0x08; break;
        case 4: set = byte & 0x10; break;
        case 5: set = byte & 0x20; break;
        case 6: set = byte & 0x40; break;
        case 7: set = byte & 0x80; break;
        }
        return set;
    }

    int mWidth;                                             // 0x04
    int mHeight;                                            // 0x08
    int mNumBytes;                                          // 0x0c
    int mCapacity;                                          // 0x10 bits
    unsigned char *mBits;                                   // 0x14
};

class MfIntArray {
public:
    MfIntArray();
    virtual ~MfIntArray();
    virtual void Print();
    void SetSize(int width, int height);
    void Clear();

    int mWidth;                                             // 0x04
    int mHeight;                                            // 0x08
    int mCapacity;                                          // 0x0c
    unsigned short *mValues;                                // 0x10
};

#endif
