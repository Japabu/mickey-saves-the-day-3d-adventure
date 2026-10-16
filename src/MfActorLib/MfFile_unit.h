// MfFile unit: MfActorLib.dll 0x10001880-0x10002230.
// Inline header members of MfFileObject and MfFile (emitted at the start of the
// object that holds MfAnimSequence) followed by MfAnimFile, the file class
// MfAnimSequence uses for per-frame node transforms (not exported; its
// functions have no names in the binary).
#ifndef MFACTORLIB_MFFILE_H
#define MFACTORLIB_MFFILE_H

#include "MfFile.h"

// ---- imported from mflinalg.dll ----
class __declspec(dllimport) MfVector3 {
public:
    MfVector3() { x = 0; y = 0; z = 0; }
    ~MfVector3() {}
    MfVector3 &operator=(const MfVector3 &);
    int operator==(const MfVector3 &v) const { return x == v.x && y == v.y && z == v.z; }
    float x, y, z;
};

class __declspec(dllimport) MfRotation3 {
public:
    MfRotation3();
    ~MfRotation3() {}
    MfRotation3 &operator=(const MfRotation3 &);
    int operator==(const MfRotation3 &) const;
private:
    float q[4];
};

class __declspec(dllimport) MfTransform3 {
public:
    MfTransform3() {}
    ~MfTransform3() {}
    MfVector3 m_translation;    // 0x00
    MfRotation3 m_rotation;     // 0x0c
};

// ---- MfActorLib internal ----
// One frame record per node: translation + rotation (28 bytes).
class MfAnimFile : public MfFile {
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

// First inline member of MfAnimSequence (0x10002220); the class itself and its
// other inline members (0x10002230-) belong to the MfAnimSequence unit. Only the
// part needed here is declared.
class __declspec(dllexport) MfAnimSequence {
public:
    int IsOpen() { return m_isOpen; }
private:
    void *m_00;                 // 0x00 (vptr in the full class)
    void *m_04;                 // 0x04
    int m_isOpen;               // 0x08
};

#endif
