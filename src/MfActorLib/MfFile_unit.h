// MfFile unit: MfActorLib.dll 0x10001880-0x10002230.
// Inline header members of MfFileObject and MfFile (emitted at the start of the
// object that holds MfAnimSequence) followed by MfAnimFile (MfAnimSequence.h), the
// file class MfAnimSequence uses for per-frame node transforms. mflinalg's
// classes as this object sees them.
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

// ---- MfActorLib ----
#include "MfAnimSequence.h"

#endif
