#pragma once
// MfAnimFile: the file of one actor's motion in an MfAnimSequence ('Aamf'): one
// frame record per node, translation + rotation (28 bytes). Not exported; its
// functions have no names in the binary (MfFile.cpp).
#include "MfFile.h"
class MfTransform3;

// size 0x4c
class MfAnimFile : public MfFile {
    friend class MfAnimSequence;
public:
    MfAnimFile();
    virtual ~MfAnimFile();
    virtual int Open(const MfString &, int);
    virtual int Close();
    virtual unsigned long ReadBytes(unsigned long, void *);

    int WriteFrame(const void *, int);
    int ReadFrame(int, void *, int);
    int ReadNodeFrames(int, MfTransform3 *, int, int &, int &);
private:
    static MfFileType ClassTypeInFile();
    void _Seek(unsigned long);
    int _InitWrite();
    int _InitRead();

    int m_numFrames;            // 0x34
    MfString m_name;            // 0x38
    int m_numNodes;             // 0x3c
    unsigned long m_headerPos;  // 0x40
    unsigned long m_dataStart;  // 0x44
    unsigned long m_dataEnd;    // 0x48
};
