#pragma once
// MfJLLinear: a joint limit that keeps the translation on a segment.
#include "MfActorLib_api.h"
#include "MfJLimit.h"

// Translation limited to a segment. size 0x2c
class MFACTOR_API MfJLLinear : public MfJLimit {
public:
    MfJLLinear();
    MfJLLinear(const MfJLLinear &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLLinear() {}
    virtual MfJLimit *Duplicate() { return new MfJLLinear(*this); }
    virtual int GetType() { return MF_JL_LINEAR; }
    virtual int FixTransform(MfTransform3 &);
    virtual void PivotLimit(MfRotation3 &);
    virtual void SetValue(const MfVector3 &, const MfVector3 &);
    virtual void GetValue(MfVector3 &, MfVector3 &) const;
    int CenterTransform(MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    MfVector3 m_start;      // 0x04
    MfVector3 m_end;        // 0x10
    MfVector3 m_direction;  // 0x1c unit vector start -> end
    float m_length;         // 0x28
};
