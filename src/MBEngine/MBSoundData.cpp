#include "MBSoundData.h"

// 0x10047790 ??0MBSoundData@@QAE@PAEK@Z
MBSoundData::MBSoundData(unsigned char *soundData, unsigned long dataSize) : mSoundData(soundData), mDataSize(dataSize)
{
}

// 0x100477b0 ??0MBSoundData@@QAE@ABV0@@Z
MBSoundData::MBSoundData(const MBSoundData &soundData)
{
    mSoundData = 0;
    mDataSize = 0;
    mSoundData = (unsigned char *)MBmalloc_release(soundData.mDataSize);
    MBmemcpy(mSoundData, soundData.mSoundData, soundData.mDataSize);
    mDataSize = soundData.mDataSize;
}

// 0x100477f0 ??1MBSoundData@@QAE@XZ
MBSoundData::~MBSoundData()
{
    if (mSoundData)
        MBfree_release(mSoundData);
}

// 0x10047800 ?GetSoundData@MBSoundData@@QBEPAEXZ
unsigned char *MBSoundData::GetSoundData() const
{
    return mSoundData;
}

// 0x10047810 ?GetDataSize@MBSoundData@@QBEKXZ
unsigned long MBSoundData::GetDataSize() const
{
    return mDataSize;
}
