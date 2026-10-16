// MfBitArray and MfIntArray: two-dimensional arrays of bits and of 16-bit
// counts (RuntimeLib.dll 10055d60-100560d0).
#include "MfBitArray.h"
#include <stdio.h>
#include <string.h>
#include "MfStream.h"

// 0x10055d60 ??0MfBitArray@@QAE@XZ
MfBitArray::MfBitArray()
{
    mWidth = 2;
    mHeight = 2;
    mCapacity = 4;
    mNumBytes = 1;
    mBits = new unsigned char[1];
    Clear();
}

// 0x10055da0 ??_GMfBitArray@@UAEPAXI@Z

// 0x10055dc0 ??1MfBitArray@@UAE@XZ
MfBitArray::~MfBitArray()
{
    delete mBits;
}

// 0x10055de0 ?SetSize@MfBitArray@@QAEXHH@Z
void MfBitArray::SetSize(int width, int height)
{
    mHeight = height;
    mWidth = width;
    int count = width * height;
    if (count > mCapacity) {
        mCapacity = count;
        mNumBytes = count / 8 + 1;
        delete mBits;
        mBits = new unsigned char[mNumBytes];
        Clear();
    }
}

// 0x10055e40 ?Clear@MfBitArray@@QAEXXZ
void MfBitArray::Clear()
{
    memset(mBits, 0, mNumBytes);
}

// 0x10055e60 ?CreateBitArray@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateBitArray()
{
    return new MfBitArray;
}

// 0x10055ec0 _$E2
// 0x10055ed0 _$E1
static MfStreamConstructorEntry constructorEntry(MfBitArray::STREAM_TYPE, CreateBitArray);

// 0x10055ef0 ?StreamType@MfBitArray@@UBEJXZ
long MfBitArray::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10055f00 ?ReadFromStream@MfBitArray@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfBitArray::ReadFromStream(MfStream &stream)
{
    int width, height;
    stream >> width >> height;
    SetSize(width, height);
    stream.ReadBytes(mNumBytes, mBits);
    return stream.Status();
}

// 0x10055f50 ?WriteToStream@MfBitArray@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfBitArray::WriteToStream(MfStream &stream) const
{
    stream << mWidth << mHeight;
    stream.WriteBytes(mNumBytes, mBits);
    return stream.Status();
}

// 0x10055f90 ??0MfIntArray@@QAE@XZ
MfIntArray::MfIntArray()
{
    mWidth = 2;
    mHeight = 2;
    mCapacity = 4;
    mValues = new unsigned short[4];
    Clear();
}

// 0x10055fd0 ??_GMfIntArray@@UAEPAXI@Z

// 0x10055ff0 ??1MfIntArray@@UAE@XZ
MfIntArray::~MfIntArray()
{
    delete mValues;
}

// 0x10056010 ?SetSize@MfIntArray@@QAEXHH@Z
void MfIntArray::SetSize(int width, int height)
{
    mHeight = height;
    mWidth = width;
    int count = width * height;
    if (count > mCapacity) {
        mCapacity = count;
        delete mValues;
        mValues = new unsigned short[mCapacity];
        Clear();
    }
}

// 0x10056060 ?Clear@MfIntArray@@QAEXXZ
void MfIntArray::Clear()
{
    for (int i = 0; i < mCapacity; i++)
        mValues[i] = 0;
}

// 0x10056080 ?Print@MfIntArray@@UAEXXZ
void MfIntArray::Print()
{
    for (int y = mHeight - 1; y >= 0; y--) {
        for (int x = 0; x < mWidth; x++)
            printf("%d    ", mValues[y * mWidth + x]);
        printf("\n");
    }
}
