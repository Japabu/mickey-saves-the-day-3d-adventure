#pragma once
// Joint limits: constraints on a skeleton joint's transform. MfJLimit is the
// unconstrained base; the inline members of all five classes are emitted at
// the start of the MfJLimit object (MfJLimit_2.cpp, 0x100080c0-).
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
class __declspec(dllexport) MfJLimit : public MfStreamObject {
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

// Rotation limited to the arc between two orientations. size 0x30
class __declspec(dllexport) MfJLHinge : public MfJLimit {
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

// Euler angles limited per axis. size 0x1c
class __declspec(dllexport) MfJLEuler : public MfJLimit {
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

// Translation limited to a segment. size 0x2c
class __declspec(dllexport) MfJLLinear : public MfJLimit {
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

// Translation limited to a box. size 0x1c
class __declspec(dllexport) MfJLTranslate : public MfJLimit {
public:
    MfJLTranslate();
    MfJLTranslate(const MfJLTranslate &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLTranslate() {}
    virtual MfJLimit *Duplicate() { return new MfJLTranslate(*this); }
    virtual int GetType() { return MF_JL_TRANSLATE; }
    virtual int FixTransform(MfTransform3 &);
    virtual void PivotLimit(MfRotation3 &);
    virtual void SetValue(const MfVector3 &, const MfVector3 &);
    virtual void GetValue(MfVector3 &, MfVector3 &) const;
    int CenterTransform(MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    MfVector3 m_min;        // 0x04
    MfVector3 m_max;        // 0x10
};
