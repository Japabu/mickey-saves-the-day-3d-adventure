// MBSoundData.h - a sound held in memory.
#ifndef MBENGINE_MBSOUNDDATA_H
#define MBENGINE_MBSOUNDDATA_H

class MBSoundData {
    friend struct PDB_Layout;
public:
public:
    MBSoundData(const MBSoundData &soundData);
    MBSoundData(unsigned char *soundData, unsigned long dataSize);
    ~MBSoundData();
    unsigned char *GetSoundData() const;
    unsigned long GetDataSize() const;
protected:
    unsigned char *mSoundData;                           // +0x0000 size 0x4
    unsigned long mDataSize;                             // +0x0004 size 0x4
public:
    // void *__vecDelDtor(unsigned int);                 // compiler-generated
};

#endif
