// MfJLimit (MfActorLib.dll): a joint limit, a constraint on a skeleton node's
// transform. Declaration as in src/MfActorLib/MfJLimit.h.
#ifndef RUNTIMELIB_MFJLIMIT_H
#define RUNTIMELIB_MFJLIMIT_H
#include "MfRotation3.h"
#include "MfStreamObject.h"
class MfRotation3;
class MfStream;
class MfTransform3;

enum {
    MF_JL_NONE = 0,
    MF_JL_HINGE = 1,
    MF_JL_EULER = 2,
    MF_JL_LINEAR = 3,
    MF_JL_TRANSLATE = 4
};

class __declspec(dllimport) MfJLimit : public MfStreamObject {
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

// rotation limited to the arc between two orientations
class __declspec(dllimport) MfJLHinge : public MfJLimit {
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
    int m_mode;                                             // 0x04
    MfRotation3 m_from;                                     // 0x08
    MfRotation3 m_to;                                       // 0x18
    float m_invSinSq;                                       // 0x28
    float m_cosTerm;                                        // 0x2c
};

#endif
