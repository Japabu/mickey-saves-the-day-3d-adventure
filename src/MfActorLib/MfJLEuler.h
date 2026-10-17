#pragma once
// MfJLEuler: a joint limit on the Euler angles of the rotation, per axis.
#include "MfActorLib_api.h"
#include "MfJLimit.h"

// Euler angles limited per axis. size 0x1c
class MFACTOR_API MfJLEuler : public MfJLimit {
public:
    MfJLEuler();
    MfJLEuler(const MfJLEuler &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLEuler() {}
    virtual MfJLimit *Duplicate() { return new MfJLEuler(*this); }
    virtual int GetType() { return MF_JL_EULER; }
    virtual int FixTransform(MfTransform3 &);
    virtual void PivotLimit(MfRotation3 &);
    virtual void SetValue(int axis, float min, float max)
    {
        m_min[axis] = min;
        m_max[axis] = max;
    }
    virtual void GetValue(int axis, float &min, float &max) const
    {
        min = m_min[axis];
        max = m_max[axis];
    }
    int CenterTransform(MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    float m_min[3];         // 0x04
    float m_max[3];         // 0x10
};
