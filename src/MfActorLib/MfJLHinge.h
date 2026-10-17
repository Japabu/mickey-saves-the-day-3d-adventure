#pragma once
// MfJLHinge: a joint limit that keeps the rotation on the arc between two
// orientations ('JLHi'; MfJLimit.h has the other joint limits).
#include "MfActorLib_api.h"
#include "MfJLimit.h"

// Rotation limited to the arc between two orientations. size 0x30
class MFACTOR_API MfJLHinge : public MfJLimit {
public:
    MfJLHinge();
    MfJLHinge(const MfJLHinge &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLHinge() {}
    virtual MfJLimit *Duplicate() { return new MfJLHinge(*this); }
    virtual int GetType() { return MF_JL_HINGE; }
    virtual int FixTransform(MfTransform3 &);
    virtual void PivotLimit(MfRotation3 &);
    virtual void SetValue(const MfRotation3 &, const MfRotation3 &, int);
    virtual void GetValue(MfRotation3 &from, MfRotation3 &to, int &mode) const
    {
        from = m_from;
        to = m_to;
        mode = m_mode;
    }
    static const long STREAM_TYPE;
protected:
    int m_mode;             // 0x04
    MfRotation3 m_from;     // 0x08
    MfRotation3 m_to;       // 0x18
    float m_invSinSq;       // 0x28 1 / (1 - (from.to)^2)
    float m_cosTerm;        // 0x2c -(from.to) / (1 - (from.to)^2)
};
