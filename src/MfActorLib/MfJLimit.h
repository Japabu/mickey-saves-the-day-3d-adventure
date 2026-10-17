#pragma once
// Joint limits: constraints on a skeleton joint's transform. MfJLimit is the
// unconstrained base of MfJLHinge, MfJLEuler, MfJLLinear and MfJLTranslate; the
// inline members of all five classes are emitted at the start of the MfJLimit
// object (MfJLimit_2.cpp, 0x100080c0-).
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfTransform3.h"

class MfStream;

enum {
    MF_JL_NONE = 0,
    MF_JL_HINGE = 1,
    MF_JL_EULER = 2,
    MF_JL_LINEAR = 3,
    MF_JL_TRANSLATE = 4
};

// size 4
class MFACTOR_API MfJLimit : public MfStreamObject {
public:
    MfJLimit() {}
    MfJLimit(const MfJLimit &) {}
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLimit() {}
    virtual MfJLimit *Duplicate() { return new MfJLimit(*this); }
    virtual int GetType() { return MF_JL_NONE; }
    virtual int FixTransform(MfTransform3 &) { return 0; }
    virtual void PivotLimit(MfRotation3 &) {}
    static const long STREAM_TYPE;
};

// The other joint limits
#include "MfJLHinge.h"
#include "MfJLEuler.h"
#include "MfJLLinear.h"
#include "MfJLTranslate.h"
